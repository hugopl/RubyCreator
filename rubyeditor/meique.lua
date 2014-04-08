editor = Library:new("RubyEditor")
editor:useQtAutomoc()
editor:use(qtCore)
editor:use(qtGui)
editor:use(ruby)

configureFile("RubyEditor.pluginspec.in", "RubyEditor.pluginspec")

-- QtCreator include paths
editor:addIncludePath(qtcSrc.."/src/plugins")
editor:addIncludePath(qtcSrc.."/src/libs")

editor:addFiles([[
    RubyEditor.cpp
    RubyEditorPlugin.cpp
    RubyEditorFactory.cpp
    RubyEditorWidget.cpp
    RubyHighlighter.cpp
    RubyScanner.cpp
    RubyCurrentDocumentFilter.cpp
    RubyCodeModel.cpp
]])
editor:addQtResource("RubyEditor.qrc")

-- Hardcoded QtCreator libraries path
editor:addLibraryPaths([[
/usr/lib/qtcreator
/usr/lib/qtcreator/plugins
/usr/lib/qtcreator/plugins/QtProject
]])

editor:addLinkLibraries([[
Core
TextEditor
]])
