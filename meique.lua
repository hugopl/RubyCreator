requiresMeique(0.93)
GCC:addCustomFlags("-std=c++0x")

qtcSrc = option("QtCreatorSources", "Where the QtCreator sources are located")

f = io.open(qtcSrc.."/qtcreator.pri", "r")
abortIf(f == nil, qtcSrc.."/qtcreator.pri not found!")
s = f:read("*all")
f:close()
_, _, QTCREATOR_VERSION = string.find(s, "QTCREATOR_VERSION = (3.0.%d)")

configureFile("RubyEditor.pluginspec.in", "RubyEditor.pluginspec")

QT_VERSION = "4.8"
qtCore = findPackage("QtCore", QT_VERSION)
qtGui = findPackage("QtGui", QT_VERSION)
ruby = findPackage("ruby-2.1")

plugin = Library:new("RubyEditor")
plugin:useQtAutomoc()
plugin:use(qtCore)
plugin:use(qtGui)
plugin:use(ruby)

-- QtCreator include paths
plugin:addIncludePath(qtcSrc.."/src/plugins")
plugin:addIncludePath(qtcSrc.."/src/libs")

plugin:addFiles([[
    RubyEditor.cpp
    RubyEditorPlugin.cpp
    RubyEditorFactory.cpp
    RubyEditorWidget.cpp
    RubyHighlighter.cpp
    RubyScanner.cpp
    RubyCurrentDocumentFilter.cpp
    RubyCodeModel.cpp
]])
plugin:addQtResource("RubyEditor.qrc")

-- Hardcoded QtCreator libraries path
plugin:addLibraryPaths([[
/usr/lib/qtcreator
/usr/lib/qtcreator/plugins
/usr/lib/qtcreator/plugins/QtProject
]])

plugin:addLinkLibraries([[
Core
TextEditor
]])
