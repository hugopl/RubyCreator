#ifndef RubyCurrentDocumentFilter_h
#define RubyCurrentDocumentFilter_h

#include <coreplugin/locator/ilocatorfilter.h>

namespace Core { class IEditor; }

namespace Ruby {

class CurrentDocumentFilter : public  Core::ILocatorFilter
{
    Q_OBJECT
public:
    explicit CurrentDocumentFilter();

    QList<Core::LocatorFilterEntry> matchesFor(QFutureInterface<Core::LocatorFilterEntry>& future, const QString& entry) override;
    void accept(Core::LocatorFilterEntry selection) const override;
    void refresh(QFutureInterface<void>& future) override;
private slots:
    void onCurrentEditorChanged(Core::IEditor* editor);

private:
    QIcon m_icon;
    QString m_fileName;
};

}

#endif
