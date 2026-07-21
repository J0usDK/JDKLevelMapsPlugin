#include "StdAfx.h"
#include "JDKLevelMapsEditor.h"

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>

#include <Cry3DEngine/I3DEngine.h>

#include "Components/MapPreview.h"

#include "../Core/Baking/IMapBaker.h"
#include "../Core/Baking/BakeManager.h"
#include "../Core/Baking/BakeRunResult.h"
#include "../Core/Baking/LevelBakeContext.h"
#include "../Core/Formats/LayerMapHeader.h"
#include "../Core/FileSystem/PathResolver.h"

#include "../Utils/ConvertUtils.h"
#include "../Settings/BakerSettings.h"

CJDKLevelMapsEditor::CJDKLevelMapsEditor(QWidget* pParent) : CDockableEditor(pParent)
{
	m_pRootWidget = new QWidget();
	m_pBakerSettings = std::make_unique<JDKLevelMaps::Settings::SBakerSettings>();
	m_pPathResolver = std::make_unique<JDKLevelMaps::FileSystem::CPathResolver>();
	m_pBakeManager = std::make_unique<JDKLevelMaps::Baking::CBakeManager>(m_pPathResolver.get(), m_pBakerSettings.get());

	LoadSettings();
	SetupWidget(m_pRootWidget);
	SetupConnections();
	SetContent(m_pRootWidget);

	auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::ELayerMapType::VegetationDensity);
	if (auto path = m_pPathResolver.get()->GetImagePath(currentBaker->GetId()))
		RefreshPreview(path.value());
}

void CJDKLevelMapsEditor::OnEditorNotifyEvent(EEditorNotifyEvent event)
{
	if (event != eNotify_OnEndLoad)
		return;

	m_pPathResolver->RecomputePath();

	auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::ELayerMapType::VegetationDensity);
	if (auto path = m_pPathResolver->GetImagePath(currentBaker->GetId()))
		RefreshPreview(path.value());
	else
		m_pMapPreview->setText("No preview generated yet");
}

void CJDKLevelMapsEditor::SetupWidget(QWidget* pWidget)
{
	m_pMapPreview = new JDKLevelMaps::Components::CMapPreview(pWidget);
	m_pCellSizeSpinBox = new QDoubleSpinBox(pWidget);
	m_pSensetivitySpinBox = new QSpinBox(pWidget);
	m_pGrassCheckBox = new QCheckBox(pWidget);
	m_pBushCheckBox = new QCheckBox(pWidget);
	m_pTreeCheckBox = new QCheckBox(pWidget);
	m_pGrassLineEdit = new QLineEdit(pWidget);
	m_pBushLineEdit = new QLineEdit(pWidget);
	m_pTreeLineEdit = new QLineEdit(pWidget);
	m_pGenerateButton = new QPushButton(tr("Generate"), pWidget);

	m_pCellSizeSpinBox->setDecimals(2);
	m_pCellSizeSpinBox->setSingleStep(0.5);
	const int terrainSize = JDKLevelMaps::Baking::GetLevelTerrainSize();
	const double maxCellSize = terrainSize > 0 ? static_cast<double>(terrainSize) : 8192.0;
	m_pCellSizeSpinBox->setRange(0.1, maxCellSize);
	m_pCellSizeSpinBox->setValue(m_pBakerSettings->cellSize);

	m_pSensetivitySpinBox->setMinimum(0);
	m_pSensetivitySpinBox->setMaximum(255);
	m_pSensetivitySpinBox->setValue(m_pBakerSettings->vegSettings.densityPerInstance);

	m_pGrassCheckBox->setChecked(m_pBakerSettings->vegSettings.enableGrass);
	m_pBushCheckBox->setChecked(m_pBakerSettings->vegSettings.enableBush);
	m_pTreeCheckBox->setChecked(m_pBakerSettings->vegSettings.enableTree);

	m_pGrassLineEdit->setText(QString::fromStdString(m_pBakerSettings->vegSettings.grassGroupName));
	m_pBushLineEdit->setText(QString::fromStdString(m_pBakerSettings->vegSettings.bushGroupName));
	m_pTreeLineEdit->setText(QString::fromStdString(m_pBakerSettings->vegSettings.treeGroupName));

	QFormLayout* pForm = new QFormLayout();
	pForm->addRow(tr("Cell Size"), m_pCellSizeSpinBox);
	pForm->addRow(tr("Sensetivity"), m_pSensetivitySpinBox);
	pForm->addRow(tr("Enable Grass Layer"), m_pGrassCheckBox);
	pForm->addRow(tr("Enable Bush Layer"), m_pBushCheckBox);
	pForm->addRow(tr("Enable Tree Layer"), m_pTreeCheckBox);
	pForm->addRow(tr("Grass Group Name"), m_pGrassLineEdit);
	pForm->addRow(tr("Bush Group Name"), m_pBushLineEdit);
	pForm->addRow(tr("Tree Group Name"), m_pTreeLineEdit);

	QVBoxLayout* const pLayout = new QVBoxLayout(pWidget);
	pLayout->addWidget(m_pMapPreview, 1);
	pLayout->addLayout(pForm);
	pLayout->addWidget(m_pGenerateButton);
	pLayout->addStretch();
}

void CJDKLevelMapsEditor::SetupConnections()
{
	connect(m_pGenerateButton, &QPushButton::clicked, this, &CJDKLevelMapsEditor::OnGenerateButtonClicked);

	connect(m_pCellSizeSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		m_pBakerSettings->cellSize = static_cast<float>(value);
		SaveSettings();
	});

	connect(m_pSensetivitySpinBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		m_pBakerSettings->vegSettings.densityPerInstance = static_cast<uint8>(value);
		SaveVegetationSettings(m_pBakerSettings->vegSettings);
	});

	connect(m_pGrassCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
		m_pBakerSettings->vegSettings.enableGrass = checked;
		SaveVegetationSettings(m_pBakerSettings->vegSettings);
	});

	connect(m_pBushCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
		m_pBakerSettings->vegSettings.enableBush = checked;
		SaveVegetationSettings(m_pBakerSettings->vegSettings);
	});

	connect(m_pTreeCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
		m_pBakerSettings->vegSettings.enableTree = checked;
		SaveVegetationSettings(m_pBakerSettings->vegSettings);
	});

	connect(m_pGrassLineEdit, &QLineEdit::editingFinished, this, [this]() {
		m_pBakerSettings->vegSettings.grassGroupName = m_pGrassLineEdit->text().toStdString();
		SaveVegetationSettings(m_pBakerSettings->vegSettings);
	});

	connect(m_pBushLineEdit, &QLineEdit::editingFinished, this, [this]() {
		m_pBakerSettings->vegSettings.bushGroupName = m_pBushLineEdit->text().toStdString();
		SaveVegetationSettings(m_pBakerSettings->vegSettings);
	});

	connect(m_pTreeLineEdit, &QLineEdit::editingFinished, this, [this]() {
		m_pBakerSettings->vegSettings.treeGroupName = m_pTreeLineEdit->text().toStdString();
		SaveVegetationSettings(m_pBakerSettings->vegSettings);
	});
}

void CJDKLevelMapsEditor::RefreshPreview(const std::string& imagePath)
{
	const QPixmap pixmap(QString::fromStdString(imagePath));
	if (pixmap.isNull())
	{
		m_pMapPreview->SetPixmap(QPixmap());
		m_pMapPreview->setText(tr("No preview generated yet"));
		return;
	}

	m_pMapPreview->SetPixmap(pixmap);
}

void CJDKLevelMapsEditor::OnGenerateButtonClicked()
{
	const JDKLevelMaps::Baking::SBakeRunResult result = m_pBakeManager.get()->RunBake(JDKLevelMaps::ELayerMapType::VegetationDensity);
	CryLogAlways(result.success ?
		"[JDKLevelMaps] Vegetation Level Map has been baked successfully" :
		"[JDKLevelMaps] Vegetation Level Map baking failed with error: %s",
		result.message.c_str());

	auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::ELayerMapType::VegetationDensity);
	if (auto path = m_pPathResolver.get()->GetImagePath(currentBaker->GetId()))
		RefreshPreview(path.value());
}

const char* CJDKLevelMapsEditor::GetEditorName() const { return "JDK Level Maps"; }


void CJDKLevelMapsEditor::SaveSettings()
{
	SetProjectProperty("JDKLevelMaps/CellSize", m_pBakerSettings->cellSize);
	SaveVegetationSettings(m_pBakerSettings->vegSettings);
}

void CJDKLevelMapsEditor::SaveVegetationSettings(const JDKLevelMaps::Settings::SVegetationBakerSettings& vegSettings)
{
	SetProjectProperty("JDKLevelMaps/DensityPerInstance", vegSettings.densityPerInstance);
	SetProjectProperty("JDKLevelMaps/GrassGroupName", QString::fromStdString(vegSettings.grassGroupName));
	SetProjectProperty("JDKLevelMaps/BushGroupName", QString::fromStdString(vegSettings.bushGroupName));
	SetProjectProperty("JDKLevelMaps/TreeGroupName", QString::fromStdString(vegSettings.treeGroupName));
	SetProjectProperty("JDKLevelMaps/EnableGrass", vegSettings.enableGrass);
	SetProjectProperty("JDKLevelMaps/EnableBush", vegSettings.enableBush);
	SetProjectProperty("JDKLevelMaps/EnableTree", vegSettings.enableTree);
}

void CJDKLevelMapsEditor::LoadSettings()
{
	m_pBakerSettings->cellSize = JDKLevelMaps::Utils::ConvertUtils::QVariantToFloat(GetProjectProperty("JDKLevelMaps/CellSize"), m_pBakerSettings->cellSize);
	const float maxCellSize = static_cast<float>(JDKLevelMaps::Baking::GetLevelTerrainSize());
	if (maxCellSize > 0.0f)
		m_pBakerSettings->cellSize = std::clamp(m_pBakerSettings->cellSize, 0.1f, maxCellSize);

	LoadVegetationSettings(m_pBakerSettings->vegSettings);
}

void CJDKLevelMapsEditor::LoadVegetationSettings(JDKLevelMaps::Settings::SVegetationBakerSettings& vegSettings)
{
	vegSettings.densityPerInstance = JDKLevelMaps::Utils::ConvertUtils::QVariantToUint8(GetProjectProperty("JDKLevelMaps/DensityPerInstance"), vegSettings.densityPerInstance);
	vegSettings.grassGroupName = JDKLevelMaps::Utils::ConvertUtils::QVariantToStdString(GetProjectProperty("JDKLevelMaps/GrassGroupName"), vegSettings.grassGroupName);
	vegSettings.bushGroupName = JDKLevelMaps::Utils::ConvertUtils::QVariantToStdString(GetProjectProperty("JDKLevelMaps/BushGroupName"), vegSettings.bushGroupName);
	vegSettings.treeGroupName = JDKLevelMaps::Utils::ConvertUtils::QVariantToStdString(GetProjectProperty("JDKLevelMaps/TreeGroupName"), vegSettings.treeGroupName);
	vegSettings.enableGrass = JDKLevelMaps::Utils::ConvertUtils::QVariantToBool(GetProjectProperty("JDKLevelMaps/EnableGrass"), vegSettings.enableGrass);
	vegSettings.enableBush = JDKLevelMaps::Utils::ConvertUtils::QVariantToBool(GetProjectProperty("JDKLevelMaps/EnableBush"), vegSettings.enableBush);
	vegSettings.enableTree = JDKLevelMaps::Utils::ConvertUtils::QVariantToBool(GetProjectProperty("JDKLevelMaps/EnableTree"), vegSettings.enableTree);
}

REGISTER_VIEWPANE_FACTORY(CJDKLevelMapsEditor, "JDK Level Maps", "Tools", true);