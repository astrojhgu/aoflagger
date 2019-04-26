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
	/*
	makePlotMenu();
	makeBrowseMenu();
	makeSimulateMenu();
	makeDataMenu();
	makeActionsMenu();
	makeHelpMenu();
	makeToolbarActions();
	*/
		
	Glib::ustring ui_info =
    "<ui>"
    "  <menubar name='MenuBar'>"
	  "    <menu action='MenuView'>"
    "      <menuitem action='ImageProperties'/>"
    "      <menuitem action='TimeGraph'/>"
    "      <separator/>"
    "      <menuitem action='OriginalFlags'/>"
    "      <menuitem action='AlternativeFlags'/>"
    "      <menuitem action='Highlight'/>"
    "      <separator/>"
    "      <menuitem action='ZoomFit'/>"
    "      <menuitem action='ZoomIn'/>"
    "      <menuitem action='ZoomOut'/>"
    "      <separator/>"
    "      <menuitem action='ShowImagePlane'/>"
    "      <menuitem action='SetAndShowImagePlane'/>"
    "      <menuitem action='AddToImagePlane'/>"
    "      <separator/>"
    "      <menuitem action='ShowStats'/>"
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
	// Gtk::AccelKey("<control>O")
	addItem(_menuFile, _miFileOpen, OnActionFileOpen, "Open _file", "document-open");
	
	// <control>D
	addItem(_menuFile, _miFileOpenDir, OnActionDirectoryOpen, "Open _directory", "folder");
	tooltip(_miFileOpenDir, "Open a directory. This action should be used to open a measurement set. For opening files (e.g. sdfits files), select 'Open file' instead.");
	
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
	
	addItem(_menuView, _miViewStats, OnShowStats, "Statistics");
}

/*
	_timeGraphButton = Gtk::ToggleAction::create("TimeGraph", "Time graph");
	_timeGraphButton->set_active(false); 
	_actionGroup->add(_timeGraphButton, RFIGuiMenu::OnTimeGraphButtonPressed);
	
  _originalFlagsButton = Gtk::ToggleAction::create("OriginalFlags", "Or flags");
	_originalFlagsButton->set_icon_name("showoriginalflags");
	_originalFlagsButton->set_tooltip("Display the first flag mask on top of the visibilities. These flags are displayed in purple and indicate the flags as they originally were stored in the measurement set.");
	_toggleConnections.push_back(_originalFlagsButton->signal_activate().connect(RFIGuiMenu::OnToggleFlags)));
	_actionGroup->add(_originalFlagsButton,
			Gtk::AccelKey("F3"));
	
  _altFlagsButton = Gtk::ToggleAction::create("AlternativeFlags", "Alt flags");
	_altFlagsButton->set_icon_name("showalternativeflags");
	_altFlagsButton->set_tooltip("Display the second flag mask on top of the visibilities. These flags are displayed in yellow and indicate flags found by running the strategy.");
	_toggleConnections.push_back(_altFlagsButton->signal_activate().connect(RFIGuiMenu::OnToggleFlags)));
	_actionGroup->add(_altFlagsButton,
			Gtk::AccelKey("F4"));
	
	_actionGroup->add( Gtk::Action::create("Highlight", "Highlight"),
  RFIGuiMenu::OnHightlightPressed);
	
	_zoomToFitButton = Gtk::Action::create("ZoomFit", "Zoom _fit");
	_zoomToFitButton->set_icon_name("zoom-fit-best");
	_actionGroup->add(_zoomToFitButton, Gtk::AccelKey("<control>0"),
		RFIGuiMenu::OnZoomFit);
	_zoomInButton = Gtk::Action::create("ZoomIn", "Zoom in");
	_zoomInButton->set_icon_name("zoom-in");
	_actionGroup->add(_zoomInButton, Gtk::AccelKey("<control>equal"),
		RFIGuiMenu::OnZoomIn);
	_zoomOutButton = Gtk::Action::create("ZoomOut", "Zoom out");
	_zoomOutButton->set_icon_name("zoom-out");
	_actionGroup->add(_zoomOutButton, Gtk::AccelKey("<control>minus"),
	RFIGuiMenu::OnZoomOut);
	_actionGroup->add( Gtk::Action::create("ShowImagePlane", "_Show image plane"),
		Gtk::AccelKey("<control>I"),
  RFIGuiMenu::OnShowImagePlane);
	_actionGroup->add( Gtk::Action::create("SetAndShowImagePlane", "S_et & show image plane"),
		Gtk::AccelKey("<control><shift>I"),
		RFIGuiMenu::OnSetAndShowImagePlane);
	_actionGroup->add( Gtk::Action::create("AddToImagePlane", "Add to _image plane"),
  RFIGuiMenu::OnAddToImagePlane);
	
	_actionGroup->add(Gtk::Action::create("ShowStats", "Show _stats"),
		Gtk::AccelKey("F2"),
		RFIGuiMenu::OnShowStats);
}

void RFIGuiMenu::makePlotMenu()
{
	_actionGroup->add( Gtk::Action::create("PlotPowerSpectrumComparison", "Power _spectrum"),
		RFIGuiMenu::OnPlotPowerSpectrumComparisonPressed);
	_actionGroup->add( Gtk::Action::create("PlotPowerTimeComparison", "Po_wer vs time"),
  RFIGuiMenu::OnPlotPowerTimeComparisonPressed);
	_actionGroup->add( Gtk::Action::create("PlotTimeScatterComparison", "Time _scatter"),
  RFIGuiMenu::OnPlotTimeScatterComparisonPressed);
	
	_actionGroup->add( Gtk::Action::create("PlotDist", "Plot _distribution"), RFIGuiMenu::OnPlotDistPressed);
	_actionGroup->add( Gtk::Action::create("PlotLogLogDist", "Plot _log-log dist"),
  RFIGuiMenu::OnPlotLogLogDistPressed);
	_actionGroup->add( Gtk::Action::create("PlotComplexPlane", "Plot _complex plane"),
		Gtk::AccelKey("<alt>C"),
		RFIGuiMenu::OnPlotComplexPlanePressed);
	_actionGroup->add( Gtk::Action::create("PlotMeanSpectrum", "Plot _mean spectrum"),
		Gtk::AccelKey("<alt>M"),
		RFIGuiMenu::OnPlotMeanSpectrumPressed);
	_actionGroup->add( Gtk::Action::create("PlotSumSpectrum", "Plot s_um spectrum"),
		Gtk::AccelKey("<alt>U"),
		RFIGuiMenu::OnPlotSumSpectrumPressed);
	_actionGroup->add( Gtk::Action::create("PlotPowerSpectrum", "Plot _power spectrum"),
		Gtk::AccelKey("<alt>W"),
		RFIGuiMenu::OnPlotPowerSpectrumPressed);
	_actionGroup->add( Gtk::Action::create("PlotFrequencyScatter", "Plot _frequency scatter"),
		RFIGuiMenu::OnPlotFrequencyScatterPressed);
	
	_actionGroup->add( Gtk::Action::create("PlotRMSSpectrum", "Plot _rms spectrum"),
		Gtk::AccelKey("<alt>R"),
		RFIGuiMenu::OnPlotPowerRMSPressed);
	_actionGroup->add( Gtk::Action::create("PlotPowerTime", "Plot power vs _time"),
		Gtk::AccelKey("<alt>T"),
		RFIGuiMenu::OnPlotPowerTimePressed);
	_actionGroup->add( Gtk::Action::create("PlotTimeScatter", "Plot t_ime scatter"),
 		Gtk::AccelKey("<alt>I"),
		RFIGuiMenu::OnPlotTimeScatterPressed);
	_actionGroup->add( Gtk::Action::create("PlotSingularValues", "Plot _singular values"),
  RFIGuiMenu::OnPlotSingularValuesPressed);
}

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

void RFIGuiMenu::makeToolbarActions()
{
  _showPPButton = Gtk::ToggleAction::create("DisplayPP", "PP");
	_showPPButton->set_active(_controller->IsPPShown());
	_showPPButton->set_icon_name("showpp");
	_showPPButton->set_tooltip("Display the PP polarization. Depending on the polarization configuration of the measurement set, this will show XX or RR");
	_toggleConnections.push_back(_showPPButton->signal_activate().connect(RFIGuiMenu::OnTogglePolarizations)));
	_actionGroup->add(_showPPButton);
	
  _showPQButton = Gtk::ToggleAction::create("DisplayPQ", "PQ");
	_showPQButton->set_active(_controller->IsPQShown());
	_showPQButton->set_icon_name("showpq");
	_showPQButton->set_tooltip("Display the PQ polarization. Depending on the polarization configuration of the measurement set, this will show XY or RL");
	_toggleConnections.push_back(_showPQButton->signal_activate().connect(RFIGuiMenu::OnTogglePolarizations)));
	_actionGroup->add(_showPQButton);
	
  _showQPButton = Gtk::ToggleAction::create("DisplayQP", "QP");
	_showQPButton->set_active(_controller->IsQPShown());
	_showQPButton->set_icon_name("showqp");
	_showQPButton->set_tooltip("Display the QP polarization. Depending on the polarization configuration of the measurement set, this will show YX or LR");
	_toggleConnections.push_back(_showQPButton->signal_activate().connect(RFIGuiMenu::OnTogglePolarizations)));
	_actionGroup->add(_showQPButton);
	
  _showQQButton = Gtk::ToggleAction::create("DisplayQQ", "QQ");
	_showQQButton->set_active(_controller->IsQQShown());
	_showQQButton->set_icon_name("showqq");
	_showQQButton->set_tooltip("Display the QQ polarization. Depending on the polarization configuration of the measurement set, this will show YY or LL");
	_toggleConnections.push_back(_showQQButton->signal_activate().connect(RFIGuiMenu::OnTogglePolarizations)));
	_actionGroup->add(_showQQButton);
	
	_selectVisualizationButton.set_label("Change visualization");
	_selectVisualizationButton.set_tooltip_text("Switch visualization");
	_selectVisualizationButton.set_arrow_tooltip_text("Select visualization");
	_selectVisualizationButton.set_icon_name("showoriginalvisibilities");
	pToolbar->append(_selectVisualizationButton);
	_selectVisualizationButton.set_menu(_tfVisualizationMenu);
	_selectVisualizationButton.signal_clicked().connect(RFIGuiMenu::OnToggleImage));
}
*/
