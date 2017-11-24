#include "aoqplotpagecontroller.h"

#include "../twodimensionalplotpage.h"

#include "../../quality/statisticsderivator.h"

#include "../../util/logger.h"

AOQPlotPageController::AOQPlotPageController() :
	_page(nullptr),
	_statCollection(nullptr)
{ }

void AOQPlotPageController::UpdatePlot()
{
	if(_page != nullptr)
	{
		updatePlotForSettings(
			_page->GetSelectedKinds(),
			_page->GetSelectedPolarizations(),
			_page->GetSelectedPhases());
	}
}

void AOQPlotPageController::updatePlotForSettings(
	const std::set<QualityTablesFormatter::StatisticKind>& kinds,
	const std::set<std::pair<unsigned int, unsigned int> >& pols,
	const std::set<PhaseType>& phases)
{
	if(HasStatistics())
	{
		_plot.Clear();
		
		for(std::set<QualityTablesFormatter::StatisticKind>::const_iterator k=kinds.begin();
				k!=kinds.end(); ++k)
		{
			for(std::set<std::pair<unsigned,unsigned> >::const_iterator p=pols.begin();
					p!=pols.end(); ++p)
			{
				for(std::set<PhaseType>::const_iterator ph=phases.begin();
						ph!=phases.end(); ++ph)
				{
					plotStatistic(*k, p->first, p->second, *ph, getYDesc(kinds));
				}
			}
		}
		
		processPlot(_plot);
		
		if(_page != nullptr)
		{
			_page->Redraw();
		}
	}
}

double AOQPlotPageController::getValue(enum PhaseType phase, const std::complex<long double>& val)
{
	switch(phase)
	{
		default:
		case AmplitudePhaseType: return sqrt(val.real()*val.real() + val.imag()*val.imag());
		case PhasePhaseType: return atan2(val.imag(), val.real());
		case RealPhaseType: return val.real();
		case ImaginaryPhaseType: return val.imag();
	}
}

void AOQPlotPageController::plotStatistic(QualityTablesFormatter::StatisticKind kind, unsigned polA, unsigned polB, PhaseType phase, const std::string& yDesc)
{
	std::ostringstream s;
	StatisticsDerivator derivator(*_statCollection);
	const std::map<double, DefaultStatistics> &statistics = getStatistics();
	if(polA == polB)
	{
		s << "Polarization " << polA;
		startLine(_plot, s.str(), yDesc);
		for(std::map<double, DefaultStatistics>::const_iterator i=statistics.begin();i!=statistics.end();++i)
		{
			const double x = i->first;
			const std::complex<long double> val = derivator.GetComplexStatistic(kind, i->second, polA);
			_plot.PushDataPoint(x, getValue(phase, val));
		}
	}
	else {
		s << "Polarization " << polA << " and " << polB;
		startLine(_plot, s.str(), yDesc);
		for(std::map<double, DefaultStatistics>::const_iterator i=statistics.begin();i!=statistics.end();++i)
		{
			const double x = i->first;
			const std::complex<long double>
				valA = derivator.GetComplexStatistic(kind, i->second, polA),
				valB = derivator.GetComplexStatistic(kind, i->second, polB),
				val = valA*0.5l + valB*0.5l;
			_plot.PushDataPoint(x, getValue(phase, val));
		}
	}
}

std::string AOQPlotPageController::getYDesc(const std::set<QualityTablesFormatter::StatisticKind>& kinds) const
{
	if(kinds.size() != 1)
		return "Value";
	else
	{
		QualityTablesFormatter::StatisticKind kind = *kinds.begin();
		return StatisticsDerivator::GetDescWithUnits(kind);
	}
}

void AOQPlotPageController::SavePdf(const string& filename, QualityTablesFormatter::StatisticKind kind)
{
	std::set<QualityTablesFormatter::StatisticKind> kinds{kind};
	std::set<std::pair<unsigned int, unsigned int> > pols{std::make_pair(0, 3)};
	std::set<PhaseType> phases{AmplitudePhaseType};

	updatePlotForSettings(kinds, pols, phases);
	
	_plot.SavePdf(filename);
}

void AOQPlotPageController::SetStatistics(const StatisticsCollection* statCollection, const std::vector<class AntennaInfo>& antennas)
{
	processStatistics(statCollection, antennas);
	
	_statCollection = statCollection;
	UpdatePlot();
}
