#ifndef RFI_STRATEGY_TYPES
#define RFI_STRATEGY_TYPES

namespace rfiStrategy
{
	class Action;
	class ActionBlock;
	class ActionContainer;
	class ActionFactory;
	class ArtifactSet;
	class CombineFlagResults;
	class ForEachPolarisationBlock;
	class ImageSet;
	class ImageSetIndex;
	class IterationBlock;
	class MSImageSet;
	class Strategy;

	enum BaselineSelection
	{
		All, CrossCorrelations, AutoCorrelations, EqualToCurrent, AutoCorrelationsOfCurrentAntennae, Current
	};
}

class Model;
class Observatorium;

// The current file format version
// 1.0 : start
// 1.1 : add AddStatisticsAction
// 1.2 : add restore-originals property to Adapter
// 1.3 : removed LoadFlags en LoadImage actions.
// 1.4 : changed the ChangeResolutionAction to contain frequency res changing in addition to
//       time res changing (parameters were renamed).
// 1.5 : added the CutAreaAction and added "file-prefix" parameter of AddStatistics action.
// 1.6 : added the BaselineSelectionAction
// 1.7 : added "abs-threshold" and "smoothing-sigma" parameters to the BaselineSelectionAction 
// 1.8 : added "compare-original-and-alternative" parameter to AddStatisticsAction, added the enum
//       value ToOriginal in enum NewFlagging, pamarameter of SetFlaggingAction.
// 1.9 : allow formatting of the xml file by white space
// 2.0 : added the TimeconvolutionAction and the ForEachComplexComponentAction (to replace the Adapter)
// 2.1 : added min-window-size to FringeStopAction
// 2.2 : removed iterate-stokes-values parameter of ForEachBaselineAction, replaced width individual
//       on-xx, on-xy, ..., on-stokes-v & added enumeration value "SetFlaggedValuesToZero"
//       in SetImageAction.
// 2.3 : option "separate-baseline-statistics" added for AddStatisticsAction.
// 2.4 : added DirectionCleanAction, FourierTransformAction and UVProjectAction
// 2.5 : added enumeration value FromRevised in SetImageAction
// 2.6 : added "data-column-name", "subtract-model" and removed "data-kind" from ForEachBaselineAction, changed
//       some filter parameters.
// 2.7 : moved data-column-name etc. to the ForEachMSAction.
// 2.8 : added interpolate nans task
// 2.9 : removed precision parameter in SlidingWindowFit action.
// 2.10 : added perform-classification in AddToStatistics action.
//        at present, it seems the reader does not support a version of 2.10, as it treats it as a float
//        hence version renamed to:
// 3.0 : as 2.10
// 3.1 : added the CollectNoiseStatisticsAction.
// 3.2 : added parameter "write-immediately" to the Add Statistics Action.
// 3.3 : the meaning of statisticalflagaction::_minimumGoodTime- en FrequencyRatio was fixed, so that
//       higher values represent a more strengent requirement as one would expect. The default value was
//       changed to accomodate the change (used to be 0.8, now approx 0.2).
// 3.4 : Renamed ThresholdAction to SumThresholdAction
// 3.5 : Added the AbsThresholdAction
// 3.6 : Added the DirectionProfileAction and the EigenValueVerticalAction.
// 3.7 : Added the NormalizeVarianceAction
// 3.8 : Added attributes min-available-*-ratio to StatisticalFlagging, removed old
//       max-contaminated*-ratio.
// 3.9 : Split SumThreshold's base-sensitivity into time-direction-sensitivity and frequency-direction-sensitivity.
// 3.91 : Added the 'visualize in UI' action, added the 'exclude-original-flags' parameter
//        to statistical flagging action.
// 3.92 : Add method to CalibrateBandpassAction
// 3.93 : Add 'use-mask-in-averaging' property to ChangeResolutionAction
#define STRATEGY_FILE_FORMAT_VERSION 3.93

// The earliest format version which can be read by this version of the software
#define STRATEGY_FILE_FORMAT_VERSION_REQUIRED 3.4

// The earliest software version which is required to read the written files
#define STRATEGY_FILE_READER_VERSION_REQUIRED 3.93

#endif // RFI_STRATEGY_TYPES
