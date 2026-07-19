#include "StdAfx.h"
#include "JDKLevelMapsEditor.h"

#include <QPushButton>
#include <QVBoxLayout>

#include <Cry3DEngine/I3DEngine.h>

#include "Components/MapPreview.h"

#include "../Core/Baking/IMapBaker.h"
#include "../Core/Baking/BakeManager.h"
#include "../Core/Baking/BakeRunResult.h"
#include "../Core/Formats/LayerMapHeader.h"
#include "../Core/FileSystem/PathResolver.h"

CJDKLevelMapsEditor::CJDKLevelMapsEditor(QWidget* pParent) : CDockableEditor(pParent)
{
	m_pRootWidget = new QWidget();
	m_pPathResolver = std::make_unique<JDKLevelMaps::FileSystem::CPathResolver>();
	m_pBakeManager = std::make_unique<JDKLevelMaps::Baking::CBakeManager>(m_pPathResolver.get());

	SetupWidget(m_pRootWidget);
	SetContent(m_pRootWidget);

	auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::ELayerMapType::VegetationDensity);
	if (auto path = m_pPathResolver.get()->GetImagePath(currentBaker->GetId()))
		RefreshPreview(path.value());
}

void CJDKLevelMapsEditor::SetupWidget(QWidget* pWidget)
{
	m_pMapPreview = new JDKLevelMaps::Components::CMapPreview(pWidget);
	m_pGenerateButton = new QPushButton(tr("Generate"), pWidget);
	connect(m_pGenerateButton, &QPushButton::clicked, this, &CJDKLevelMapsEditor::OnGenerateButtonClicked);

	QVBoxLayout* const pLayout = new QVBoxLayout(pWidget);
	pLayout->addWidget(m_pMapPreview, 1);
	pLayout->addWidget(m_pGenerateButton);
	pLayout->addStretch();

	pWidget->adjustSize();
	pWidget->layout()->activate();
}

void CJDKLevelMapsEditor::RefreshPreview(const std::string& imagePath)
{
	const QPixmap pixmap(QString::fromStdString(imagePath));
	if (pixmap.isNull())
	{
		m_pMapPreview->setText(tr("No preview generated yet"));
		return;
	}

	m_pMapPreview->SetPixmap(pixmap);
}

void CJDKLevelMapsEditor::OnGenerateButtonClicked()
{
	const JDKLevelMaps::Baking::SBakeRunResult result = m_pBakeManager.get()->RunBake(JDKLevelMaps::ELayerMapType::VegetationDensity, 2.0f);
	CryLogAlways(result.success ?
		"[JDKLevelMaps] Vegetation Level Map has been baked successfully" :
		"[JDKLevelMaps] Vegetation Level Map baking failed with error: %s",
		result.message.c_str());

	auto currentBaker = m_pBakeManager->GetBaker(JDKLevelMaps::ELayerMapType::VegetationDensity);
	if (auto path = m_pPathResolver.get()->GetImagePath(currentBaker->GetId()))
		RefreshPreview(path.value());
}

const char* CJDKLevelMapsEditor::GetEditorName() const { return "JDK Level Maps"; }

REGISTER_VIEWPANE_FACTORY(CJDKLevelMapsEditor, "JDK Level Maps", "Tools", true);