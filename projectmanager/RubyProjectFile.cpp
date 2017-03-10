#include "RubyProjectFile.h"
#include "../RubyConstants.h"

#include "coreplugin/id.h"
#include "utils/fileutils.h"

namespace Ruby {

ProjectFile::ProjectFile(const Utils::FileName &filePath)
{
    setId(Core::Id(Constants::ProjectFileId));
    setMimeType(QLatin1String(Constants::ProjectMimeType));
    setFilePath(filePath);
}

}
