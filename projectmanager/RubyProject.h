#ifndef Ruby_Project
#define Ruby_Project

#include <projectexplorer/project.h>

namespace TextEditor { class TextDocument; }

namespace Ruby {

class Project : public ProjectExplorer::Project
{
    Q_OBJECT

public:
    explicit Project(const Utils::FilePath &fileName);

private:
    RestoreResult fromMap(const QVariantMap &map, QString *errorMessage) override;
    bool needsConfiguration() const final { return false; }
};

}

#endif
