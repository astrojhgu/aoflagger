#ifndef HEAT_MAP_WIDGET_H
#define HEAT_MAP_WIDGET_H

#include <gtkmm/drawingarea.h>

#include <vector>

#include "plot/heatmapplot.h"

class HeatMapWidget : public Gtk::DrawingArea, public HeatMapPlot {
public:
	HeatMapWidget();

	sigc::signal<void, size_t, size_t> &OnMouseMovedEvent() { return _onMouseMoved; }
	sigc::signal<void> &OnMouseLeaveEvent() { return _onMouseLeft; }
	sigc::signal<void, size_t, size_t> &OnButtonReleasedEvent() { return _onButtonReleased; }
	
	bool IsMouseInImage() const { return _mouseIsIn; }
	size_t MouseX() { return _mouseX; }
	size_t MouseY() { return _mouseY; }
	
	void Update();
	
	void SavePdf(const std::string &filename)
	{
		HeatMapPlot::SavePdf(filename, get_width(), get_height());
	}
	void SaveSvg(const std::string &filename)
	{
		HeatMapPlot::SaveSvg(filename, get_width(), get_height());
	}
	void SavePng(const std::string &filename)
	{
		HeatMapPlot::SavePng(filename, get_width(), get_height());
	}
private:
	bool onDraw(const Cairo::RefPtr<Cairo::Context>& cr);
	bool onMotion(GdkEventMotion *event);
	bool onLeave(GdkEventCrossing *event);
	bool onButtonReleased(GdkEventButton *event);

	bool _mouseIsIn;
	size_t _mouseX, _mouseY;

	sigc::signal<void, size_t, size_t> _onMouseMoved;
	sigc::signal<void> _onMouseLeft;
	sigc::signal<void, size_t, size_t> _onButtonReleased;
};

#endif
