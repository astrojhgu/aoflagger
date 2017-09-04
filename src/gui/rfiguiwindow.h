#ifndef MSWINDOW_H
#define MSWINDOW_H

#include <set>
#include <mutex>
#include <memory>

#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/paned.h>
#include <gtkmm/radioaction.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/window.h>

#include "../structures/timefrequencydata.h"
#include "../structures/timefrequencymetadata.h"

#include "../strategy/control/types.h"

#include "plot/plotwidget.h"

#include "plotframe.h"
#include "imagecomparisonwidget.h"
#include "interfaces.h"

#include "../imaging/defaultmodels.h"

class RFIGuiWindow : public Gtk::Window, private StrategyController {
	public:
		RFIGuiWindow();
		~RFIGuiWindow();

		void SetImageSet(std::unique_ptr<rfiStrategy::ImageSet> newImageSet, bool loadBaseline);
		void SetImageSetIndex(std::unique_ptr<rfiStrategy::ImageSetIndex> newImageSetIndex);
		rfiStrategy::ImageSet &GetImageSet() const { return *_imageSet; }
		rfiStrategy::ImageSetIndex &GetImageSetIndex() const { return *_imageSetIndex; }
		void SetRevisedData(const TimeFrequencyData &data)
		{
			_timeFrequencyWidget.SetRevisedData(data);
		}
		void Update()
		{
			_timeFrequencyWidget.Update();
		}
 		bool HasImageSet() const { return _imageSet != nullptr; }
		bool HasImage() const { return _timeFrequencyWidget.HasImage(); }
		Mask2DCPtr Mask() const { return GetOriginalData().GetSingleMask(); }
		Mask2DCPtr AltMask() const { return GetContaminatedData().GetSingleMask(); }
		
		TimeFrequencyData GetActiveData() const
		{
			return _timeFrequencyWidget.GetActiveData();
		}
		const TimeFrequencyData &GetOriginalData() const
		{
			return _timeFrequencyWidget.OriginalData();
		}
		const TimeFrequencyData &GetContaminatedData() const
		{
			return _timeFrequencyWidget.ContaminatedData();
		}

		class ImageComparisonWidget &GetTimeFrequencyWidget()
		{
			return _timeFrequencyWidget;
		}
		
		class ThresholdConfig &HighlightConfig()
		{
			return _timeFrequencyWidget.HighlightConfig();
		}
		void SetHighlighting(bool newValue)
		{
			_timeFrequencyWidget.SetHighlighting(newValue);
		}
		TimeFrequencyMetaDataCPtr SelectedMetaData()
		{
			return _timeFrequencyWidget.GetSelectedMetaData();
		}
		
		void onExecuteStrategyFinished();
		void OpenPath(const std::string &path);
		void ShowHistogram(class HistogramCollection &histogramCollection);
		
		class RFIGuiController& Controller() { return *_controller; }
	private:
		rfiStrategy::Strategy &Strategy() final override { return *_strategy; }
		void SetStrategy(std::unique_ptr<rfiStrategy::Strategy> newStrategy) final override;

		void createToolbar();
		void loadCurrentTFData();

		void onLoadPrevious();
		void onLoadNext();
		void onToggleFlags();
		void onTogglePolarizations();
		void onToggleImage();
		void onQuit() { hide(); }
		void onActionFileOpen();
		void onActionDirectoryOpen();
		void onActionDirectoryOpenForSpatial();
		void onActionDirectoryOpenForST();
		void onTFZoomChanged();
		void onZoomFit() {
			_timeFrequencyWidget.ZoomFit();
			_timeFrequencyWidget.Update();
		}
		void onZoomIn()
		{ 
			if(_timeFrequencyWidget.IsMouseInImage())
				_timeFrequencyWidget.ZoomInOn(_timeFrequencyWidget.MouseX(), _timeFrequencyWidget.MouseY());
			else
				_timeFrequencyWidget.ZoomIn();
			_timeFrequencyWidget.Update();
		}
		void onZoomOut() {
			_timeFrequencyWidget.ZoomOut(); 
			_timeFrequencyWidget.Update();
		}
		void onShowImagePlane();
		void onSetAndShowImagePlane();
		void onAddToImagePlane();
		void onClearAltFlagsPressed();
		void onDifferenceToOriginalPressed();
		void onBackgroundToOriginalPressed();
		void onHightlightPressed();
		void keepPhasePart(enum TimeFrequencyData::ComplexRepresentation phaseRepresentation);
		void onKeepRealPressed() { keepPhasePart(TimeFrequencyData::RealPart); }
		void onKeepImaginaryPressed() { keepPhasePart(TimeFrequencyData::ImaginaryPart); }
		void onKeepPhasePressed() { keepPhasePart(TimeFrequencyData::PhasePart); }
		void keepPolarisation(PolarizationEnum polarisation);
		void onKeepStokesIPressed() { keepPolarisation(Polarization::StokesI); }
		void onKeepStokesQPressed() { keepPolarisation(Polarization::StokesQ); }
		void onKeepStokesUPressed() { keepPolarisation(Polarization::StokesU); }
		void onKeepStokesVPressed() { keepPolarisation(Polarization::StokesV); }
		void onKeepRRPressed() { keepPolarisation(Polarization::RR); }
		void onKeepRLPressed() { keepPolarisation(Polarization::RL); }
		void onKeepLRPressed() { keepPolarisation(Polarization::LR); }
		void onKeepLLPressed() { keepPolarisation(Polarization::LL); }
		void onKeepXXPressed() { keepPolarisation(Polarization::XX); }
		void onKeepXYPressed() { keepPolarisation(Polarization::XY); }
		void onKeepYXPressed() { keepPolarisation(Polarization::YX); }
		void onKeepYYPressed() { keepPolarisation(Polarization::YY); }
		void onImagePropertiesPressed();
		void onOpenTestSetNoise() { openTestSet(2); }
		void onOpenTestSetA() { openTestSet(3); }
		void onOpenTestSetB() { openTestSet(4); }
		void onOpenTestSetC() { openTestSet(5); }
		void onOpenTestSetD() { openTestSet(18); }
		void onOpenTestSetE() { openTestSet(14); }
		void onOpenTestSetF() { openTestSet(16); }
		void onOpenTestSetG() { openTestSet(17); }
		void onOpenTestSetH() { openTestSet(7); }
		void onOpenTestSetNoise3Model() { openTestSet(19); }
		void onOpenTestSetNoise5Model() { openTestSet(20); }
		void onOpenTestSet3Model() { openTestSet(21); }
		void onOpenTestSet5Model() { openTestSet(22); }
		void onOpenTestSetBStrong() { openTestSet(24); }
		void onOpenTestSetBWeak() { openTestSet(23); }
		void onOpenTestSetBAligned() { openTestSet(25); }
		void onOpenTestSetGaussianBroadband() { openTestSet(26); }
		void onOpenTestSetSinusoidalBroadband() { openTestSet(27); }
		void onOpenTestSetSlewedGaussianBroadband() { openTestSet(28); }
		void onOpenTestSetBurstBroadband() { openTestSet(29); }
		void onOpenTestSetRFIDistributionLow() { openTestSet(32); }
		void onOpenTestSetRFIDistributionMid() { openTestSet(31); }
		void onOpenTestSetRFIDistributionHigh() { openTestSet(30); }
		void onGaussianTestSets() { _gaussianTestSets = 1; }
		void onRayleighTestSets() { _gaussianTestSets = 0; }
		void onZeroTestSets() { _gaussianTestSets = 2; }
		void onAddStaticFringe();
		void onAdd1SigmaFringe();
		void onSetToOne();
		void onSetToI();
		void onSetToOnePlusI();
		void onShowStats();
		void onPlotDistPressed();
		void onPlotLogLogDistPressed();
		void onPlotComplexPlanePressed();
		void onPlotMeanSpectrumPressed();
		void onPlotSumSpectrumPressed();
		void onPlotPowerSpectrumPressed();
		void onPlotPowerSpectrumComparisonPressed();
		void onPlotFrequencyScatterPressed();
		void onPlotPowerRMSPressed();
		void onPlotPowerSNRPressed();
		void onPlotPowerTimePressed();
		void onPlotPowerTimeComparisonPressed();
		void onPlotTimeScatterPressed();
		void onPlotTimeScatterComparisonPressed();
		void onPlotSingularValuesPressed();
		void onEditStrategyPressed();
		void onExecuteStrategyPressed();
		void onReloadPressed();
		void onGoToPressed();
		void onLoadLongestBaselinePressed();
		void onLoadShortestBaselinePressed();
		void onTFWidgetMouseMoved(size_t x, size_t y);
		void onTFWidgetButtonReleased(size_t x, size_t y);
		void onMultiplyData();
		void onSegment();
		void onCluster();
		void onClassify();
		void onRemoveSmallSegments();
		void onTimeGraphButtonPressed();
//		void onFrequencyGraphButtonPressed();
		void onUnrollPhaseButtonPressed();
		void onVertEVD();
		void onApplyTimeProfile();
		void onApplyVertProfile();
		void onRestoreTimeProfile() { onUseTimeProfile(true); }
		void onRestoreVertProfile() { onUseVertProfile(true); }
		void onReapplyTimeProfile() { onUseTimeProfile(false); }
		void onReapplyVertProfile() { onUseVertProfile(false); }
		void onUseTimeProfile(bool inverse);
		void onUseVertProfile(bool inverse);
		void onStoreData();
		void onRecallData();
		void onSubtractDataFromMem();
		
		void showError(const std::string &description);
		void setSetNameInStatusBar();
		
		DefaultModels::SetLocation getSetLocation(bool empty = false);
		void loadDefaultModel(DefaultModels::Distortion distortion, bool withNoise, bool empty = false);
		void onSimulateCorrelation() { loadDefaultModel(DefaultModels::ConstantDistortion, false); }
		void onSimulateSourceSetA() { loadDefaultModel(DefaultModels::ConstantDistortion, true); }
		void onSimulateSourceSetB() { loadDefaultModel(DefaultModels::VariableDistortion, true); }
		void onSimulateSourceSetC() { loadDefaultModel(DefaultModels::FaintDistortion, true); }
		void onSimulateSourceSetD() { loadDefaultModel(DefaultModels::MislocatedDistortion, true); }
		void onSimulateOffAxisSource() { loadDefaultModel(DefaultModels::ConstantDistortion, false, true); }
		void onSimulateOnAxisSource() { loadDefaultModel(DefaultModels::OnAxisSource, false, true); }
		
		void onHelpAbout();
		
		void openTestSet(unsigned index);
		
		void onControllerStateChange();
		
		void onExecutePythonStrategy();
		
		std::unique_ptr<class RFIGuiController> _controller;
		
		Gtk::Box _mainVBox;
		Gtk::Paned _panedArea;
		ImageComparisonWidget _timeFrequencyWidget;
		Glib::RefPtr<Gtk::ActionGroup> _actionGroup;
		Gtk::Statusbar _statusbar;
		PlotFrame _plotFrame;
		std::string _imageSetName, _imageSetIndexDescription;

		Glib::RefPtr<Gtk::Action>
			_previousButton, _reloadButton, _nextButton,
			_zoomToFitButton, _zoomInButton, _zoomOutButton;
		Glib::RefPtr<Gtk::ToggleAction>
			_originalFlagsButton, _altFlagsButton,
			_showPPButton, _showPQButton,
			_showQPButton, _showQQButton,
			_originalImageButton, _backgroundImageButton, _diffImageButton,
			_timeGraphButton, _simFixBandwidthButton,
			_closeExecuteFrameButton;
		std::vector<sigc::connection> _toggleConnections;
		Glib::RefPtr<Gtk::RadioAction>
			_gaussianTestSetsButton, _rayleighTestSetsButton, _zeroTestSetsButton,
			_ncpSetButton, _b1834SetButton, _emptySetButton,
			_sim16ChannelsButton, _sim64ChannelsButton, _sim256ChannelsButton;
		std::unique_ptr<class ImagePlaneWindow> _imagePlaneWindow;
		std::unique_ptr<class HistogramWindow> _histogramWindow;
		std::unique_ptr<class PlotWindow> _plotWindow;
		std::unique_ptr<Gtk::Window>
			_optionWindow, _editStrategyWindow,
			_gotoWindow,
			_progressWindow, _highlightWindow,
			_plotComplexPlaneWindow, _imagePropertiesWindow;

		std::unique_ptr<rfiStrategy::ImageSet> _imageSet;
		std::unique_ptr<rfiStrategy::ImageSetIndex> _imageSetIndex;
		std::unique_ptr<rfiStrategy::Strategy> _strategy;
		int _gaussianTestSets;
		std::mutex _ioMutex;
		SegmentedImagePtr _segmentedImage;
		std::unique_ptr<class SpatialMatrixMetaData> _spatialMetaData;
		std::vector<double> _horProfile, _vertProfile;
		TimeFrequencyData _storedData;
		TimeFrequencyMetaDataCPtr _storedMetaData;
};

#endif
