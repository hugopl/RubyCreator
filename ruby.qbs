import qbs 1.0
import QtcPlugin

QtcPlugin {
    name: "Ruby"

    Depends { name: "Qt.widgets" }
    Depends { name: "Utils" }

    Depends { name: "Core" }
    Depends { name: "TextEditor" }
    Depends { name: "ProjectExplorer" }

    Group {
        name: "General"
        files: [
            "RubyPlugin.cpp", "RubyPlugin.h",
            "RubyConstants.h",
            "Ruby.qrc",
            "README.md",
        ]
    }

    Group {
        name: "Editor"
        prefix: "editor/"
        files: [
            "RubyAmbiguousMethodAssistProvider.cpp", "RubyAmbiguousMethodAssistProvider.h",
            "RubyAutoCompleter.cpp", "RubyAutoCompleter.h",
            "RubyCodeModel.cpp", "RubyCodeModel.h",
            "RubyCodeStylePreferencesFactory.cpp", "RubyCodeStylePreferencesFactory.h",
            "RubyCompletionAssist.cpp", "RubyCompletionAssist.h",
            "RubyEditor.cpp",
            "RubyEditorFactory.cpp", "RubyEditorFactory.h",
            "RubyEditor.h",
            "RubyEditorWidget.cpp", "RubyEditorWidget.h",
            "RubyHighlighter.cpp", "RubyHighlighter.h",
            "RubyIndenter.cpp", "RubyIndenter.h",
            "RubyQuickFixAssistProvider.cpp", "RubyQuickFixAssistProvider.h",
            "RubyQuickFixes.cpp", "RubyQuickFixes.h",
            "RubyRubocopHighlighter.cpp", "RubyRubocopHighlighter.h",
            "RubyScanner.cpp", "RubyScanner.h",
            "RubySnippetProvider.h",
            "RubySymbolFilter.cpp", "RubySymbolFilter.h",
            "RubySymbol.h",
            "SourceCodeStream.h",
        ]
    }

    Group {
        name: "Project Manager"
        prefix: "projectmanager/"
        files: [
            "RubyProject.cpp", "RubyProject.h",
            "RubyProjectManager.cpp", "RubyProjectManager.h",
            "RubyProjectNode.h",
            "RubyProjectWizard.cpp", "RubyProjectWizard.h",
            "RubyRunConfiguration.cpp", "RubyRunConfiguration.h",
        ]
    }

    Group {
        name: "Tests"
        condition: qtc.testsEnabled
        files: ["editor/ScannerTest.cpp"]
    }
}
