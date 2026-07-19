#include "StdAfx.h"
#include "MapPreview.h"

JDKLevelMaps::Components::CMapPreview::CMapPreview(QWidget* pParent) : QLabel(pParent)
{
	setAlignment(Qt::AlignCenter);
	setMinimumSize(200, 200);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void JDKLevelMaps::Components::CMapPreview::resizeEvent(QResizeEvent* pEvent)
{
	QLabel::resizeEvent(pEvent);
	UpdatePixmap();
}

void JDKLevelMaps::Components::CMapPreview::SetPixmap(const QPixmap& pixmap)
{
	m_pixmap = pixmap;
	UpdatePixmap();
}

void JDKLevelMaps::Components::CMapPreview::UpdatePixmap()
{
	if (m_pixmap.isNull())
		return;

	QLabel::setPixmap(m_pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}