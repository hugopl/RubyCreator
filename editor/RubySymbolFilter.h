#ifndef RubySymbolFilter_h
#define RubySymbolFilter_h

#include "RubySymbol.h"

#include <coreplugin/locator/ilocatorfilter.h>

#include <utils/fileutils.h>

#include <functional>

namespace Core { class IEditor; }

namespace Ruby {

typedef std::function<QList<Symbol>(const QString &)> SymbolProvider;

class SymbolFilter : public  Core::ILocatorFilter
{
    Q_OBJECT
public:
    SymbolFilter(SymbolProvider provider, const char *description, QChar shortcut);

    QList<Core::LocatorFilterEntry> matchesFor(QFutureInterface<Core::LocatorFilterEntry> &future, const QString &entry) override;
    void accept(Core::LocatorFilterEntry selection) const override;
    void refresh(QFutureInterface<void> &future) override;

private:
    void onCurrentEditorChanged(Core::IEditor *editor);

private:
    QIcon m_icon;
    Utils::FileName m_fileName;
    SymbolProvider m_symbolProvider;
};

}

#endif
