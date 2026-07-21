#pragma once
#include <EditorFramework/Editor.h>

class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
class QLineEdit;
class QPushButton;

namespace JDKLevelMaps::Components
{
	class CMapPreview;
}

namespace JDKLevelMaps::Settings
{
	struct SBakerSettings;
	struct SVegetationBakerSettings;
}

namespace JDKLevelMaps::Baking
{
	class CBakeManager;
}

namespace JDKLevelMaps::FileSystem
{
	class CPathResolver;
}

class CJDKLevelMapsEditor final : public CDockableEditor, public IAutoEditorNotifyListener
{
public:
	CJDKLevelMapsEditor(QWidget* pParent = nullptr);
	~CJDKLevelMapsEditor() = default;

	void OnEditorNotifyEvent(EEditorNotifyEvent event) override;

	const char* GetEditorName() const override;

private:
	void SetupWidget(QWidget* pWidget);
	void SetupConnections();
	void RefreshPreview(const std::string& imagePath);

	void OnGenerateButtonClicked();

	void LoadSettings();
	void LoadVegetationSettings(JDKLevelMaps::Settings::SVegetationBakerSettings& vegSettings);

	void SaveSettings();
	void SaveVegetationSettings(const JDKLevelMaps::Settings::SVegetationBakerSettings& vegSettings);

private:
	QWidget* m_pRootWidget = nullptr;
	QDoubleSpinBox* m_pCellSizeSpinBox = nullptr;
	QSpinBox* m_pSensetivitySpinBox = nullptr;
	QCheckBox* m_pGrassCheckBox = nullptr;
	QCheckBox* m_pBushCheckBox = nullptr;
	QCheckBox* m_pTreeCheckBox = nullptr;
	QLineEdit* m_pGrassLineEdit = nullptr;
	QLineEdit* m_pBushLineEdit = nullptr;
	QLineEdit* m_pTreeLineEdit = nullptr;
	QPushButton* m_pGenerateButton = nullptr;
	JDKLevelMaps::Components::CMapPreview* m_pMapPreview = nullptr;

	std::unique_ptr<JDKLevelMaps::Settings::SBakerSettings> m_pBakerSettings = nullptr;
	std::unique_ptr<JDKLevelMaps::FileSystem::CPathResolver> m_pPathResolver = nullptr;
	std::unique_ptr<JDKLevelMaps::Baking::CBakeManager> m_pBakeManager = nullptr;
};