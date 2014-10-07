requiresMeique("1.0")
GCC:addCustomFlags("-std=c++0x")

qtCreatorLibPath = option("QtCreatorLibDir", "Where QtCreator plugins folder is.", "/usr/lib/qtcreator")

function readQtCVersion()
    local f = io.popen("qtcreator -version 2>&1", 'r')
    local s = f:read('*a')
    f:close()
    local version = string.match(s, "Qt Creator (%d.%d.%d+)")
    abortIf(not version, "qtcreator not in your system PATH or we failed to find the QtCreator version in the output of 'qtcreator -version'.")
    return version
end

function getQtCreatorSources()
    if os.execute("test -d "..QTC_DIR) == 0 then
        return true
    end

    print("Downloading QtCreator sources... (using curl)")
    url = string.format("http://download.qt-project.org/official_releases/qtcreator/%s/%s/qt-creator-opensource-src-%s.tar.gz", QTC_SHORT_VERSION, QTC_VERSION, QTC_VERSION)
    if os.execute("cd "..buildDir().." && curl -L -O "..url) == 0 then
        untar = string.format("tar -xf %sqt-creator-opensource-src-%s.tar.gz -C %s", buildDir(), QTC_VERSION, buildDir())
        print(untar)
        abortIf(os.execute(untar) ~= 0, "Failed to untar QtCreator sources.")
    end
end

QTC_VERSION = readQtCVersion()
QTC_SHORT_VERSION = string.match(QTC_VERSION, "(%d.%d).%d+")
-- Seems QtCreator guys forgot to upload 3.2.1 tarball, so using 3.2.0.
QTC_VERSION = QTC_SHORT_VERSION..".0"
QTC_DIR = string.format("%sqt-creator-opensource-src-%s", buildDir(), QTC_VERSION)

getQtCreatorSources()

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
plugin:addIncludePath(QTC_DIR.."/src/plugins")
plugin:addIncludePath(QTC_DIR.."/src/libs")

plugin:addFiles([[
    editor/RubyAutoCompleter.cpp
    editor/RubyCodeModel.cpp
    editor/RubyCodeStylePreferencesFactory.cpp
    editor/RubyCompletionAssist.cpp
    editor/RubySymbolFilter.cpp
    editor/RubyEditor.cpp
    editor/RubyEditorFactory.cpp
    editor/RubyEditorWidget.cpp
    editor/RubyHighlighter.cpp
    editor/RubyIndenter.cpp
    editor/RubyScanner.cpp

    projectmanager/RubyProject.cpp
    projectmanager/RubyProjectManager.cpp
    projectmanager/RubyProjectWizard.cpp

    RubyPlugin.cpp
]])
plugin:addQtResource("Ruby.qrc")

plugin:addLibraryPath(qtCreatorLibPath)
plugin:addLibraryPath(qtCreatorLibPath.."/plugins")
plugin:addLibraryPath(qtCreatorLibPath.."/plugins/QtProject")

plugin:addLinkLibraries([[
Core
TextEditor
ProjectExplorer
]])

addSubdirectory("tests")

plugin:install("plugins")
plugin:install(plugin:buildDir().."/RubySupport.pluginspec", "plugins")
