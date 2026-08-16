#include "StdAfx.h"
#include "JDKLevelMapsEditor.h"

#include <cmath>

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QProgressBar>

#include <Cry3DEngine/I3DEngine.h>

#include "Components/MapPreview.h"

#include "../Core/Baking/IMapBaker.h"
#include "../Core/Baking/BakeManager.h"
#include "../Core/Baking/BakeRunResult.h"
#include "../Core/Baking/LevelBakeContext.h"
#include "../Core/Baking/BakeProgress.h"
#include "../Core/FileSystem/PathResolver.h"
#include "Shared/MapHeader.h"

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

	auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::EMapType::VegetationDensity);
	if (auto path = m_pPathResolver.get()->GetImagePath(currentBaker->GetId()))
		RefreshPreview(path.value());
}

CJDKLevelMapsEditor::~CJDKLevelMapsEditor()
{
	if (m_bakeThread.joinable())
	{
		CryLogAlways("[JDKLevelMaps] Editor closed. Waiting for background baking thread to finish securely...");
		m_bakeThread.join();
	}
}

void CJDKLevelMapsEditor::OnEditorNotifyEvent(EEditorNotifyEvent event)
{
	if (event != eNotify_OnEndLoad)
		return;

	m_pPathResolver->RecomputePath();

	auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::EMapType::VegetationDensity);
	if (auto path = m_pPathResolver->GetImagePath(currentBaker->GetId()))
		RefreshPreview(path.value());
	else
		m_pMapPreview->setText("No preview generated yet");
}

void CJDKLevelMapsEditor::SetupWidget(QWidget* pWidget)
{
	m_pMapPreview = new JDKLevelMaps::Components::CMapPreview(pWidget);
	m_pCellSizeSpinBox = new QDoubleSpinBox(pWidget);
	m_pTileSizeSpinBox = new QSpinBox(pWidget);
	m_pSensetivitySpinBox = new QSpinBox(pWidget);
	m_pGrassCheckBox = new QCheckBox(pWidget);
	m_pBushCheckBox = new QCheckBox(pWidget);
	m_pTreeCheckBox = new QCheckBox(pWidget);
	m_pGrassLineEdit = new QLineEdit(pWidget);
	m_pBushLineEdit = new QLineEdit(pWidget);
	m_pTreeLineEdit = new QLineEdit(pWidget);
	m_pGenerateButton = new QPushButton(tr("Generate"), pWidget);
	m_pProgressBar = new QProgressBar(pWidget);

	const int terrainSize = JDKLevelMaps::Baking::GetLevelTerrainSize();
	const double maxCellSize = terrainSize > 0 ? static_cast<double>(terrainSize) : 8192.0;
	const int maxTileSize = terrainSize > 0 ? static_cast<int>(terrainSize / m_pBakerSettings->cellSize) : 8192.0;

	m_pCellSizeSpinBox->setDecimals(2);
	m_pCellSizeSpinBox->setSingleStep(0.5);
	m_pCellSizeSpinBox->setRange(0.1, maxCellSize);
	m_pCellSizeSpinBox->setValue(m_pBakerSettings->cellSize);

	m_pTileSizeSpinBox->setMinimum(1);
	m_pTileSizeSpinBox->setMaximum(maxTileSize);
	m_pTileSizeSpinBox->setValue(m_pBakerSettings->tileSize);

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
	pForm->addRow(tr("Tile Size"), m_pTileSizeSpinBox);
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
	pLayout->addWidget(m_pProgressBar);
	pLayout->addStretch();
}

void CJDKLevelMapsEditor::SetupConnections()
{
	connect(m_pGenerateButton, &QPushButton::clicked, this, &CJDKLevelMapsEditor::OnGenerateButtonClicked);

	connect(m_pCellSizeSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged), this, [this](double value) {
		m_pBakerSettings->cellSize = static_cast<float>(value);

		const int terrainSize = JDKLevelMaps::Baking::GetLevelTerrainSize();
		const double safeTerrainSize = terrainSize > 0 ? static_cast<double>(terrainSize) : 8192.0;
		const int maxTileSize = std::max(1, static_cast<int>(std::round(safeTerrainSize / m_pBakerSettings->cellSize)));

		m_pTileSizeSpinBox->setMaximum(maxTileSize);
		SaveSettings();
	});

	connect(m_pTileSizeSpinBox, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
		m_pBakerSettings->tileSize = static_cast<uint32>(value);
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
	m_pGenerateButton->setEnabled(false);
	m_pProgressBar->setValue(0);

	if (m_bakeThread.joinable())
		m_bakeThread.join();

	auto pProgress = std::make_shared<JDKLevelMaps::Baking::SBakeProgress>();

	QTimer* pTimer = new QTimer(this);
	connect(pTimer, &QTimer::timeout, this, [this, pProgress, pTimer]()
	{
		int percent = static_cast<int>((pProgress->writeProgress.load() + pProgress->imageProgress.load()) / 2.0f * 100.0f);
		m_pProgressBar->setValue(percent);

		if (pProgress->isCompleted.load())
		{
			m_pProgressBar->setValue(100);
			pTimer->stop();
			pTimer->deleteLater();

			m_pGenerateButton->setEnabled(true);
			if (pProgress->isSuccess.load())
			{
				CryLogAlways("[JDKLevelMaps] Vegetation Level Map has been baked successfully");
				auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::EMapType::VegetationDensity);
				if (auto path = m_pPathResolver.get()->GetImagePath(currentBaker->GetId()))
					RefreshPreview(path.value());
			}
			else
			{
				CryWarning(VALIDATOR_MODULE_EDITOR, VALIDATOR_ERROR, "[JDKLevelMaps] Vegetation Level Map baking failed with error: %s", pProgress->resultMessage.c_str());
				QMessageBox::critical(this, tr("Bake Failed"), QString::fromStdString(pProgress->resultMessage));
			}
		}
	});

	pTimer->start(33);
	m_bakeThread = std::thread([this, pProgress]()
	{
		JDKLevelMaps::Baking::SBakeRunResult result = m_pBakeManager->RunBake(JDKLevelMaps::EMapType::VegetationDensity, pProgress);

		pProgress->resultMessage = result.message;
		pProgress->isSuccess.store(result.success);
		pProgress->isCompleted.store(true);
	});
}

const char* CJDKLevelMapsEditor::GetEditorName() const { return "JDK Level Maps"; }


void CJDKLevelMapsEditor::SaveSettings()
{
	SetProjectProperty("JDKLevelMaps/CellSize", m_pBakerSettings->cellSize);
	SetProjectProperty("JDKLevelMaps/TileSize", m_pBakerSettings->tileSize);
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
	int terrainSize = JDKLevelMaps::Baking::GetLevelTerrainSize();
	const float maxCellSize = terrainSize > 0 ? static_cast<float>(terrainSize) : 8192.0f;

	const float loadedCellSize = JDKLevelMaps::Utils::ConvertUtils::QVariantToFloat(GetProjectProperty("JDKLevelMaps/CellSize"), m_pBakerSettings->cellSize);
	m_pBakerSettings->cellSize = std::clamp(loadedCellSize, 0.1f, maxCellSize);

	const uint32 loadedTileSize = JDKLevelMaps::Utils::ConvertUtils::QVariantToUint32(GetProjectProperty("JDKLevelMaps/TileSize"), m_pBakerSettings->tileSize);
	const uint32 maxTileSize = std::max(1u, static_cast<uint32>(std::round(terrainSize / m_pBakerSettings->cellSize)));
	m_pBakerSettings->tileSize = std::clamp(loadedTileSize, static_cast<uint32>(1), maxTileSize);

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