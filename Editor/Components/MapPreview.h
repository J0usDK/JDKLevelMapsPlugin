#pragma once
#include <QLabel>

namespace JDKLevelMaps::Components
{
	class CMapPreview : public QLabel
	{
	public:
		CMapPreview(QWidget* pParent = nullptr);

		void SetPixmap(const QPixmap& pixmap);

	protected:
		void paintEvent(QPaintEvent* oEvent) override;
		void mousePressEvent(QMouseEvent* pEvent) override;
		void mouseMoveEvent(QMouseEvent* pEvent) override;
		void mouseReleaseEvent(QMouseEvent* pEvent) override;
		void mouseDoubleClickEvent(QMouseEvent* pEvent) override;
		void wheelEvent(QWheelEvent* pEvent) override;

	private:
		QPixmap m_pixmap;
		float m_zoomFactor = 1.0f;

		QPointF m_offset;
		QPoint m_lastMousePos;
		bool m_isDragging = false;
	};
}