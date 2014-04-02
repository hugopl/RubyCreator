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
qtCore = findPackage("QtCore", QT_VERSION, REQUIRED)
qtGui = findPackage("QtGui", QT_VERSION, REQUIRED)

plugin = Library:new("RubyEditor")
plugin:useQtAutomoc()
plugin:use(qtCore)
plugin:use(qtGui)

-- QtCreator include paths
plugin:addIncludePath(qtcSrc.."/src/plugins")
plugin:addIncludePath(qtcSrc.."/src/libs")

plugin:addFiles([[
    RubyEditorPlugin.cpp
]])
plugin:addQtResource("RubyEditor.qrc")