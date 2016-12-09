#include "timefrequencydata.h"
#include "stokesimager.h"

#include "../util/ffttools.h"

Image2DCPtr TimeFrequencyData::GetAbsoluteFromComplex(const Image2DCPtr &real, const Image2DCPtr &imag) const
{
	return Image2DPtr(FFTTools::CreateAbsoluteImage(*real, *imag));
}
			
Image2DCPtr TimeFrequencyData::GetSum(const Image2DCPtr &left, const Image2DCPtr &right) const
{
	return StokesImager::CreateSum(left, right);
}

Image2DCPtr TimeFrequencyData::GetNegatedSum(const Image2DCPtr &left, const Image2DCPtr &right) const
{
	return StokesImager::CreateNegatedSum(left, right);
}

Image2DCPtr TimeFrequencyData::GetDifference(const Image2DCPtr &left, const Image2DCPtr &right) const
{
	return StokesImager::CreateDifference(left, right);
}

Image2DCPtr TimeFrequencyData::getSinglePhaseFromTwoPolPhase(size_t polA, size_t polB) const
{
	return StokesImager::CreateAvgPhase(_data[polA]._images.first, _data[polB]._images.first);
}

Image2DCPtr TimeFrequencyData::GetZeroImage() const
{
	return Image2D::CreateZeroImagePtr(ImageWidth(), ImageHeight());
}

Mask2DCPtr TimeFrequencyData::GetCombinedMask() const
{
	if(MaskCount() == 0)
		return GetSetMask<false>();
	else if(MaskCount() == 1)
		return GetMask(0);
	else
	{
		Mask2DPtr mask = Mask2D::CreateCopy(GetMask(0));
		size_t i = 0;
		while(i!= MaskCount())
		{
			const Mask2DCPtr& curMask = GetMask(i);
			for(unsigned y=0;y<mask->Height();++y) {
				for(unsigned x=0;x<mask->Width();++x) {
					bool v = curMask->Value(x, y);
					if(v)
						mask->SetValue(x, y, true);
				}
			}
			++i;
		}
		return mask;
	}
}

TimeFrequencyData *TimeFrequencyData::CreateTFDataFromSingleComplex(enum PhaseRepresentation phase) const
{
	enum PolarisationType pol = _data[0]._polarisation;
	TimeFrequencyData *data;
	switch(phase)
	{
		case RealPart:
			data = new TimeFrequencyData(RealPart, pol, _data[0]._images.first);
			break;
		case ImaginaryPart:
			data = new TimeFrequencyData(ImaginaryPart, pol, _data[0]._images.second);
			break;
		case AmplitudePart:
			data = new TimeFrequencyData(AmplitudePart, pol, getAbsoluteFromComplex(0));
			break;
		case PhasePart:
			data = new TimeFrequencyData(PhasePart, pol, getPhaseFromComplex(0));
			break;
		default:
			throw BadUsageException("Creating TF data with non implemented phase parameters");
	}
	CopyFlaggingTo(data);
	return data;
}

TimeFrequencyData *TimeFrequencyData::CreateTFDataFromDipoleComplex(enum PhaseRepresentation phase) const
{
	TimeFrequencyData *data;
	switch(phase)
	{
		case RealPart:
			data = new TimeFrequencyData(RealPart,
				GetRealPartFromDipole(XXPolarisation),
				GetRealPartFromDipole(XYPolarisation),
				GetRealPartFromDipole(YXPolarisation),
				GetRealPartFromDipole(YYPolarisation));
			break;
		case ImaginaryPart:
			data = new TimeFrequencyData(ImaginaryPart,
				GetImaginaryPartFromDipole(XXPolarisation),
				GetImaginaryPartFromDipole(XYPolarisation),
				GetImaginaryPartFromDipole(YXPolarisation),
				GetImaginaryPartFromDipole(YYPolarisation));
			break;
		case AmplitudePart:
			{
				Image2DCPtr
					xx = GetAmplitudePartFromDipole(XXPolarisation),
					xy = GetAmplitudePartFromDipole(XYPolarisation),
					yx = GetAmplitudePartFromDipole(YXPolarisation),
					yy = GetAmplitudePartFromDipole(YYPolarisation);
				data = new TimeFrequencyData(AmplitudePart, xx, xy, yx, yy);
			}
			break;
		case PhasePart:
			{
				Image2DCPtr
					xx = GetPhasePartFromDipole(XXPolarisation),
					xy = GetPhasePartFromDipole(XYPolarisation),
					yx = GetPhasePartFromDipole(YXPolarisation),
					yy = GetPhasePartFromDipole(YYPolarisation);
				data = new TimeFrequencyData(PhasePart, xx, xy, yx, yy);
			}
			break;
		default:
			throw BadUsageException("Creating TF data with non implemented phase parameters (not real/imaginary/amplitude)");
	}
	CopyFlaggingTo(data);
	return data;
}

TimeFrequencyData *TimeFrequencyData::CreateTFDataFromAutoDipoleComplex(enum PhaseRepresentation phase) const
{
	TimeFrequencyData *data;
	switch(phase)
	{
		case RealPart:
			data = new TimeFrequencyData(RealPart,
				XXPolarisation, GetRealPartFromAutoDipole(XXPolarisation),
				YYPolarisation, GetRealPartFromAutoDipole(YYPolarisation));
			break;
		case ImaginaryPart:
			data = new TimeFrequencyData(ImaginaryPart,
				XXPolarisation, GetImaginaryPartFromAutoDipole(XXPolarisation),
				YYPolarisation, GetImaginaryPartFromAutoDipole(YYPolarisation));
			break;
		case AmplitudePart:
			{
				Image2DCPtr
					xx = GetAmplitudePartFromAutoDipole(XXPolarisation),
					yy = GetAmplitudePartFromAutoDipole(YYPolarisation);
				data = new TimeFrequencyData(AmplitudePart,
					XXPolarisation, xx, YYPolarisation, yy);
			}
			break;
		case PhasePart:
			{
				Image2DCPtr
					xx = GetPhasePartFromAutoDipole(XXPolarisation),
					yy = GetPhasePartFromAutoDipole(YYPolarisation);
				data = new TimeFrequencyData(PhasePart,
					XXPolarisation, xx, YYPolarisation, yy);
			}
			break;
		default:
			throw BadUsageException("Creating TF data with non implemented phase parameters (not real/imaginary/amplitude)");
	}
	CopyFlaggingTo(data);
	return data;
}

TimeFrequencyData *TimeFrequencyData::CreateTFDataFromCrossDipoleComplex(enum PhaseRepresentation phase) const
{
	TimeFrequencyData *data;
	switch(phase)
	{
		case RealPart:
			data = new TimeFrequencyData(RealPart,
				XYPolarisation, GetRealPartFromCrossDipole(XYPolarisation),
				YXPolarisation, GetRealPartFromCrossDipole(YXPolarisation));
			break;
		case ImaginaryPart:
			data = new TimeFrequencyData(ImaginaryPart,
				XYPolarisation, GetImaginaryPartFromCrossDipole(XYPolarisation),
				YXPolarisation, GetImaginaryPartFromCrossDipole(YXPolarisation));
			break;
		case AmplitudePart:
			{
				Image2DCPtr
					xy = GetAmplitudePartFromCrossDipole(XYPolarisation),
					yx = GetAmplitudePartFromCrossDipole(YXPolarisation);
				data = new TimeFrequencyData(AmplitudePart,
					XYPolarisation, xy, YXPolarisation, yx);
			}
			break;
		case PhasePart:
			{
				Image2DCPtr
					xy = GetPhasePartFromAutoDipole(XYPolarisation),
					yx = GetPhasePartFromAutoDipole(YXPolarisation);
				data = new TimeFrequencyData(PhasePart, 
					XYPolarisation, xy, YXPolarisation, yx);
			}
			break;
		default:
			throw BadUsageException("Creating TF data with non implemented phase parameters (not real/imaginary/amplitude)");
	}
	CopyFlaggingTo(data);
	return data;
}

TimeFrequencyData *TimeFrequencyData::CreateTFDataFromComplexCombination(const TimeFrequencyData &real, const TimeFrequencyData &imaginary)
{
	if(real.PhaseRepresentation() == imaginary.PhaseRepresentation())
		throw BadUsageException("Trying to create TF data from different complex represented data");
	if(real.Polarisations() != imaginary.Polarisations())
		throw BadUsageException("Combining real/imaginary time frequency data from different polarisations");
	TimeFrequencyData* data = new TimeFrequencyData();
	data->_data.resize(real._data.size());
	data->_phaseRepresentation = real._phaseRepresentation;
	for(size_t i=0; i!=real._data.size(); ++i)
	{
		data->_data[i]._polarisation = real._data[i]._polarisation;
		data->_data[i]._images.first = real._data[i]._images.first;
		data->_data[i]._images.second = imaginary._data[i]._images.first;
		data->_data[i]._flagging = real._data[i]._flagging;
	}
	return data;
}

TimeFrequencyData *TimeFrequencyData::CreateTFDataFromPolarizationCombination(const TimeFrequencyData &xx, const TimeFrequencyData &xy, const TimeFrequencyData &yx, const TimeFrequencyData &yy)
{
	if(xx.PhaseRepresentation() != xy.PhaseRepresentation() ||
		xx.PhaseRepresentation() != yx.PhaseRepresentation() ||
		xx.PhaseRepresentation() != yy.PhaseRepresentation())
		throw BadUsageException("Trying to create dipole time frequency combination from data with different phase representations!");

	TimeFrequencyData *data = new TimeFrequencyData();
	data->_data.resize(4);
	data->_phaseRepresentation = xx._phaseRepresentation;
	for(size_t i=0; i!=xx._data.size(); ++i)
	{
		data->_data[0] = xx._data[0];
		data->_data[1] = xy._data[0];
		data->_data[2] = yx._data[0];
		data->_data[3] = yy._data[0];
	}
	return data;
}

TimeFrequencyData *TimeFrequencyData::CreateTFDataFromPolarizationCombination(const TimeFrequencyData &xx, const TimeFrequencyData &yy)
{
	if(xx.PhaseRepresentation() != yy.PhaseRepresentation())
		throw BadUsageException("Trying to create auto dipole time frequency combination from data with different phase representations!");

	TimeFrequencyData *data = new TimeFrequencyData();
	data->_data.resize(2);
	data->_phaseRepresentation = xx._phaseRepresentation;
	for(size_t i=0; i!=xx._data.size(); ++i)
	{
		data->_data[0] = xx._data[0];
		data->_data[1] = yy._data[0];
	}
	return data;
}

void TimeFrequencyData::SetImagesToZero()
{
	if(!IsEmpty())
	{
		Image2DPtr zeroImage = Image2D::CreateZeroImagePtr(ImageWidth(), ImageHeight());
		Mask2DPtr mask = Mask2D::CreateSetMaskPtr<false>(ImageWidth(), ImageHeight());
		for(PolarizedTimeFrequencyData& data : _data)
		{
			data._images.first = zeroImage;
			if(data._images.second)
				data._images.second = zeroImage;
			data._flagging = mask;
		}
	}
}

void TimeFrequencyData::MultiplyImages(long double factor)
{
	for(PolarizedTimeFrequencyData& data : _data)
	{
		if(data._images.first)
		{
			Image2DPtr newImage = Image2D::CreateCopy(data._images.first);
			newImage->MultiplyValues(factor);
			data._images.first = newImage;
		}
		if(data._images.second)
		{
			Image2DPtr newImage = Image2D::CreateCopy(data._images.second);
			newImage->MultiplyValues(factor);
			data._images.second = newImage;
		}
	}
}

void TimeFrequencyData::JoinMask(const TimeFrequencyData &other)
{
	if(other.MaskCount() == 0)
	{
		// Nothing to be done; other has no flags
	} else if(other.MaskCount() == MaskCount())
	{
		for(size_t i=0;i<MaskCount();++i)
		{
			Mask2DPtr mask = Mask2D::CreateCopy(GetMask(i));
			mask->Join(other.GetMask(i));
			SetMask(i, mask);
		}
	} else if(other.MaskCount() == 1)
	{
		for(size_t i=0;i<MaskCount();++i)
		{
			Mask2DPtr mask = Mask2D::CreateCopy(GetMask(i));
			mask->Join(other.GetMask(0));
			SetMask(i, mask);
		}
	} else if(MaskCount() == 1)
	{
		Mask2DPtr mask = Mask2D::CreateCopy(GetMask(0));
		mask->Join(other.GetSingleMask());
		SetMask(0, mask);
	}	else if(MaskCount() == 0 && _data.size() == other._data.size())
	{
		for(size_t i=0; i!=_data.size(); ++i)
			_data[i]._flagging = other._data[i]._flagging;
	}
	else
		throw BadUsageException("Joining time frequency flagging with incompatible structures");
}

Image2DCPtr TimeFrequencyData::GetPhaseFromComplex(const Image2DCPtr &real, const Image2DCPtr &imag) const
{
	return FFTTools::CreatePhaseImage(real, imag);
}
