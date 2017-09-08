#ifndef AOFLAGGER_DEFAULTSTRATEGYSPEEDTEST_H
#define AOFLAGGER_DEFAULTSTRATEGYSPEEDTEST_H

#include "../testingtools/asserter.h"
#include "../testingtools/unittest.h"

#include "../../strategy/algorithms/mitigationtester.h"
#include "../../strategy/algorithms/siroperator.h"
#include "../../strategy/algorithms/thresholdmitigater.h"

#include "../../strategy/actions/baselineselectionaction.h"
#include "../../strategy/actions/changeresolutionaction.h"
#include "../../strategy/actions/combineflagresultsaction.h"
#include "../../strategy/actions/foreachcomplexcomponentaction.h"
#include "../../strategy/actions/foreachpolarisationaction.h"
#include "../../strategy/actions/frequencyselectionaction.h"
#include "../../strategy/actions/highpassfilteraction.h"
#include "../../strategy/actions/iterationaction.h"
#include "../../strategy/actions/plotaction.h"
#include "../../strategy/actions/setflaggingaction.h"
#include "../../strategy/actions/setimageaction.h"
#include "../../strategy/actions/slidingwindowfitaction.h"
#include "../../strategy/actions/statisticalflagaction.h"
#include "../../strategy/actions/strategy.h"
#include "../../strategy/actions/sumthresholdaction.h"
#include "../../strategy/actions/timeselectionaction.h"

#include "../../strategy/control/artifactset.h"
#include "../../strategy/control/defaultstrategy.h"

#include "../../structures/timefrequencydata.h"

#include "../../util/aologger.h"

class DefaultStrategySpeedTest : public UnitTest {
	public:
		DefaultStrategySpeedTest() : UnitTest("Default strategy speed test")
		{
			if(false)
			{
				AddTest(TimeLoopUntilAmplitude(), "Timing loop until amplitude");
				AddTest(TimeLoop(), "Timing loop");
				AddTest(TimeSumThreshold(), "Timing SumThreshold method");
				AddTest(TimeRankOperator(), "Timing scale-invariant rank operator");
				//AddTest(TimeSumThresholdN(), "Timing varying SumThreshold method");
			}
			AddTest(TimeSlidingWindowFit(), "Timing sliding window fit");
			AddTest(TimeHighPassFilter(), "Timing high-pass filter");
			AddTest(TimeStrategy(), "Timing strategy");
#ifdef __SSE__
			AddTest(TimeSSEHighPassFilterStrategy(), "Timing SSE high-pass filter strategy");
#endif
		}
		
		explicit DefaultStrategySpeedTest(const std::string &) : UnitTest("Default strategy speed test")
		{
			AddTest(TimeSumThresholdN(), "Timing varying SumThreshold method");
		}
		
	private:
		struct TimeStrategy : public Asserter
		{
			void operator()();
		};
		struct TimeSlidingWindowFit : public Asserter
		{
			void operator()();
		};
		struct TimeHighPassFilter : public Asserter
		{
			void operator()();
		};
		struct TimeLoop : public Asserter
		{
			void operator()();
		};
		struct TimeLoopUntilAmplitude : public Asserter
		{
			void operator()();
		};
		struct TimeSumThreshold : public Asserter
		{
			void operator()();
		};
		struct TimeSumThresholdN : public Asserter
		{
			void operator()();
		};
		struct TimeRankOperator : public Asserter
		{
			void operator()();
		};
#ifdef __SSE__
		struct TimeSSEHighPassFilterStrategy : public Asserter
		{
			void operator()();
		};
#endif
		
		static void prepareStrategy(rfiStrategy::ArtifactSet &artifacts);
};

inline void DefaultStrategySpeedTest::prepareStrategy(rfiStrategy::ArtifactSet &artifacts)
{
	const unsigned
		width = 10000,
		height = 256;
	Mask2DPtr rfi = Mask2D::CreateUnsetMaskPtr(width, height);
	Image2DPtr
		xxReal = MitigationTester::CreateTestSet(26, rfi, width, height),
		xxImag = MitigationTester::CreateTestSet(26, rfi, width, height),
		xyReal = MitigationTester::CreateTestSet(26, rfi, width, height),
		xyImag = MitigationTester::CreateTestSet(26, rfi, width, height),
		yxReal = MitigationTester::CreateTestSet(26, rfi, width, height),
		yxImag = MitigationTester::CreateTestSet(26, rfi, width, height),
		yyReal = MitigationTester::CreateTestSet(26, rfi, width, height),
		yyImag = MitigationTester::CreateTestSet(26, rfi, width, height);
	TimeFrequencyData data = TimeFrequencyData::FromLinear(
		xxReal, xxImag, xyReal, xyImag,
		yxReal, yxImag, yyReal, yyImag);
	artifacts.SetOriginalData(data);
	artifacts.SetContaminatedData(data);
	data.SetImagesToZero();
	artifacts.SetRevisedData(data);
}

inline void DefaultStrategySpeedTest::TimeStrategy::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);
	std::unique_ptr<rfiStrategy::Strategy> strategy = rfiStrategy::DefaultStrategy::CreateStrategy(
		rfiStrategy::DefaultStrategy::GENERIC_TELESCOPE, rfiStrategy::DefaultStrategy::FLAG_NONE
	);
	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy->Perform(artifacts, progressListener);
	AOLogger::Info << "Default strategy took: " << watch.ToString() << '\n';
}

inline void DefaultStrategySpeedTest::TimeSlidingWindowFit::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);
	rfiStrategy::ActionBlock *current, *scratch;

	rfiStrategy::Strategy strategy;
	
	std::unique_ptr<rfiStrategy::ForEachPolarisationBlock> fepBlock(new rfiStrategy::ForEachPolarisationBlock());
	current = fepBlock.get();
	strategy.Add(std::move(fepBlock));

	std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> focAction(new rfiStrategy::ForEachComplexComponentAction());
	focAction->SetOnAmplitude(true);
	focAction->SetOnImaginary(false);
	focAction->SetOnReal(false);
	focAction->SetOnPhase(false);
	focAction->SetRestoreFromAmplitude(false);
	scratch = focAction.get();
	current->Add(std::move(focAction));
	current = scratch;

	std::unique_ptr<rfiStrategy::IterationBlock> iteration(new rfiStrategy::IterationBlock());
	iteration->SetIterationCount(2);
	iteration->SetSensitivityStart(4.0);
	scratch = iteration.get();
	current->Add(std::move(iteration));
	current = scratch;
	
	std::unique_ptr<rfiStrategy::ChangeResolutionAction> changeResAction2(new rfiStrategy::ChangeResolutionAction());
	changeResAction2->SetTimeDecreaseFactor(3);
	changeResAction2->SetFrequencyDecreaseFactor(3);

	std::unique_ptr<rfiStrategy::SlidingWindowFitAction> swfAction2(new rfiStrategy::SlidingWindowFitAction());
	swfAction2->Parameters().timeDirectionKernelSize = 2.5;
	swfAction2->Parameters().timeDirectionWindowSize = 10;
	swfAction2->Parameters().frequencyDirectionKernelSize = 5.0;
	swfAction2->Parameters().frequencyDirectionWindowSize = 15;
	changeResAction2->Add(std::move(swfAction2));

	current->Add(std::move(changeResAction2));
	
	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy.Perform(artifacts, progressListener);
	AOLogger::Info << "Sliding window fit took (loop + fit): " << watch.ToString() << '\n';
}

inline void DefaultStrategySpeedTest::TimeHighPassFilter::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);
	rfiStrategy::ActionBlock *current, *scratch;

	rfiStrategy::Strategy strategy;
	
	std::unique_ptr<rfiStrategy::ForEachPolarisationBlock> fepBlock(new rfiStrategy::ForEachPolarisationBlock());
	scratch = fepBlock.get();
	strategy.Add(std::move(fepBlock));
	current = scratch;

	std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> focAction(new rfiStrategy::ForEachComplexComponentAction());
	focAction->SetOnAmplitude(true);
	focAction->SetOnImaginary(false);
	focAction->SetOnReal(false);
	focAction->SetOnPhase(false);
	focAction->SetRestoreFromAmplitude(false);
	scratch = focAction.get();
	current->Add(std::move(focAction));
	current = scratch;

	std::unique_ptr<rfiStrategy::IterationBlock> iteration(new rfiStrategy::IterationBlock());
	iteration->SetIterationCount(2);
	iteration->SetSensitivityStart(4.0);
	scratch = iteration.get();
	current->Add(std::move(iteration));
	current = scratch;
	
	std::unique_ptr<rfiStrategy::ChangeResolutionAction> changeResAction2(new rfiStrategy::ChangeResolutionAction());
	changeResAction2->SetTimeDecreaseFactor(3);
	changeResAction2->SetFrequencyDecreaseFactor(3);

	std::unique_ptr<rfiStrategy::HighPassFilterAction> hpAction(new rfiStrategy::HighPassFilterAction());
	hpAction->SetHKernelSigmaSq(2.5);
	hpAction->SetWindowWidth(10);
	hpAction->SetVKernelSigmaSq(5.0);
	hpAction->SetWindowHeight(15);
	hpAction->SetMode(rfiStrategy::HighPassFilterAction::StoreRevised);
	changeResAction2->Add(std::move(hpAction));

	current->Add(std::move(changeResAction2));
	
	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy.Perform(artifacts, progressListener);
	AOLogger::Info << "High-pass filter took (loop + fit): " << watch.ToString() << '\n';
}

inline void DefaultStrategySpeedTest::TimeLoop::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);
	rfiStrategy::ActionBlock *current, *scratch;

	rfiStrategy::Strategy strategy;
	
	std::unique_ptr<rfiStrategy::ForEachPolarisationBlock> fepBlock(new rfiStrategy::ForEachPolarisationBlock());
	scratch = fepBlock.get();
	strategy.Add(std::move(fepBlock));
	current = scratch;

	std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> focAction(new rfiStrategy::ForEachComplexComponentAction());
	focAction->SetOnAmplitude(true);
	focAction->SetOnImaginary(false);
	focAction->SetOnReal(false);
	focAction->SetOnPhase(false);
	focAction->SetRestoreFromAmplitude(false);
	scratch = focAction.get();
	current->Add(std::move(focAction));
	current = scratch;

	std::unique_ptr<rfiStrategy::IterationBlock> iteration(new rfiStrategy::IterationBlock());
	iteration->SetIterationCount(2);
	iteration->SetSensitivityStart(4.0);
	scratch = iteration.get();
	current->Add(std::move(iteration));
	current = scratch;
	
	std::unique_ptr<rfiStrategy::ChangeResolutionAction> changeResAction2(new rfiStrategy::ChangeResolutionAction());
	changeResAction2->SetTimeDecreaseFactor(3);
	changeResAction2->SetFrequencyDecreaseFactor(3);
	current->Add(std::move(changeResAction2));
	
	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy.Perform(artifacts, progressListener);
	AOLogger::Info << "Loop took: " << watch.ToString() << '\n';
}

inline void DefaultStrategySpeedTest::TimeLoopUntilAmplitude::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);

	rfiStrategy::Strategy strategy;
	
	std::unique_ptr<rfiStrategy::ForEachPolarisationBlock> fepBlock(new rfiStrategy::ForEachPolarisationBlock());
	
	std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> focAction(new rfiStrategy::ForEachComplexComponentAction());
	focAction->SetOnAmplitude(true);
	focAction->SetOnImaginary(false);
	focAction->SetOnReal(false);
	focAction->SetOnPhase(false);
	focAction->SetRestoreFromAmplitude(false);
	fepBlock->Add(std::move(focAction));

	strategy.Add(std::move(fepBlock));

	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy.Perform(artifacts, progressListener);
	AOLogger::Info << "Loop took: " << watch.ToString() << '\n';
}

inline void DefaultStrategySpeedTest::TimeSumThreshold::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);

	rfiStrategy::Strategy strategy;
	
	std::unique_ptr<rfiStrategy::ForEachPolarisationBlock> fepBlock(new rfiStrategy::ForEachPolarisationBlock());

	std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> focAction(new rfiStrategy::ForEachComplexComponentAction());
	focAction->SetOnAmplitude(true);
	focAction->SetOnImaginary(false);
	focAction->SetOnReal(false);
	focAction->SetOnPhase(false);
	focAction->SetRestoreFromAmplitude(false);

	std::unique_ptr<rfiStrategy::IterationBlock> iteration(new rfiStrategy::IterationBlock());
	iteration->SetIterationCount(2);
	iteration->SetSensitivityStart(4.0);
	
	std::unique_ptr<rfiStrategy::SumThresholdAction> t2(new rfiStrategy::SumThresholdAction());
	t2->SetBaseSensitivity(1.0);
	iteration->Add(std::move(t2));
		
	std::unique_ptr<rfiStrategy::ChangeResolutionAction> changeResAction2(new rfiStrategy::ChangeResolutionAction());
	changeResAction2->SetTimeDecreaseFactor(3);
	changeResAction2->SetFrequencyDecreaseFactor(3);
	iteration->Add(std::move(changeResAction2));
	
	std::unique_ptr<rfiStrategy::SumThresholdAction> t3(new rfiStrategy::SumThresholdAction());
	focAction->Add(std::move(t3));
	
	focAction->Add(std::move(iteration));
	fepBlock->Add(std::move(focAction));
	strategy.Add(std::move(fepBlock));
		
	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy.Perform(artifacts, progressListener);
	AOLogger::Info << "Sum threshold took (loop + threshold): " << watch.ToString() << '\n';
}

inline void DefaultStrategySpeedTest::TimeSumThresholdN::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);
	prepareStrategy(artifacts);

	ThresholdConfig config;
	config.InitializeLengthsDefault(9);
	num_t stddev = artifacts.OriginalData().GetSingleImage()->GetStdDev();
	num_t mode = artifacts.OriginalData().GetSingleImage()->GetMode();
	AOLogger::Info << "Stddev: " << stddev << '\n';
	AOLogger::Info << "Mode: " << mode << '\n';
	config.InitializeThresholdsFromFirstThreshold(6.0 * stddev, ThresholdConfig::Rayleigh);
	for(unsigned i=0;i<9;++i)
	{
		const unsigned length = config.GetHorizontalLength(i);
		const double threshold = config.GetHorizontalThreshold(i);
		Image2DCPtr input = artifacts.OriginalData().GetSingleImage();
		
		Mask2DPtr maskA = Mask2D::CreateCopy(artifacts.OriginalData().GetSingleMask());
		Stopwatch watchA(true);
		ThresholdMitigater::HorizontalSumThresholdLargeReference(input.get(), maskA.get(), length, threshold);
		AOLogger::Info << "Horizontal, length " << length << ": " << watchA.ToString() << '\n';
		
#ifdef __SSE__
		Mask2DPtr maskC = Mask2D::CreateCopy(artifacts.OriginalData().GetSingleMask());
		Stopwatch watchC(true);
		ThresholdMitigater::HorizontalSumThresholdLargeSSE(input.get(), maskC.get(), length, threshold);
		AOLogger::Info << "Horizontal SSE, length " << length << ": " << watchC.ToString() << '\n';
#endif
		
		Mask2DPtr maskB = Mask2D::CreateCopy(artifacts.OriginalData().GetSingleMask());
		Stopwatch watchB(true);
		ThresholdMitigater::VerticalSumThresholdLargeReference(input.get(), maskB.get(), length, threshold);
		AOLogger::Info << "Vertical, length " << length << ": " << watchB.ToString() << '\n';
		
#ifdef __SSE__
		Mask2DPtr maskD = Mask2D::CreateCopy(artifacts.OriginalData().GetSingleMask());
		Stopwatch watchD(true);
		ThresholdMitigater::VerticalSumThresholdLargeSSE(input.get(), maskD.get(), length, threshold);
		AOLogger::Info << "SSE Vertical, length " << length << ": " << watchD.ToString() << '\n';
#endif
	}
}

inline void DefaultStrategySpeedTest::TimeRankOperator::operator()()
{
	rfiStrategy::ArtifactSet artifacts(0);

	std::unique_ptr<rfiStrategy::Strategy> strategy = rfiStrategy::DefaultStrategy::CreateStrategy(
		rfiStrategy::DefaultStrategy::GENERIC_TELESCOPE, rfiStrategy::DefaultStrategy::FLAG_NONE
	);
	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy->Perform(artifacts, progressListener);
	watch.Pause();
	strategy.reset();
	
	Mask2DPtr input = Mask2D::CreateCopy(artifacts.ContaminatedData().GetSingleMask());
	
	Stopwatch operatorTimer(true);
	SIROperator::OperateHorizontally(input, 0.2);
	SIROperator::OperateVertically(input, 0.2);
	operatorTimer.Pause();
	
	long double operatorTime = operatorTimer.Seconds();
	long double totalTime = watch.Seconds();
	
	AOLogger::Info
		<< "Rank operator took " << operatorTimer.ToShortString() << " (" << operatorTime << ")"
		<< " of " << watch.ToShortString() << " (" << totalTime << ")"
		<< ", " << ( operatorTime * 100.0 / totalTime) << "%\n";
}

#ifdef __SSE__
inline void DefaultStrategySpeedTest::TimeSSEHighPassFilterStrategy::operator()()
{
	std::unique_ptr<rfiStrategy::Strategy> strategy(new rfiStrategy::Strategy());
	rfiStrategy::ActionBlock &block = *strategy;
	rfiStrategy::ActionBlock *current, *scratch;

	block.Add(std::unique_ptr<rfiStrategy::SetFlaggingAction>(new rfiStrategy::SetFlaggingAction()));

	std::unique_ptr<rfiStrategy::ForEachPolarisationBlock> fepBlock(new rfiStrategy::ForEachPolarisationBlock());
	scratch = fepBlock.get();
	block.Add(std::move(fepBlock));
	current = scratch;

	std::unique_ptr<rfiStrategy::ForEachComplexComponentAction> focAction(new rfiStrategy::ForEachComplexComponentAction());
	focAction->SetOnAmplitude(true);
	focAction->SetOnImaginary(false);
	focAction->SetOnReal(false);
	focAction->SetOnPhase(false);
	focAction->SetRestoreFromAmplitude(false);
	rfiStrategy::ForEachComplexComponentAction *focActionPtr = focAction.get();
	current->Add(std::move(focAction));
	current = focActionPtr;

	std::unique_ptr<rfiStrategy::IterationBlock> iteration(new rfiStrategy::IterationBlock());
	iteration->SetIterationCount(2);
	iteration->SetSensitivityStart(4.0);
	scratch = iteration.get();
	current->Add(std::move(iteration));
	current = scratch;
	
	std::unique_ptr<rfiStrategy::SumThresholdAction> t2(new rfiStrategy::SumThresholdAction());
	t2->SetBaseSensitivity(1.0);
	current->Add(std::move(t2));

	std::unique_ptr<rfiStrategy::CombineFlagResults> cfr2(new rfiStrategy::CombineFlagResults());
	current->Add(std::move(cfr2));

	cfr2->Add(std::unique_ptr<rfiStrategy::FrequencySelectionAction>(new rfiStrategy::FrequencySelectionAction()));
	cfr2->Add(std::unique_ptr<rfiStrategy::TimeSelectionAction>(new rfiStrategy::TimeSelectionAction()));

	current->Add(std::unique_ptr<rfiStrategy::SetImageAction>(new rfiStrategy::SetImageAction()));
	std::unique_ptr<rfiStrategy::ChangeResolutionAction>
		changeResAction2(new rfiStrategy::ChangeResolutionAction());
	changeResAction2->SetTimeDecreaseFactor(3);
	changeResAction2->SetFrequencyDecreaseFactor(3);

	std::unique_ptr<rfiStrategy::HighPassFilterAction> hpAction(new rfiStrategy::HighPassFilterAction());
	hpAction->SetHKernelSigmaSq(2.5);
	hpAction->SetWindowWidth(10*2+1);
	hpAction->SetVKernelSigmaSq(5.0);
	hpAction->SetWindowHeight(15*2+1);
	hpAction->SetMode(rfiStrategy::HighPassFilterAction::StoreRevised);
	changeResAction2->Add(std::move(hpAction));

	current->Add(std::move(changeResAction2));

	current = focActionPtr;
	std::unique_ptr<rfiStrategy::SumThresholdAction> t3(new rfiStrategy::SumThresholdAction());
	current->Add(std::move(t3));
	
	std::unique_ptr<rfiStrategy::PlotAction> plotPolarizationStatistics(new rfiStrategy::PlotAction());
	plotPolarizationStatistics->SetPlotKind(rfiStrategy::PlotAction::PolarizationStatisticsPlot);
	block.Add(std::move(plotPolarizationStatistics));
	
	std::unique_ptr<rfiStrategy::SetFlaggingAction>
		setFlagsInAllPolarizations(new rfiStrategy::SetFlaggingAction());
	setFlagsInAllPolarizations->SetNewFlagging(rfiStrategy::SetFlaggingAction::PolarisationsEqual);
	
	block.Add(std::move(setFlagsInAllPolarizations));
	block.Add(std::unique_ptr<rfiStrategy::StatisticalFlagAction>(new rfiStrategy::StatisticalFlagAction()));
	block.Add(std::unique_ptr<rfiStrategy::TimeSelectionAction>(new rfiStrategy::TimeSelectionAction()));

	std::unique_ptr<rfiStrategy::BaselineSelectionAction>
		baselineSelection(new rfiStrategy::BaselineSelectionAction());
	baselineSelection->SetPreparationStep(true);
	block.Add(std::move(baselineSelection));

	std::unique_ptr<rfiStrategy::SetFlaggingAction> orWithOriginals(new rfiStrategy::SetFlaggingAction());
	orWithOriginals->SetNewFlagging(rfiStrategy::SetFlaggingAction::OrOriginal);
	block.Add(std::move(orWithOriginals));

	rfiStrategy::ArtifactSet artifacts(0);
	prepareStrategy(artifacts);
	DummyProgressListener progressListener;
	Stopwatch watch(true);
	strategy->Perform(artifacts, progressListener);
	AOLogger::Info << "Default strategy took: " << watch.ToString() << '\n';
}
#endif // __SSE__

#endif
