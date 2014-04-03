#ifndef RubyCurrentDocumentFilter_h
#define RubyCurrentDocumentFilter_h

#include <locator/ilocatorfilter.h>

namespace Core { class IEditor; }

namespace RubyEditor {

class RubyCurrentDocumentFilter : public  Locator::ILocatorFilter
{
    Q_OBJECT
public:
    explicit RubyCurrentDocumentFilter();

    QList<Locator::FilterEntry> matchesFor(QFutureInterface<Locator::FilterEntry>& future, const QString& entry) override;
    void accept(Locator::FilterEntry selection) const override;
    void refresh(QFutureInterface<void>& future) override;
private slots:
    void onCurrentEditorChanged(Core::IEditor* editor);

private:
    QIcon m_icon;
    QString m_fileName;
    bool m_enabled;
};

}

#endif
