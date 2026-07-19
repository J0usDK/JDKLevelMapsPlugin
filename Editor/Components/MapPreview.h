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
		void resizeEvent(QResizeEvent* pEvent) override;

	private:
		void UpdatePixmap();

	private:
		QPixmap m_pixmap;
	};
}