isEmpty(QTC_SOURCE):error(QTC_SOURCE must be set)
isEmpty(QTC_BUILD):error(QTC_BUILD must be set)
IDE_BUILD_TREE=$$QTC_BUILD
QTC_PLUGIN_NAME = Ruby
QTC_PLUGIN_DEPENDS = coreplugin texteditor projectexplorer
include($$QTC_SOURCE/src/qtcreatorplugin.pri)

CONFIG += qjson

SOURCES += RubyPlugin.cpp \
    editor/RubyAmbiguousMethodAssistProvider.cpp \
    editor/RubyAutoCompleter.cpp \
    editor/RubyCodeModel.cpp \
    editor/RubyCodeStylePreferencesFactory.cpp \
    editor/RubyCompletionAssist.cpp \
    editor/RubyEditor.cpp \
    editor/RubyEditorDocument.cpp \
    editor/RubyEditorFactory.cpp \
    editor/RubyEditorWidget.cpp \
    editor/RubyHighlighter.cpp \
    editor/RubyHoverHandler.cpp \
    editor/RubyIndenter.cpp \
    editor/RubyQuickFixAssistProvider.cpp \
    editor/RubyQuickFixes.cpp \
    editor/RubyRubocopHighlighter.cpp \
    editor/RubyScanner.cpp \
    editor/RubySymbolFilter.cpp \
    projectmanager/RubyProject.cpp \
    projectmanager/RubyProjectNode.cpp \
    projectmanager/RubyProjectWizard.cpp

equals(TEST, 1) {
    SOURCES += editor/ScannerTest.cpp
}

HEADERS += RubyPlugin.h \
    RubyConstants.h \
    editor/RubyAmbiguousMethodAssistProvider.h \
    editor/RubyAutoCompleter.h \
    editor/RubyCodeModel.h \
    editor/RubyCodeStylePreferencesFactory.h \
    editor/RubyCompletionAssist.h \
    editor/RubyEditor.h \
    editor/RubyEditorDocument.h \
    editor/RubyEditorFactory.h \
    editor/RubyEditorWidget.h \
    editor/RubyHighlighter.h \
    editor/RubyHoverHandler.h \
    editor/RubyIndenter.h \
    editor/RubyQuickFixAssistProvider.h \
    editor/RubyQuickFixes.h \
    editor/RubyRubocopHighlighter.h \
    editor/RubyScanner.h \
    editor/RubySnippetProvider.h \
    editor/RubySymbol.h \
    editor/RubySymbolFilter.h \
    editor/SourceCodeStream.h \
    projectmanager/RubyProject.h \
    projectmanager/RubyProjectNode.h \
    projectmanager/RubyProjectWizard.h

RESOURCES += \
    Ruby.qrc

OTHER_FILES += \
    Readme.md Ruby.json.in
