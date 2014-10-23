#ifndef RubySymbolFilter_h
#define RubySymbolFilter_h

#include <coreplugin/locator/ilocatorfilter.h>
#include <functional>
#include "RubySymbol.h"

namespace Core { class IEditor; }

namespace Ruby {

typedef std::function<QList<Symbol>(const QString &)> SymbolProvider;

class SymbolFilter : public  Core::ILocatorFilter
{
    Q_OBJECT
public:
    SymbolFilter(SymbolProvider provider, const char *description, QChar shortcut);

    QList<Core::LocatorFilterEntry> matchesFor(QFutureInterface<Core::LocatorFilterEntry> &future, const QString &entry) Q_DECL_OVERRIDE;
    void accept(Core::LocatorFilterEntry selection) const Q_DECL_OVERRIDE;
    void refresh(QFutureInterface<void> &future) Q_DECL_OVERRIDE;
private slots:
    void onCurrentEditorChanged(Core::IEditor *editor);

private:
    QIcon m_icon;
    QString m_fileName;
    SymbolProvider m_symbolProvider;
};

}

#endif
