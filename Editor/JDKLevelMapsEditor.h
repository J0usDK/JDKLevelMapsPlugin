#pragma once
#include <EditorFramework/Editor.h>

namespace JDKLevelMaps
{
	class CBakeManager;
}

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

	std::unique_ptr<JDKLevelMaps::CBakeManager> m_pBakeManager;
};