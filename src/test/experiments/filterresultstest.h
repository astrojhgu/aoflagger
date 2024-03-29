#ifndef AOFLAGGER_FILTERRESULTSTEST_H
#define AOFLAGGER_FILTERRESULTSTEST_H

#include <complex>
#include <fstream>

#include "../testingtools/asserter.h"
#include "../testingtools/unittest.h"

#include "../../msio/pngfile.h"

#include "../../util/ffttools.h"

#include "../../structures/colormap.h"

#include "../../imaging/defaultmodels.h"

#include "../../strategy/actions/foreachcomplexcomponentaction.h"
#include "../../strategy/actions/frequencyconvolutionaction.h"
#include "../../strategy/actions/fringestopaction.h"
#include "../../strategy/actions/iterationaction.h"
#include "../../strategy/actions/setimageaction.h"
#include "../../strategy/actions/strategy.h"
#include "../../strategy/actions/timeconvolutionaction.h"

class FilterResultsTest : public UnitTest {
	public:
		FilterResultsTest() : UnitTest("Filter results")
		{
			std::ofstream("filter-stats.txt", std::ios_base::out | std::ios_base::trunc);
			AddTest(TestNoSource(), "No source");
			AddTest(TestConstantSource(), "Constant source");
			AddTest(TestVariableSource(), "Variable source");
			AddTest(TestFaintSource(), "Faint source");
			AddTest(TestMissedSource(), "Misslocated source");
		}
		
	private:
		const static unsigned CHANNEL_COUNT;
		const static double BANDWIDTH;
		
		struct TestNoSource : public Asserter
		{
			void operator()();
		};
		struct TestConstantSource : public Asserter
		{
			void operator()();
		};
		struct TestVariableSource : public Asserter
		{
			void operator()();
		};
		struct TestFaintSource : public Asserter
		{
			void operator()();
		};
		struct TestMissedSource : public Asserter
		{
			void operator()();
		};
		
		static rfiStrategy::Strategy *createStrategy(bool withFringeFilter, bool withTimeConv, bool withFrequencyConv)
		{
			rfiStrategy::Strategy *strategy = new rfiStrategy::Strategy();
			
			if(withFringeFilter)
			{
				std::unique_ptr<rfiStrategy::FringeStopAction> fsAction(
					new rfiStrategy::FringeStopAction());
				fsAction->SetNewPhaseCentreRA(DefaultModels::DistortionRA());
				fsAction->SetNewPhaseCentreDec(DefaultModels::DistortionDec());
				fsAction->SetFringesToConsider(3.0);
				fsAction->SetMinWindowSize(500);
				fsAction->SetMaxWindowSize(500);
				strategy->Add(std::move(fsAction));

				std::unique_ptr<rfiStrategy::SetImageAction> setAction(
					new rfiStrategy::SetImageAction());
				setAction->SetNewImage(rfiStrategy::SetImageAction::SwapRevisedAndContaminated);
				strategy->Add(std::move(setAction));
			}
			
			if(withFrequencyConv || withTimeConv)
			{
				std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> foccAction(
					new rfiStrategy::ForEachComplexComponentAction());

				if(withFrequencyConv)
				{
					std::unique_ptr<rfiStrategy::FrequencyConvolutionAction> fcAction(
						new rfiStrategy::FrequencyConvolutionAction());
					fcAction->SetConvolutionSize(5.0);
					fcAction->SetKernelKind(rfiStrategy::FrequencyConvolutionAction::SincKernel);
					foccAction->Add(std::move(fcAction));
				}
				
				if(withTimeConv)
				{
					std::unique_ptr<rfiStrategy::TimeConvolutionAction> tcAction(
						new rfiStrategy::TimeConvolutionAction());
					tcAction->SetSincScale(6.0);
					tcAction->SetIsSincScaleInSamples(false);
					tcAction->SetOperation(rfiStrategy::TimeConvolutionAction::SingleSincOperation);
					foccAction->Add(std::move(tcAction));
					
					std::unique_ptr<rfiStrategy::SetImageAction> setAction(
						new rfiStrategy::SetImageAction());
					setAction->SetNewImage(rfiStrategy::SetImageAction::SwapRevisedAndContaminated);
					foccAction->Add(std::move(setAction));
				}
				
				strategy->Add(std::move(foccAction));
			}
			
			return strategy;
		}
		
		static double SquaredValue(const UVImager &imager, unsigned x, unsigned y)
		{
			double a = imager.FTReal().Value(x, y), b = imager.FTImaginary().Value(x, y);
			return a*a + b*b;
		}
		
		static void AddStatistics(const UVImager &imager, const std::string filename, double centerPower, double sidelobePower, double onAxisPower)
		{
			std::ofstream statFile("filter-stats.txt", std::ios_base::out | std::ios_base::app);
			double sourceSum =
				SquaredValue(imager, 625, 172) +
				SquaredValue(imager, 625, 173) +
				SquaredValue(imager, 625, 174) +
				SquaredValue(imager, 625, 175) +
				SquaredValue(imager, 626, 173) +
				SquaredValue(imager, 626, 174);
			statFile << filename << '\t' << sqrt(sourceSum/6.0);
			
			double sourceSidelobeRMS = 0.0;
			unsigned countSource = 0;
			for(unsigned y=0; y<346; ++y)
			{
				for(unsigned x=510; x<740; ++x)
				{
					sourceSidelobeRMS += SquaredValue(imager, x, y);
					++countSource;
				}
			}
			statFile << '\t' << sqrt(sourceSidelobeRMS/countSource);
			
			double onAxisRMS = 0.0;
			unsigned countOnAxis = 0;
			for(unsigned y=650; y<1000; ++y)
			{
				for(unsigned x=700; x<850; ++x)
				{
					onAxisRMS += SquaredValue(imager, x, y);
					++countOnAxis;
				}
			}
			statFile << '\t' << sqrt(onAxisRMS/countOnAxis);
			
			statFile << '\t' << 10.0*log10((centerPower - 1172.84) / (sqrt(sourceSum/6.0) - 1172.84));
			statFile << '\t' << 10.0*log10((sidelobePower - 2018.02) / (sqrt(sourceSidelobeRMS/countSource) - 2018.02));
			statFile << '\t' << 100.0*(1.0-(sqrt(onAxisRMS/countOnAxis)) / (onAxisPower)) << '%';
			statFile << '\n';
		}
		
		static void SaveImaged(const TimeFrequencyData &original, rfiStrategy::ArtifactSet &artifacts, const std::string &filename, bool difference, double centerPower, double sidelobePower, double onAxisPower)
		{
			UVImager imager(1024*1.5, 1024*1.5);
			
			TimeFrequencyData data;
			if(difference)
			{
				data = TimeFrequencyData::MakeFromDiff(original, artifacts.ContaminatedData());
			} else {
				data = artifacts.ContaminatedData();
			}
			
			imager.SetUVScaling(0.0012);
			imager.Image(data, artifacts.MetaData());
			imager.PerformFFT();
			
			if(!difference)
				AddStatistics(imager, filename, centerPower, sidelobePower, onAxisPower);

			InvPositiveMap colorMap;
			//const Image2DPtr uvImage = Image2D::CreateCopyPtr(imager.RealUVImage());
			//PngFile::Save(*uvImage, std::string("UV-") + filename, colorMap);
			
			Image2D image(imager.FTReal());
			
			image.SetTrim(400, 0, 1000, 1200);
			FFTTools::SignedSqrt(image);
			image.SetToAbs();
			
			PngFile::Save(image, filename, colorMap, 0.01);
		}
		
		static void Run(rfiStrategy::Strategy *strategy, std::pair<TimeFrequencyData, TimeFrequencyMetaDataPtr> data, const std::string &appliedName, const std::string &differenceName, double centerPower, double sidelobePower, double onAxisPower)
		{
			rfiStrategy::ArtifactSet artifacts(0);
			DummyProgressListener listener;
			
			artifacts.SetOriginalData(data.first);
			artifacts.SetContaminatedData(data.first);
			TimeFrequencyData zero(data.first);
			zero.SetImagesToZero();
			artifacts.SetRevisedData(zero);
			artifacts.SetMetaData(data.second);
			
			strategy->Perform(artifacts, listener);
			
			SaveImaged(data.first, artifacts, appliedName, false, centerPower, sidelobePower, onAxisPower);
			SaveImaged(data.first, artifacts, differenceName, true, centerPower, sidelobePower, onAxisPower);
		}
		
		static void RunAllMethods(std::pair<TimeFrequencyData, TimeFrequencyMetaDataPtr> data, const std::string &setPrefix, const std::string &setName, double centerPower, double sidelobePower, double onAxisPower)
		{
			rfiStrategy::Strategy *strategy = new rfiStrategy::Strategy();
			Run(strategy, data, setPrefix + "0-" + setName + "-Original.png", "Empty.png", centerPower, sidelobePower, onAxisPower);
			delete strategy;

			strategy = createStrategy(true, false, false);
			Run(strategy, data, setPrefix + "1-" + setName + "-FringeFilter-Applied.png", setPrefix + "1-" + setName + "-FringeFilter-Difference.png", centerPower, sidelobePower, onAxisPower);
			delete strategy;
			
			strategy = createStrategy(false, true, false);
			Run(strategy, data, setPrefix + "2-" + setName + "-TimeFilter-Applied.png", setPrefix + "2-" + setName + "-TimeFilter-Difference.png", centerPower, sidelobePower, onAxisPower);
			delete strategy;
			
			strategy = createStrategy(false, false, true);
			Run(strategy, data, setPrefix + "3-" + setName + "-FreqFilter-Applied.png", setPrefix + "3-" + setName + "-FreqFilter-Difference.png", centerPower, sidelobePower, onAxisPower);
			delete strategy;

			strategy = createStrategy(false, true, true);
			Run(strategy, data, setPrefix + "5-" + setName + "-TimeFreq-Applied.png", setPrefix + "5-" + setName + "-TimeFreq-Difference.png", centerPower, sidelobePower, onAxisPower);
			delete strategy;
		}
};

const unsigned FilterResultsTest::CHANNEL_COUNT = 64;
const double FilterResultsTest::BANDWIDTH = 2500000.0;

inline void FilterResultsTest::TestNoSource::operator()()
{
	std::pair<TimeFrequencyData, TimeFrequencyMetaDataPtr> data
		= DefaultModels::LoadSet(DefaultModels::B1834Set, DefaultModels::NoDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "N", "NoSource", 1172.84, 2018.02, 34340.5);

	data = DefaultModels::LoadSet(DefaultModels::EmptySet, DefaultModels::NoDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "O", "NoSource", 0.0, 0.0, 0.0);
}

inline void FilterResultsTest::TestConstantSource::operator()()
{
	std::pair<TimeFrequencyData, TimeFrequencyMetaDataPtr> data
		= DefaultModels::LoadSet(DefaultModels::B1834Set, DefaultModels::ConstantDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "A", "ConstantSource", 541855.0, 36144.3, 47121.4);

	data = DefaultModels::LoadSet(DefaultModels::EmptySet, DefaultModels::ConstantDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "P", "ConstantSource", 6627.86, 959.102, 1172.99);
}

inline void FilterResultsTest::TestVariableSource::operator()()
{
	std::pair<TimeFrequencyData, TimeFrequencyMetaDataPtr> data
		= DefaultModels::LoadSet(DefaultModels::B1834Set, DefaultModels::VariableDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "B", "VariableSource", 1.36159e+06, 99793.8, 56885.0);

	data = DefaultModels::LoadSet(DefaultModels::EmptySet, DefaultModels::VariableDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "Q", "VariableSource", 629649.0, 50508.9, 2456.41);
}

inline void FilterResultsTest::TestFaintSource::operator()()
{
	std::pair<TimeFrequencyData, TimeFrequencyMetaDataPtr> data
		= DefaultModels::LoadSet(DefaultModels::B1834Set, DefaultModels::FaintDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "C", "FaintSource", 79725.9, 5972.41, 36504.8);

	data = DefaultModels::LoadSet(DefaultModels::EmptySet, DefaultModels::FaintDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "R", "FaintSource", 6031.9, 584.827, 335.651);
}

inline void FilterResultsTest::TestMissedSource::operator()()
{
	std::pair<TimeFrequencyData, TimeFrequencyMetaDataPtr> data
		= DefaultModels::LoadSet(DefaultModels::B1834Set, DefaultModels::MislocatedDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "D", "MissedSource", 232796.0, 100212.6, 57039.4);

	data = DefaultModels::LoadSet(DefaultModels::EmptySet, DefaultModels::MislocatedDistortion, 0.0, CHANNEL_COUNT, BANDWIDTH);
 	RunAllMethods(data, "S", "MissedSource", 95403.1, 50750.5, 2526.13);
}

#endif
