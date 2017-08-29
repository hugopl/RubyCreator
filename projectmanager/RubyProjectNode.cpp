#include "RubyProjectNode.h"

using namespace ProjectExplorer;

namespace Ruby {

bool ProjectNode::supportsAction(ProjectAction action, const Node *) const
{
    switch (action) {
    case AddNewFile:
    case EraseFile:
    case Rename:
    case DuplicateFile:
        return true;
    default:
        return false;
    }
}

}
