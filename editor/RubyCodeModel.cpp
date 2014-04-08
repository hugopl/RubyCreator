#include "RubyCodeModel.h"

#include <QElapsedTimer>
#include <QFile>
#include <QDebug>

namespace Ruby {

Symbol::Symbol(const QString& name, int line, int column)
    : name(name), line(line), column(column)
{
}

CodeModel::CodeModel()
{
    RUBY_INIT_STACK;
    ruby_init();
    ruby_init_loadpath();
    const char*  options[]  =  { "", "-enil", 0 };
    ruby_exec_node(ruby_options(2, const_cast<char**>(options)));

    QFile parser(":/rubysupport/RubyParser.rb");
    parser.open(QFile::ReadOnly);
    QByteArray parserData = parser.readAll();

    int result;
    rb_eval_string_protect(parserData.data(), &result);
    Q_ASSERT(result == 0);

    m_getMethodDeclarations = rb_intern("get_method_declarations");
    Q_ASSERT(m_getMethodDeclarations);
}

CodeModel::~CodeModel()
{
    ruby_cleanup(0);
}

CodeModel* CodeModel::instance()
{
    static CodeModel model;
    return &model;
}

void CodeModel::updateModel(const QString& file)
{
    QElapsedTimer timer;
    timer.start();

    QFile f(file);
    if (!f.open(QFile::ReadOnly))
        return;

    QByteArray data = f.readAll();
    VALUE input = rb_str_new(data.data(), data.length());
    VALUE result = rb_funcall(rb_cObject, m_getMethodDeclarations, 1, input);
    char* resultData = StringValuePtr(result);

    QList<Symbol> symbols;

    QByteArray foo(resultData);
    foreach (QByteArray line, foo.split('\n')) {
        if (line.isEmpty())
            continue;
        QList<QByteArray> d = line.split(' ');
        symbols << Symbol(d[2], atoi(d[0]), atoi(d[1]));
    }
    m_symbols[file] = symbols;
    qDebug() << "Code model updated in" << timer.elapsed() << "ms";
}

QList<Symbol> CodeModel::methodsIn(const QString& file)
{
    return m_symbols[file];
}

}

#include "RubyCodeModel.moc"
