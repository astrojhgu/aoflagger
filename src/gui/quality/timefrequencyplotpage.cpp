#include <limits>

#include "timefrequencyplotpage.h"

#include "../../quality/statisticscollection.h"
#include "../../quality/statisticsderivator.h"

TimeFrequencyPlotPage::TimeFrequencyPlotPage() :
	_statCollection(0)
{
	grayScaleWidget().OnMouseMovedEvent().connect(sigc::mem_fun(*this, &TimeFrequencyPlotPage::onMouseMoved));
	grayScaleWidget().Plot().SetXAxisDescription("Time index");
	grayScaleWidget().Plot().SetYAxisDescription("Frequency index");
}

TimeFrequencyPlotPage::~TimeFrequencyPlotPage()
{
}

std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> TimeFrequencyPlotPage::constructImage(QualityTablesFormatter::StatisticKind kind)
{
	if(HasStatistics())
	{
		StatisticsDerivator derivator(*_statCollection);
		
		std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> data = derivator.CreateTFData(kind);
		if(data.second == 0)
		{
			grayScaleWidget().Plot().SetXAxisDescription("Time index");
			grayScaleWidget().Plot().SetYAxisDescription("Frequency index");
		} else {
			grayScaleWidget().Plot().SetXAxisDescription("Time");
			grayScaleWidget().Plot().SetYAxisDescription("Frequency (MHz)");
		}
		return data;
	} else {
		return std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr>(TimeFrequencyData(), TimeFrequencyMetaDataCPtr());
	}
}

void TimeFrequencyPlotPage::onMouseMoved(size_t x, size_t y)
{
	std::stringstream text;

	const QualityTablesFormatter::StatisticKind kind = getSelectedStatisticKind();
	const std::string &kindName = QualityTablesFormatter::KindToName(kind);
	
	text << kindName << " = " << grayScaleWidget().Plot().Image()->Value(x, y) << " (" << x << ", " << y << ")";
	_signalStatusChange(text.str());
}
