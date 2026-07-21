#include "StdAfx.h"
#include "MapPreview.h"

#include <QPainter>
#include <QWheelEvent>

JDKLevelMaps::Components::CMapPreview::CMapPreview(QWidget* pParent) : QLabel(pParent)
{
	setAlignment(Qt::AlignCenter);
	setMinimumSize(200, 200);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void JDKLevelMaps::Components::CMapPreview::SetPixmap(const QPixmap& pixmap)
{
	m_zoomFactor = 1.0f;
	m_offset = QPointF(0, 0);
	m_pixmap = pixmap;
	update();
}

void JDKLevelMaps::Components::CMapPreview::paintEvent(QPaintEvent* pEvent)
{
	if (m_pixmap.isNull())
	{
		QLabel::paintEvent(pEvent);
		return;
	}

	QPainter painter(this);
	painter.setRenderHint(QPainter::SmoothPixmapTransform, m_zoomFactor <= 1.0f);

	QSizeF targetSize = m_pixmap.size();
	targetSize.scale(size(), Qt::KeepAspectRatio);
	targetSize *= m_zoomFactor;

	QRectF targetRect(
		(width() - targetSize.width()) / 2.0f + m_offset.x(),
		(height() - targetSize.height()) / 2.0f + m_offset.y(),
		targetSize.width(),
		targetSize.height());

	painter.drawPixmap(targetRect, m_pixmap, m_pixmap.rect());
}

void JDKLevelMaps::Components::CMapPreview::mousePressEvent(QMouseEvent* pEvent)
{
	if (pEvent->button() == Qt::MiddleButton)
	{
		m_isDragging = true;
		m_lastMousePos = pEvent->pos();
		setCursor(Qt::ClosedHandCursor);
		pEvent->accept();
	}
	else
		QLabel::mousePressEvent(pEvent);
}

void JDKLevelMaps::Components::CMapPreview::mouseMoveEvent(QMouseEvent* pEvent)
{
	if (m_isDragging)
	{
		QPoint delta = pEvent->pos() - m_lastMousePos;
		m_offset += delta;
		m_lastMousePos = pEvent->pos();

		update();
		pEvent->accept();
	}
	else
		QLabel::mouseMoveEvent(pEvent);
}

void JDKLevelMaps::Components::CMapPreview::mouseReleaseEvent(QMouseEvent* pEvent)
{
	if (pEvent->button() == Qt::MiddleButton && m_isDragging)
	{
		m_isDragging = false;
		unsetCursor();
		pEvent->accept();
	}
	else
		QLabel::mouseReleaseEvent(pEvent);
}

void JDKLevelMaps::Components::CMapPreview::mouseDoubleClickEvent(QMouseEvent* pEvent)
{
	if (pEvent->button() == Qt::LeftButton)
	{
		m_zoomFactor = 1.0f;
		m_offset = { 0.0f, 0.0f };
		update();
	}
	else
		QLabel::mouseDoubleClickEvent(pEvent);
}

void JDKLevelMaps::Components::CMapPreview::wheelEvent(QWheelEvent* pEvent)
{
	if (pEvent->modifiers() & Qt::ControlModifier)
	{
		const float step = pEvent->angleDelta().y() > 0 ? 1.25f : 0.8f;
		m_zoomFactor = std::clamp(m_zoomFactor * step, 1.0f, 16.0f);

		update();
		pEvent->accept();
	}
	else
		QLabel::wheelEvent(pEvent);

}