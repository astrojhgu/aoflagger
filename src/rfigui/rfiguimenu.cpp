#include "rfiguimenu.h"

#include <gtkmm/icontheme.h>

RFIGuiMenu::RFIGuiMenu() : _blockVisualizationSignals(false)
{
	topMenu(_menuFile, _miFile, "_File");
	topMenu(_menuView, _miView, "_View");
	topMenu(_menuPlot, _miPlot, "_Plot");
	topMenu(_menuBrowse, _miBrowse, "_Browse");
	topMenu(_menuSimulate, _miSimulate, "_Simulate");
	topMenu(_menuData, _miData, "_Data");
	topMenu(_menuActions, _miActions, "_Actions");
	topMenu(_menuHelp, _miHelp, "_Help");
	
	//_actionGroup->add( Gtk::Action::create("MenuPlotFlagComparison", "_Compare flags") );
	
	makeFileMenu();
	makeViewMenu();
	makePlotMenu();
	makeBrowseMenu();
	makeSimulateMenu();
	/*
	makeDataMenu();
	makeActionsMenu();
	makeHelpMenu();
	*/
	makeToolbarActions();
		
	Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
	  "    </menu>"
	  "    <menu action='MenuData'>"
    "      <menuitem action='VisToOriginal'/>"
    "      <separator/>"
    "      <menuitem action='KeepReal'/>"
    "      <menuitem action='KeepImaginary'/>"
    "      <menuitem action='KeepPhase'/>"
    "      <menuitem action='UnrollPhase'/>"
    "      <separator/>"
    "      <menuitem action='KeepStokesI'/>"
    "      <menuitem action='KeepStokesQ'/>"
    "      <menuitem action='KeepStokesU'/>"
    "      <menuitem action='KeepStokesV'/>"
    "      <separator/>"
    "      <menuitem action='KeepRR'/>"
    "      <menuitem action='KeepRL'/>"
    "      <menuitem action='KeepLR'/>"
    "      <menuitem action='KeepLL'/>"
    "      <separator/>"
    "      <menuitem action='KeepXX'/>"
    "      <menuitem action='KeepXY'/>"
    "      <menuitem action='KeepYX'/>"
    "      <menuitem action='KeepYY'/>"
    "      <separator/>"
    "      <menuitem action='StoreData'/>"
    "      <menuitem action='RecallData'/>"
    "      <menuitem action='SubtractDataFromMem'/>"
    "      <menuitem action='ClearOriginalFlags'/>"
    "      <menuitem action='ClearAltFlags'/>"
	  "    </menu>"
	  "    <menu action='MenuActions'>"
    "      <menuitem action='EditStrategy'/>"
    "      <menuitem action='ExecuteStrategy'/>"
    "      <menuitem action='CloseExecuteFrame'/>"
    "      <separator/>"
    "      <menuitem action='ExecutePythonStrategy'/>"
    "      <separator/>"
    "      <menuitem action='Segment'/>"
    "      <menuitem action='Cluster'/>"
    "      <menuitem action='Classify'/>"
    "      <menuitem action='RemoveSmallSegments'/>"
    "      <menuitem action='InterpolateFlagged'/>"
    "      <separator/>"
    "      <menuitem action='VertEVD'/>"
    "      <menuitem action='ApplyTimeProfile'/>"
    "      <menuitem action='ApplyVertProfile'/>"
    "      <menuitem action='RestoreTimeProfile'/>"
    "      <menuitem action='RestoreVertProfile'/>"
    "      <menuitem action='ReapplyTimeProfile'/>"
    "      <menuitem action='ReapplyVertProfile'/>"
	  "    </menu>"
	  "    <menu action='MenuHelp'>"
    "      <menuitem action='About'/>"
	  "    </menu>"
    "  </menubar>"
    "  <toolbar  name='ToolBar'>"
    "    <toolitem action='OpenDirectory'/>"
    "    <separator/>"
    "    <toolitem action='ExecuteStrategy'/>"
    "    <toolitem action='OriginalFlags'/>"
    "    <toolitem action='AlternativeFlags'/>"
    "    <separator/>"
    "    <toolitem action='Previous'/>"
    "    <toolitem action='Reload'/>"
    "    <toolitem action='Next'/>"
    "    <separator/>"
    "    <toolitem action='ZoomFit'/>"
    "    <toolitem action='ZoomIn'/>"
    "    <toolitem action='ZoomOut'/>"
    "    <toolitem action='DisplayPP'/>"
    "    <toolitem action='DisplayPQ'/>"
    "    <toolitem action='DisplayQP'/>"
    "    <toolitem action='DisplayQQ'/>"
    "  </toolbar>"
    "</ui>";

	if(Gtk::IconTheme::get_default()->has_icon("aoflagger"))
	{
		_toolbar.set_toolbar_style(Gtk::TOOLBAR_ICONS);
		_toolbar.set_icon_size(Gtk::ICON_SIZE_LARGE_TOOLBAR);
	}
	else {
		_toolbar.set_toolbar_style(Gtk::TOOLBAR_TEXT);
		_toolbar.set_icon_size(Gtk::ICON_SIZE_SMALL_TOOLBAR);
	}
}

void RFIGuiMenu::makeFileMenu()
{
	// <control>D
	addItem(_menuFile, _miFileOpenDir, OnActionDirectoryOpen, "Open _directory", "folder");
	
	// Gtk::AccelKey("<control>O")
	addItem(_menuFile, _miFileOpen, OnActionFileOpen, "Open _file", "document-open");
	
	addItem(_menuFile, _miFileOpenSpatial, OnActionDirectoryOpenForSpatial, "Open _directory as spatial");
	
	addItem(_menuFile, _miFileOpenST, OnActionDirectoryOpenForST, "Open _directory as spatial/time"),
	
	addItem(_menuFile, _miFileSaveBaseline, OnSaveBaseline, "Save baseline as...");
	
	// <control>Q
	addItem(_menuFile, _miFileQuit, OnQuit, "_Quit", "application-exit");
}

void RFIGuiMenu::makeViewMenu()
{
	// <control>P
	addItem(_menuView, _miViewProperties, OnImagePropertiesPressed, "Plot properties...");
	addItem(_menuView, _miViewTimeGraph, OnTimeGraphButtonPressed, "Time graph");
	addItem(_menuView, _miViewSep1);
	
	addItem(_menuView, _miViewOriginalFlags, [&](){
		if(!_blockVisualizationSignals) OnToggleFlags(); }, "Ori. flags", "showoriginalflags"
	);
	
	addItem(_menuView, _miViewAlternativeFlags, [&](){
		if(!_blockVisualizationSignals) OnToggleFlags(); }, "Alt. flags", "showalternativeflags"
	);
	addItem(_menuView, _miViewHighlight, OnHightlightPressed, "Highlighting");
	addItem(_menuView, _miViewSep2);
	
	addItem(_menuView, _miViewZoomFit, [&](){
		if(!_blockVisualizationSignals) OnZoomFit(); }, "Zoom _fit", "zoom-fit-best"
	);
	addItem(_menuView, _miViewZoomIn, [&](){
		if(!_blockVisualizationSignals) OnZoomIn(); }, "Zoom in", "zoom-in"
	);
	addItem(_menuView, _miViewZoomOut, [&](){
		if(!_blockVisualizationSignals) OnZoomOut(); }, "Zoom out", "zoom-out"
	);
	addItem(_menuView, _miViewSep3);
	
	addItem(_menuView, _miViewImagePlane, OnShowImagePlane, "Image plane");
	addItem(_menuView, _miViewSetImagePlane, OnSetAndShowImagePlane, "Set image");
	addItem(_menuView, _miViewAddToImagePlane, OnAddToImagePlane, "Add to image");
	addItem(_menuView, _miViewSep4);
	
	// F2
	addItem(_menuView, _miViewStats, OnShowStats, "Statistics");
}

void RFIGuiMenu::makePlotMenu()
{
	addItem(_menuFlagComparison, _miPlotComparisonPowerSpectrum, OnPlotPowerSpectrumComparisonPressed, "Power _spectrum");
	addItem(_menuFlagComparison, _miPlotComparisonPowerTime, OnPlotPowerTimeComparisonPressed, "Po_wer vs time");
	addItem(_menuFlagComparison, _miPlotComparisonTimeScatter, OnPlotTimeScatterComparisonPressed, "Time _scatter");
	
	_miFlagComparison.set_submenu(_menuFlagComparison);
	addItem(_menuPlot, _miFlagComparison, "Flag comparison");
	
	addItem(_menuPlot, _miPlotDistribution, OnPlotDistPressed, "Plot _distribution");
	addItem(_menuPlot, _miPlotLogLogDistribution, OnPlotLogLogDistPressed, "Plot _log-log dist");
	addItem(_menuPlot, _miPlotComplexPlane, OnPlotComplexPlanePressed, "Plot _complex plane");
	addItem(_menuPlot, _miPlotMeanSpectrum, OnPlotMeanSpectrumPressed, "Plot _mean spectrum");
	
	addItem(_menuPlot, _miPlotSumSpectrum, OnPlotSumSpectrumPressed, "Plot s_um spectrum");
	addItem(_menuPlot, _miPlotPowerSpectrum, OnPlotPowerSpectrumPressed, "Plot _power spectrum");
	addItem(_menuPlot, _miPlotFrequencyScatter, OnPlotFrequencyScatterPressed, "Plot _frequency scatter");
	addItem(_menuPlot, _miPlotRMSSpectrum, OnPlotPowerRMSPressed, "Plot _rms spectrum");
	
	addItem(_menuPlot, _miPlotPowerTime, OnPlotPowerTimePressed, "Plot power vs _time");
	addItem(_menuPlot, _miPlotTimeScatter, OnPlotTimeScatterPressed, "Plot t_ime scatter");
	addItem(_menuPlot, _miPlotSingularValues, OnPlotSingularValuesPressed, "Plot _singular values");
}

void RFIGuiMenu::makeBrowseMenu()
{
	// F6
	addItem(_menuBrowse, _miBrowsePrevious, OnLoadPrevious, "Previous", "go-previous");
	_miBrowsePrevious.item.set_sensitive(false);
	// F5
	addItem(_menuBrowse, _miBrowseReload, OnReloadPressed, "_Reload", "view-refresh");
	_miBrowseReload.item.set_sensitive(false);
	// F7
	addItem(_menuBrowse, _miBrowseNext, OnLoadPrevious, "Next", "go-next");
	_miBrowseNext.item.set_sensitive(false);
	addItem(_menuBrowse, _miBrowseSep1);
	// "<control>G"
	addItem(_menuBrowse, _miBrowseGoto, OnGoToPressed, "_Go to...");
	addItem(_menuBrowse, _miBrowseSep2);
	addItem(_menuBrowse, _miBrowseLongestBaseline, OnLoadLongestBaselinePressed, "Longest baseline");
	addItem(_menuBrowse, _miBrowseShortestBaseline, OnLoadShortestBaselinePressed, "Shortest baseline");
}

void RFIGuiMenu::makeSimulateMenu()
{
	_miTestSets.set_submenu(_menuTestSets);
	addItem(_menuSimulate, _miTestSets, "Open _testset");
	
	Gtk::RadioMenuItem::Group testSetGroup;
	addItem(_menuTestSets, _miTestGaussian, OnGaussianTestSets, "Gaussian");
	_miTestGaussian.set_group(testSetGroup);
	_miTestGaussian.set_active(true);
	addItem(_menuTestSets, _miTestRayleigh, OnRayleighTestSets, "Rayleigh");
	_miTestRayleigh.set_group(testSetGroup);
	addItem(_menuTestSets, _miTestZero, OnZeroTestSets, "Zero");
	_miTestZero.set_group(testSetGroup);

	addItem(_menuTestSets, _miTestSep1);
	
	addItem(_menuTestSets, _miTestA, OnOpenTestSetA, "A Full spikes");
	addItem(_menuTestSets, _miTestB, OnOpenTestSetB, "B Half spikes");
	addItem(_menuTestSets, _miTestC, OnOpenTestSetC, "C Varying spikes");
	addItem(_menuTestSets, _miTestD, OnOpenTestSetD, "D 3 srcs + spikes");
	addItem(_menuTestSets, _miTestE, OnOpenTestSetE, "E 5 srcs + spikes");
	addItem(_menuTestSets, _miTestF, OnOpenTestSetF, "F 5 srcs + spikes");
	addItem(_menuTestSets, _miTestG, OnOpenTestSetG, "G Test set G");
	addItem(_menuTestSets, _miTestH, OnOpenTestSetG, "H filtered srcs + spikes");
	
	addItem(_menuTestSets, _miTestNoise, OnOpenTestSetNoise, "Noise");
	addItem(_menuTestSets, _miTestModel3, OnOpenTestSet3Model, "3-source model");
	addItem(_menuTestSets, _miTestModel5, OnOpenTestSet5Model, "5-source model");
	addItem(_menuTestSets, _miTestNoiseModel3, OnOpenTestSetNoise3Model, "3-source model with noise");
	addItem(_menuTestSets, _miTestNoiseModel5, OnOpenTestSetNoise5Model, "5-source model with noise");
	addItem(_menuTestSets, _miTestBStrong, OnOpenTestSetBStrong, "Test set B (strong RFI)");
	addItem(_menuTestSets, _miTestBWeak, OnOpenTestSetBWeak, "Test set B (weak RFI)");
	addItem(_menuTestSets, _miTestBAligned, OnOpenTestSetBAligned, "Test set B (aligned)");
	
	addItem(_menuTestSets, _miTestGaussianBroadband, OnOpenTestSetGaussianBroadband, "Gaussian broadband");
	addItem(_menuTestSets, _miTestSenusoidalBroadband, OnOpenTestSetSinusoidalBroadband, "Sinusoidal broadband");
	addItem(_menuTestSets, _miTestSlewedGaussianBroadband, OnOpenTestSetSlewedGaussianBroadband, "Slewed Gaussian");
	addItem(_menuTestSets, _miTestBurstBroadband, OnOpenTestSetBurstBroadband, "Burst");
	addItem(_menuTestSets, _miTestRFIDistLow, OnOpenTestSetRFIDistributionLow, "Slope -2 dist low");
	addItem(_menuTestSets, _miTestRFIDistMid, OnOpenTestSetRFIDistributionMid, "Slope -2 dist mid");
	addItem(_menuTestSets, _miTestRFIDistHigh, OnOpenTestSetRFIDistributionHigh, "Slope -2 dist high");
	
	_miSimulateModify.set_submenu(_menuModify);
	addItem(_menuSimulate, _miSimulateModify, "Modify");
	
	addItem(_menuModify, _miModifyStaticFringe, OnAddStaticFringe, "Static fringe");
	addItem(_menuModify, _miModify1SigmaStaticFringe, OnAdd1SigmaFringe, "Static 1 sigma fringe");
	addItem(_menuModify, _miModifyToOne, OnSetToOne, "Set to 1");
	addItem(_menuModify, _miModifyToI, OnSetToI, "Set to i");
	addItem(_menuModify, _miModifyToOnePlusI, OnSetToOnePlusI, "Set to 1+i");
	addItem(_menuModify, _miModifyCorrelatorFault, OnAddCorrelatorFault, "Add correlator fault");
	addItem(_menuModify, _miModifyMultiply, OnMultiplyData, "Multiply data...");
	
	Gtk::RadioMenuItem::Group setGroup;
	_miSimNCP.set_group(setGroup);
	addItem(_menuSimulate, _miSimNCP, "Use NCP set");
	_miSimB1834.set_group(setGroup);
	addItem(_menuSimulate, _miSimB1834, "Use B1834 set");
	_miSimEmpty.set_group(setGroup);
	addItem(_menuSimulate, _miSimEmpty, "Use empty set");
	_miSimNCP.set_active(true); 
	
	addItem(_menuSimulate, _miSimSep1);
	
	Gtk::RadioMenuItem::Group chGroup;
	_miSim16channels.set_group(chGroup);
	addItem(_menuSimulate, _miSim16channels, "16 channels");
	_miSim64channels.set_group(chGroup);
	addItem(_menuSimulate, _miSim64channels, "64 channels");
	_miSim256channels.set_group(chGroup);
	addItem(_menuSimulate, _miSim256channels, "256 channels");
	_miSim64channels.set_active(true); 
	
	addItem(_menuSimulate, _miSimSep2);
	
	addItem(_menuSimulate, _miSimFixBandwidth, "Fix bandwidth");
	_miSimFixBandwidth.set_active(false); 
	
	addItem(_menuSimulate, _miSimCorrelation, OnSimulateCorrelation, "Simulate correlation");
	addItem(_menuSimulate, _miSimSourceSetA, OnSimulateSourceSetA, "Simulate source set A");
	addItem(_menuSimulate, _miSimSourceSetB, OnSimulateSourceSetB, "Simulate source set B");
	addItem(_menuSimulate, _miSimSourceSetC, OnSimulateSourceSetC, "Simulate source set C");
	addItem(_menuSimulate, _miSimSourceSetD, OnSimulateSourceSetD, "Simulate source set D");
	addItem(_menuSimulate, _miSimSourceOffAxis, OnSimulateOffAxisSource, "Simulate off-axis source");
	addItem(_menuSimulate, _miSimSourceOnAxis, OnSimulateOnAxisSource, "Simulate on-axis source");
}

/*
void RFIGuiMenu::makeDataMenu()
{
	_actionGroup->add( Gtk::Action::create("VisToOriginal", "Current->Original"),
  RFIGuiMenu::OnVisualizedToOriginalPressed);

	_actionGroup->add( Gtk::Action::create("KeepReal", "Keep _real part"),
		Gtk::AccelKey("<control>,"),
		RFIGuiMenu::OnKeepRealPressed);
	_actionGroup->add( Gtk::Action::create("KeepImaginary", "Keep _imaginary part"),
		Gtk::AccelKey("<control>."),
		RFIGuiMenu::OnKeepImaginaryPressed);
	_actionGroup->add( Gtk::Action::create("KeepPhase", "Keep _phase part"),
		Gtk::AccelKey("<control>1"),
		RFIGuiMenu::OnKeepPhasePressed);
	_actionGroup->add( Gtk::Action::create("KeepStokesI", "Keep _stokesI part"),
  RFIGuiMenu::OnKeepStokesIPressed);
	_actionGroup->add( Gtk::Action::create("KeepStokesQ", "Keep stokes_Q part"),
  RFIGuiMenu::OnKeepStokesQPressed);
	_actionGroup->add( Gtk::Action::create("KeepStokesU", "Keep stokes_U part"),
  RFIGuiMenu::OnKeepStokesUPressed);
	_actionGroup->add( Gtk::Action::create("KeepStokesV", "Keep stokes_V part"),
  RFIGuiMenu::OnKeepStokesVPressed);
	_actionGroup->add( Gtk::Action::create("KeepRR", "Keep _rr part"),
		Gtk::AccelKey("<control>R"),
		RFIGuiMenu::OnKeepRRPressed);
	_actionGroup->add( Gtk::Action::create("KeepRL", "Keep rl part"),
		RFIGuiMenu::OnKeepRLPressed);
	_actionGroup->add( Gtk::Action::create("KeepLR", "Keep lr part"),
		Gtk::AccelKey("<control><alt>L"),
		RFIGuiMenu::OnKeepLRPressed);
	_actionGroup->add( Gtk::Action::create("KeepLL", "Keep _ll part"),
		Gtk::AccelKey("<control>L"),
		RFIGuiMenu::OnKeepLLPressed);
	_actionGroup->add( Gtk::Action::create("KeepXX", "Keep _xx part"),
		Gtk::AccelKey("<control>X"),
		RFIGuiMenu::OnKeepXXPressed);
	_actionGroup->add( Gtk::Action::create("KeepXY", "Keep xy part"),
		Gtk::AccelKey("<control><alt>X"),
		RFIGuiMenu::OnKeepXYPressed);
	_actionGroup->add( Gtk::Action::create("KeepYX", "Keep yx part"),
		Gtk::AccelKey("<control><alt>Y"),
		RFIGuiMenu::OnKeepYXPressed);
	_actionGroup->add( Gtk::Action::create("KeepYY", "Keep _yy part"),
		Gtk::AccelKey("<control>Y"),
		RFIGuiMenu::OnKeepYYPressed);
	_actionGroup->add( Gtk::Action::create("UnrollPhase", "_Unroll phase"),
	RFIGuiMenu::OnUnrollPhaseButtonPressed);

	_actionGroup->add( Gtk::Action::create("ClearOriginalFlags", "Clear ori flags"),
  RFIGuiMenu::OnClearOriginalFlagsPressed);
	_actionGroup->add( Gtk::Action::create("ClearAltFlags", "Clear alt flags"),
  RFIGuiMenu::OnClearAltFlagsPressed);
	
	_actionGroup->add( Gtk::Action::create("StoreData", "Store"),
		Gtk::AccelKey("<control>M"),
  RFIGuiMenu::OnStoreData);
	_actionGroup->add( Gtk::Action::create("RecallData", "Recall"),
		Gtk::AccelKey("<control><alt>R"),
		RFIGuiMenu::OnRecallData);
	_actionGroup->add( Gtk::Action::create("SubtractDataFromMem", "Subtract from mem"),
  RFIGuiMenu::OnSubtractDataFromMem);
}

void RFIGuiMenu::makeActionsMenu()
{
	_actionGroup->add( Gtk::Action::create("EditStrategy", "_Edit strategy"),
		Gtk::AccelKey("F8"),
  RFIGuiMenu::OnEditStrategyPressed);
	action = Gtk::Action::create("ExecuteStrategy", "E_xecute strategy");
	action->set_tooltip("Run the currently loaded strategy. Normally this will not write back the results to the opened set. The flagging results are displayed in the plot as yellow ('alternative') flag mask.");
	action->set_icon_name("system-run");
	_actionGroup->add(action, Gtk::AccelKey("F9"),
			RFIGuiMenu::OnExecuteStrategyPressed));
	_closeExecuteFrameButton = Gtk::ToggleAction::create("CloseExecuteFrame", "Close execute frame");
	_actionGroup->add(_closeExecuteFrameButton);
	_closeExecuteFrameButton->set_active(true); 
	
	_actionGroup->add( Gtk::Action::create("ExecutePythonStrategy", "_Execute script"),
		RFIGuiMenu::OnExecutePythonStrategy);
	
	_actionGroup->add( Gtk::Action::create("Segment", "Segment"),
  RFIGuiMenu::OnSegment);
	_actionGroup->add( Gtk::Action::create("Cluster", "Cluster"),
  RFIGuiMenu::OnCluster);
	_actionGroup->add( Gtk::Action::create("Classify", "Classify"),
  RFIGuiMenu::OnClassify);
	_actionGroup->add( Gtk::Action::create("RemoveSmallSegments", "Remove small segments"),
  RFIGuiMenu::OnRemoveSmallSegments);
	_actionGroup->add( Gtk::Action::create("InterpolateFlagged", "Interpolate flagged"),
  sigc::mem_fun(_controller, &RFIGuiController::InterpolateFlagged);
	
	_actionGroup->add( Gtk::Action::create("VertEVD", "Vert EVD"),
  RFIGuiMenu::OnVertEVD);
	_actionGroup->add( Gtk::Action::create("ApplyTimeProfile", "Apply time profile"),
  RFIGuiMenu::OnApplyTimeProfile);
	_actionGroup->add( Gtk::Action::create("ApplyVertProfile", "Apply vert profile"),
  RFIGuiMenu::OnApplyVertProfile);
	_actionGroup->add( Gtk::Action::create("RestoreTimeProfile", "Restore time profile"),
  RFIGuiMenu::OnRestoreTimeProfile);
	_actionGroup->add( Gtk::Action::create("RestoreVertProfile", "Restore vert profile"),
  RFIGuiMenu::OnRestoreVertProfile);
	_actionGroup->add( Gtk::Action::create("ReapplyTimeProfile", "Reapply time profile"),
  RFIGuiMenu::OnReapplyTimeProfile);
	_actionGroup->add( Gtk::Action::create("ReapplyVertProfile", "Reapply vert profile"),
  RFIGuiMenu::OnReapplyVertProfile);
}

void RFIGuiMenu::makeHelpMenu()
{
	Glib::RefPtr<Gtk::Action> action = Gtk::Action::create("About", "_About");
	action->set_icon_name("aoflagger");
	_actionGroup->add(action, RFIGuiMenu::OnHelpAbout));
}
*/

void RFIGuiMenu::makeToolbarActions()
{
	addTool(_tbOpenDirectory, OnActionDirectoryOpen, "Open", "Open a directory. This action should be used to open a measurement set. For opening files (e.g. sdfits files), select 'Open file' instead.", "folder");
	
	_toolbar.append(_tbSep1);
	
	// Gtk::AccelKey("F9")
	addTool(_tbExecuteStrategy, OnExecuteStrategyPressed, "E_xecute strategy", "Run the currently loaded strategy. Normally this will not write back the results to the opened set. The flagging results are displayed in the plot as yellow ('alternative') flag mask.", "system-run");
	// F3
	addTool(_tbOriginalFlags, OnToggleFlags, "Ori flags", "Display the first flag mask on top of the visibilities. These flags are displayed in purple and indicate the flags as they originally were stored in the measurement set.", "showoriginalflags");
	// F4
	addTool(_tbAlternativeFlags, OnToggleFlags, "Alt flags", "Display the second flag mask on top of the visibilities. These flags are displayed in yellow and indicate flags found by running the strategy.", "showalternativeflags");
	
	_toolbar.append(_tbSep2);
	
	// F6
	addTool(_tbPrevious, OnLoadPrevious, "Previous", "Load and display the previous baseline. Normally, this steps from the baseline between antennas (i) and (j) to (i) and (j-1).", "go-previous");
	_tbPrevious.set_sensitive(false);
	// F5
	addTool(_tbReload, OnReloadPressed, "Reload", "Reload the currently displayed baseline. This will reset the purple flags to the measurement set flags, and clear the yellow flags.", "view-refresh");
	_tbReload.set_sensitive(false);
	// F7
	addTool(_tbNext, OnLoadNext, "Next", "Load and display the next baseline. Normally, this steps from the baseline between antennas (i) and (j) to (i) and (j+1).", "go-next");
	_tbNext.set_sensitive(false);
	
	_toolbar.append(_tbSep3);
	
	// <control>0
	addTool(_tbZoomFit, OnZoomFit, "Fit", "Zoom fit", "zoom-fit-best");
	addTool(_tbZoomIn, OnZoomIn, "+", "Zoom in", "zoom-in");
	addTool(_tbZoomOut, OnZoomOut, "-", "Zoom out", "zoom-out");
	
	auto sig = [&]() { if(!_blockVisualizationSignals) OnTogglePolarizations(); };
	addTool(_tbDisplayPP, sig, "PP", "Display the PP polarization. Depending on the polarization configuration of the measurement set, this will show XX or RR", "showpp");
	addTool(_tbDisplayPQ, sig, "PQ", "Display the PQ polarization. Depending on the polarization configuration of the measurement set, this will show XY or RL", "showpq");
	addTool(_tbDisplayQP, sig, "QP", "Display the QP polarization. Depending on the polarization configuration of the measurement set, this will show YX or LR", "showqp");
	addTool(_tbDisplayQQ, sig, "QQ", "Display the QQ polarization. Depending on the polarization configuration of the measurement set, this will show YY or LL", "showqq");
	addTool(_tbSelectVisualization, OnToggleImage, "Change visualization", "Switch visualization", "showoriginalvisibilities");
	_tbSelectVisualization.set_menu(_tfVisualizationMenu);
}

