#ifndef RubyCodeStylePreferencesFactory_h
#define RubyCodeStylePreferencesFactory_h

#include <texteditor/icodestylepreferencesfactory.h>

namespace Ruby {

class CodeStylePreferencesFactory : public TextEditor::ICodeStylePreferencesFactory
{
public:
    Core::Id languageId() Q_DECL_OVERRIDE;
    QString displayName() Q_DECL_OVERRIDE;
    TextEditor::ICodeStylePreferences *createCodeStyle() const Q_DECL_OVERRIDE;
    QWidget *createEditor(TextEditor::ICodeStylePreferences*, QWidget *parent) const Q_DECL_OVERRIDE;
    TextEditor::Indenter *createIndenter() const Q_DECL_OVERRIDE;
    TextEditor::ISnippetProvider *snippetProvider() const Q_DECL_OVERRIDE;
    QString previewText() const Q_DECL_OVERRIDE;

};

}

#endif
