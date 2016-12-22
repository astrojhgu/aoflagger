#ifndef PLOT2DPOINTSET_H
#define PLOT2DPOINTSET_H

#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#include "../../structures/types.h"

class Plot2DPointSet{
	public:
		Plot2DPointSet() :
			_xIsTime(false),
			_rotateUnits(false),
			_drawingStyle(DrawLines)
		{ }
		~Plot2DPointSet() { }
		
		enum DrawingStyle { DrawLines, DrawPoints, DrawColumns };

		void SetLabel(const std::string &label) { _label = label; }
		const std::string &Label() const { return _label; }
		
		void SetXIsTime(const bool xIsTime) { _xIsTime = xIsTime; }
		bool XIsTime() const { return _xIsTime; }
		
		const std::string XUnits() const { return _xDesc; }
		const std::string YUnits() const { return _yDesc; }

		const std::string &XDesc() const { return _xDesc; }
		void SetXDesc(std::string xDesc) { _xDesc = xDesc; }

		const std::string &YDesc() const { return _yDesc; }
		void SetYDesc(std::string yDesc) { _yDesc = yDesc; }

		enum DrawingStyle DrawingStyle() const { return _drawingStyle; }
		void SetDrawingStyle(enum DrawingStyle drawingStyle) { _drawingStyle = drawingStyle; }

		void Clear()
		{
			_points.clear();
		}

		void PushDataPoint(double x, double y)
		{
			if(std::isfinite(x))
				_points.push_back(Point2D(x,y));
		}
		double GetX(size_t index) const { return _points[index].x; }
		double GetY(size_t index) const { return _points[index].y; }
		size_t Size() const { return _points.size(); }

		double MaxX() const
		{
			if(_points.empty())
				return std::numeric_limits<double>::quiet_NaN();
			double max = std::numeric_limits<double>::quiet_NaN();
			for(std::vector<Point2D>::const_iterator i = _points.begin();i!=_points.end();++i)
			{
				if((i->x > max || (!std::isfinite(max))) && std::isfinite(i->x) ) max = i->x;
			}
			return max;
		}
		double MinX() const
		{
			if(_points.empty())
				return std::numeric_limits<double>::quiet_NaN();
			double min = std::numeric_limits<double>::quiet_NaN();
			for(std::vector<Point2D>::const_iterator i = _points.begin();i!=_points.end();++i)
			{
				if((i->x < min || (!std::isfinite(min))) && std::isfinite(i->x) ) min = i->x;
			}
			return min;
		}
		double MaxY() const
		{
			if(_points.empty())
				return std::numeric_limits<double>::quiet_NaN();
			double max = std::numeric_limits<double>::quiet_NaN();
			for(std::vector<Point2D>::const_iterator i = _points.begin();i!=_points.end();++i)
			{
				if((i->y > max || (!std::isfinite(max))) && std::isfinite(i->y) ) max = i->y;
			}
			return max;
		}
		double MaxPositiveY() const
		{
			double max = 0.0;
			for(std::vector<Point2D>::const_iterator i = _points.begin();i!=_points.end();++i)
			{
				if((i->y > max) && std::isfinite(i->y)) max = i->y;
			}
			if(max == 0.0)
				return std::numeric_limits<double>::quiet_NaN();
			else
				return max;
		}
		double MinY() const
		{
			if(_points.empty())
				return std::numeric_limits<double>::quiet_NaN();
			double min = std::numeric_limits<double>::quiet_NaN();
			for(std::vector<Point2D>::const_iterator i = _points.begin();i!=_points.end();++i)
			{
				if((i->y < min || (!std::isfinite(min))) && std::isfinite(i->y) ) min = i->y;
			}
			return min;
		}
		double MinPositiveY() const
		{
			std::vector<Point2D>::const_iterator i;
			double min = 0.0;
			// Find first positive element
			for(i = _points.begin();i!=_points.end();++i)
			{
				if((i->y > 0.0) && std::isfinite(i->y))
				{
					min = i->y;
					break;
				}
			}
			if(min == 0.0) return std::numeric_limits<double>::quiet_NaN();
			for(;i!=_points.end();++i)
			{
				if((i->y > 0.0) && (i->y < min) && std::isfinite(i->y)) min = i->y;
			}
			return min;
		}
		void Sort()
		{
			std::sort(_points.begin(), _points.end());
		}
		double XRangeMin() const
		{
			if(_points.empty())
				return 0.0;
			else
				return _points.begin()->x;
		}
		double XRangePositiveMin() const
		{
			if(_points.empty())
				return 1.0;
			else {
				std::vector<Point2D>::const_iterator iter = std::upper_bound(_points.begin(), _points.end(), Point2D(0.0,0.0));
				if(iter == _points.end() || iter->x < 0.0 || !std::isfinite(iter->x))
					return 1.0;
				else
					return iter->x;
			}
		}
		double XRangeMax() const
		{
			if(_points.empty())
				return 1.0;
			else
				return _points.rbegin()->x;
		}
		double XRangePositiveMax() const
		{
			if(_points.empty())
				return 1.0;
			else if(_points.rbegin()->x < 0.0 || !std::isfinite(_points.rbegin()->x))
				return 1.0;
			else
				return _points.rbegin()->x;
		}
		double YRangeMin() const
		{
			return MinY();
		}
		double YRangePositiveMin() const
		{
			return MinPositiveY();
		}
		double YRangeMax() const
		{
			return MaxY();
		}
		double YRangePositiveMax() const
		{
			return MaxPositiveY();
		}
		void SetTickLabels(const std::vector<std::string> &tickLabels)
		{
			_tickLabels = tickLabels;
		}
		bool HasTickLabels() const
		{
			return !_tickLabels.empty();
		}
		const std::vector<std::string> &TickLabels() const
		{
			return _tickLabels;
		}
		void SetRotateUnits(bool rotateUnits)
		{
			_rotateUnits = rotateUnits;
		}
		bool RotateUnits() const
		{
			return _rotateUnits;
		}
		/**
		 * Set the range that this point set minimally wants to have visualized. Other point sets might
		 * request a larger range, which might enlarge this request.
		 */
		void SetYRange(double yMin, double yMax)
		{
		}
	private:
		struct Point2D
		{
			Point2D(double _x, double _y) : x(_x), y(_y) { }
			double x, y;
			bool operator<(const Point2D &other) const
			{
				if(!std::isfinite(x))
					return false;
				else if(!std::isfinite(other.x))
					return true;
				else
					return x < other.x;
			}
		};

		std::vector<Point2D> _points;
		std::string _label;
		std::string _xDesc;
		std::string _yDesc;
		bool _xIsTime;
		std::vector<std::string> _tickLabels;
		bool _rotateUnits;
		enum DrawingStyle _drawingStyle;
};

#endif
