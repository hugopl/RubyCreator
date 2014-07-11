#ifndef RubyCodeStylePreferencesFactory_h
#define RubyCodeStylePreferencesFactory_h

#include <texteditor/icodestylepreferencesfactory.h>

namespace Ruby {

class CodeStylePreferencesFactory : public TextEditor::ICodeStylePreferencesFactory
{
public:
    Core::Id languageId() override;
    QString displayName() override;
    TextEditor::ICodeStylePreferences* createCodeStyle() const override;
    QWidget* createEditor(TextEditor::ICodeStylePreferences*, QWidget* parent) const override;
    TextEditor::Indenter* createIndenter() const override;
    TextEditor::ISnippetProvider* snippetProvider() const override;
    QString previewText() const override;

};

}

#endif
