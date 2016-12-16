#ifndef Ruby_ProjectFile_h
#define Ruby_ProjectFile_h

#include "coreplugin/idocument.h"

namespace Ruby {

class ProjectFile : public Core::IDocument
{
public:
    ProjectFile(const QString &filePath);

};

}

#endif
