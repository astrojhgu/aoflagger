#ifndef RFI_FREQUENCY_CONVOLUTION_ACTION
#define RFI_FREQUENCY_CONVOLUTION_ACTION

#include <stdexcept>

#include "../../structures/samplerow.h"

#include "../../imaging/uvimager.h"

#include "../../util/logger.h"
#include "../../util/progresslistener.h"

#include "action.h"

#include "../control/actionblock.h"
#include "../control/artifactset.h"

#include "../algorithms/thresholdtools.h"

namespace rfiStrategy {

	class FrequencyConvolutionAction : public Action
	{
		public:
			enum KernelKind { RectangleKernel, SincKernel, TotalKernel };
			
			FrequencyConvolutionAction() : Action(), _kernelKind(SincKernel), _convolutionSize(4.0), _inSamples(false)
			{
			}
			virtual std::string Description() final override
			{
				return "Frequency convolution";
			}
			virtual ActionType Type() const final override { return FrequencyConvolutionActionType; }
			virtual void Perform(ArtifactSet &artifacts, class ProgressListener &listener) final override
			{
				TimeFrequencyData &data = artifacts.ContaminatedData();
				if(_kernelKind == TotalKernel)
				{
					Image2DPtr
						rImage(new Image2D(*data.GetImage(0))),
						iImage(new Image2D(*data.GetImage(1)));
					Convolve(rImage, iImage, artifacts.MetaData(), listener);
					data.SetImage(0, rImage);
					data.SetImage(1, iImage);
					artifacts.SetRevisedData(data);
				}
				else
				{
					if(data.ImageCount() != 1)
						throw std::runtime_error("A frequency convolution can only be applied on single component data");

					Image2DPtr newImage;
					switch(_kernelKind)
					{
						default:
						case RectangleKernel:
						newImage = ThresholdTools::FrequencyRectangularConvolution(data.GetImage(0).get(), (unsigned) roundn(_convolutionSize));
						break;
						case SincKernel:
						newImage = sincConvolution(artifacts.MetaData(), data.GetImage(0).get());
						break;
					}
					
					data.SetImage(0, newImage);
				}
			}
			
			numl_t ConvolutionSize() const { return _convolutionSize; }
			void SetConvolutionSize(numl_t size) { _convolutionSize = size; }
			
			enum KernelKind KernelKind() const { return _kernelKind; }
			void SetKernelKind(enum KernelKind kind) { _kernelKind = kind; }
			
			bool InSamples() const { return _inSamples; }
			void SetInSamples(bool inSamples) { _inSamples = inSamples; }
		private:
			Image2DPtr sincConvolution(TimeFrequencyMetaDataCPtr metaData, const Image2D* source)
			{
				numl_t uvDist = averageUVDist(metaData);
				Logger::Debug << "Avg uv dist: " << uvDist << '\n';
				numl_t convolutionSize = convolutionSizeInSamples(uvDist, source->Height());
				Logger::Debug << "Convolution size: " << convolutionSize << '\n';
				Image2DPtr destination = Image2D::CreateUnsetImagePtr(source->Width(), source->Height());
				for(unsigned x=0;x<source->Width();++x)
				{
					SampleRow row = SampleRow::MakeFromColumn(source, x);
					row.ConvolveWithSinc(1.0 / convolutionSize);
					row.SetVerticalImageValues(destination.get(), x);
				}
				return destination;
			}
			
			numl_t averageUVDist(TimeFrequencyMetaDataCPtr metaData)
			{
				numl_t sum = 0.0;
				const numl_t
					lowFreq = metaData->Band().channels.begin()->frequencyHz,
					highFreq = metaData->Band().channels.rbegin()->frequencyHz;
				const std::vector<UVW> &uvw = metaData->UVW();
				for(std::vector<UVW>::const_iterator i=uvw.begin();i!=uvw.end();++i)
				{
					const numl_t
						lowU = i->u * lowFreq,
						lowV = i->v * lowFreq,
						highU = i->u * highFreq,
						highV = i->v * highFreq,
						ud = lowU - highU,
						vd = lowV - highV;
					sum += sqrtnl(ud * ud + vd * vd);
				}
				return sum / ((numl_t) uvw.size() * UVImager::SpeedOfLight());
			}
			
			numl_t convolutionSizeInSamples(numl_t uvDist, unsigned height)
			{
				if(_inSamples)
					return _convolutionSize;
				else
					return _convolutionSize * height / uvDist;
			}
			
			void Convolve(Image2DPtr rImage, Image2DPtr iImage, TimeFrequencyMetaDataCPtr metaData, ProgressListener &listener)
			{
				Image2DPtr copyReal(new Image2D(*rImage));
				Image2DPtr copyImag(new Image2D(*iImage));
				const size_t
					width = rImage->Width(),
					height = rImage->Height();
				const std::vector<UVW> &uvws = metaData->UVW();
				
				const num_t factor = M_PIn / _convolutionSize;
				
				for(size_t y=0;y<height;++y)
				{
					for(size_t x=0;x<width;++x)
					{
						const UVW uvw = uvws[x];
						const num_t sol = UVImager::SpeedOfLight();
						const double freq1 = metaData->Band().channels[y].frequencyHz / sol;
						const num_t
							u1 = uvw.u * freq1,
							v1 = uvw.v * freq1;
						//	w1 = uvw.w * freq1;
						num_t real = 0.0f, imaginary = 0.0f, weight = 1.0f;
						
						for(size_t yi=0;yi<height;++yi)
						{
							const double freq2 = metaData->Band().channels[yi].frequencyHz / sol;
							for(size_t xi=0;xi<width;++xi)
							{
								if(xi == x && yi == y)
								{
									real += copyReal->Value(xi, yi);
									imaginary += copyImag->Value(xi, yi);
								} else {
									const UVW uvwi = uvws[xi];
									const num_t
										du = uvwi.u * freq2 - u1,
										dv = uvwi.v * freq2 - v1;
										//dw = uvwi.w * freq2 - w1;
									const num_t dist = sqrtn(du*du + dv*dv) * factor; // + dw*dw
									const num_t sincVal = sinn(dist) / dist;
									/*if(xi == 0)
									{
										Logger::Debug << dist << '*' << factor << " -> " << sincVal << '\n';
									}*/
									
									real += sincVal * copyReal->Value(xi, yi);
									imaginary += sincVal * copyImag->Value(xi, yi);
									weight += sincVal;
								}
							}
						}
						rImage->SetValue(x, y, real / weight);
						iImage->SetValue(x, y, imaginary / weight);
					}
					listener.OnProgress(*this, y+1, height);
				}
			}
			
			enum KernelKind _kernelKind;
			numl_t _convolutionSize;
			bool _inSamples;
	};

} // namespace

#endif // RFI_FREQUENCY_CONVOLUTION_ACTION
