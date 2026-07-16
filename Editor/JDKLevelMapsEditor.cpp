#include "StdAfx.h"
#include "JDKLevelMapsEditor.h"

#include <QPushButton>
#include <QVBoxLayout>

#include <Cry3DEngine/I3DEngine.h>

#include "../Core/Baking/BakeManager.h"
#include "../Core/Baking/BakeRunResult.h"
#include "../Core/Formats/LayerMapHeader.h"

CJDKLevelMapsEditor::CJDKLevelMapsEditor(QWidget* pParent) : CDockableEditor(pParent)
{
	m_pRootWidget = new QWidget();
	m_pBakeManager = std::make_unique<JDKLevelMaps::CBakeManager>();

	SetupWidget(m_pRootWidget);
	SetContent(m_pRootWidget);
}

void CJDKLevelMapsEditor::SetupWidget(QWidget* pWidget)
{
	m_pGenerateButton = new QPushButton(tr("Generate"), pWidget);
	connect(m_pGenerateButton, &QPushButton::clicked, this, &CJDKLevelMapsEditor::OnGenerateButtonClicked);

	QVBoxLayout* const pLayout = new QVBoxLayout(pWidget);
	pLayout->addWidget(m_pGenerateButton);
	pLayout->addStretch();
}

void CJDKLevelMapsEditor::OnGenerateButtonClicked()
{
	const JDKLevelMaps::SBakeRunResult result = m_pBakeManager.get()->RunBake(JDKLevelMaps::ELayerMapType::VegetationDensity, 2.0f);
	CryLogAlways(result.success ?
		"[JDKLevelMaps] Vegetation Level Map has been baked successfully" :
		"[JDKLevelMaps] Vegetation Level Map baking failed with error: %s",
		result.message.c_str());
}

const char* CJDKLevelMapsEditor::GetEditorName() const { return "JDK Level Maps"; }

REGISTER_VIEWPANE_FACTORY(CJDKLevelMapsEditor, "JDK Level Maps", "Tools", true);