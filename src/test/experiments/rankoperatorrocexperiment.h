#ifndef AOFLAGGER_RANKOPERATORROCEXPERIMENT_H
#define AOFLAGGER_RANKOPERATORROCEXPERIMENT_H

#include <iostream>

#include "../testingtools/asserter.h"
#include "../testingtools/unittest.h"

#include "../../strategy/algorithms/siroperator.h"
#include "../../strategy/algorithms/morphologicalflagger.h"
#include "../../strategy/algorithms/testsetgenerator.h"
#include "../../strategy/algorithms/thresholdtools.h"

#include "../../msio/pngfile.h"

#include "../../util/rng.h"

#include "../../strategy/actions/changeresolutionaction.h"
#include "../../strategy/actions/foreachcomplexcomponentaction.h"
#include "../../strategy/actions/iterationaction.h"
#include "../../strategy/actions/setflaggingaction.h"
#include "../../strategy/actions/setimageaction.h"
#include "../../strategy/actions/strategy.h"
#include "../../strategy/actions/sumthresholdaction.h"
#include "../../strategy/actions/slidingwindowfitaction.h"

class RankOperatorROCExperiment : public UnitTest {
	public:
		RankOperatorROCExperiment() : UnitTest("Rank operator ROC experiments")
		{
			AddTest(RankOperatorROCGaussian(), "Constructing rank operator & dilation ROC curve for Gaussian broadband RFI");
			AddTest(RankOperatorROCSinusoidal(), "Constructing rank operator & dilation ROC curve for sinusoidal broadband RFI");
			AddTest(RankOperatorROCSGaussian(), "Constructing rank operator & dilation ROC curve for slewed Gaussian RFI");
			AddTest(RankOperatorROCSBurst(), "Constructing rank operator & dilation ROC curve for burst RFI");
		}
		
	private:
		struct EvaluationResult
		{
			EvaluationResult() :
				tpRatio(0.0), fpRatio(0.0),
				tpFuzzy(0.0), fpFuzzy(0.0),
				tpFuzzySq(0.0), fpFuzzySq(0.0)
			{
			}
			
			double tpRatio, fpRatio;
			double tpFuzzy, fpFuzzy;
			double tpFuzzySq, fpFuzzySq; // to calculate std dev
			std::vector<double> tpRefTP, tpRefFP;
		};

		struct RankOperatorROCGaussian : public Asserter
		{
			void operator()() { executeTest(GaussianBroadband); }
		};
		struct RankOperatorROCSinusoidal : public Asserter
		{
			void operator()() { executeTest(SinusoidalBroadband); }
		};
		struct RankOperatorROCSGaussian : public Asserter
		{
			void operator()() { executeTest(SlewedGaussianBroadband); }
		};
		struct RankOperatorROCSBurst : public Asserter
		{
			void operator()() { executeTest(BurstBroadband); }
		};
		
		static const unsigned _repeatCount;
		
		enum TestType { GaussianBroadband, SinusoidalBroadband, SlewedGaussianBroadband, BurstBroadband };
		
		static void TestNoisePerformance(size_t totalRFI, double totalRFISum, const std::string &testname);
		static void evaluateIterationResults(Mask2D& result, Mask2D& maskGroundTruth, Image2D& fuzzyGroundTruth, const size_t totalRFI, struct EvaluationResult &evaluationResult);
		static double stddev(double sum, double sumSq, unsigned n)
		{
			const double sumMeanSquared = sum * sum / n;
			return sqrt((sumSq - sumMeanSquared) / n);
		}
		
		static rfiStrategy::Strategy *createThresholdStrategy();
		static void executeTest(enum TestType testType);
		
		static num_t getRatio(Image2D& groundTruth, Mask2D& resultMask, bool inverseTruth, bool invertMask)
		{
			num_t totalTruth = groundTruth.Sum();
			if(inverseTruth)
			{
				totalTruth = groundTruth.Width() * groundTruth.Height() - totalTruth;
			}
			num_t sum = 0.0;
			for(size_t y=0;y<groundTruth.Height();++y)
			{
				for(size_t x=0;x<groundTruth.Width();++x)
				{
					num_t truth = inverseTruth ? (1.0 - groundTruth.Value(x, y)) : groundTruth.Value(x, y);
					if(resultMask.Value(x, y) != invertMask)
					{
						sum += truth;
					}
				}
			}
			return sum / totalTruth;
		}
};

const unsigned RankOperatorROCExperiment::_repeatCount = 100;

inline rfiStrategy::Strategy* RankOperatorROCExperiment::createThresholdStrategy()
{
	rfiStrategy::Strategy *strategy = new rfiStrategy::Strategy();
	
	rfiStrategy::ActionBlock *current = strategy;

	current->Add(std::unique_ptr<rfiStrategy::SetFlaggingAction>(new rfiStrategy::SetFlaggingAction()));

	std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> focAction(
		new rfiStrategy::ForEachComplexComponentAction());
	focAction->SetOnAmplitude(true);
	focAction->SetOnImaginary(false);
	focAction->SetOnReal(false);
	focAction->SetOnPhase(false);
	focAction->SetRestoreFromAmplitude(false);
	rfiStrategy::ActionBlock* focActionPtr = focAction.get();
	current->Add(std::move(focAction));
	current = focActionPtr;

	std::unique_ptr<rfiStrategy::IterationBlock> iteration(
		new rfiStrategy::IterationBlock());
	iteration->SetIterationCount(2);
	iteration->SetSensitivityStart(4.0);
	rfiStrategy::ActionBlock* scratch = iteration.get();
	current->Add(std::move(iteration));
	current = scratch;
	
	std::unique_ptr<rfiStrategy::SumThresholdAction> t2(
		new rfiStrategy::SumThresholdAction());
	t2->SetTimeDirectionSensitivity(1.0);
	t2->SetFrequencyDirectionSensitivity(1.0);
	current->Add(std::move(t2));

	current->Add(std::unique_ptr<rfiStrategy::SetImageAction>(
		new rfiStrategy::SetImageAction()));
	std::unique_ptr<rfiStrategy::ChangeResolutionAction> changeResAction2(
		new rfiStrategy::ChangeResolutionAction());
	changeResAction2->SetTimeDecreaseFactor(3);
	changeResAction2->SetFrequencyDecreaseFactor(3);

	std::unique_ptr<rfiStrategy::SlidingWindowFitAction> swfAction2(
		new rfiStrategy::SlidingWindowFitAction());
	swfAction2->Parameters().timeDirectionKernelSize = 2.5;
	swfAction2->Parameters().timeDirectionWindowSize = 10;
	swfAction2->Parameters().frequencyDirectionKernelSize = 5.0;
	swfAction2->Parameters().frequencyDirectionWindowSize = 15;
	changeResAction2->Add(std::move(swfAction2));

	current->Add(std::move(changeResAction2));
	
	current = focActionPtr;
	std::unique_ptr<rfiStrategy::SumThresholdAction> t3(
		new rfiStrategy::SumThresholdAction());
	current->Add(std::move(t3));
	
	return strategy;
}

void RankOperatorROCExperiment::evaluateIterationResults(Mask2D& result, Mask2D& maskGroundTruth, Image2D& fuzzyGroundTruth, const size_t totalRFI, EvaluationResult &evaluationResult)
{
	size_t totalPositives = result.GetCount<true>();
	double tpFuzzyRatio = getRatio(fuzzyGroundTruth, result, false, false);
	double fpFuzzyRatio = getRatio(fuzzyGroundTruth, result, true, false);
	
	Mask2D originalResult(result);
	result.Intersect(maskGroundTruth);
	size_t truePositives = result.GetCount<true>();
	size_t falsePositives = totalPositives - truePositives;
	
	double tpR = (double) truePositives / totalRFI;
	double fpR = (double) falsePositives / totalRFI;
	
	evaluationResult.tpRatio += tpR;
	evaluationResult.fpRatio += fpR;
	evaluationResult.tpFuzzy += tpFuzzyRatio;
	evaluationResult.fpFuzzy += fpFuzzyRatio;
	evaluationResult.tpFuzzySq += tpFuzzyRatio*tpFuzzyRatio;
	evaluationResult.fpFuzzySq += fpFuzzyRatio*fpFuzzyRatio;
	
	/*for(size_t i=1;i<=10;++i)
	{
		double groundTruthThreshold = (double) i / 20;
		Mask2DPtr thrGroundTruth = ThresholdTools::Threshold(fuzzyGroundTruth, groundTruthThreshold);
		Mask2DPtr intersectResult = Mask2D::CreateCopy(originalResult);
		size_t refTotalRFI = thrGroundTruth->GetCount<true>();
		intersectResult->Intersect(thrGroundTruth);
		size_t refTruePositives = intersectResult->GetCount<true>();
		size_t refFalsePositives = totalPositives - refTruePositives;
		
		evaluationResult.tpRefTP.push_back((double) refTruePositives / refTotalRFI);
		evaluationResult.tpRefFP.push_back((double) refFalsePositives / refTotalRFI);
	}*/
}

void RankOperatorROCExperiment::executeTest(enum TestType testType)
{
	const size_t ETA_STEPS = 400, DIL_STEPS = 512;
	const size_t width = 1024, height = 1024;
	const double MAX_DILATION = 1024;
	
	std::string testname;
	switch(testType)
	{
		case GaussianBroadband:
			testname = "gaussian";
			break;
		case SinusoidalBroadband:
			testname = "sinusoidal";
			break;
		case SlewedGaussianBroadband:
			testname = "slewed_gaussian";
			break;
		case BurstBroadband:
			testname = "burst";
			break;
	}
	
	EvaluationResult rocResults[ETA_STEPS+1], dilResults[DIL_STEPS+1];
	
	for(unsigned repeatIndex=0 ; repeatIndex<_repeatCount ; ++repeatIndex)
	{
		Mask2D mask = Mask2D::MakeSetMask<false>(width, height);
		Image2D groundTruth = Image2D::MakeZeroImage(width, height);
		Image2DPtr realImage, imagImage;
		Image2DCPtr rfiLessImage;
		TimeFrequencyData rfiLessData, data;
		switch(testType)
		{
			case GaussianBroadband:
			{
				Image2DPtr
					realTruth  = Image2D::CreateZeroImagePtr(width, height),
					imagTruth  = Image2D::CreateZeroImagePtr(width, height);
				realImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height))),
				imagImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height)));
				//realImage->MultiplyValues(0.5); //for different SNR
				//imagImage->MultiplyValues(0.5);
				rfiLessData = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
				rfiLessData.Trim(0, 0, 180, height);
				rfiLessImage = rfiLessData.GetSingleImage();
				TestSetGenerator::AddGaussianBroadbandToTestSet(*realImage, mask);
				TestSetGenerator::AddGaussianBroadbandToTestSet(*imagImage, mask);
				TestSetGenerator::AddGaussianBroadbandToTestSet(*realTruth, mask);
				TestSetGenerator::AddGaussianBroadbandToTestSet(*imagTruth, mask);
				groundTruth = *TimeFrequencyData(Polarization::StokesI, realTruth, imagTruth).GetSingleImage();
				data = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
			} break;
			case SlewedGaussianBroadband:
			{
				Image2DPtr
					realTruth  = Image2D::CreateZeroImagePtr(width, height),
					imagTruth  = Image2D::CreateZeroImagePtr(width, height);
				realImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height))),
				imagImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height)));
				rfiLessData = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
				rfiLessData.Trim(0, 0, 180, height);
				rfiLessImage = rfiLessData.GetSingleImage();
				TestSetGenerator::AddSlewedGaussianBroadbandToTestSet(*realImage, mask);
				TestSetGenerator::AddSlewedGaussianBroadbandToTestSet(*imagImage, mask);
				TestSetGenerator::AddSlewedGaussianBroadbandToTestSet(*realTruth, mask);
				TestSetGenerator::AddSlewedGaussianBroadbandToTestSet(*imagTruth, mask);
				groundTruth = *TimeFrequencyData(Polarization::StokesI, realTruth, imagTruth).GetSingleImage();
				data = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
			} break;
			case SinusoidalBroadband:
			{
				Image2DPtr
					realTruth  = Image2D::CreateZeroImagePtr(width, height),
					imagTruth  = Image2D::CreateZeroImagePtr(width, height);
				realImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height))),
				imagImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height)));
				//realImage->MultiplyValues(0.5); //for different SNR
				//imagImage->MultiplyValues(0.5);
				rfiLessData = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
				rfiLessData.Trim(0, 0, 180, height);
				rfiLessImage = rfiLessData.GetSingleImage();
				TestSetGenerator::AddSinusoidalBroadbandToTestSet(*realImage, mask);
				TestSetGenerator::AddSinusoidalBroadbandToTestSet(*imagImage, mask);
				TestSetGenerator::AddSinusoidalBroadbandToTestSet(*realTruth, mask);
				TestSetGenerator::AddSinusoidalBroadbandToTestSet(*imagTruth, mask);
				groundTruth = *TimeFrequencyData(Polarization::StokesI, realTruth, imagTruth).GetSingleImage();
				data = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
			} break;
			case BurstBroadband:
			{
				Image2DPtr
					realTruth  = Image2D::CreateZeroImagePtr(width, height),
					imagTruth  = Image2D::CreateZeroImagePtr(width, height);
				realImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height))),
				imagImage.reset(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height)));
				//realImage->MultiplyValues(0.5); //for different SNR
				//imagImage->MultiplyValues(0.5);
				rfiLessData = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
				rfiLessData.Trim(0, 0, 180, height);
				rfiLessImage = rfiLessData.GetSingleImage();
				TestSetGenerator::AddBurstBroadbandToTestSet(*realImage, mask);
				TestSetGenerator::AddBurstBroadbandToTestSet(*imagImage, mask);
				TestSetGenerator::AddBurstBroadbandToTestSet(*realTruth, mask);
				TestSetGenerator::AddBurstBroadbandToTestSet(*imagTruth, mask);
				groundTruth = *TimeFrequencyData(Polarization::StokesI, realTruth, imagTruth).GetSingleImage();
				data = TimeFrequencyData(Polarization::StokesI, realImage, imagImage);
			} break;
		}
		std::cout << '_' << std::flush;
		
		data.Trim(0, 0, 180, height);
		groundTruth.SetTrim(0, 0, 180, height);
		groundTruth.MultiplyValues(1.0/groundTruth.GetMaximum());
		Image2DCPtr inputImage = data.GetSingleImage();
		
		rfiStrategy::ArtifactSet artifacts(0);
		
		artifacts.SetOriginalData(data);
		artifacts.SetContaminatedData(data);
		data.SetImagesToZero();
		artifacts.SetRevisedData(data);

		rfiStrategy::Strategy *strategy = createThresholdStrategy();
		DummyProgressListener listener;
		strategy->Perform(artifacts, listener);
		delete strategy;
		
		const size_t totalRFI = mask.GetCount<true>();
		
		Mask2DCPtr input = artifacts.ContaminatedData().GetSingleMask();
		for(unsigned i=0;i<ETA_STEPS+1;++i)
		{
			const num_t eta = (num_t) i / (num_t) ETA_STEPS;
			Mask2D resultMask = *input;
			SIROperator::OperateVertically(resultMask, eta);
			
			evaluateIterationResults(resultMask, mask, groundTruth, totalRFI, rocResults[i]);
		}
			
		std::cout << '.' << std::flush;
		
		for(size_t i=0;i<DIL_STEPS;++i)
		{
			const size_t dilSize = i * MAX_DILATION / DIL_STEPS;
			
			Mask2D resultMask = *input;
			MorphologicalFlagger::DilateFlagsVertically(&resultMask, dilSize);
			
			evaluateIterationResults(resultMask, mask, groundTruth, totalRFI, dilResults[i]);
		}
		
		//grTotalRFI += totalRFI;
		//grTotalRFISum += totalRFISum;
		
		std::cout << '.' << std::flush;
	} // next repetition
	
	const std::string rankOperatorFilename(std::string("rank-operator-roc-") + testname + ".txt");
	std::ofstream rankOperatorFile(rankOperatorFilename.c_str());
	for(unsigned i=0;i<ETA_STEPS+1;++i)
	{
		const num_t eta = (num_t) i / (num_t) ETA_STEPS;
			rankOperatorFile
			<< "eta\t" << eta
			<< "\tTP\t" << rocResults[i].tpRatio / _repeatCount
			<< "\tFP\t" << rocResults[i].fpRatio / _repeatCount
			<< "\ttpSum\t" << rocResults[i].tpFuzzy / _repeatCount
			<< "\tfpSum\t" << rocResults[i].fpFuzzy / _repeatCount;
			for(size_t j=0;j<rocResults[i].tpRefTP.size();++j)
			{
				rankOperatorFile << "\t" << rocResults[i].tpRefTP[j] << "\t" << rocResults[i].tpRefFP[j];
			}
			rankOperatorFile
			<< "\ttpSumStdDev\t" << stddev(rocResults[i].tpFuzzy, rocResults[i].tpFuzzySq, _repeatCount)
			<< "\tfpSumStdDev\t" << stddev(rocResults[i].fpFuzzy, rocResults[i].fpFuzzySq, _repeatCount);
			rankOperatorFile << '\n';
	}
	const std::string dilationFilename(std::string("dilation-roc-") + testname + ".txt");
	std::ofstream dilationFile(dilationFilename.c_str());
	for(size_t i=0;i<DIL_STEPS;++i)
	{
		const size_t dilSize = i * MAX_DILATION / DIL_STEPS;
		dilationFile
		<< "size\t" << dilSize
		<< "\tTP\t" << dilResults[i].tpRatio / _repeatCount
		<< "\tFP\t" << dilResults[i].fpRatio / _repeatCount
		<< "\ttpSum\t" << dilResults[i].tpFuzzy / _repeatCount
		<< "\tfpSum\t" << dilResults[i].fpFuzzy / _repeatCount;
			for(size_t j=0;j<dilResults[i].tpRefTP.size();++j)
			{
				dilationFile << "\t" << dilResults[i].tpRefTP[j] << "\t" << dilResults[i].tpRefFP[j];
			}
			dilationFile
			<< "\ttpSumStdDev\t" << stddev(dilResults[i].tpFuzzy, dilResults[i].tpFuzzySq, _repeatCount)
			<< "\tfpSumStdDev\t" << stddev(dilResults[i].fpFuzzy, dilResults[i].fpFuzzySq, _repeatCount);
			dilationFile << '\n';
	}
	
	//TestNoisePerformance(grTotalRFI / _repeatCount, grTotalRFISum / _repeatCount, testname);
}

inline void RankOperatorROCExperiment::TestNoisePerformance(size_t totalRFI, double totalRFISum, const std::string &testname)
{
	const size_t ETA_STEPS = 100, DIL_STEPS = 128;
	const size_t width = 1024, height = 1024;
	numl_t
		grRankFpRatio[ETA_STEPS+1], grRankFpSum[ETA_STEPS+1],
		grDilFpRatio[DIL_STEPS+1], grDilFpSum[DIL_STEPS+1];
		
	for(unsigned i=0;i<ETA_STEPS+1;++i)
	{
		grRankFpRatio[i] = 0.0;
		grRankFpSum[i] = 0.0;
	}
	for(unsigned i=0;i<DIL_STEPS+1;++i)
	{
		grDilFpRatio[i] = 0.0;
		grDilFpSum[i] = 0.0;
	}
	
	for(unsigned repeatIndex=0; repeatIndex<_repeatCount; ++repeatIndex)
	{
		Mask2D mask = Mask2D::MakeSetMask<false>(width, height);
		Image2DPtr
			realImage(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height))),
			imagImage(new Image2D(TestSetGenerator::MakeTestSet(2, mask, width, height)));
			
		TimeFrequencyData data(Polarization::StokesI, realImage, imagImage);
		data.Trim(0, 0, 180, height);
		Image2D inputImage = *data.GetSingleImage();
		
		rfiStrategy::ArtifactSet artifacts(0);
		
		artifacts.SetOriginalData(data);
		artifacts.SetContaminatedData(data);
		data.SetImagesToZero();
		artifacts.SetRevisedData(data);

		rfiStrategy::Strategy *strategy = createThresholdStrategy();
		DummyProgressListener listener;
		strategy->Perform(artifacts, listener);
		delete strategy;
		
		Mask2DCPtr input = artifacts.ContaminatedData().GetSingleMask();
		for(unsigned i=0;i<101;++i)
		{
			Mask2D tempMask(*input);
			const num_t eta = i/100.0;
			SIROperator::OperateVertically(tempMask, eta);
			size_t falsePositives = tempMask.GetCount<true>();
			tempMask.Invert();
			num_t fpSum = ThresholdTools::Sum(&inputImage, &tempMask);
			double fpRatio = (double) falsePositives / totalRFI;
			
			grRankFpRatio[i] += fpRatio;
			grRankFpSum[i] += fpSum/totalRFISum;
		}
		
		for(size_t i=0;i<DIL_STEPS;++i)
		{
			const size_t dilSize = i * height / (4 * DIL_STEPS);
			Mask2D tempMask = *input;
			MorphologicalFlagger::DilateFlags(&tempMask, 0, dilSize);
			size_t falsePositives = tempMask.GetCount<true>();
			tempMask.Invert();
			num_t fpSum = ThresholdTools::Sum(&inputImage, &tempMask);
			double fpRatio = (double) falsePositives / totalRFI;
			
			grDilFpRatio[i] += fpRatio;
			grDilFpSum[i] += fpSum/totalRFISum;
		}
		
		std::cout << '.' << std::flush;
	}
	
	const std::string rankOperatorFilename(std::string("rank-operator-noise-") + testname + ".txt");
	std::ofstream rankOperatorFile(rankOperatorFilename.c_str());
	for(unsigned i=0;i<ETA_STEPS+1;++i)
	{
		const num_t eta = (num_t) i / (num_t) ETA_STEPS;
			rankOperatorFile
			<< "eta\t" << eta
			<< "\tTP\t" << 0
			<< "\tFP\t" << grRankFpRatio[i] / _repeatCount
			<< "\ttpSum\t" << 0.0
			<< "\tfpSum\t" << grRankFpSum[i] / _repeatCount
			<< '\n';
	}
	const std::string dilationFilename(std::string("dilation-noise-") + testname + ".txt");
	std::ofstream dilationFile(dilationFilename.c_str());
	for(size_t i=0;i<DIL_STEPS;++i)
	{
		const size_t dilSize = i * height / (4 * DIL_STEPS);
		dilationFile
		<< "size\t" << dilSize
		<< "\tTP\t" << 0
		<< "\tFP\t" << grDilFpRatio[i] / _repeatCount
		<< "\ttpSum\t" << 0.0
		<< "\tfpSum\t" << grDilFpSum[i] / _repeatCount
		<< '\n';
	}
}

#endif
