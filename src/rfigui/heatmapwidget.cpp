#include "heatmapwidget.h"

HeatMapWidget::HeatMapWidget(HeatMapPlot* plot) :
	_invalidated(true),
	_mouseIsIn(false),
	_mouseX(0), _mouseY(0),
	_isButtonPressed(false),
	_isZooming(false),
	_bpressStartX(0), _bpressStartY(0),
	_plot(plot)
{
	add_events(
		Gdk::POINTER_MOTION_MASK |
		Gdk::BUTTON_RELEASE_MASK |
		Gdk::BUTTON_PRESS_MASK |
		Gdk::LEAVE_NOTIFY_MASK);
	signal_motion_notify_event().connect(sigc::mem_fun(*this, &HeatMapWidget::onMotion));
	signal_leave_notify_event().connect(sigc::mem_fun(*this, &HeatMapWidget::onLeave));
	signal_button_press_event().connect(sigc::mem_fun(*this, &HeatMapWidget::onButtonPress));
	signal_button_release_event().connect(sigc::mem_fun(*this, &HeatMapWidget::onButtonRelease));
	signal_draw().connect(sigc::mem_fun(*this, &HeatMapWidget::onDraw) );
}

bool HeatMapWidget::onDraw(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(window && get_width() > 0 && get_height() > 0)
	{
		_plot->Draw(cr, get_width(), get_height(), _invalidated);
		if(_isZooming)
		{
			double x1, y1, x2, y2;
			_plot->ConvertToScreen(_bpressStartX, _bpressStartY, x1, y1);
			_plot->ConvertToScreen(_mouseX, _mouseY, x2, y2);
			cr->rectangle(x1, y1, x2-x1, y2-y1);
			cr->stroke();
		}
	}
	return true;
}

void HeatMapWidget::update(bool invalidated) {
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(window && get_width() > 0 && get_height() > 0)
	{
		_invalidated = _invalidated || invalidated;
		queue_draw();
	}
}

bool HeatMapWidget::onMotion(GdkEventMotion *event)
{
	if(_plot->HasImage())
	{
		int posX, posY;
		bool isInside = _plot->ConvertToUnits(event->x, event->y, posX, posY);
		if(_isButtonPressed)
		{
			_mouseX = posX;
			_mouseY = posY;
			update(false);
		}
		else {
			if(isInside)
			{
				_mouseX = posX;
				_mouseY = posY;
				_mouseIsIn = true;
				_onMouseMoved(posX, posY);
			} else if(_mouseIsIn) {
				_onMouseLeft();
				_mouseIsIn = false;
			}
		}
	}
	return true;
}

bool HeatMapWidget::onLeave(GdkEventCrossing *event)
{
	if(_mouseIsIn)
	{
		_onMouseLeft();
		_mouseIsIn = false;
	}
	return true;
}

bool HeatMapWidget::onButtonPress(GdkEventButton *event)
{
	_isButtonPressed = true;
	if(_plot->HasImage())
	{
		int posX, posY;
		if(_plot->ConvertToUnits(event->x, event->y, posX, posY))
		{
			_mouseX = posX;
			_mouseY = posY;
			_bpressStartX = posX;
			_bpressStartY = posY;
			_isZooming = true;
			_onButtonReleased(posX, posY);
		}
	}
	return true;
}

bool HeatMapWidget::onButtonRelease(GdkEventButton *event)
{
	_isButtonPressed = false;
	if(_plot->HasImage())
	{
		int posX, posY;
		if(_plot->ConvertToUnits(event->x, event->y, posX, posY))
		{
			_mouseX = posX;
			_mouseY = posY;
			_onButtonReleased(posX, posY);
		}
		if(_isZooming)
		{
			_isZooming = false;
			_plot->ZoomTo(_bpressStartX, _bpressStartY, _mouseX, _mouseY);
		}
		
		update(true);
	}
	return true;
}
