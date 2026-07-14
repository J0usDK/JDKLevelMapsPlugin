#include "StdAfx.h"
#include "JDKLevelMapsEditor.h"

#include <QPushButton>
#include <QVBoxLayout>

CJDKLevelMapsEditor::CJDKLevelMapsEditor(QWidget* pParent) : CDockableEditor(pParent)
{
	m_pRootWidget = new QWidget();

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
	CryLogAlways("[JDKLevelMaps] Generate button clicked");
}

const char* CJDKLevelMapsEditor::GetEditorName() const { return "JDK Level Maps"; }

REGISTER_VIEWPANE_FACTORY(CJDKLevelMapsEditor, "JDK Level Maps", "Tools", true);