#ifndef PLOT_COLORSCALE_H
#define PLOT_COLORSCALE_H

#include <string>
#include <map>

#include "verticalplotscale.h"

#include <gtkmm/drawingarea.h>

class ColorScale {
	public:
		ColorScale();
		
		void SetPlotDimensions(double plotWidth, double plotHeight, double topMargin, bool textOnLeft)
		{
			_plotWidth = plotWidth;
			_plotHeight = plotHeight;
			_topMargin = topMargin;
			_width = 0.0;
			_textOnLeft = textOnLeft;
		}
		double GetWidth(const Cairo::RefPtr<Cairo::Context>& cairo)
		{
			if(_width == 0.0)
				initWidth(cairo);
			return _width;
		}
		void Draw(const Cairo::RefPtr<Cairo::Context>& cairo);
		void InitializeNumericTicks(double min, double max)
		{
			_width = 0.0;
			_min = min;
			_max = max;
			_isLogaritmic = false;
			_verticalPlotScale.InitializeNumericTicks(min, max);
		}
		void InitializeLogarithmicTicks(double min, double max)
		{
			_width = 0.0;
			_min = min;
			_max = max;
			_isLogaritmic = true;
			_verticalPlotScale.InitializeLogarithmicTicks(min, max);
		}
		void SetColorValue(double value, double red, double green, double blue)
		{
			ColorValue cValue;
			cValue.red = red;
			cValue.green = green;
			cValue.blue = blue;
			_colorValues.insert(std::pair<double, ColorValue>(value, cValue));
		}
		void SetDescriptionFontSize(double fontSize)
		{
			_verticalPlotScale.SetDescriptionFontSize(fontSize);
		}
		void SetTickValuesFontSize(double fontSize)
		{
			_verticalPlotScale.SetTickValuesFontSize(fontSize);
		}
		void SetDrawWithDescription(bool drawWithDescription)
		{
			_verticalPlotScale.SetDrawWithDescription(drawWithDescription);
		}
		void SetUnitsCaption(const std::string& caption)
		{
			_verticalPlotScale.SetUnitsCaption(caption);
		}
	private:
		static const double BAR_WIDTH;
		
		struct ColorValue
		{
			double red, green, blue;
		};
		
		void initWidth(const Cairo::RefPtr<Cairo::Context>& cairo);
		
		double _plotWidth, _plotHeight, _topMargin;
		double _scaleWidth, _width, _textHeight;
		bool _textOnLeft;
		double _min, _max;
		class VerticalPlotScale _verticalPlotScale;
		std::map<double, ColorValue> _colorValues;
		bool _isLogaritmic;
};

#endif
