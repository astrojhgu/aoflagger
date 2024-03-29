#include "actionfactory.h"

#include "../actions/all.h"

namespace rfiStrategy {

const std::vector<std::string> ActionFactory::GetActionList()
{
	std::vector<std::string> list;
	list.push_back("Absolute threshold");
	list.push_back("Apply bandpass");
	list.push_back("Baseline selection");
	list.push_back("Calibrate passband");
	list.push_back("Change resolution");
	list.push_back("Combine flag results");
	list.push_back("Cut area");
	list.push_back("Eigen value decompisition (vertical)");
	list.push_back("For each baseline");
	list.push_back("For each complex component");
	list.push_back("For each polarisation");
	list.push_back("For each simulated baseline");
	list.push_back("For each measurement set");
	list.push_back("Frequency convolution");
	list.push_back("Frequency selection");
	list.push_back("Fringe stopping recovery");
	list.push_back("High-pass filter");
	list.push_back("Image");
	list.push_back("Iteration");
	list.push_back("Normalize variance");
	list.push_back("Phase adapter");
	list.push_back("Plot");
	list.push_back("Quickly calibrate");
	list.push_back("Resample");
	list.push_back("Save heat map");
	list.push_back("Set flagging");
	list.push_back("Set image");
	list.push_back("Singular value decomposition");
	list.push_back("Sliding window fit");
	list.push_back("Statistical flagging");
	list.push_back("SumThreshold");
	list.push_back("Time convolution");
	list.push_back("Time selection");
	list.push_back("Visualization");
	list.push_back("Write data");
	list.push_back("Write flags");
	return list;
}

std::unique_ptr<Action> ActionFactory::CreateAction(const std::string &action)
{
	if(action == "Absolute threshold")
		return make<AbsThresholdAction>();
	else if(action == "Apply bandpass")
		return make<ApplyBandpassAction>();
	else if(action == "Baseline selection")
		return make<BaselineSelectionAction>();
	else if(action == "Calibrate passband")
		return make<CalibrateBandpassAction>();
	else if(action == "Change resolution")
		return make<ChangeResolutionAction>();
	else if(action == "Combine flag results")
		return make<CombineFlagResults>();
	else if(action == "Cut area")
		return make<CutAreaAction>();
	else if(action == "Eigen value decompisition (vertical)")
		return make<EigenValueVerticalAction>();
	else if(action == "For each baseline")
		return make<ForEachBaselineAction>();
	else if(action == "For each complex component")
		return make<ForEachComplexComponentAction>();
	else if(action == "For each measurement set")
		return make<ForEachMSAction>();
	else if(action == "For each polarisation")
		return make<ForEachPolarisationBlock>();
	else if(action == "For each simulated baseline")
		return make<ForEachSimulatedBaselineAction>();
	else if(action == "Frequency convolution")
		return make<FrequencyConvolutionAction>();
	else if(action == "Frequency selection")
		return make<FrequencySelectionAction>();
	else if(action == "Fringe stopping recovery")
		return make<FringeStopAction>();
	else if(action == "High-pass filter")
		return make<HighPassFilterAction>();
	else if(action == "Image")
		return make<ImagerAction>();
	else if(action == "Iteration")
		return make<IterationBlock>();
	else if(action == "Normalize variance")
		return make<NormalizeVarianceAction>();
	else if(action == "Plot")
		return make<PlotAction>();
	else if(action == "Quickly calibrate")
		return make<QuickCalibrateAction>();
	else if(action == "Resample")
		return make<ResamplingAction>();
	else if(action == "Save heat map")
		return make<SaveHeatMapAction>();
	else if(action == "Set flagging")
		return make<SetFlaggingAction>();
	else if(action == "Set image")
		return make<SetImageAction>();
	else if(action == "Singular value decomposition")
		return make<SVDAction>();
	else if(action == "Sliding window fit")
		return make<SlidingWindowFitAction>();
	else if(action == "Statistical flagging")
		return make<MorphologicalFlagAction>();
	else if(action == "SumThreshold")
		return make<SumThresholdAction>();
	else if(action == "Time convolution")
		return make<TimeConvolutionAction>();
	else if(action == "Time selection")
		return make<TimeSelectionAction>();
	else if(action == "Visualization")
		return make<VisualizeAction>();
	else if(action == "Write data")
		return make<WriteDataAction>();
	else if(action == "Write flags")
		return make<WriteFlagsAction>();
	else
		throw BadUsageException(std::string("Trying to create unknown action \"") + action + "\"");
}

const char *ActionFactory::GetDescription(ActionType action)
{
	switch(action)
	{
		case ChangeResolutionActionType:
			return
				"Changes the resolution of the time frequency data currently in memory. This is "
				"part of the algorithm and should normally not be changed. Currently changes only "
				"the time direction.";
		case CombineFlagResultsType:
			return
				"Runs each of its children and combines the flags (by OR-ing) afterwards.";
		case ForEachBaselineActionType:
			return
				"Iterate over baselines in the measurement set. Parameters: selection : which "
				"baselines to iterate over (0. All, 1. CrossCorrelations, 2. AutoCorrelations, "
				"3. Baselines that are redundant to current selected, 4. Auto correlations of "
				"the current antenna, 5. don't iterate, only process current), thread-count : "
				"number of threads to spawn simultaneously.";
		case ForEachPolarisationBlockType:
			return
				"Iterate over the polarisations that are already in memory. Note that the "
				"LoadImageAction actually defines which polarisations are read in memory in "
				"the first place. This action has no parameters.";
		case FrequencySelectionActionType:
			return
				"Flag frequency channels that are very different from other channels.";
		case SetFlaggingActionType:
			return
				"This is an action that is part of the algorithm and should normally not be "
				"changed. "
				"It initializes or changes the flags in memory. Its single parameter new-flagging "
				"defines how to initialize or set the flags (0. None, 1. Everything, 2. FromOriginal, "
				"3. Invert, 4. PolarisationsEqual, 5. FlagZeros).";
		case SetImageActionType:
			return
				"(Re-)initialize the time frequency data in memory. Parameter 'new-image' defines "
				"how to initialize (0 means set the entire image to zero, 1 means that, if the "
				"image has been changed by e.g. a surface fit, restore the image (a copy "
				"of the original data is left in memory, so this does not perform IO). This is "
				"part of the algorithm and should normally not be changed.";
		case SlidingWindowFitActionType:
			return
				"Performs a surface fit / smoothing operation and subtracts the fit from the data. "
				"The window size parameters are currently absolute values, and the default values "
				"have been optimized for LOFAR 1 Hz 1 sec resolution. Nevertheless, these "
				"settings work well for other configurations as well (and surface fitting is not "
				"the most crucial part of the flagger - because of the SumThreshold method, even "
				"bad fits produce reasonable results). Note that the ChangeResolutionAction"
				"changes the meaning of these absolute values as well.";
		case TimeSelectionActionType:
			return
				"Flag time scans that are very different from other time steps";
		case SumThresholdActionType:
			return
				"This executes the SumThreshold method. It has parameters to change its sensitivity and "
				"to set whether the method is allowed to look to combinations of time and frequency directions. "
				"If you expect strong transient effects that you do not want to flag, set frequency-direction-flagging "
				"to 0 to make sure that frequencies are not combined. Not that the algorithm normally iterates "
				"and executes the SumThreshold method several times with different sensitivities. Therefore, if you "
				"like to change the sensitivity "
				"of the algorithm in order to flag less or more samples, you have to change the sensitivity of all "
				"Threshold actions by the same factor. A higher sensitivity-value means that less values are flagged.";
		case StatisticalFlagActionType:
			return
				"This action implements a novel and rather complex method to flag samples that are likely RFI "
				"based on its surrounding flags. It is sort of a dillation.";
		case WriteFlagsActionType:
			return
				"Write the newly constructed flags to the measurement set. Normally it is executed once at the end "
				"of the algorithm.";
		default: return 0;
	}
}

} // namespace
