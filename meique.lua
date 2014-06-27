requiresMeique("0.93")
GCC:addCustomFlags("-std=c++0x")

qtcSrc = option("QtCreatorSources", "Where the QtCreator sources are located")

f = io.open(qtcSrc.."/qtcreator.pri", "r")
abortIf(f == nil, qtcSrc.."/qtcreator.pri not found!")
s = f:read("*all")
f:close()
_, _, QTCREATOR_VERSION = string.find(s, "QTCREATOR_VERSION = (3.%d.%d+)")

qtCore = findPackage("Qt5Core")
qtGui = findPackage("Qt5Gui")
qtWidgets = findPackage("Qt5Widgets")
qtTest = findPackage("Qt5Test")

plugin = Library:new("RubySupport")
plugin:useQtAutomoc()
plugin:use(qtCore)
plugin:use(qtGui)
plugin:use(qtWidgets)

configureFile("RubySupport.pluginspec.in", "RubySupport.pluginspec")

-- QtCreator include paths
plugin:addIncludePath(qtcSrc.."/src/plugins")
plugin:addIncludePath(qtcSrc.."/src/libs")

plugin:addFiles([[
    editor/RubyAutoCompleter.cpp
    editor/RubyCodeModel.cpp
    editor/RubyCompletionAssist.cpp
    editor/RubySymbolFilter.cpp
    editor/RubyEditor.cpp
    editor/RubyEditorFactory.cpp
    editor/RubyEditorWidget.cpp
    editor/RubyHighlighter.cpp
    editor/RubyIndenter.cpp
    editor/RubyScanner.cpp
    editor/RubySimpleScanner.cpp

    projectmanager/RubyProject.cpp
    projectmanager/RubyProjectManager.cpp
    projectmanager/RubyProjectWizard.cpp

    RubyPlugin.cpp
]])
plugin:addQtResource("Ruby.qrc")

-- Hardcoded QtCreator libraries path
plugin:addLibraryPaths([[
/usr/lib/qtcreator
/usr/lib/qtcreator/plugins
/usr/lib/qtcreator/plugins/QtProject
]])

plugin:addLinkLibraries([[
Core
TextEditor
ProjectExplorer
]])

addSubdirectory("tests")
