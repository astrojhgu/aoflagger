#include "heatmappagecontroller.h"

#include "../../quality/statisticscollection.h"
#include "../../quality/statisticsderivator.h"

#include "../grayscaleplotpage.h"

#include "../../structures/samplerow.h"

HeatMapPageController::HeatMapPageController() :
	_page(nullptr),
	_statisticKind(QualityTablesFormatter::StandardDeviationStatistic),
	_polarization(Polarization::StokesI),
	_phase(TimeFrequencyData::AmplitudePart)
{ }

void HeatMapPageController::updateImageImpl(QualityTablesFormatter::StatisticKind statisticKind, PolarizationEnum polarisation, enum TimeFrequencyData::ComplexRepresentation phase)
{
	std::pair<TimeFrequencyData, TimeFrequencyMetaDataCPtr> pair = constructImage(statisticKind);
	
	TimeFrequencyData &data = pair.first;
	
	if(!data.IsEmpty())
	{
		setToPolarization(data, polarisation);
		
		setToPhase(data, phase);
		
		Image2DCPtr image = data.GetSingleImage();
		if(_page != nullptr)
		{
			if(_page->NormalizeXAxis())
				image = normalizeXAxis(image);
			if(_page->NormalizeYAxis())
				image = normalizeYAxis(image);
		}
		
		_heatMap.SetZAxisDescription(StatisticsDerivator::GetDescWithUnits(statisticKind));
		_heatMap.SetImage(image);
		_heatMap.SetOriginalMask(data.GetSingleMask());
		if(pair.second != 0)
			_heatMap.SetMetaData(pair.second);
		
		if(_page != nullptr)
			_page->Redraw();
	}
}

Image2DCPtr HeatMapPageController::normalizeXAxis(Image2DCPtr input)
{
	Image2DPtr output = Image2D::CreateUnsetImagePtr(input->Width(), input->Height());
	for(size_t x=0;x<input->Width();++x)
	{
		SampleRowPtr row = SampleRow::CreateFromColumn(input.get(), x);
		num_t norm;
		if(_normalization == Mean)
			norm = 1.0 / row->MeanWithMissings();
		else if(_normalization == Winsorized)
			norm = 1.0 / row->WinsorizedMeanWithMissings();
		else // _medianNormButton
			norm = 1.0 / row->MedianWithMissings();
		for(size_t y=0;y<input->Height();++y)
			output->SetValue(x, y, input->Value(x, y) * norm);
	}
	return output;
}

Image2DCPtr HeatMapPageController::normalizeYAxis(Image2DCPtr input)
{
	Image2DPtr output = Image2D::CreateUnsetImagePtr(input->Width(), input->Height());
	for(size_t y=0;y<input->Height();++y)
	{
		SampleRowPtr row = SampleRow::CreateFromRow(input.get(), y);
		num_t norm;
		if(_normalization == Mean)
			norm = 1.0 / row->MeanWithMissings();
		else if(_normalization == Winsorized)
			norm = 1.0 / row->WinsorizedMeanWithMissings();
		else // _medianNormButton
			norm = 1.0 / row->MedianWithMissings();
		for(size_t x=0;x<input->Width();++x)
			output->SetValue(x, y, input->Value(x, y) * norm);
	}
	return output;
}

void HeatMapPageController::setToPolarization(TimeFrequencyData &data, PolarizationEnum polarisation)
{
	try {
		data = data.Make(polarisation);
		if(polarisation == Polarization::StokesI)
			data.MultiplyImages(0.5);
	} catch(std::exception& e)
	{
		// probably a conversion error -- polarisation was not available.
		// Best solution is probably to ignore.
	}
}

void HeatMapPageController::setToPhase(TimeFrequencyData &data, enum TimeFrequencyData::ComplexRepresentation phase)
{
	data = data.Make(phase);
}

