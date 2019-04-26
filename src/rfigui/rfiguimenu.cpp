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
	/*
	makeBrowseMenu();
	makeSimulateMenu();
	makeDataMenu();
	makeActionsMenu();
	makeHelpMenu();
	*/
	makeToolbarActions();
		
	Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
	  "    </menu>"
	  "    <menu action='MenuPlot'>"
    "      <menu action='MenuPlotFlagComparison'>"
    "        <menuitem action='PlotPowerSpectrumComparison'/>"
    "        <menuitem action='PlotPowerTimeComparison'/>"
    "        <menuitem action='PlotTimeScatterComparison'/>"
		"      </menu>"
    "      <separator/>"
    "      <menuitem action='PlotDist'/>"
    "      <menuitem action='PlotLogLogDist'/>"
    "      <menuitem action='PlotComplexPlane'/>"
    "      <menuitem action='PlotMeanSpectrum'/>"
    "      <menuitem action='PlotSumSpectrum'/>"
    "      <menuitem action='PlotPowerSpectrum'/>"
    "      <menuitem action='PlotFrequencyScatter'/>"
    "      <menuitem action='PlotRMSSpectrum'/>"
    "      <menuitem action='PlotPowerTime'/>"
    "      <menuitem action='PlotTimeScatter'/>"
    "      <menuitem action='PlotSingularValues'/>"
	  "    </menu>"
    "    <menu action='MenuBrowse'>"
    "      <menuitem action='Previous'/>"
    "      <menuitem action='Reload'/>"
    "      <menuitem action='Next'/>"
    "      <separator/>"
    "      <menuitem action='GoTo'/>"
    "      <separator/>"
    "      <menuitem action='LoadLongestBaseline'/>"
    "      <menuitem action='LoadShortestBaseline'/>"
    "    </menu>"
	  "    <menu action='MenuSimulate'>"
    "      <menu action='OpenTestSet'>"
		"        <menuitem action='GaussianTestSets'/>"
		"        <menuitem action='RayleighTestSets'/>"
		"        <menuitem action='ZeroTestSets'/>"
    "        <separator/>"
		"        <menuitem action='OpenTestSetA'/>"
		"        <menuitem action='OpenTestSetB'/>"
		"        <menuitem action='OpenTestSetC'/>"
		"        <menuitem action='OpenTestSetD'/>"
		"        <menuitem action='OpenTestSetE'/>"
		"        <menuitem action='OpenTestSetF'/>"
		"        <menuitem action='OpenTestSetG'/>"
		"        <menuitem action='OpenTestSetH'/>"
		"        <menuitem action='OpenTestSetNoise'/>"
		"        <menuitem action='OpenTestSetModel3'/>"
		"        <menuitem action='OpenTestSetModel5'/>"
		"        <menuitem action='OpenTestSetNoiseModel3'/>"
		"        <menuitem action='OpenTestSetNoiseModel5'/>"
		"        <menuitem action='OpenTestSetBStrong'/>"
		"        <menuitem action='OpenTestSetBWeak'/>"
		"        <menuitem action='OpenTestSetBAligned'/>"
		"        <menuitem action='OpenTestSetGaussianBroadband'/>"
		"        <menuitem action='OpenTestSetSinusoidalBroadband'/>"
		"        <menuitem action='OpenTestSetSlewedGaussianBroadband'/>"
		"        <menuitem action='OpenTestSetBurstBroadband'/>"
		"        <menuitem action='OpenTestSetRFIDistributionLow'/>"
		"        <menuitem action='OpenTestSetRFIDistributionMid'/>"
		"        <menuitem action='OpenTestSetRFIDistributionHigh'/>"
		"      </menu>"
		"      <menu action='AddTestModification'>"
		"        <menuitem action='AddStaticFringe'/>"
		"        <menuitem action='Add1SigmaStaticFringe'/>"
		"        <menuitem action='SetToOne'/>"
		"        <menuitem action='SetToI'/>"
		"        <menuitem action='SetToOnePlusI'/>"
		"        <menuitem action='AddCorrelatorFault'/>"
		"        <menuitem action='MultiplyData'/>"
		"      </menu>"
		"      <separator/>"
    "      <menuitem action='NCPSet'/>"
    "      <menuitem action='B1834Set'/>"
    "      <menuitem action='EmptySet'/>"
    "      <separator/>"
    "      <menuitem action='Sim16Channels'/>"
    "      <menuitem action='Sim64Channels'/>"
    "      <menuitem action='Sim256Channels'/>"
    "      <menuitem action='SimFixBandwidth'/>"
    "      <separator/>"
    "      <menuitem action='SimulateCorrelation'/>"
    "      <menuitem action='SimulateSourceSetA'/>"
    "      <menuitem action='SimulateSourceSetB'/>"
    "      <menuitem action='SimulateSourceSetC'/>"
    "      <menuitem action='SimulateSourceSetD'/>"
    "      <menuitem action='SimulateOffAxisSource'/>"
    "      <menuitem action='SimulateOnAxisSource'/>"
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
/*
void RFIGuiMenu::makeBrowseMenu()
{
	_previousButton = Gtk::Action::create("Previous", "Previous");
	_previousButton->set_icon_name("go-previous");
	_previousButton->set_tooltip("Load and display the previous baseline. Normally, this steps from the baseline between antennas (i) and (j) to (i) and (j-1).");
	_previousButton->set_sensitive(false);
	
	_actionGroup->add(_previousButton,
		Gtk::AccelKey("F6"),
		RFIGuiMenu::OnLoadPrevious);
	_nextButton = Gtk::Action::create("Next", "Next");
	_nextButton->set_icon_name("go-next");
	_nextButton->set_tooltip("Load and display the next baseline. Normally, this steps from the baseline between antennas (i) and (j) to (i) and (j+1).");
	_nextButton->set_sensitive(false);
	_actionGroup->add(_nextButton,
		Gtk::AccelKey("F7"),
		RFIGuiMenu::OnLoadNext);
	_reloadButton = Gtk::Action::create("Reload", "_Reload");
	_reloadButton->set_icon_name("view-refresh");
	_reloadButton->set_tooltip("Reload the currently displayed baseline. This will reset the purple flags to the measurement set flags, and clear the yellow flags.");
	_reloadButton->set_sensitive(false);
	_actionGroup->add(_reloadButton, Gtk::AccelKey("F5"),
  RFIGuiMenu::OnReloadPressed);
	_actionGroup->add( Gtk::Action::create("GoTo", "_Go to..."),
		Gtk::AccelKey("<control>G"),
  RFIGuiMenu::OnGoToPressed);
	_actionGroup->add( Gtk::Action::create("LoadLongestBaseline", "Longest baseline"),
		RFIGuiMenu::OnLoadLongestBaselinePressed);
	_actionGroup->add( Gtk::Action::create("LoadShortestBaseline", "Shortest baseline"),
		RFIGuiMenu::OnLoadShortestBaselinePressed);	
}

void RFIGuiMenu::makeSimulateMenu()
{
	_actionGroup->add( Gtk::Action::create("OpenTestSet", "Open _testset");
	Gtk::RadioButtonGroup testSetGroup;
	_gaussianTestSetsButton = Gtk::RadioAction::create(testSetGroup, "GaussianTestSets", "Gaussian");
	_gaussianTestSetsButton->set_active(true);
	_rayleighTestSetsButton = Gtk::RadioAction::create(testSetGroup, "RayleighTestSets", "Rayleigh");
	_zeroTestSetsButton = Gtk::RadioAction::create(testSetGroup, "ZeroTestSets", "Zero");
	_actionGroup->add(_gaussianTestSetsButton, RFIGuiMenu::OnGaussianTestSets);
	_actionGroup->add(_rayleighTestSetsButton, RFIGuiMenu::OnRayleighTestSets);
	_actionGroup->add(_zeroTestSetsButton, RFIGuiMenu::OnZeroTestSets);
	
	_actionGroup->add( Gtk::Action::create("OpenTestSetA", "A Full spikes"),
	RFIGuiMenu::OnOpenTestSetA);
	_actionGroup->add( Gtk::Action::create("OpenTestSetB", "B Half spikes"),
	RFIGuiMenu::OnOpenTestSetB);
	_actionGroup->add( Gtk::Action::create("OpenTestSetC", "C Varying spikes"),
	RFIGuiMenu::OnOpenTestSetC);
	_actionGroup->add( Gtk::Action::create("OpenTestSetD", "D 3 srcs + spikes"),
	RFIGuiMenu::OnOpenTestSetD);
	_actionGroup->add( Gtk::Action::create("OpenTestSetE", "E 5 srcs + spikes"),
	RFIGuiMenu::OnOpenTestSetE);
	_actionGroup->add( Gtk::Action::create("OpenTestSetF", "F 5 srcs + spikes"),
	RFIGuiMenu::OnOpenTestSetF);
	_actionGroup->add( Gtk::Action::create("OpenTestSetG", "G Test set G"),
	RFIGuiMenu::OnOpenTestSetG);
	_actionGroup->add( Gtk::Action::create("OpenTestSetH", "H filtered srcs + spikes"),
	RFIGuiMenu::OnOpenTestSetH);
	_actionGroup->add( Gtk::Action::create("OpenTestSetNoise", "Noise"),
	RFIGuiMenu::OnOpenTestSetNoise));
	_actionGroup->add( Gtk::Action::create("OpenTestSetModel3", "3-source model"),
	RFIGuiMenu::OnOpenTestSet3Model));
	_actionGroup->add( Gtk::Action::create("OpenTestSetModel5", "5-source model"),
	RFIGuiMenu::OnOpenTestSet5Model));
	_actionGroup->add( Gtk::Action::create("OpenTestSetNoiseModel3", "3-source model with noise"),
	RFIGuiMenu::OnOpenTestSetNoise3Model));
	_actionGroup->add( Gtk::Action::create("OpenTestSetNoiseModel5", "5-source model with noise"),
	RFIGuiMenu::OnOpenTestSetNoise5Model));
	_actionGroup->add( Gtk::Action::create("OpenTestSetBStrong", "Test set B (strong RFI)"),
	RFIGuiMenu::OnOpenTestSetBStrong));
	_actionGroup->add( Gtk::Action::create("OpenTestSetBWeak", "Test set B (weak RFI)"),
	RFIGuiMenu::OnOpenTestSetBWeak));
	_actionGroup->add( Gtk::Action::create("OpenTestSetBAligned", "Test set B (aligned)"),
	RFIGuiMenu::OnOpenTestSetBAligned));
	_actionGroup->add( Gtk::Action::create("OpenTestSetGaussianBroadband", "Gaussian broadband"),
	RFIGuiMenu::OnOpenTestSetGaussianBroadband));
	_actionGroup->add( Gtk::Action::create("OpenTestSetSinusoidalBroadband", "Sinusoidal broadband"),
	RFIGuiMenu::OnOpenTestSetSinusoidalBroadband));
	_actionGroup->add( Gtk::Action::create("OpenTestSetSlewedGaussianBroadband", "Slewed Gaussian"),
	RFIGuiMenu::OnOpenTestSetSlewedGaussianBroadband));
	_actionGroup->add( Gtk::Action::create("OpenTestSetBurstBroadband", "Burst"),
	RFIGuiMenu::OnOpenTestSetBurstBroadband));
	_actionGroup->add( Gtk::Action::create("OpenTestSetRFIDistributionLow", "Slope -2 dist low"),
	RFIGuiMenu::OnOpenTestSetRFIDistributionLow));
	_actionGroup->add( Gtk::Action::create("OpenTestSetRFIDistributionMid", "Slope -2 dist mid"),
	RFIGuiMenu::OnOpenTestSetRFIDistributionMid));
	_actionGroup->add( Gtk::Action::create("OpenTestSetRFIDistributionHigh", "Slope -2 dist high"),
	RFIGuiMenu::OnOpenTestSetRFIDistributionHigh));
	_actionGroup->add( Gtk::Action::create("AddTestModification", "Test modify");
	_actionGroup->add( Gtk::Action::create("AddStaticFringe", "Static fringe"),
	RFIGuiMenu::OnAddStaticFringe);
	_actionGroup->add( Gtk::Action::create("Add1SigmaStaticFringe", "Static 1 sigma fringe"),
	RFIGuiMenu::OnAdd1SigmaFringe);
	_actionGroup->add( Gtk::Action::create("SetToOne", "Set to 1"),
	RFIGuiMenu::OnSetToOne);
	_actionGroup->add( Gtk::Action::create("SetToI", "Set to i"),
	RFIGuiMenu::OnSetToI);
	_actionGroup->add( Gtk::Action::create("SetToOnePlusI", "Set to 1+i"),
	RFIGuiMenu::OnSetToOnePlusI);
	_actionGroup->add( Gtk::Action::create("AddCorrelatorFault", "Add correlator fault"),
	RFIGuiMenu::OnAddCorrelatorFault);
	_actionGroup->add( Gtk::Action::create("MultiplyData", "Multiply data..."),
	RFIGuiMenu::OnMultiplyData);
	
	Gtk::RadioButtonGroup setGroup;
	_ncpSetButton = Gtk::RadioAction::create(setGroup, "NCPSet", "Use NCP set");
	_b1834SetButton = Gtk::RadioAction::create(setGroup, "B1834Set", "Use B1834 set");
	_emptySetButton = Gtk::RadioAction::create(setGroup, "EmptySet", "Use empty set");
	_ncpSetButton->set_active(true); 
	_actionGroup->add(_ncpSetButton);
	_actionGroup->add(_b1834SetButton);
	_actionGroup->add(_emptySetButton);
	
	Gtk::RadioButtonGroup chGroup;
	_sim16ChannelsButton = Gtk::RadioAction::create(chGroup, "Sim16Channels", "16 channels");
	_sim64ChannelsButton = Gtk::RadioAction::create(chGroup, "Sim64Channels", "64 channels");
	_sim256ChannelsButton = Gtk::RadioAction::create(chGroup, "Sim256Channels", "256 channels");
	_sim64ChannelsButton->set_active(true); 
	_actionGroup->add(_sim16ChannelsButton);
	_actionGroup->add(_sim64ChannelsButton);
	_actionGroup->add(_sim256ChannelsButton);
	
	_simFixBandwidthButton = Gtk::ToggleAction::create("SimFixBandwidth", "Fix bandwidth");
	_simFixBandwidthButton->set_active(false); 
	_actionGroup->add(_simFixBandwidthButton);
	
	_actionGroup->add( Gtk::Action::create("SimulateCorrelation", "Simulate correlation"),
  RFIGuiMenu::OnSimulateCorrelation);
	_actionGroup->add( Gtk::Action::create("SimulateSourceSetA", "Simulate source set A"),
  RFIGuiMenu::OnSimulateSourceSetA);
	_actionGroup->add( Gtk::Action::create("SimulateSourceSetB", "Simulate source set B"),
  RFIGuiMenu::OnSimulateSourceSetB);
	_actionGroup->add( Gtk::Action::create("SimulateSourceSetC", "Simulate source set C"),
  RFIGuiMenu::OnSimulateSourceSetC);
	_actionGroup->add( Gtk::Action::create("SimulateSourceSetD", "Simulate source set D"),
  RFIGuiMenu::OnSimulateSourceSetD);
	_actionGroup->add( Gtk::Action::create("SimulateOffAxisSource", "Simulate off-axis source"),
  RFIGuiMenu::OnSimulateOffAxisSource);
	_actionGroup->add( Gtk::Action::create("SimulateOnAxisSource", "Simulate on-axis source"),
  RFIGuiMenu::OnSimulateOnAxisSource);
}

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

