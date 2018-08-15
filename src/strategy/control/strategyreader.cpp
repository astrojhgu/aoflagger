#include "strategyreader.h"

#include "../../util/numberparser.h"

#include "../actions/all.h"

#define ENCODING "UTF-8"

namespace rfiStrategy {

int StrategyReader::useCount = 0;

StrategyReader::StrategyReader()
{
	if(useCount == 0)
	{
		LIBXML_TEST_VERSION ;
	}
	++useCount;
}


StrategyReader::~StrategyReader()
{
	if(useCount == 0)
		xmlCleanupParser();
}

std::unique_ptr<Strategy> StrategyReader::CreateStrategyFromFile(const std::string &filename)
{
	_xmlDocument = xmlReadFile(filename.c_str(), NULL, 0);
	if (_xmlDocument == NULL)
		throw StrategyReaderError("Failed to read file");

	xmlNode *rootElement = xmlDocGetRootElement(_xmlDocument);
	std::unique_ptr<Strategy> strategy;

	for (xmlNode *curNode=rootElement; curNode!=NULL; curNode=curNode->next)
	{
		if(curNode->type == XML_ELEMENT_NODE)
		{
			if(strategy != nullptr)
				throw StrategyReaderError("Multiple root elements found.");
			if(std::string((const char *) curNode->name) != "rfi-strategy")
				throw StrategyReaderError("Invalid structure in xml file: no rfi-strategy root node found. Maybe this is not an rfi strategy?");
			
			xmlChar *formatVersionCh = xmlGetProp(curNode, BAD_CAST "format-version");
			if(formatVersionCh == 0)
				throw StrategyReaderError("Missing attribute 'format-version'");
			double formatVersion = NumberParser::ToDouble((const char*) formatVersionCh);
			xmlFree(formatVersionCh);

			xmlChar *readerVersionRequiredCh = xmlGetProp(curNode, BAD_CAST "reader-version-required");
			if(readerVersionRequiredCh == 0)
				throw StrategyReaderError("Missing attribute 'reader-version-required'");
			double readerVersionRequired = NumberParser::ToDouble((const char*) readerVersionRequiredCh);
			xmlFree(readerVersionRequiredCh);
			
			if(readerVersionRequired > STRATEGY_FILE_FORMAT_VERSION)
				throw StrategyReaderError("This file requires a newer software version");
			if(formatVersion < STRATEGY_FILE_FORMAT_VERSION_REQUIRED)
			{
				std::stringstream s;
				s << "This file is too old for the software, please recreate the strategy. File format version: " << formatVersion << ", oldest version that this software understands: " << STRATEGY_FILE_FORMAT_VERSION_REQUIRED << " (these numbered are for the file format version, which is different from the version of the software).";
				throw StrategyReaderError(s.str());
			}
			
			strategy = parseRootChildren(curNode);
		}
	}
	if(strategy == nullptr)
		throw StrategyReaderError("Could not find root element in file.");

	xmlFreeDoc(_xmlDocument);

	return strategy;
}

std::unique_ptr<Strategy> StrategyReader::parseRootChildren(xmlNode *rootNode)
{
	std::unique_ptr<Strategy> strategy;
	for (xmlNode *curNode=rootNode->children; curNode!=NULL; curNode=curNode->next) {
		if(curNode->type == XML_ELEMENT_NODE)
		{
			if(strategy != nullptr)
				throw StrategyReaderError("More than one root element in file!");
			strategy = std::unique_ptr<Strategy>(dynamic_cast<Strategy*>(parseAction(curNode)));
			if(strategy == nullptr)
				throw StrategyReaderError("Root element was not a strategy!");
		}
	}
	if(strategy == nullptr)
		throw StrategyReaderError("Root element not found.");

	return strategy;
}

std::unique_ptr<Action> StrategyReader::parseChild(xmlNode *node)
{
	if (node->type == XML_ELEMENT_NODE) {
		std::string name((const char*) node->name);
		if(name == "action")
			return std::unique_ptr<Action>(parseAction(node));
	}
	throw StrategyReaderError("Invalid structure in xml file: an action was expected");
}

class Strategy *StrategyReader::parseStrategy(xmlNode *node)
{
	Strategy *strategy = new Strategy();
	parseChildren(node, strategy);
	return strategy;
}

void StrategyReader::parseChildren(xmlNode *node, ActionContainer *parent) 
{
	for (xmlNode *curOuterNode=node->children; curOuterNode!=NULL; curOuterNode=curOuterNode->next) {
		if(curOuterNode->type == XML_ELEMENT_NODE)
		{
			std::string nameStr((const char *) curOuterNode->name);
			if(nameStr == "children")
			{
				for (xmlNode *curNode=curOuterNode->children; curNode!=NULL; curNode=curNode->next) {
					if (curNode->type == XML_ELEMENT_NODE) {
						parent->Add(parseChild(curNode));
					}
				}
			}
		}
	}
}

xmlNode *StrategyReader::getTextNode(xmlNode *node, const char *subNodeName, bool allowEmpty) const 
{
	for (xmlNode *curNode=node->children; curNode!=NULL; curNode=curNode->next) {
		if(curNode->type == XML_ELEMENT_NODE)
		{
			std::string nameStr((const char *) curNode->name);
			if(nameStr == subNodeName)
			{
				curNode = curNode->children;
				if(curNode == 0 || curNode->type != XML_TEXT_NODE)
				{
					if(allowEmpty)
						return 0;
					else
						throw StrategyReaderError("Error occured in reading xml file: value node did not contain text");
				}
				return curNode;
			}
		}
	}
	if(allowEmpty)
		return 0;
	else {
		std::ostringstream str;
		str << "Error occured in reading xml file: could not find value node \"" << subNodeName << '\"';
		throw StrategyReaderError(str.str());
	}
}

int StrategyReader::getInt(xmlNode *node, const char *name) const 
{
	xmlNode *valNode = getTextNode(node, name);
	return atoi((const char *) valNode->content);
}

int StrategyReader::getIntOr(xmlNode *node, const char *name, int alternative) const 
{
	xmlNode *valNode = getTextNode(node, name, true);
	if(valNode == nullptr)
		return alternative;
	else
		return atoi((const char *) valNode->content);
}

double StrategyReader::getDouble(xmlNode *node, const char *name) const 
{
	xmlNode *valNode = getTextNode(node, name);
	return NumberParser::ToDouble((const char *) valNode->content);
}

double StrategyReader::getDoubleOr(xmlNode *node, const char *name, double alternative) const 
{
	xmlNode *valNode = getTextNode(node, name, true);
	if(valNode == nullptr)
		return alternative;
	else
		return NumberParser::ToDouble((const char *) valNode->content);
}

std::string StrategyReader::getString(xmlNode *node, const char *name) const 
{
	xmlNode *valNode = getTextNode(node, name, true);
	if(valNode == 0)
		return std::string();
	else
		return std::string((const char *) valNode->content);
}

Action *StrategyReader::parseAction(xmlNode *node)
{
	Action *newAction = 0;
	xmlChar *typeCh = xmlGetProp(node, BAD_CAST "type");
	if(typeCh == 0)
		throw StrategyReaderError("Action tag did not have 'type' parameter");
	std::string typeStr((const char*) typeCh);
	if(typeStr == "AbsThresholdAction")
		newAction = parseAbsThresholdAction(node);
	else if(typeStr == "BaselineSelectionAction")
		newAction = parseBaselineSelectionAction(node);
	else if(typeStr == "CalibratePassbandAction")
		newAction = parseCalibratePassbandAction(node);
	else if(typeStr == "ChangeResolutionAction")
		newAction = parseChangeResolutionAction(node);
	else if(typeStr == "CombineFlagResults")
		newAction = parseCombineFlagResults(node);
	else if(typeStr == "CutAreaAction")
		newAction = parseCutAreaAction(node);
	else if(typeStr == "EigenValueVerticalAction")
	  newAction = parseEigenValueVerticalAction(node);
	else if(typeStr == "ForEachBaselineAction")
		newAction = parseForEachBaselineAction(node);
	else if(typeStr == "ForEachComplexComponentAction")
		newAction = parseForEachComplexComponentAction(node);
	else if(typeStr == "ForEachMSAction")
		newAction = parseForEachMSAction(node);
	else if(typeStr == "ForEachPolarisationBlock")
		newAction = parseForEachPolarisationBlock(node);
	else if(typeStr == "FrequencyConvolutionAction")
		newAction = parseFrequencyConvolutionAction(node);
	else if(typeStr == "FrequencySelectionAction")
		newAction = parseFrequencySelectionAction(node);
	else if(typeStr == "FringeStopAction")
		newAction = parseFringeStopAction(node);
	else if(typeStr == "HighPassFilterAction")
		newAction = parseHighPassFilterAction(node);
	else if(typeStr == "ImagerAction")
		newAction = parseImagerAction(node);
	else if(typeStr == "IterationBlock")
		newAction = parseIterationBlock(node);
	else if(typeStr == "NormalizeVarianceAction")
		newAction = parseNormalizeVarianceAction(node);
	else if(typeStr == "PlotAction")
		newAction = parsePlotAction(node);
	else if(typeStr == "QuickCalibrateAction")
		newAction = parseQuickCalibrateAction(node);
	else if(typeStr == "SetFlaggingAction")
		newAction = parseSetFlaggingAction(node);
	else if(typeStr == "SetImageAction")
		newAction = parseSetImageAction(node);
	else if(typeStr == "SlidingWindowFitAction")
		newAction = parseSlidingWindowFitAction(node);
	else if(typeStr == "StatisticalFlagAction")
		newAction = parseMorphologicalFlagAction(node);
	else if(typeStr == "SVDAction")
		newAction = parseSVDAction(node);
	else if(typeStr == "Strategy")
		newAction = parseStrategy(node);
	else if(typeStr == "SumThresholdAction")
		newAction = parseSumThresholdAction(node);
	else if(typeStr == "TimeConvolutionAction")
		newAction = parseTimeConvolutionAction(node);
	else if(typeStr == "TimeSelectionAction")
		newAction = parseTimeSelectionAction(node);
	else if(typeStr == "VisualizeAction")
		newAction = parseVisualizeAction(node);
	else if(typeStr == "WriteDataAction")
		newAction = parseWriteDataAction(node);
	else if(typeStr == "WriteFlagsAction")
		newAction = parseWriteFlagsAction(node);
	xmlFree(typeCh);
	if(newAction == 0)
	{
		std::stringstream s;
		s << "Unknown action type '" << typeStr << "' in xml file";
		throw StrategyReaderError(s.str());
	}
	return newAction;
}

Action *StrategyReader::parseAbsThresholdAction(xmlNode *node)
{
	AbsThresholdAction *newAction = new AbsThresholdAction();
	newAction->SetThreshold(getDouble(node, "threshold"));
	return newAction;
}

/*
Action *StrategyReader::parseAddStatistics(xmlNode *node)
{
	AddStatisticsAction *newAction = new AddStatisticsAction();
	newAction->SetFilePrefix(getString(node, "file-prefix"));
	newAction->SetCompareOriginalAndAlternative(getBool(node, "compare-original-and-alternative"));
	newAction->SetSeparateBaselineStatistics(getBool(node, "separate-baseline-statistics"));
	newAction->SetPerformClassification(getBool(node, "perform-classification"));
	newAction->SetWriteImmediately(getBool(node, "write-immediately"));
	return newAction;
}*/

Action *StrategyReader::parseBaselineSelectionAction(xmlNode *node)
{
	BaselineSelectionAction *newAction = new BaselineSelectionAction();
	newAction->SetPreparationStep(getBool(node, "preparation-step"));
	newAction->SetFlagBadBaselines(getBool(node, "flag-bad-baselines"));
	newAction->SetThreshold(getDouble(node, "threshold"));
	newAction->SetAbsThreshold(getDouble(node, "abs-threshold"));
	newAction->SetSmoothingSigma(getDouble(node, "smoothing-sigma"));
	newAction->SetMakePlot(getBool(node, "make-plot"));
	return newAction;
}

Action *StrategyReader::parseCalibratePassbandAction(xmlNode *node)
{
	CalibratePassbandAction *newAction = new CalibratePassbandAction();
	newAction->SetSteps(getInt(node, "steps"));
	return newAction;
}

Action *StrategyReader::parseChangeResolutionAction(xmlNode *node)
{
	ChangeResolutionAction *newAction = new ChangeResolutionAction();
	newAction->SetTimeDecreaseFactor(getInt(node, "time-decrease-factor"));
	newAction->SetFrequencyDecreaseFactor(getInt(node, "frequency-decrease-factor"));
	newAction->SetRestoreRevised(getBool(node, "restore-revised"));
	newAction->SetRestoreMasks(getBool(node, "restore-masks"));
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseCombineFlagResults(xmlNode *node)
{
	CombineFlagResults *newAction = new CombineFlagResults();
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseCutAreaAction(xmlNode *node)
{
	CutAreaAction *newAction = new CutAreaAction();
	newAction->SetStartTimeSteps(getInt(node, "start-time-steps"));
	newAction->SetEndTimeSteps(getInt(node, "end-time-steps"));
	newAction->SetTopChannels(getInt(node, "top-channels"));
	newAction->SetBottomChannels(getInt(node, "bottom-channels"));
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseEigenValueVerticalAction(xmlNode *)
{
  EigenValueVerticalAction *newAction = new EigenValueVerticalAction();
  return newAction;
}

Action *StrategyReader::parseForEachBaselineAction(xmlNode *node)
{
	ForEachBaselineAction *newAction = new ForEachBaselineAction();
	newAction->SetSelection((BaselineSelection) getInt(node, "selection"));
	newAction->SetThreadCount(getInt(node, "thread-count"));

	for (xmlNode *curNode=node->children; curNode!=NULL; curNode=curNode->next) {
		if(curNode->type == XML_ELEMENT_NODE)
		{
			std::string nameStr((const char *) curNode->name);
			if(nameStr == "antennae-to-skip")
			{
				for (xmlNode *curNode2=curNode->children; curNode2!=NULL; curNode2=curNode2->next) {
					if (curNode2->type == XML_ELEMENT_NODE) {
						std::string innerNameStr((const char *) curNode2->name);
						if(innerNameStr != "antenna")
							throw StrategyReaderError("Format of the for each baseline action is incorrect");
						xmlNode *textNode = curNode2->children;
						if(textNode->type != XML_TEXT_NODE)
							throw StrategyReaderError("Error occured in reading xml file: value node did not contain text");
						if(textNode->content != NULL)
						{
							newAction->AntennaeToSkip().insert(atoi((const char *) textNode->content));
						}
					}
				}
			}
			if(nameStr == "antennae-to-include")
			{
				for (xmlNode *curNode2=curNode->children; curNode2!=NULL; curNode2=curNode2->next) {
					if (curNode2->type == XML_ELEMENT_NODE) {
						std::string innerNameStr((const char *) curNode2->name);
						if(innerNameStr != "antenna")
							throw StrategyReaderError("Format of the for each baseline action is incorrect");
						xmlNode *textNode = curNode2->children;
						if(textNode->type != XML_TEXT_NODE)
							throw StrategyReaderError("Error occured in reading xml file: value node did not contain text");
						if(textNode->content != NULL)
						{
							newAction->AntennaeToInclude().insert(atoi((const char *) textNode->content));
						}
					}
				}
			}
		}
	}
	
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseForEachComplexComponentAction(xmlNode *node)
{
	ForEachComplexComponentAction *newAction = new ForEachComplexComponentAction();
	newAction->SetOnAmplitude(getBool(node, "on-amplitude"));
	newAction->SetOnPhase(getBool(node, "on-phase"));
	newAction->SetOnReal(getBool(node, "on-real"));
	newAction->SetOnImaginary(getBool(node, "on-imaginary"));
	newAction->SetRestoreFromAmplitude(getBool(node, "restore-from-amplitude"));
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseForEachMSAction(xmlNode *node)
{
	ForEachMSAction *newAction = new ForEachMSAction();
	newAction->SetDataColumnName(getString(node, "data-column-name"));
	newAction->SetSubtractModel(getBool(node, "subtract-model"));

	for (xmlNode *curNode=node->children; curNode!=NULL; curNode=curNode->next) {
		if(curNode->type == XML_ELEMENT_NODE)
		{
			std::string nameStr((const char *) curNode->name);
			if(nameStr == "filenames")
			{
				for (xmlNode *curNode2=curNode->children; curNode2!=NULL; curNode2=curNode2->next) {
					if (curNode2->type == XML_ELEMENT_NODE) {
						std::string innerNameStr((const char *) curNode2->name);
						if(innerNameStr != "filename")
							throw StrategyReaderError("Format of the for each MS action is incorrect");
						newAction->Filenames().push_back((const char *) curNode2->content);
					}
				}
			}
		}
	}
	
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseForEachPolarisationBlock(xmlNode *node)
{
	ForEachPolarisationBlock *newAction = new ForEachPolarisationBlock();
	newAction->SetOnPP(getBool(node, "on-xx"));
	newAction->SetOnPQ(getBool(node, "on-xy"));
	newAction->SetOnQP(getBool(node, "on-yx"));
	newAction->SetOnQQ(getBool(node, "on-yy"));
	newAction->SetOnStokesI(getBool(node, "on-stokes-i"));
	newAction->SetOnStokesQ(getBool(node, "on-stokes-q"));
	newAction->SetOnStokesU(getBool(node, "on-stokes-u"));
	newAction->SetOnStokesV(getBool(node, "on-stokes-v"));
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseFrequencyConvolutionAction(xmlNode *node)
{
	FrequencyConvolutionAction *newAction = new FrequencyConvolutionAction();
	newAction->SetConvolutionSize(getInt(node, "convolution-size"));
	newAction->SetKernelKind((enum FrequencyConvolutionAction::KernelKind) getInt(node, "kernel-kind"));
	return newAction;
}

Action *StrategyReader::parseFrequencySelectionAction(xmlNode *node)
{
	FrequencySelectionAction *newAction = new FrequencySelectionAction();
	newAction->SetThreshold(getDouble(node, "threshold"));
	return newAction;
}

Action *StrategyReader::parseFringeStopAction(xmlNode *node)
{
	FringeStopAction *newAction = new FringeStopAction();
	newAction->SetFitChannelsIndividually(getBool(node, "fit-channels-individually"));
	newAction->SetFringesToConsider(getDouble(node, "fringes-to-consider"));
	newAction->SetOnlyFringeStop(getBool(node, "only-fringe-stop"));
	newAction->SetMinWindowSize(getInt(node, "min-window-size"));
	newAction->SetMaxWindowSize(getInt(node, "max-window-size"));
	return newAction;
}

Action *StrategyReader::parseHighPassFilterAction(xmlNode *node)
{
	HighPassFilterAction *newAction = new HighPassFilterAction();
	newAction->SetHKernelSigmaSq(getDouble(node, "horizontal-kernel-sigma-sq"));
	newAction->SetVKernelSigmaSq(getDouble(node, "vertical-kernel-sigma-sq"));
	newAction->SetWindowWidth(getInt(node, "window-width"));
	newAction->SetWindowHeight(getInt(node, "window-height"));
	newAction->SetMode((enum HighPassFilterAction::Mode) getInt(node, "mode"));
	return newAction;
}

Action *StrategyReader::parseImagerAction(xmlNode *)
{
	ImagerAction *newAction = new ImagerAction();
	return newAction;
}

Action *StrategyReader::parseIterationBlock(xmlNode *node)
{
	IterationBlock *newAction = new IterationBlock();
	newAction->SetIterationCount(getInt(node, "iteration-count"));
	newAction->SetSensitivityStart(getDouble(node, "sensitivity-start"));
	parseChildren(node, newAction);
	return newAction;
}

Action *StrategyReader::parseNormalizeVarianceAction(xmlNode *node)
{
	NormalizeVarianceAction *newAction = new NormalizeVarianceAction();
	newAction->SetMedianFilterSizeInS(getDouble(node, "median-filter-size-in-s"));
	return newAction;
}

Action *StrategyReader::parseSetFlaggingAction(xmlNode *node)
{
	SetFlaggingAction *newAction = new SetFlaggingAction();
	newAction->SetNewFlagging((enum SetFlaggingAction::NewFlagging) getInt(node, "new-flagging"));
	return newAction;
}

Action *StrategyReader::parsePlotAction(xmlNode *node)
{
	PlotAction *newAction = new PlotAction();
	newAction->SetPlotKind((enum PlotAction::PlotKind) getInt(node, "plot-kind"));
	newAction->SetLogarithmicYAxis(getBool(node, "logarithmic-y-axis"));
	return newAction;
}

Action *StrategyReader::parseQuickCalibrateAction(xmlNode *)
{
	QuickCalibrateAction *newAction = new QuickCalibrateAction();
	return newAction;
}

class Action *StrategyReader::parseSetImageAction(xmlNode *node)
{
	SetImageAction *newAction = new SetImageAction();
	newAction->SetNewImage((enum SetImageAction::NewImage) getInt(node, "new-image"));
	return newAction;
}

class Action *StrategyReader::parseSlidingWindowFitAction(xmlNode *node)
{
	SlidingWindowFitAction *newAction = new SlidingWindowFitAction();
	newAction->Parameters().frequencyDirectionKernelSize = getDouble(node, "frequency-direction-kernel-size");
	newAction->Parameters().frequencyDirectionWindowSize = getInt(node, "frequency-direction-window-size");
	newAction->Parameters().method = (enum SlidingWindowFitParameters::Method) getInt(node, "method");
	newAction->Parameters().timeDirectionKernelSize = getDouble(node, "time-direction-kernel-size");
	newAction->Parameters().timeDirectionWindowSize = getInt(node, "time-direction-window-size");
	return newAction;
}

class Action *StrategyReader::parseMorphologicalFlagAction(xmlNode *node)
{
	MorphologicalFlagAction* newAction = new MorphologicalFlagAction();
	newAction->SetEnlargeFrequencySize(getInt(node, "enlarge-frequency-size"));
	newAction->SetEnlargeTimeSize(getInt(node, "enlarge-time-size"));
	newAction->SetMinAvailableFrequenciesRatio(getDoubleOr(node, "min-available-frequencies-ratio", 0.0));
	newAction->SetMinAvailableTimesRatio(getDoubleOr(node, "min-available-times-ratio", 0.0));
	newAction->SetMinAvailableTFRatio(getDoubleOr(node, "min-available-tf-ratio", 0.0));
	newAction->SetMinimumGoodFrequencyRatio(getDouble(node, "minimum-good-frequency-ratio"));
	newAction->SetMinimumGoodTimeRatio(getDouble(node, "minimum-good-time-ratio"));
	newAction->SetExcludeOriginalFlags(getBoolOr(node, "exclude-original-flags", false));
	return newAction;
}

class Action *StrategyReader::parseSVDAction(xmlNode *node)
{
	SVDAction *newAction = new SVDAction();
	newAction->SetSingularValueCount(getInt(node, "singular-value-count"));
	return newAction;
}

class Action *StrategyReader::parseSumThresholdAction(xmlNode *node)
{
	SumThresholdAction *newAction = new SumThresholdAction();
	double timeSensitivity = getDoubleOr(node, "time-direction-sensitivity", -1.0);
	double freqSensitivity = getDoubleOr(node, "frequency-direction-sensitivity", -1.0);
	if(timeSensitivity==-1.0 || freqSensitivity == -1.0)
	{
		timeSensitivity = getDouble(node, "base-sensitivity");
		freqSensitivity = timeSensitivity;
	}
	newAction->SetTimeDirectionSensitivity(timeSensitivity);
	newAction->SetFrequencyDirectionSensitivity(freqSensitivity);
	newAction->SetTimeDirectionFlagging(getBool(node, "time-direction-flagging"));
	newAction->SetFrequencyDirectionFlagging(getBool(node, "frequency-direction-flagging"));
	newAction->SetExcludeOriginalFlags(getBoolOr(node, "exclude-original-flags", false));
	return newAction;
}

class Action *StrategyReader::parseTimeConvolutionAction(xmlNode *node)
{
	TimeConvolutionAction *newAction = new TimeConvolutionAction();
	newAction->SetOperation((enum TimeConvolutionAction::Operation) getInt(node, "operation"));
	newAction->SetSincScale(getDouble(node, "sinc-scale"));
	newAction->SetIsSincScaleInSamples(getBool(node, "is-sinc-scale-in-samples"));
	newAction->SetDirectionRad(getDouble(node, "direction-rad"));
	newAction->SetEtaParameter(getDouble(node, "eta-parameter"));
	newAction->SetAutoAngle(getBool(node, "auto-angle"));
	newAction->SetIterations(getInt(node, "iterations"));
	return newAction;
}

class Action *StrategyReader::parseTimeSelectionAction(xmlNode *node)
{
	TimeSelectionAction *newAction = new TimeSelectionAction();
	newAction->SetThreshold(getDouble(node, "threshold"));
	return newAction;
}

class Action *StrategyReader::parseVisualizeAction(xmlNode *node)
{
	VisualizeAction* newAction = new VisualizeAction();
	newAction->SetLabel(getString(node, "label"));
	std::string sourceStr = getString(node, "source");
	if(sourceStr == "original")
		newAction->SetSource(VisualizeAction::FromOriginal);
	else if(sourceStr == "revised")
		newAction->SetSource(VisualizeAction::FromRevised);
	else if(sourceStr == "contaminated")
		newAction->SetSource(VisualizeAction::FromContaminated);
	else
		throw StrategyReaderError("Incorrect 'source' given in visualize action");
	newAction->SetSortingIndex(getInt(node, "sorting-index"));
	return newAction;
}

class Action *StrategyReader::parseWriteDataAction(xmlNode *)
{
	WriteDataAction *newAction = new WriteDataAction();
	return newAction;
}

class Action *StrategyReader::parseWriteFlagsAction(xmlNode *)
{
	WriteFlagsAction *newAction = new WriteFlagsAction();
	return newAction;
}

} // end of namespace
