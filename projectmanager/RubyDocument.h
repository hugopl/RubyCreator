#ifndef Ruby_Document_h
#define Ruby_Document_h

#include <coreplugin/idocument.h>

namespace Ruby {

class Document : public Core::IDocument
{
public:
    virtual bool save(QString*, const QString&, bool) override { return false; }
    virtual QString defaultPath() const override { return QString(); }
    virtual QString suggestedFileName() const override { return QString(); }

    virtual bool isModified() const override { return false; }
    virtual bool isSaveAsAllowed() const override { return false; }

    virtual ReloadBehavior reloadBehavior(ChangeTrigger, ChangeType) const override { return BehaviorSilent; }
    virtual bool reload(QString*, ReloadFlag, ChangeType) override { return false; }
};

}

#endif
