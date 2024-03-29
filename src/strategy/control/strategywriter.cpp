#include "strategywriter.h"

#include "../actions/all.h"

#include "../../version.h"

namespace rfiStrategy {

	void StrategyWriter::write(const Strategy &strategy)
	{
		_describedActions.clear();

		std::string commentStr = 
			"This is a strategy configuration file for the AOFlagger RFI\n"
			"detector by André Offringa (offringa@gmail.com).\n"
			"Created by AOFlagger " AOFLAGGER_VERSION_STR " (" AOFLAGGER_VERSION_DATE_STR ")\n";
		if(_writeDescriptions)
			commentStr += "\nIf you like to take a look at the structure of this file,\n"
				"try opening it in e.g. Firefox.\n";

		Comment(commentStr.c_str());

		Start("rfi-strategy");
		Attribute("format-version", STRATEGY_FILE_FORMAT_VERSION);
		Attribute("reader-version-required", STRATEGY_FILE_READER_VERSION_REQUIRED);
		writeAction(strategy);
		End();

		Close();
	}
	
	void StrategyWriter::writeAction(const Action &action)
	{
		if(_writeDescriptions)
		{
			if(_describedActions.count(action.Type()) == 0)
			{
				const char *description = ActionFactory::GetDescription(action.Type());
				if(description != 0)
					Comment(wrap(description, 70).c_str());
				_describedActions.insert(action.Type());
			}
		}

		Start("action");
		switch(action.Type())
		{
			case AbsThresholdActionType:
				writeAbsThresholdAction(static_cast<const AbsThresholdAction&>(action));
				break;
			case ActionBlockType:
				throw std::runtime_error("Can not store action blocks");
			case ApplyBandpassType:
				writeApplyBandpassAction(static_cast<const ApplyBandpassAction&>(action));
				break;
			case BaselineSelectionActionType:
				writeBaselineSelectionAction(static_cast<const BaselineSelectionAction&>(action));
				break;
			case CalibrateBandpassActionType:
				writeCalibrateBandpassAction(static_cast<const CalibrateBandpassAction&>(action));
				break;
			case ChangeResolutionActionType:
				writeChangeResolutionAction(static_cast<const ChangeResolutionAction&>(action));
				break;
			case CombineFlagResultsType:
				writeCombineFlagResults(static_cast<const CombineFlagResults&>(action));
				break;
			case CutAreaActionType:
				writeCutAreaAction(static_cast<const CutAreaAction&>(action));
				break;
			case EigenValueVerticalActionType:
				writeEigenValueVerticalAction(static_cast<const EigenValueVerticalAction&>(action));
				break;
			case ForEachBaselineActionType:
				writeForEachBaselineAction(static_cast<const ForEachBaselineAction&>(action));
				break;
			case ForEachComplexComponentActionType:
				writeForEachComplexComponentAction(static_cast<const ForEachComplexComponentAction&>(action));
				break;
			case ForEachMSActionType:
				writeForEachMSAction(static_cast<const ForEachMSAction&>(action));
				break;
			case ForEachPolarisationBlockType:
				writeForEachPolarisationBlock(static_cast<const ForEachPolarisationBlock&>(action));
				break;
			case FrequencyConvolutionActionType:
				writeFrequencyConvolutionAction(static_cast<const FrequencyConvolutionAction&>(action));
				break;
			case FrequencySelectionActionType:
				writeFrequencySelectionAction(static_cast<const FrequencySelectionAction&>(action));
				break;
			case FringeStopActionType:
				writeFringeStopAction(static_cast<const FringeStopAction&>(action));
				break;
			case HighPassFilterActionType:
				writeHighPassFilterAction(static_cast<const HighPassFilterAction&>(action));
				break;
			case ImagerActionType:
				writeImagerAction(static_cast<const ImagerAction&>(action));
				break;
			case IterationBlockType:
				writeIterationBlock(static_cast<const IterationBlock&>(action));
				break;
			case NormalizeVarianceActionType:
				writeNormalizeVarianceAction(static_cast<const NormalizeVarianceAction&>(action));
				break;
			case PlotActionType:
				writePlotAction(static_cast<const PlotAction&>(action));
				break;
			case QuickCalibrateActionType:
				writeQuickCalibrateAction(static_cast<const QuickCalibrateAction&>(action));
				break;
			case SetFlaggingActionType:
				writeSetFlaggingAction(static_cast<const SetFlaggingAction&>(action));
				break;
			case SetImageActionType:
				writeSetImageAction(static_cast<const SetImageAction&>(action));
				break;
			case SlidingWindowFitActionType:
				writeSlidingWindowFitAction(static_cast<const SlidingWindowFitAction&>(action));
				break;
			case StatisticalFlagActionType:
				writeMorphologicalFlagAction(static_cast<const MorphologicalFlagAction&>(action));
				break;
			case StrategyType:
				writeStrategy(static_cast<const Strategy&>(action));
				break;
			case SVDActionType:
				writeSVDAction(static_cast<const SVDAction&>(action));
				break;
			case SumThresholdActionType:
				writeSumThresholdAction(static_cast<const SumThresholdAction&>(action));
				break;
			case TimeConvolutionActionType:
				writeTimeConvolutionAction(static_cast<const TimeConvolutionAction&>(action));
			break;
			case TimeSelectionActionType:
				writeTimeSelectionAction(static_cast<const TimeSelectionAction&>(action));
				break;
			case VisualizeActionType:
				writeVisualizeAction(static_cast<const VisualizeAction&>(action));
				break;
			case WriteDataActionType:
				writeWriteDataAction(static_cast<const WriteDataAction&>(action));
				break;
			case WriteFlagsActionType:
				writeWriteFlagsAction(static_cast<const WriteFlagsAction&>(action));
				break;
			case ForEachSimulatedBaselineActionType:
			case ResamplingActionType:
			case SaveHeatMapActionType:
				throw std::runtime_error("Strategy contains an action for which saving is not supported");
				break;
		}
		End();
	}
	
	void StrategyWriter::writeContainerItems(const ActionContainer &actionContainer)
	{
		Start("children");
		for(size_t i=0;i<actionContainer.GetChildCount();++i)
		{
			writeAction(actionContainer.GetChild(i));
		}
		End();
	}
	
	void StrategyWriter::writeApplyBandpassAction(const class ApplyBandpassAction& action)
	{
		Attribute("type", "ApplyBandpassAction");
		Write("filename", action.Filename());
	}
	
	void StrategyWriter::writeAbsThresholdAction(const AbsThresholdAction &action)
	{
		Attribute("type", "AbsThreshold");
		Write<num_t>("threshold", action.Threshold());
	}
	
	void StrategyWriter::writeBaselineSelectionAction(const class BaselineSelectionAction &action)
	{
		Attribute("type", "BaselineSelectionAction");
		Write<bool>("preparation-step", action.PreparationStep());
		Write<bool>("flag-bad-baselines", action.FlagBadBaselines());
		Write<num_t>("threshold", action.Threshold());
		Write<num_t>("smoothing-sigma", action.SmoothingSigma());
		Write<num_t>("abs-threshold", action.AbsThreshold());
		Write<bool>("make-plot", action.MakePlot());
	}

	void StrategyWriter::writeCalibrateBandpassAction(const CalibrateBandpassAction &action)
	{
		// Note that the 'CalibrateBandpassAction' was unfortunately misnamed in the xml
		// strategy files. Due to backward compatibility it is better not to fix this.
		Attribute("type", "CalibratePassbandAction");
		Write<int>("method", int(action.GetMethod()));
		Write<int>("steps", action.Steps());
	}

	void StrategyWriter::writeChangeResolutionAction(const ChangeResolutionAction &action)
	{
		Attribute("type", "ChangeResolutionAction");
		Write<int>("time-decrease-factor", action.TimeDecreaseFactor());
		Write<int>("frequency-decrease-factor", action.FrequencyDecreaseFactor());
		Write<bool>("restore-revised", action.RestoreRevised());
		Write<bool>("restore-masks", action.RestoreMasks());
		Write<bool>("use-mask-in-averaging", action.UseMaskInAveraging());
		writeContainerItems(action);
	}

	void StrategyWriter::writeCombineFlagResults(const CombineFlagResults &action)
	{
		Attribute("type", "CombineFlagResults");
		writeContainerItems(action);
	}

	void StrategyWriter::writeCutAreaAction(const CutAreaAction &action)
	{
		Attribute("type", "CutAreaAction");
		Write<int>("start-time-steps", action.StartTimeSteps());
		Write<int>("end-time-steps", action.EndTimeSteps());
		Write<int>("top-channels", action.TopChannels());
		Write<int>("bottom-channels", action.BottomChannels());
		writeContainerItems(action);
	}

  void StrategyWriter::writeEigenValueVerticalAction(const EigenValueVerticalAction &)
  {
    Attribute("type", "EigenValueVerticalAction");
  }

	void StrategyWriter::writeForEachBaselineAction(const ForEachBaselineAction &action)
	{
		Attribute("type", "ForEachBaselineAction");
		Write<int>("selection", action.Selection());
		Write<int>("thread-count", action.ThreadCount());
		writeContainerItems(action);
	}

	void StrategyWriter::writeForEachComplexComponentAction(const ForEachComplexComponentAction &action)
	{
		Attribute("type", "ForEachComplexComponentAction");
		Write<bool>("on-amplitude", action.OnAmplitude());
		Write<bool>("on-phase", action.OnPhase());
		Write<bool>("on-real", action.OnReal());
		Write<bool>("on-imaginary", action.OnImaginary());
		Write<bool>("restore-from-amplitude", action.RestoreFromAmplitude());
		writeContainerItems(action);
	}

	void StrategyWriter::writeForEachMSAction(const ForEachMSAction &action)
	{
		Attribute("type", "ForEachMSAction");
		Start("filenames");
		Write("data-column-name", action.DataColumnName());
		Write<bool>("subtract-model", action.SubtractModel());
		const std::vector<std::string> &filenames = action.Filenames();
		for(std::vector<std::string>::const_iterator i=filenames.begin();i!=filenames.end();++i)
		{
			Write("filename", i->c_str());
		}
		End();
		writeContainerItems(action);
	}

	void StrategyWriter::writeForEachPolarisationBlock(const ForEachPolarisationBlock &action)
	{
		Attribute("type", "ForEachPolarisationBlock");
		Write<bool>("on-xx", action.OnPP());
		Write<bool>("on-xy", action.OnPQ());
		Write<bool>("on-yx", action.OnQP());
		Write<bool>("on-yy", action.OnQQ());
		Write<bool>("on-stokes-i", action.OnStokesI());
		Write<bool>("on-stokes-q", action.OnStokesQ());
		Write<bool>("on-stokes-u", action.OnStokesU());
		Write<bool>("on-stokes-v", action.OnStokesV());
		writeContainerItems(action);
	}

	void StrategyWriter::writeFourierTransformAction(const FourierTransformAction &)
	{
		Attribute("type", "FourierTransformAction");
	}

	void StrategyWriter::writeFrequencyConvolutionAction(const FrequencyConvolutionAction &action)
	{
		Attribute("type", "FrequencyConvolutionAction");
		Write<double>("convolution-size", action.ConvolutionSize());
		Write<int>("kernel-kind", (int) action.KernelKind());
	}

	void StrategyWriter::writeFrequencySelectionAction(const FrequencySelectionAction &action)
	{
		Attribute("type", "FrequencySelectionAction");
		Write<double>("threshold", action.Threshold());
	}

	void StrategyWriter::writeFringeStopAction(const FringeStopAction &action)
	{
		Attribute("type", "FringeStopAction");
		Write<bool>("fit-channels-individually", action.FitChannelsIndividually());
		Write<num_t>("fringes-to-consider", action.FringesToConsider());
		Write<bool>("only-fringe-stop", action.OnlyFringeStop());
		Write<int>("min-window-size", action.MinWindowSize());
		Write<int>("max-window-size", action.MaxWindowSize());
	}

	void StrategyWriter::writeHighPassFilterAction(const HighPassFilterAction &action)
	{
		Attribute("type", "HighPassFilterAction");
		Write<num_t>("horizontal-kernel-sigma-sq", action.HKernelSigmaSq());
		Write<num_t>("vertical-kernel-sigma-sq", action.VKernelSigmaSq());
		Write<int>("window-width", action.WindowWidth());
		Write<int>("window-height", action.WindowHeight());
		Write<int>("mode", action.Mode());
	}

	void StrategyWriter::writeImagerAction(const ImagerAction &)
	{
		Attribute("type", "ImagerAction");
	}

	void StrategyWriter::writeIterationBlock(const IterationBlock &action)
	{
		Attribute("type", "IterationBlock");
		Write<int>("iteration-count", action.IterationCount());
		Write<double>("sensitivity-start", action.SensitivityStart());
		writeContainerItems(action);
	}

	void StrategyWriter::writeNormalizeVarianceAction(const NormalizeVarianceAction &action)
	{
		Attribute("type", "NormalizeVarianceAction");
		Write<double>("median-filter-size-in-s", action.MedianFilterSizeInS());
	}

	void StrategyWriter::writePlotAction(const PlotAction &action)
	{
		Attribute("type", "PlotAction");
		Write<int>("plot-kind", action.PlotKind());
		Write<bool>("logarithmic-y-axis", action.LogarithmicYAxis());
	}

	void StrategyWriter::writeQuickCalibrateAction(const QuickCalibrateAction &)
	{
		Attribute("type", "QuickCalibrateAction");
	}

	void StrategyWriter::writeRawAppenderAction(const RawAppenderAction &)
	{
		Attribute("type", "RawAppenderAction");
	}

	void StrategyWriter::writeSetFlaggingAction(const SetFlaggingAction &action)
	{
		Attribute("type", "SetFlaggingAction");
		Write<int>("new-flagging", action.NewFlagging());
	}

	void StrategyWriter::writeSetImageAction(const SetImageAction &action)
	{
		Attribute("type", "SetImageAction");
		Write<int>("new-image", action.NewImage());
	}

	void StrategyWriter::writeSlidingWindowFitAction(const SlidingWindowFitAction &action)
	{
		Attribute("type", "SlidingWindowFitAction");
		Write<num_t>("frequency-direction-kernel-size", action.Parameters().frequencyDirectionKernelSize);
		Write<int>("frequency-direction-window-size", action.Parameters().frequencyDirectionWindowSize);
		Write<int>("method", action.Parameters().method);
		Write<num_t>("time-direction-kernel-size", action.Parameters().timeDirectionKernelSize);
		Write<int>("time-direction-window-size", action.Parameters().timeDirectionWindowSize);
	}

	void StrategyWriter::writeMorphologicalFlagAction(const MorphologicalFlagAction &action)
	{
		Attribute("type", "StatisticalFlagAction");
		Write<size_t>("enlarge-frequency-size", action.EnlargeFrequencySize());
		Write<size_t>("enlarge-time-size", action.EnlargeTimeSize());
		Write<num_t>("min-available-frequencies-ratio", action.MinAvailableFrequenciesRatio());
		Write<num_t>("min-available-times-ratio", action.MinAvailableTimesRatio());
		Write<num_t>("min-available-tf-ratio", action.MinAvailableTFRatio());
		Write<num_t>("minimum-good-frequency-ratio", action.MinimumGoodFrequencyRatio());
		Write<num_t>("minimum-good-time-ratio", action.MinimumGoodTimeRatio());
		Write<bool>("exclude-original-flags", action.ExcludeOriginalFlags());
	}

	void StrategyWriter::writeStrategy(const class Strategy &action)
	{
		Attribute("type", "Strategy");
		writeContainerItems(action);
	}

	void StrategyWriter::writeSVDAction(const SVDAction &action)
	{
		Attribute("type", "SVDAction");
		Write<int>("singular-value-count", action.SingularValueCount());
	}

	void StrategyWriter::writeSumThresholdAction(const SumThresholdAction &action)
	{
		Attribute("type", "SumThresholdAction");
		Write<num_t>("time-direction-sensitivity", action.TimeDirectionSensitivity());
		Write<num_t>("frequency-direction-sensitivity", action.FrequencyDirectionSensitivity());
		Write<bool>("time-direction-flagging", action.TimeDirectionFlagging());
		Write<bool>("frequency-direction-flagging", action.FrequencyDirectionFlagging());
		Write<bool>("exclude-original-flags", action.ExcludeOriginalFlags());
	}

	void StrategyWriter::writeTimeConvolutionAction(const TimeConvolutionAction &action)
	{
		Attribute("type", "TimeConvolutionAction");
		Write<int>("operation", (int) action.Operation());
		Write<num_t>("sinc-scale", action.SincScale());
		Write<bool>("is-sinc-scale-in-samples", action.IsSincScaleInSamples());
		Write<num_t>("direction-rad", action.DirectionRad());
		Write<num_t>("eta-parameter", action.EtaParameter());
		Write<bool>("auto-angle", action.AutoAngle());
		Write<unsigned>("iterations", action.Iterations());
	}

	void StrategyWriter::writeTimeSelectionAction(const TimeSelectionAction &action)
	{
		Attribute("type", "TimeSelectionAction");
		Write<double>("threshold", action.Threshold());
	}
	
	void StrategyWriter::writeVisualizeAction(const VisualizeAction& action)
	{
		Attribute("type", "VisualizeAction");
		Write("label", action.Label());
		switch(action.Source())
		{
			default:
			case VisualizeAction::FromOriginal:
				Write("source", "original");
				break;
			case VisualizeAction::FromRevised:
				Write("source", "revised");
				break;
			case VisualizeAction::FromContaminated:
				Write("source", "contaminated");
				break;
		}
		Write("sorting-index", action.SortingIndex());
	}

	void StrategyWriter::writeWriteDataAction(const WriteDataAction &)
	{
		Attribute("type", "WriteDataAction");
	}

	void StrategyWriter::writeWriteFlagsAction(const WriteFlagsAction &)
	{
		Attribute("type", "WriteFlagsAction");
	}

	std::string StrategyWriter::wrap(const std::string &input, size_t max) const
	{
		int start = 0;
		bool first = true;
		std::stringstream s;
		int length = input.size();
		while(start < length)
		{
			int end = start + max;
			if(end > length)
				end = length;
			else {
				do {
					--end;
				} while(end > start && input[end] != ' ');
				if(end <= start)
					end = start + max;
				else
					++end;
			}
			int nextStart = end;
			while(end > start && input[end-1] == ' ') --end;

			if(!first)
				s << "\n";
			for(int i=start;i<end;++i)
				s << input[i];
				
			first = false;
			start = nextStart;
		}
		return s.str();
	}
}
