requiresMeique(0.93)
GCC:addCustomFlags("-std=c++0x")

qtcSrc = option("QtCreatorSources", "Where the QtCreator sources are located")

f = io.open(qtcSrc.."/qtcreator.pri", "r")
abortIf(f == nil, qtcSrc.."/qtcreator.pri not found!")
s = f:read("*all")
f:close()
_, _, QTCREATOR_VERSION = string.find(s, "QTCREATOR_VERSION = (3.0.%d)")

QT_VERSION = "4.8"
qtCore = findPackage("QtCore", QT_VERSION)
qtGui = findPackage("QtGui", QT_VERSION)
ruby = findPackage("ruby-2.1")

editor = Library:new("RubySupport")
editor:useQtAutomoc()
editor:use(qtCore)
editor:use(qtGui)
editor:use(ruby)

configureFile("RubySupport.pluginspec.in", "RubySupport.pluginspec")

-- QtCreator include paths
editor:addIncludePath(qtcSrc.."/src/plugins")
editor:addIncludePath(qtcSrc.."/src/libs")

editor:addFiles([[
    editor/RubyCodeModel.cpp
    editor/RubyCurrentDocumentFilter.cpp
    editor/RubyEditor.cpp
    editor/RubyEditorFactory.cpp
    editor/RubyEditorWidget.cpp
    editor/RubyHighlighter.cpp
    editor/RubyScanner.cpp

    projectmanager/RubyProjectManager.cpp
    projectmanager/RubyProject.cpp

    RubyPlugin.cpp
]])
editor:addQtResource("Ruby.qrc")

-- Hardcoded QtCreator libraries path
editor:addLibraryPaths([[
/usr/lib/qtcreator
/usr/lib/qtcreator/plugins
/usr/lib/qtcreator/plugins/QtProject
]])

editor:addLinkLibraries([[
Core
TextEditor
ProjectExplorer
]])
