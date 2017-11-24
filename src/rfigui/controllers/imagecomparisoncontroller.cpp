#include "imagecomparisoncontroller.h"

ImageComparisonController::ImageComparisonController() :
	_visualizedImage(TFOriginalImage),
	_showPP(true),
	_showPQ(false),
	_showQP(false),
	_showQQ(true)
{
}

TimeFrequencyData ImageComparisonController::GetActiveData() const
{
	TimeFrequencyData data(getActiveDataWithOriginalFlags());
	setActiveMask(data);
	if(_plot.StartHorizontal() != 0.0 ||
		_plot.EndHorizontal() != 1.0 ||
		_plot.StartVertical() != 0.0 ||
		_plot.EndVertical() != 1.0)
	{
		data.Trim(round(_plot.StartHorizontal() * data.ImageWidth()),
							round(_plot.StartVertical() * data.ImageHeight()),
							round(_plot.EndHorizontal() * data.ImageWidth()),
							round(_plot.EndVertical() * data.ImageHeight())); 
	}
	return data;
}

void ImageComparisonController::SetNewData(const TimeFrequencyData &data, TimeFrequencyMetaDataCPtr metaData)
{
	_plot.Clear();
	
	_original = data;
	_revised = _original;
	_revised.SetImagesToZero();
	_contaminated = _original;
	_contaminated.SetNoMask();
	updateVisualizedImage();
	
	_plot.SetOriginalMask(data.GetSingleMask());
	_plot.SetMetaData(metaData);
	_plot.ZoomFit();
}

void ImageComparisonController::updateVisualizedImage()
{
  Image2DCPtr image;
	const TimeFrequencyData* selectedData;
  switch(_visualizedImage)
	{
	default:
	case TFOriginalImage:
		selectedData = &_original;
		break;
	case TFRevisedImage:
		selectedData = &_revised;
		break;
	case TFContaminatedImage:
		selectedData = &_contaminated;
		break;
	}
	if(!selectedData->IsEmpty())
	{
		if(_showPP && _showQQ)
		{
			if((selectedData->HasPolarization(Polarization::XX) && selectedData->HasPolarization(Polarization::YY)) ||
				(selectedData->HasPolarization(Polarization::RR) && selectedData->HasPolarization(Polarization::LL)) ||
				(selectedData->HasPolarization(Polarization::StokesI)) )
			image = selectedData->Make(Polarization::StokesI).GetSingleImage();
		}
		else if(_showQP && _showPQ)
		{
			if(selectedData->HasPolarization(Polarization::XY) && selectedData->HasPolarization(Polarization::YX))
				image = selectedData->Make(Polarization::StokesU).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::RL) && selectedData->HasPolarization(Polarization::LR))
				image = selectedData->Make(Polarization::StokesQ).GetSingleImage();
		}
		else if(_showPP)
		{
			if(selectedData->HasPolarization(Polarization::XX))
				image = selectedData->Make(Polarization::XX).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::RR))
				image = selectedData->Make(Polarization::RR).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::StokesI))
				image = selectedData->Make(Polarization::StokesI).GetSingleImage();
		}
		else if(_showPQ)
		{
			if(selectedData->HasPolarization(Polarization::XY))
				image = selectedData->Make(Polarization::XY).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::RL))
				image = selectedData->Make(Polarization::RL).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::StokesQ))
				image = selectedData->Make(Polarization::StokesQ).GetSingleImage();
		}
		else if(_showQP)
		{
			if(selectedData->HasPolarization(Polarization::YX))
				image = selectedData->Make(Polarization::YX).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::LR))
				image = selectedData->Make(Polarization::LR).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::StokesU))
				image = selectedData->Make(Polarization::StokesU).GetSingleImage();
		}
		else if(_showQQ)
		{
			if(selectedData->HasPolarization(Polarization::YY))
				image = selectedData->Make(Polarization::YY).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::LL))
				image = selectedData->Make(Polarization::LL).GetSingleImage();
			else if(selectedData->HasPolarization(Polarization::StokesV))
				image = selectedData->Make(Polarization::StokesV).GetSingleImage();
		}
	}
	if(image == nullptr)
		_plot.Clear();
	else
		_plot.SetImage(image);
}

void ImageComparisonController::ClearBackground()
{
	_revised.SetImagesToZero();
}
