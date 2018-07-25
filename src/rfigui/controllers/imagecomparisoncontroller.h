#ifndef IMAGECOMPARISONCONTROLLER_H
#define IMAGECOMPARISONCONTROLLER_H

#include <gtkmm/drawingarea.h>

#include <vector>

#include "../../structures/image2d.h"
#include "../../structures/timefrequencydata.h"
#include "../../structures/timefrequencymetadata.h"
#include "../../structures/segmentedimage.h"

#include "../../plot/heatmapplot.h"

class ImageComparisonController {
	public:
		ImageComparisonController();
		void SetNewData(const class TimeFrequencyData &image, TimeFrequencyMetaDataCPtr metaData);

		TimeFrequencyData GetActiveData() const;

		TimeFrequencyData &OriginalData() { return _dataList.front().data; }
		const TimeFrequencyData &OriginalData() const { return _dataList.front().data; }

		size_t AddVisualization(const std::string& label, const TimeFrequencyData& data)
		{
			_dataList.emplace_back(label, data);
			size_t v = _dataList.size() - 1;
			_visualizationListChange.emit();
			return v;
		}
		
		TimeFrequencyData& VisualizedData() { return _dataList[_visualizedImage].data; }
		const TimeFrequencyData& VisualizedData() const { return _dataList[_visualizedImage].data; }
		size_t VisualizedIndex() const { return _visualizedImage; }
		const TimeFrequencyData& GetVisualizedData(size_t index) const { return _dataList[index].data; }
		const std::string& GetVisualizedLabel(size_t index) { return _dataList[index].label; }
		size_t VisualizationCount() const { return _dataList.size(); }
		
		void SetVisualization(size_t visualizationIndex)
		{
			if(_visualizedImage != visualizationIndex && _visualizedImage < _dataList.size())
			{
				_visualizedImage = visualizationIndex;
				if(_visualizedImage == 0)
					_plot.SetAlternativeMask(_dataList.back().data.GetSingleMask());
				else
					_plot.SetAlternativeMask(_dataList[_visualizedImage].data.GetSingleMask());
				updateVisualizedImage();
			}
		}
		void TryVisualizePolarizations(bool& pp, bool& pq, bool& qp, bool& qq) const;
		void SetVisualizedPolarization(bool pp, bool pq, bool qp, bool qq)
		{
			if(_showPP != pp || _showPQ != pq || _showQP != qp || _showQQ != qq)
			{
				_showPP = pp;
				_showPQ = pq;
				_showQP = qp;
				_showQQ = qq;
				updateVisualizedImage();
			}
		}
		
		void ClearAllButOriginal();
		
		HeatMapPlot& Plot() { return _plot; }
		const HeatMapPlot& Plot() const { return _plot; }
		
		sigc::signal<void> &VisualizationListChange()
		{ return _visualizationListChange; }
	private:
		void getFirstAvailablePolarization(bool& pp, bool& pq, bool& qp, bool& qq) const;
		void updateVisualizedImage();
		void getActiveMask(TimeFrequencyData& data) const
		{
			bool orActive = _plot.ShowOriginalMask() && _dataList[0].data.MaskCount()!=0;
			size_t altMask = _visualizedImage;
			if(altMask == 0)
				altMask = _dataList.size()-1;
			bool altActive = _plot.ShowAlternativeMask() && _visualizedImage!=0 && _dataList[_visualizedImage].data.MaskCount()!=0;
			if(orActive && altActive)
			{
				data.SetMask(_dataList[0].data);
				data.JoinMask(_dataList[_visualizedImage].data);
			}
			else if(orActive)
				data.SetMask(_dataList[0].data);
			else if(altActive)
				data.SetMask(_dataList[_visualizedImage].data);
			else
				data.SetMasksToValue<false>();
		}
		HeatMapPlot _plot;
		bool _showPP, _showPQ, _showQP, _showQQ;
		size_t _visualizedImage;
		struct DataEntry
		{
			DataEntry(const std::string& _label, const TimeFrequencyData& _data)
			{ label = _label; data = _data; }
			std::string label;
			TimeFrequencyData data;
		};
		std::vector<DataEntry> _dataList;
		TimeFrequencyMetaDataCPtr _metaData;
		sigc::signal<void> _visualizationListChange;
};

#endif
