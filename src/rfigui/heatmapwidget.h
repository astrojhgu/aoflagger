#ifndef HEAT_MAP_WIDGET_H
#define HEAT_MAP_WIDGET_H

#include <gtkmm/drawingarea.h>

#include <vector>

#include "../plot/heatmapplot.h"

class HeatMapWidget : public Gtk::DrawingArea {
public:
	HeatMapWidget(HeatMapPlot* plot);

	sigc::signal<void, size_t, size_t> &OnMouseMovedEvent() { return _onMouseMoved; }
	sigc::signal<void> &OnMouseLeaveEvent() { return _onMouseLeft; }
	sigc::signal<void, size_t, size_t> &OnButtonReleasedEvent() { return _onButtonReleased; }
	
	bool IsMouseInImage() const { return _mouseIsIn; }
	size_t MouseX() { return _mouseX; }
	size_t MouseY() { return _mouseY; }
	
	void Update() { update(true); }
	
	void SavePdf(const std::string &filename)
	{
		_plot->SavePdf(filename, get_width(), get_height());
	}
	void SaveSvg(const std::string &filename)
	{
		_plot->SaveSvg(filename, get_width(), get_height());
	}
	void SavePng(const std::string &filename)
	{
		_plot->SavePng(filename, get_width(), get_height());
	}
	HeatMapPlot& Plot() { return *_plot; }
	const HeatMapPlot& Plot() const { return *_plot; }
private:
	void update(bool invalidated);
	bool onDraw(const Cairo::RefPtr<Cairo::Context>& cr);
	bool onMotion(GdkEventMotion *event);
	bool onLeave(GdkEventCrossing *event);
	bool onButtonPress(GdkEventButton *event);
	bool onButtonRelease(GdkEventButton *event);

	bool _invalidated;
	bool _mouseIsIn;
	int _mouseX, _mouseY;
	bool _isButtonPressed;
	int _bpressStartX, _bpressStartY;

	sigc::signal<void, size_t, size_t> _onMouseMoved;
	sigc::signal<void> _onMouseLeft;
	sigc::signal<void, size_t, size_t> _onButtonReleased;
	HeatMapPlot* _plot;
};

#endif
