#ifndef TIMEFREQUENCYDATA_H
#define TIMEFREQUENCYDATA_H

#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <stdexcept>

#include "image2d.h"
#include "mask2d.h"
#include "types.h"

#include "../baseexception.h"

class TimeFrequencyData
{
	public:
		enum PhaseRepresentation { PhasePart, AmplitudePart, RealPart, ImaginaryPart, ComplexRepresentation };

		TimeFrequencyData() :
			_phaseRepresentation(AmplitudePart),
			_data()
		{ }

		TimeFrequencyData(PhaseRepresentation phaseRepresentation,
				PolarisationType polarisationType, const Image2DCPtr& image) :
			_phaseRepresentation(phaseRepresentation)
		{
			if(phaseRepresentation == ComplexRepresentation)
				throw BadUsageException("Incorrect construction of time/frequency data: trying to create complex representation from single image");
			_data.emplace_back(polarisationType, image);
		}

		TimeFrequencyData(PolarisationType polarisationType,
											const Image2DCPtr &real,
											const Image2DCPtr &imaginary) :
				_phaseRepresentation(ComplexRepresentation)
		{
			_data.emplace_back(polarisationType, real, imaginary);
		}
		
		TimeFrequencyData(PhaseRepresentation phase,
											PolarisationType polarisationA,
											const Image2DCPtr &imageA,
											PolarisationType polarisationB,
											const Image2DCPtr &imageB) :
				_phaseRepresentation(phase)
		{
			_data.reserve(2);
			_data.emplace_back(polarisationA, imageA);
			_data.emplace_back(polarisationB, imageB);
		}
		
		TimeFrequencyData(PolarisationType polarisationA,
											const Image2DCPtr &realA,
											const Image2DCPtr &imaginaryA,
											PolarisationType polarisationB,
											const Image2DCPtr &realB,
											const Image2DCPtr &imaginaryB) :
				_phaseRepresentation(ComplexRepresentation)
		{
			_data.reserve(2);
			_data.emplace_back(polarisationA, realA, imaginaryA);
			_data.emplace_back(polarisationB, realB, imaginaryB);
		}
		
		TimeFrequencyData(PhaseRepresentation phaseRepresentation,
											const Image2DCPtr& xx,
											const Image2DCPtr& xy,
											const Image2DCPtr& yx,
											const Image2DCPtr& yy) :
				_phaseRepresentation(phaseRepresentation)
		{
			if(phaseRepresentation == ComplexRepresentation)
				throw BadUsageException("Incorrect construction of time/frequency data: trying to create complex full-Stokes representation from four images");
			_data.reserve(4);
			_data.emplace_back(XXPolarisation, xx);
			_data.emplace_back(XYPolarisation, xy);
			_data.emplace_back(YXPolarisation, yx);
			_data.emplace_back(YYPolarisation, yy);
		}

		TimeFrequencyData(const Image2DCPtr &xxReal,
											const Image2DCPtr &xxImag,
											const Image2DCPtr &xyReal,
											const Image2DCPtr &xyImag,
											const Image2DCPtr &yxReal,
											const Image2DCPtr &yxImag,
											const Image2DCPtr &yyReal,
											const Image2DCPtr &yyImag) :
				_phaseRepresentation(ComplexRepresentation)
		{
			_data.reserve(4);
			_data.emplace_back(XXPolarisation, xxReal, xxImag);
			_data.emplace_back(XYPolarisation, xyReal, xyImag);
			_data.emplace_back(YXPolarisation, yxReal, yxImag);
			_data.emplace_back(YYPolarisation, yyReal, yyImag);
		}

		static TimeFrequencyData CreateComplexTFData(size_t polarisationCount, Image2DCPtr *realImages, Image2DCPtr *imagImages)
		{
			switch(polarisationCount)
			{
				case 1:
					return TimeFrequencyData(StokesIPolarisation, realImages[0], imagImages[0]);
				case 2:
					return TimeFrequencyData(XXPolarisation, realImages[0], imagImages[0],
																	 YYPolarisation, realImages[1], imagImages[1]);
				case 4:
					return TimeFrequencyData(realImages[0], imagImages[0], realImages[1], imagImages[1], realImages[2], imagImages[2], realImages[3], imagImages[3]);
				default:
					throw BadUsageException("Can not create TimeFrequencyData structure with polarization type other than 1, 2 or 4 polarizations.");
			}
		}
		
		bool IsEmpty() const { return _data.empty(); }

		bool HasPolarisation(PolarisationType polarisation) const
		{
			for(const PolarizedTimeFrequencyData& data : _data)
				if(data._polarisation == polarisation)
					return true;
			return false;
		}
		
		enum PolarisationType GetPolarisation(size_t index) const
		{ return _data[index]._polarisation; }
		
		bool HasXX() const { return HasPolarisation(XXPolarisation); }

		bool HasXY() const { return HasPolarisation(XYPolarisation); }

		bool HasYX() const { return HasPolarisation(YXPolarisation); }

		bool HasYY() const { return HasPolarisation(YYPolarisation); }
		
		/**
		 * This function returns a new Image2D that contains
		 * an image that can be used best for thresholding-like
		 * RFI methods, or visualization. The encapsulated data
		 * may be converted in order to do so.
		 * @return A new image containing the TF-data.
		 */
		Image2DCPtr GetSingleImage() const
		{
			switch(_phaseRepresentation)
			{
				case PhasePart:
				case AmplitudePart:
				case RealPart:
				case ImaginaryPart:
					return GetSingleImageFromSinglePhaseImage();
				case ComplexRepresentation:
					return GetSingleAbsoluteFromComplex();
			}
			throw BadUsageException("Incorrect phase representation");
		}

		Mask2DCPtr GetSingleMask() const
		{
			return GetCombinedMask();
		}

		std::pair<Image2DCPtr,Image2DCPtr> GetSingleComplexImage() const
		{
			if(_phaseRepresentation != ComplexRepresentation)
				throw BadUsageException("Trying to create single complex image, but no complex data available");
			if(_data.size() != 1)
				throw BadUsageException("Not implemented");
			if(_data[0]._images.first == nullptr || _data[0]._images.second == nullptr)
				throw BadUsageException("Requesting non-existing image");
			return _data[0]._images;
		}
		
		void Set(PolarisationType polarisationType,
			const Image2DCPtr &real,
			const Image2DCPtr &imaginary)
		{
			_phaseRepresentation = ComplexRepresentation;
			_data.clear();
			_data.emplace_back(polarisationType, real, imaginary);
		}

		void SetNoMask()
		{
			for(PolarizedTimeFrequencyData& data : _data)
				data._flagging = nullptr;
		}

		void SetGlobalMask(const Mask2DCPtr &mask)
		{
			SetNoMask();
			for(PolarizedTimeFrequencyData& data : _data)
				data._flagging = mask;
		}

		Mask2DCPtr GetMask(enum PolarisationType polarisation) const
		{
			for(const PolarizedTimeFrequencyData& data : _data)
			{
				if(data._polarisation == polarisation)
				{
					if(data._flagging == nullptr)
						return GetSetMask<false>();
					else
						return data._flagging;
				}
			}
			return GetSingleMask();
		}

		void SetIndividualPolarisationMasks(const Mask2DCPtr &maskA, const Mask2DCPtr &maskB)
		{
			if(_data.size() != 2)
				throw BadUsageException("Trying to set two individual mask in non-matching time frequency data");
			_data[0]._flagging = maskA;
			_data[1]._flagging = maskB;
		}

		void SetIndividualPolarisationMasks(const Mask2DCPtr &maskA, const Mask2DCPtr &maskB, const Mask2DCPtr &maskC, const Mask2DCPtr &maskD)
		{
			if(_data.size() != 4)
				throw BadUsageException("Trying to set four individual mask in non-matching time frequency data");
			_data[0]._flagging = maskA;
			_data[1]._flagging = maskB;
			_data[2]._flagging = maskC;
			_data[3]._flagging = maskD;
		}

		TimeFrequencyData *CreateTFData(PhaseRepresentation phase) const;

		TimeFrequencyData *CreateTFData(PolarisationType polarisation) const
		{
			TimeFrequencyData *data = 0;
			for(const PolarizedTimeFrequencyData& data : _data)
			{
				if(data._polarisation == polarisation)
					return new TimeFrequencyData(_phaseRepresentation, data);
			}
			size_t
				xxPol = getPolarisationIndex(XXPolarisation),
				xyPol = getPolarisationIndex(XYPolarisation),
				yxPol = getPolarisationIndex(YXPolarisation),
				yyPol = getPolarisationIndex(YYPolarisation);
			bool hasLinear = xxPol < _data.size() || xyPol < _data.size();
			if(_phaseRepresentation == ComplexRepresentation && hasLinear )
			{
				switch(polarisation)
				{
				case StokesIPolarisation:
					data = new TimeFrequencyData(StokesIPolarisation, getFirstSum(xxPol, yyPol), getSecondSum(xxPol, yyPol));
					break;
				case StokesQPolarisation:
					data = new TimeFrequencyData(StokesQPolarisation, getFirstDiff(xxPol, yyPol), getSecondDiff(xxPol, yyPol));
					break;
				case StokesUPolarisation:
					data = new TimeFrequencyData(StokesUPolarisation, getFirstSum(xyPol, yxPol), getSecondSum(xyPol, yxPol));
					break;
				case StokesVPolarisation:
					data = new TimeFrequencyData(StokesVPolarisation, getStokesVReal(xyPol, yxPol), getStokesVImag(xyPol, yxPol));
					break;
				default:
					throw BadUsageException("Polarisation not available or not implemented");
				}
				data->SetGlobalMask(GetMask(polarisation));
			}
			else if(_phaseRepresentation != ComplexRepresentation && hasLinear) {
				switch(polarisation)
				{
					case StokesIPolarisation:
						data = new TimeFrequencyData(_phaseRepresentation, StokesIPolarisation, getFirstSum(xxPol, yyPol));
						break;
					case StokesQPolarisation:
						data = new TimeFrequencyData(_phaseRepresentation, StokesQPolarisation, getFirstDiff(xxPol, yyPol));
						break;
					default:
						throw BadUsageException("Requested polarisation type not available in time frequency data");
				}
				data->SetGlobalMask(GetMask(polarisation));
			}
			else {
				throw BadUsageException("Trying to convert the polarization in time frequency data in an invalid way");
			}
			return data;
		}

		Image2DCPtr GetRealPart() const
		{
			if(_data.size() != 1)
			{
				throw BadUsageException("This tfdata contains !=1 polarizations; which real part should I return?");
			} else if(_phaseRepresentation == ComplexRepresentation || _phaseRepresentation == RealPart) {
				return _data[0]._images.first;
			} else {
				throw BadUsageException("Trying to retrieve real part from time frequency data in which phase doesn't have a complex or real representation");
			}
		}

		Image2DCPtr GetImaginaryPart() const
		{
			if(_data.size() != 1)
			{
				throw BadUsageException("This tfdata contains !=1 polarizations; which imaginary part should I return?");
			} else if(_phaseRepresentation == ComplexRepresentation) {
				return _data[0]._images.second;
			} else if(_phaseRepresentation == ImaginaryPart) {
				return _data[0]._images.first;
			} else {
				throw BadUsageException("Trying to retrieve imaginary part from time frequency data in which phase doesn't have a complex or real representation");
			}
		}

		size_t ImageWidth() const
		{
			if(!_data.empty() && _data[0]._images.first != nullptr)
				return _data[0]._images.first->Width();
			else
				return 0;
		}
		
		size_t ImageHeight() const
		{
			if(!_data.empty() && _data[0]._images.first != nullptr)
				return _data[0]._images.first->Height();
			else
				return 0;
		}

		enum PhaseRepresentation PhaseRepresentation() const
		{
			return _phaseRepresentation;
		}
		
		std::vector<PolarisationType> Polarisations() const
		{
			std::vector<PolarisationType> pols;
			for(const PolarizedTimeFrequencyData& data : _data)
				pols.push_back(data._polarisation);
			return pols;
		}

		void Subtract(const TimeFrequencyData &rhs)
		{
			if(rhs._data.size() != _data.size() || rhs._phaseRepresentation != _phaseRepresentation)
			{
				std::stringstream s;
				s << "Can not subtract time-frequency data: they do not have the same number of polarisations or phase representation! ("
					<< rhs._data.size() << " vs. " << _data.size() << ")";
				throw BadUsageException(s.str());
			}
			for(size_t i=0; i!=_data.size(); ++i)
			{
				if(_data[i]._images.first == nullptr)
					throw BadUsageException("Can't subtract TFs with unset image data");
				_data[i]._images.first = Image2D::CreateFromDiff(_data[i]._images.first, rhs._data[i]._images.first);
				if(_data[i]._images.second)
					_data[i]._images.second = Image2D::CreateFromDiff(_data[i]._images.second, rhs._data[i]._images.second);
			}
		}

		void SubtractAsRHS(const TimeFrequencyData &lhs)
		{
			if(lhs._data.size() != _data.size() || lhs._phaseRepresentation != _phaseRepresentation)
			{
				std::stringstream s;
				s << "Can not subtract time-frequency data: they do not have the same number of polarisations or phase representation! ("
					<< lhs._data.size() << " vs. " << _data.size() << ")";
				throw BadUsageException(s.str());
			}
			for(size_t i=0; i!=_data.size(); ++i)
			{
				if(_data[i]._images.first == nullptr)
					throw BadUsageException("Can't subtract TFs with unset image data");
				_data[i]._images.first = Image2D::CreateFromDiff(lhs._data[i]._images.first, _data[i]._images.first);
				if(_data[i]._images.second)
					_data[i]._images.second = Image2D::CreateFromDiff(lhs._data[i]._images.second, _data[i]._images.second);
			}
		}

		static TimeFrequencyData *CreateTFDataFromDiff(const TimeFrequencyData &lhs, const TimeFrequencyData &rhs)
		{
			if(lhs._data.size() != rhs._data.size() || lhs._phaseRepresentation != rhs._phaseRepresentation)
			{
				std::stringstream s;
				s << "Can not subtract time-frequency data: they do not have the same number of polarisations or phase representation! ("
					<< lhs._data.size() << " vs. " << rhs._data.size() << ")";
				throw BadUsageException(s.str());
			}
			TimeFrequencyData *data = new TimeFrequencyData(lhs);
			for(size_t i=0;i<lhs._data.size();++i)
			{
				if(lhs._data[i]._images.first == nullptr)
					throw BadUsageException("Can't subtract TFs with unset image data");
				data->_data[i]._images.first = Image2D::CreateFromDiff(lhs._data[i]._images.first, rhs._data[i]._images.first);
				if(lhs._data[i]._images.second)
					data->_data[i]._images.second = Image2D::CreateFromDiff(lhs._data[i]._images.second, rhs._data[i]._images.second);
			}
			return data;
		}

		static TimeFrequencyData *CreateTFDataFromSum(const TimeFrequencyData &lhs, const TimeFrequencyData &rhs)
		{
			if(lhs._data.size() != rhs._data.size() || lhs._phaseRepresentation != rhs._phaseRepresentation)
			{
				std::stringstream s;
				s << "Can not add time-frequency data: they do not have the same number of polarisations or phase representation! ("
					<< lhs._data.size() << " vs. " << rhs._data.size() << ")";
				throw BadUsageException(s.str());
			}
			TimeFrequencyData *data = new TimeFrequencyData(lhs);
			for(size_t i=0;i<lhs._data.size();++i)
			{
				if(lhs._data[i]._images.first == nullptr)
					throw BadUsageException("Can't add TFs with unset image data");
				data->_data[i]._images.first = Image2D::CreateFromSum(lhs._data[i]._images.first, rhs._data[i]._images.first);
				if(lhs._data[i]._images.second)
					data->_data[i]._images.second = Image2D::CreateFromSum(lhs._data[i]._images.second, rhs._data[i]._images.second);
			}
			return data;
		}

		size_t ImageCount() const {
			size_t masks = 0;
			for(const PolarizedTimeFrequencyData& data : _data)
			{
				if(data._images.first) ++masks;
				if(data._images.second) ++masks;
			}
			return masks; 
		}
		
		size_t MaskCount() const {
			size_t masks = 0;
			for(const PolarizedTimeFrequencyData& data : _data)
				if(data._flagging) ++masks;
			return masks; 
		}

		const Image2DCPtr &GetImage(size_t imageIndex) const
		{
			size_t index = 0;
			for(const PolarizedTimeFrequencyData& data : _data)
			{
				if(data._images.first)
				{
					if(index == imageIndex)
						return data._images.first;
					++index;
				}
				if(data._images.second)
				{
					if(index == imageIndex)
						return data._images.second;
					++index;
				}
			}
			throw BadUsageException("Invalid image index in GetImage()");
		}
		const Mask2DCPtr &GetMask(size_t maskIndex) const
		{
			size_t index = 0;
			for(const PolarizedTimeFrequencyData& data : _data)
			{
				if(data._flagging)
				{
					if(index == maskIndex)
						return data._flagging;
					++index;
				}
			}
			std::ostringstream msg;
			msg << "Invalid mask index of " << maskIndex << " in GetMask(): mask count is " << MaskCount();
			throw BadUsageException(msg.str());
		}
		void SetImage(size_t imageIndex, const Image2DCPtr &image)
		{
			size_t index = 0;
			for(PolarizedTimeFrequencyData& data : _data)
			{
				if(data._images.first)
				{
					if(index == imageIndex)
					{
						data._images.first = image;
						return;
					}
					++index;
				}
				if(data._images.second)
				{
					if(index == imageIndex)
					{
						data._images.second = image;
						return;
					}
					++index;
				}
			}
			throw BadUsageException("Invalid image index in SetImage()");
		}
		void SetMask(size_t maskIndex, const Mask2DCPtr &mask)
		{
			size_t index = 0;
			for(PolarizedTimeFrequencyData& data : _data)
			{
				if(data._flagging)
				{
					if(index == maskIndex)
					{
						data._flagging = mask;
						return;
					}
					++index;
				}
			}
			throw BadUsageException("Invalid mask index in SetMask()");
		}
		void SetMask(const TimeFrequencyData &source)
		{
			source.CopyFlaggingTo(this);
		}

		static TimeFrequencyData *CreateTFDataFromComplexCombination(const TimeFrequencyData &real, const TimeFrequencyData &imaginary);

		static TimeFrequencyData *CreateTFDataFromPolarizationCombination(const TimeFrequencyData &xx, const TimeFrequencyData &xy, const TimeFrequencyData &yx, const TimeFrequencyData &yy);

		static TimeFrequencyData *CreateTFDataFromPolarizationCombination(const TimeFrequencyData &xx, const TimeFrequencyData &yy);

		void SetImagesToZero();
		template<bool Value>
		void SetMasksToValue()
		{
			if(!IsEmpty())
			{
				Mask2DPtr mask = Mask2D::CreateSetMaskPtr<Value>(ImageWidth(), ImageHeight());
				for(PolarizedTimeFrequencyData& data : _data)
				{
					data._flagging = mask;
				}
			}
		}
		
		void MultiplyImages(long double factor);
		void JoinMask(const TimeFrequencyData &other);

		void Trim(unsigned timeStart, unsigned freqStart, unsigned timeEnd, unsigned freqEnd)
		{
			for(PolarizedTimeFrequencyData& data : _data)
			{
				if(data._images.first)
					data._images.first = data._images.first->Trim(timeStart, freqStart, timeEnd, freqEnd);
				if(data._images.second)
					data._images.second = data._images.second->Trim(timeStart, freqStart, timeEnd, freqEnd);
				if(data._flagging)
					data._flagging = data._flagging->Trim(timeStart, freqStart, timeEnd, freqEnd);
			}
		}
		static std::string GetPolarisationName(enum PolarisationType polarization)
		{
			switch(polarization)
			{
				case StokesIPolarisation: return "Stokes I";
				case StokesQPolarisation: return "Stokes Q";
				case StokesUPolarisation: return "Stokes U";
				case StokesVPolarisation: return "Stokes V";
				case XXPolarisation: return "XX";
				case XYPolarisation: return "XY";
				case YXPolarisation: return "YX";
				case YYPolarisation: return "YY";
				case RRPolarisation: return "RR";
				case RLPolarisation: return "RL";
				case LRPolarisation: return "LR";
				case LLPolarisation: return "LL";
				default: return "Unknown polarization";
			}
		}
		
		std::string Description() const
		{
			std::ostringstream s;
			switch(_phaseRepresentation)
			{
				case RealPart: s << "Real component of "; break;
				case ImaginaryPart: s << "Imaginary component of "; break;
				case PhasePart: s << "Phase of "; break;
				case AmplitudePart: s << "Amplitude of "; break;
				case ComplexRepresentation:
					break;
			}
			if(_data.empty())
				s << "empty";
			else
			{
				s << "(" << GetPolarisationName(_data[0]._polarisation);
				for(size_t i=1; i!=_data.size(); ++i)
					s << "," << GetPolarisationName(_data[i]._polarisation);
				s << ")";
			}
			return s.str();
		}
		
		size_t PolarisationCount() const
		{
			return _data.size();
		}
		
		TimeFrequencyData *CreateTFDataFromPolarisationIndex(size_t index) const
		{
			return new TimeFrequencyData(_phaseRepresentation, _data[index]);
		}
		
		void SetPolarizationData(size_t polarizationIndex, const TimeFrequencyData &data)
		{
			if(data.PolarisationCount() != 1)
				throw BadUsageException("Trying to set multiple polarizations by single polarization index");
			else if(data.PhaseRepresentation() != PhaseRepresentation())
				throw BadUsageException("Trying to combine TFData's with different phase representations");
			else
				_data[polarizationIndex] = data._data[0];
		}

		void SetImageSize(size_t width, size_t height)
		{
			for(size_t i=0;i<_data.size();++i)
			{
				if(_data[i]._images.first)
					_data[i]._images.first = Image2D::CreateUnsetImagePtr(width, height);
				if(_data[i]._images.second)
					_data[i]._images.second = Image2D::CreateUnsetImagePtr(width, height);
				if(_data[i]._flagging)
					_data[i]._flagging = Mask2D::CreateUnsetMaskPtr(width, height);
			}
		}

		void CopyFrom(const TimeFrequencyData &source, size_t destX, size_t destY)
		{
			if(source._data.size() != _data.size())
				throw BadUsageException("CopyFrom: tf data do not match");
			for(size_t i=0;i<_data.size();++i)
			{
				if(_data[i]._images.first)
				{
					Image2DPtr image = Image2D::CreateCopy(_data[i]._images.first);
					image->CopyFrom(source._data[i]._images.first, destX, destY);
					_data[i]._images.first = image;
				}
				if(_data[i]._images.second)
				{
					Image2DPtr image = Image2D::CreateCopy(_data[i]._images.second);
					image->CopyFrom(source._data[i]._images.second, destX, destY);
					_data[i]._images.second = image;
				}
				if(_data[i]._flagging)
				{
					Mask2DPtr mask = Mask2D::CreateCopy(_data[i]._flagging);
					mask->CopyFrom(source._data[i]._flagging, destX, destY);
					_data[i]._flagging = mask;
				}
			}
		}
		
		/**
		 * Will return true when this is the imaginary part of the visibilities. Will throw
		 * an exception when the data is neither real nor imaginary.
		 */
		bool IsImaginary() const
		{
			if(PhaseRepresentation() == RealPart)
				return false;
			else if(PhaseRepresentation() == ImaginaryPart)
				return true;
			else
				throw std::runtime_error("Data is not real or imaginary");
		}
		
	private:
		Image2DCPtr GetSingleAbsoluteFromComplex() const
		{
			if(_data.size() == 4)
				return GetAbsoluteFromComplex(getFirstSum(0, 3), getSecondSum(0, 3));
			else if(_data.size() == 2)
				return GetAbsoluteFromComplex(getFirstSum(0, 1), getSecondSum(0, 1));
			else
				return getAbsoluteFromComplex(0);
		}

		Image2DCPtr GetSingleImageFromSinglePhaseImage() const
		{
			if(_data.size() == 4)
			{
				if(_phaseRepresentation == PhasePart)
					return getSinglePhaseFromTwoPolPhase(0, 3);
				else
					return getFirstSum(0, 3);
			}
			if(_data.size() == 2)
			{
				if(_phaseRepresentation == PhasePart)
					return getSinglePhaseFromTwoPolPhase(0, 1);
				else
					return getFirstSum(0, 1);
			}
			else // if(_data.size() == 1)
				return _data[0]._images.first;
		}

		Image2DCPtr GetRealPartFromDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				default:
				case XXPolarisation: return _data[0]._images.first;
				case XYPolarisation: return _data[1]._images.first;
				case YXPolarisation: return _data[2]._images.first;
				case YYPolarisation: return _data[3]._images.first;
			}
		}

		Image2DCPtr GetRealPartFromAutoDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				default:
				case XXPolarisation: return _data[0]._images.first;
				case YYPolarisation: return _data[1]._images.first;
			}
		}

		Image2DCPtr GetRealPartFromCrossDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XYPolarisation: return _data[0]._images.first;
				case YXPolarisation: return _data[1]._images.first;
				default: throw BadUsageException("Could not extract real part for given polarisation");
			}
		}

		Image2DCPtr GetImaginaryPartFromDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XXPolarisation: return _data[0]._images.second;
				case XYPolarisation: return _data[1]._images.second;
				case YXPolarisation: return _data[2]._images.second;
				case YYPolarisation: return _data[3]._images.second;
				default: throw BadUsageException("Could not extract imaginary part for given polarisation");
			}
		}

		Image2DCPtr GetImaginaryPartFromAutoDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XXPolarisation: return _data[0]._images.second;
				case YYPolarisation: return _data[1]._images.second;
				default: throw BadUsageException("Could not extract imaginary part for given polarisation");
			}
		}

		Image2DCPtr GetImaginaryPartFromCrossDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XYPolarisation: return _data[0]._images.second;
				case YXPolarisation: return _data[1]._images.second;
				default: throw BadUsageException("Could not extract imaginary part for given polarisation");
			}
		}

		Image2DCPtr GetAmplitudePartFromDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XXPolarisation: return getAbsoluteFromComplex(0);
				case XYPolarisation: return getAbsoluteFromComplex(1);
				case YXPolarisation: return getAbsoluteFromComplex(2);
				case YYPolarisation: return getAbsoluteFromComplex(3);
				default: throw BadUsageException("Could not extract amplitude part for given polarisation");
			}
		}

		Image2DCPtr GetAmplitudePartFromAutoDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XXPolarisation: return getAbsoluteFromComplex(0);
				case YYPolarisation: return getAbsoluteFromComplex(1);
				default: throw BadUsageException("Could not extract amplitude part for given polarisation");
			}
		}

		Image2DCPtr GetAmplitudePartFromCrossDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XYPolarisation: return getAbsoluteFromComplex(0);
				case YXPolarisation: return getAbsoluteFromComplex(1);
				default: throw BadUsageException("Could not extract amplitude part for given polarisation");
			}
		}

		Image2DCPtr GetPhasePartFromDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XXPolarisation: return getPhaseFromComplex(0);
				case XYPolarisation: return getPhaseFromComplex(1);
				case YXPolarisation: return getPhaseFromComplex(2);
				case YYPolarisation: return getPhaseFromComplex(3);
				default: throw BadUsageException("Could not extract phase part for given polarisation");
			}
		}

		Image2DCPtr GetPhasePartFromAutoDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XXPolarisation: return getPhaseFromComplex(0);
				case YYPolarisation: return getPhaseFromComplex(1);
				default: throw BadUsageException("Could not extract phase part for given polarisation");
			}
		}

		Image2DCPtr GetPhasePartFromCrossDipole(enum PolarisationType polarisation) const
		{
			switch(polarisation)
			{
				case XYPolarisation: return getPhaseFromComplex(0);
				case YXPolarisation: return getPhaseFromComplex(1);
				default: throw BadUsageException("Could not extract phase part for given polarisation");
			}
		}

		void CopyFlaggingTo(TimeFrequencyData *data) const
		{
			if(MaskCount() == 0)
				data->SetNoMask();
			else if(MaskCount() == 1)
				data->SetGlobalMask(GetMask(0));
			else
			{
				if(_data.size() == data->_data.size())
				{
					for(size_t i=0; i!=_data.size(); ++i)
						data->_data[i]._flagging = _data[i]._flagging;
				} else
					throw BadUsageException("Trying to copy flagging from incompatible time frequency data");
			}
		}
		
		/*Image2DCPtr GetSingleAbsoluteFromComplexDipole() const
		{
			Image2DCPtr real = getFirstSum(0, 3);
			Image2DCPtr imag = getSecondSum(0, 3);
			return GetAbsoluteFromComplex(real, imag);
		}

		Image2DCPtr GetSingleAbsoluteFromComplexAutoDipole() const
		{
			Image2DCPtr real = getFirstSum(0, 1);
			Image2DCPtr imag = getSecondSum(0, 1);
			return GetAbsoluteFromComplex(real, imag);
		}

		Image2DCPtr GetSingleAbsoluteFromComplexCrossDipole() const
		{
			Image2DCPtr real = getFirstSum(0, 1);
			Image2DCPtr imag = getSecondSum(0, 1);
			return GetAbsoluteFromComplex(real, imag);
		}*/

		Image2DCPtr getAbsoluteFromComplex(size_t polIndex) const
		{
			return GetAbsoluteFromComplex(_data[polIndex]._images.first, _data[polIndex]._images.second);
		}

		Image2DCPtr getPhaseFromComplex(size_t polIndex) const
		{
			return GetPhaseFromComplex(_data[polIndex]._images.first, _data[polIndex]._images.second);
		}
		
		Image2DCPtr GetAbsoluteFromComplex(const Image2DCPtr &real, const Image2DCPtr &imag) const;
		Image2DCPtr GetPhaseFromComplex(const Image2DCPtr &real, const Image2DCPtr &imag) const;

		Image2DCPtr getFirstSum(size_t dataIndexA, size_t dataIndexB) const
		{
			if(dataIndexA >= _data.size())
				throw BadUsageException("Polarisation not available");
			if(dataIndexB >= _data.size())
				throw BadUsageException("Polarisation not available");
			return GetSum(_data[dataIndexA]._images.first, _data[dataIndexB]._images.first);
		}
		
		Image2DCPtr getSecondSum(size_t dataIndexA, size_t dataIndexB) const
		{
			if(dataIndexA >= _data.size())
				throw BadUsageException("Polarisation not available");
			if(dataIndexB >= _data.size())
				throw BadUsageException("Polarisation not available");
			return GetSum(_data[dataIndexA]._images.second, _data[dataIndexB]._images.second);
		}
		
		Image2DCPtr getFirstDiff(size_t dataIndexA, size_t dataIndexB) const
		{
			if(dataIndexA >= _data.size())
				throw BadUsageException("Polarisation not available");
			if(dataIndexB >= _data.size())
				throw BadUsageException("Polarisation not available");
			return GetDifference(_data[dataIndexA]._images.first, _data[dataIndexB]._images.first);
		}
		
		Image2DCPtr getSecondDiff(size_t dataIndexA, size_t dataIndexB) const
		{
			if(dataIndexA >= _data.size())
				throw BadUsageException("Polarisation not available");
			if(dataIndexB >= _data.size())
				throw BadUsageException("Polarisation not available");
			return GetDifference(_data[dataIndexA]._images.second, _data[dataIndexB]._images.second);
		}
		
		Image2DCPtr getStokesVReal(size_t xyIndex, size_t yxIndex) const
		{
			if(xyIndex >= _data.size())
				throw BadUsageException("Polarisation not available");
			if(yxIndex >= _data.size())
				throw BadUsageException("Polarisation not available");
			return GetNegatedSum(_data[xyIndex]._images.second, _data[yxIndex]._images.first);
		}
		
		Image2DCPtr getStokesVImag(size_t xyIndex, size_t yxIndex) const
		{
			if(xyIndex >= _data.size())
				throw BadUsageException("Polarisation not available");
			if(yxIndex >= _data.size())
				throw BadUsageException("Polarisation not available");
			return GetDifference(_data[xyIndex]._images.first, _data[yxIndex]._images.second);
		}
		
		Image2DCPtr GetSum(const Image2DCPtr &left, const Image2DCPtr &right) const;
		Image2DCPtr GetNegatedSum(const Image2DCPtr &left, const Image2DCPtr &right) const;
		Image2DCPtr GetDifference(const Image2DCPtr &left, const Image2DCPtr &right) const;

		Image2DCPtr getSinglePhaseFromTwoPolPhase(size_t polA, size_t polB) const;
		Image2DCPtr GetZeroImage() const;
		template<bool InitValue>
		Mask2DCPtr GetSetMask() const
		{
			if(ImageWidth() == 0 || ImageHeight() == 0)
				throw BadUsageException("Can't make a mask without an image");
			return Mask2D::CreateSetMaskPtr<InitValue>(ImageWidth(), ImageHeight());
		}
		Mask2DCPtr GetCombinedMask() const;

		//TimeFrequencyData *CreateTFDataFromSingleComplex(enum PhaseRepresentation phase) const;
		//TimeFrequencyData *CreateTFDataFromDipoleComplex(enum PhaseRepresentation phase) const;
		//TimeFrequencyData *CreateTFDataFromAutoDipoleComplex(enum PhaseRepresentation phase) const;
		
		/**
		 * Returns the data index of the given polarisation, or _data.size() if
		 * not found.
		 */
		size_t getPolarisationIndex(PolarisationType polarisation) const
		{
			for(size_t i=0; i!=_data.size(); ++i)
				if(_data[i]._polarisation == polarisation)
					return i;
			return _data.size();
		}

		struct PolarizedTimeFrequencyData
		{
			PolarizedTimeFrequencyData() :
				_images(nullptr, nullptr),
				_flagging(nullptr),
				_polarisation(StokesIPolarisation)
			{ }
			PolarizedTimeFrequencyData(PolarisationType polarisation, const Image2DCPtr& image) :
				_images(image, nullptr),
				_flagging(nullptr),
				_polarisation(polarisation)
			{ }
				
			PolarizedTimeFrequencyData(PolarisationType polarisation, const Image2DCPtr& imageA, const Image2DCPtr& imageB) :
				_images(imageA, imageB),
				_flagging(nullptr),
				_polarisation(polarisation)
			{ }
				
			// Second image is only filled when phase representation = complex
			std::pair<Image2DCPtr, Image2DCPtr> _images;
			Mask2DCPtr _flagging;
			enum PolarisationType _polarisation;
		};
		
		TimeFrequencyData(enum PhaseRepresentation phase, const PolarizedTimeFrequencyData& source) :
		_phaseRepresentation(phase), _data(1, source)
		{
		}

		enum PhaseRepresentation _phaseRepresentation;
		
		std::vector<PolarizedTimeFrequencyData> _data;
};

#endif
