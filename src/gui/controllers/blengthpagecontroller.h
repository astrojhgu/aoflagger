#ifndef BLENGTH_PATH_CONTROLLER_H
#define BLENGTH_PATH_CONTROLLER_H

#include "aoqplotpagecontroller.h"

class BLengthPageController : public AOQPlotPageController
{
public:
protected:
	virtual void processStatistics(const StatisticsCollection *statCollection, const std::vector<AntennaInfo> &antennas) override final
	{
		_statisticsWithAutocorrelations.clear();
		_statisticsWithoutAutocorrelations.clear();
		
		const BaselineStatisticsMap &map = statCollection->BaselineStatistics();
		
		vector<std::pair<unsigned, unsigned> > baselines = map.BaselineList();
		for(vector<std::pair<unsigned, unsigned> >::const_iterator i=baselines.begin();i!=baselines.end();++i)
		{
			Baseline bline(antennas[i->first], antennas[i->second]);
			const DefaultStatistics &statistics = map.GetStatistics(i->first, i->second);
			_statisticsWithAutocorrelations.insert(std::pair<double, DefaultStatistics>(bline.Distance(), statistics));
			if(i->first != i->second)
				_statisticsWithoutAutocorrelations.insert(std::pair<double, DefaultStatistics>(bline.Distance(), statistics));
		}
	}
	
	virtual const std::map<double, class DefaultStatistics> &getStatistics() const override final
	{
		return _includeAutoCorrelationsButton.get_active() ? _statisticsWithAutocorrelations : _statisticsWithoutAutocorrelations;
	}
	
	virtual void startLine(Plot2D &plot, const std::string &name, const std::string &yAxisDesc) override final
	{
		plot.StartLine(name, "Baseline length (m)", yAxisDesc, false, Plot2DPointSet::DrawPoints);
	}
	
private:
	std::map<double, DefaultStatistics> _statisticsWithAutocorrelations;
	std::map<double, DefaultStatistics> _statisticsWithoutAutocorrelations;
};

#endif

