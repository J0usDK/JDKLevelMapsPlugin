#pragma once
#include <EditorFramework/Editor.h>

class CJDKLevelMapsEditor final : public CDockableEditor
{
public:
	CJDKLevelMapsEditor(QWidget* pParent = nullptr);
	~CJDKLevelMapsEditor() = default;

	const char* GetEditorName() const override;

private:
	void SetupWidget(QWidget* pWidget);

	void OnGenerateButtonClicked();

private:
	QWidget* m_pRootWidget = nullptr;
	QPushButton* m_pGenerateButton = nullptr;
};