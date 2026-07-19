#pragma once
#include <EditorFramework/Editor.h>

class QPushButton;

namespace JDKLevelMaps::Components
{
	class CMapPreview;
}

namespace JDKLevelMaps::Baking
{
	class CBakeManager;
}

namespace JDKLevelMaps::FileSystem
{
	class CPathResolver;
}

class CJDKLevelMapsEditor final : public CDockableEditor
{
public:
	CJDKLevelMapsEditor(QWidget* pParent = nullptr);
	~CJDKLevelMapsEditor() = default;

	const char* GetEditorName() const override;

private:
	void SetupWidget(QWidget* pWidget);
	void RefreshPreview(const std::string& imagePath);

	void OnGenerateButtonClicked();

private:
	QWidget* m_pRootWidget = nullptr;
	QPushButton* m_pGenerateButton = nullptr;
	JDKLevelMaps::Components::CMapPreview* m_pMapPreview = nullptr;

	std::unique_ptr<JDKLevelMaps::FileSystem::CPathResolver> m_pPathResolver = nullptr;
	std::unique_ptr<JDKLevelMaps::Baking::CBakeManager> m_pBakeManager = nullptr;
};