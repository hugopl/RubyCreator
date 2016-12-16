#include "RubyProjectNode.h"

namespace Ruby {

QList<ProjectExplorer::ProjectAction> ProjectNode::supportedActions(ProjectExplorer::Node *) const
{
    using namespace ProjectExplorer;
    return QList<ProjectAction>({ AddNewFile, EraseFile, Rename, DuplicateFile });
}

}
