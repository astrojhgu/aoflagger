#include <gtkmm/actiongroup.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/image.h>
#include <gtkmm/imagemenuitem.h>
#include <gtkmm/menu.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menutoolbutton.h>
#include <gtkmm/radiomenuitem.h>
#include <gtkmm/radiotoolbutton.h>
#include <gtkmm/separatormenuitem.h>
#include <gtkmm/toggletoolbutton.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/toolbutton.h>

#include "../structures/timefrequencydata.h"

class RFIGuiMenu
{
public:
	RFIGuiMenu();
	
	Gtk::MenuBar& Menu() { return _menuBar; }
	Gtk::Toolbar& Toolbar() { return _toolbar; }
	
	sigc::signal<void()> OnLoadPrevious;
	sigc::signal<void()> OnLoadNext;
	sigc::signal<void()> OnToggleFlags;
	sigc::signal<void()> OnTogglePolarizations;
	sigc::signal<void()> OnToggleImage;
	sigc::signal<void()> OnSelectImage;
	sigc::signal<void()> OnQuit;
	sigc::signal<void()> OnActionFileOpen;
	sigc::signal<void()> OnActionDirectoryOpen;
	sigc::signal<void()> OnActionDirectoryOpenForSpatial;
	sigc::signal<void()> OnActionDirectoryOpenForST;
	sigc::signal<void()> OnSaveBaseline;
	sigc::signal<void()> OnTFZoomChanged;
	sigc::signal<void()> OnZoomFit;
	sigc::signal<void()> OnZoomIn;
	sigc::signal<void()> OnZoomOut;
	sigc::signal<void()> OnShowImagePlane;
	sigc::signal<void()> OnSetAndShowImagePlane;
	sigc::signal<void()> OnAddToImagePlane;
	sigc::signal<void()> OnClearOriginalFlagsPressed;
	sigc::signal<void()> OnClearAltFlagsPressed;
	sigc::signal<void()> OnVisualizedToOriginalPressed;
	sigc::signal<void()> OnHightlightPressed;
	sigc::signal<void()> OnKeepRealPressed;
	sigc::signal<void()> OnKeepImaginaryPressed;
	sigc::signal<void()> OnKeepPhasePressed;
	sigc::signal<void()> OnKeepStokesIPressed;
	sigc::signal<void()> OnKeepStokesQPressed;
	sigc::signal<void()> OnKeepStokesUPressed;
	sigc::signal<void()> OnKeepStokesVPressed;
	sigc::signal<void()> OnKeepRRPressed;
	sigc::signal<void()> OnKeepRLPressed;
	sigc::signal<void()> OnKeepLRPressed;
	sigc::signal<void()> OnKeepLLPressed;
	sigc::signal<void()> OnKeepXXPressed;
	sigc::signal<void()> OnKeepXYPressed;
	sigc::signal<void()> OnKeepYXPressed;
	sigc::signal<void()> OnKeepYYPressed;
	sigc::signal<void()> OnImagePropertiesPressed;
	sigc::signal<void()> OnOpenTestSetNoise;
	sigc::signal<void()> OnOpenTestSetA;
	sigc::signal<void()> OnOpenTestSetB;
	sigc::signal<void()> OnOpenTestSetC;
	sigc::signal<void()> OnOpenTestSetD;
	sigc::signal<void()> OnOpenTestSetE;
	sigc::signal<void()> OnOpenTestSetF;
	sigc::signal<void()> OnOpenTestSetG;
	sigc::signal<void()> OnOpenTestSetH;
	sigc::signal<void()> OnOpenTestSetNoise3Model;
	sigc::signal<void()> OnOpenTestSetNoise5Model;
	sigc::signal<void()> OnOpenTestSet3Model;
	sigc::signal<void()> OnOpenTestSet5Model;
	sigc::signal<void()> OnOpenTestSetBStrong;
	sigc::signal<void()> OnOpenTestSetBWeak;
	sigc::signal<void()> OnOpenTestSetBAligned;
	sigc::signal<void()> OnOpenTestSetGaussianBroadband;
	sigc::signal<void()> OnOpenTestSetSinusoidalBroadband;
	sigc::signal<void()> OnOpenTestSetSlewedGaussianBroadband;
	sigc::signal<void()> OnOpenTestSetBurstBroadband;
	sigc::signal<void()> OnOpenTestSetRFIDistributionLow;
	sigc::signal<void()> OnOpenTestSetRFIDistributionMid;
	sigc::signal<void()> OnOpenTestSetRFIDistributionHigh;
	sigc::signal<void()> OnGaussianTestSets;
	sigc::signal<void()> OnRayleighTestSets;
	sigc::signal<void()> OnZeroTestSets;
	sigc::signal<void()> OnAddStaticFringe;
	sigc::signal<void()> OnAdd1SigmaFringe;
	sigc::signal<void()> OnSetToOne;
	sigc::signal<void()> OnSetToI;
	sigc::signal<void()> OnSetToOnePlusI;
	sigc::signal<void()> OnAddCorrelatorFault;
	sigc::signal<void()> OnShowStats;
	sigc::signal<void()> OnPlotDistPressed;
	sigc::signal<void()> OnPlotLogLogDistPressed;
	sigc::signal<void()> OnPlotComplexPlanePressed;
	sigc::signal<void()> OnPlotMeanSpectrumPressed;
	sigc::signal<void()> OnPlotSumSpectrumPressed;
	sigc::signal<void()> OnPlotPowerSpectrumPressed;
	sigc::signal<void()> OnPlotPowerSpectrumComparisonPressed;
	sigc::signal<void()> OnPlotFrequencyScatterPressed;
	sigc::signal<void()> OnPlotPowerRMSPressed;
	sigc::signal<void()> OnPlotPowerTimePressed;
	sigc::signal<void()> OnPlotPowerTimeComparisonPressed;
	sigc::signal<void()> OnPlotTimeScatterPressed;
	sigc::signal<void()> OnPlotTimeScatterComparisonPressed;
	sigc::signal<void()> OnPlotSingularValuesPressed;
	sigc::signal<void()> OnEditStrategyPressed;
	sigc::signal<void()> OnExecuteStrategyPressed;
	sigc::signal<void()> OnReloadPressed;
	sigc::signal<void()> OnGoToPressed;
	sigc::signal<void()> OnLoadLongestBaselinePressed;
	sigc::signal<void()> OnLoadShortestBaselinePressed;
	sigc::signal<void()> OnMultiplyData;
	sigc::signal<void()> OnSegment;
	sigc::signal<void()> OnCluster;
	sigc::signal<void()> OnClassify;
	sigc::signal<void()> OnRemoveSmallSegments;
	sigc::signal<void()> OnTimeGraphButtonPressed;
	sigc::signal<void()> OnUnrollPhaseButtonPressed;
	sigc::signal<void()> OnVertEVD;
	sigc::signal<void()> OnApplyTimeProfile;
	sigc::signal<void()> OnApplyVertProfile;
	sigc::signal<void()> OnRestoreTimeProfile;
	sigc::signal<void()> OnRestoreVertProfile;
	sigc::signal<void()> OnReapplyTimeProfile;
	sigc::signal<void()> OnReapplyVertProfile;
	sigc::signal<void(bool)> OnUseTimeProfile;
	sigc::signal<void(bool)> OnUseVertProfile;
	sigc::signal<void()> OnStoreData;
	sigc::signal<void()> OnRecallData;
	sigc::signal<void()> OnSubtractDataFromMem;
	sigc::signal<void()> OnSimulateCorrelation;
	sigc::signal<void()> OnSimulateSourceSetA;
	sigc::signal<void()> OnSimulateSourceSetB;
	sigc::signal<void()> OnSimulateSourceSetC;
	sigc::signal<void()> OnSimulateSourceSetD;
	sigc::signal<void()> OnSimulateOffAxisSource;
	sigc::signal<void()> OnSimulateOnAxisSource;
	
	sigc::signal<void()> OnHelpAbout;
	
	sigc::signal<void(unsigned index)> openTestSet;
	
	sigc::signal<void()> OnControllerStateChange;
	
	sigc::signal<void()> OnExecutePythonStrategy;
	
	bool OriginalFlagsActive() const { return _originalFlagsButton.get_active(); }
	bool AlternativeFlagsActive() const { return _altFlagsButton.get_active(); }
	
	void SetOriginalFlagsActive(bool originalFlags) {
		_originalFlagsButton.set_active(originalFlags);
	}
	void SetAlternativeFlagsActive(bool alternativeFlags) {
		_altFlagsButton.set_active(alternativeFlags);
	}
	
	bool ShowPPActive() const { return _showPPButton.get_active(); }
	bool ShowPQActive() const { return _showPQButton.get_active(); }
	bool ShowQPActive() const { return _showQPButton.get_active(); }
	bool ShowQQActive() const { return _showQQButton.get_active(); }
	
	void SetShowPPActive(bool active) { _showPPButton.set_active(active); }
	void SetShowPQActive(bool active) { _showPQButton.set_active(active); }
	void SetShowQPActive(bool active) { _showQPButton.set_active(active); }
	void SetShowQQActive(bool active) { _showQQButton.set_active(active); }
	
	bool CloseExecuteFrame() const { return _closeExecuteFrameButton.get_active(); }
	
	bool TimeGraphActive() const { return _timeGraphButton.get_active(); }
	
	bool SimulateNCPActive() const { return _ncpSetButton.get_active(); }
	bool SimulateB1834Active() const { return _b1834SetButton.get_active(); }
	
	bool Simulate16ChActive() const { return _sim16ChannelsButton.get_active(); }
	bool Simulate64ChActive() const { return _sim64ChannelsButton.get_active(); }
	bool SimFixBandwidthActive() const { return _simFixBandwidthButton.get_active(); }
	
	void SetShowPPSensitive(bool sensitive) { _showPPButton.set_sensitive(sensitive); }
	void SetShowPQSensitive(bool sensitive) { _showPQButton.set_sensitive(sensitive); }
	void SetShowQPSensitive(bool sensitive) { _showQPButton.set_sensitive(sensitive); }
	void SetShowQQSensitive(bool sensitive) { _showQQButton.set_sensitive(sensitive); }
	
	void SetZoomToFitSensitive(bool sensitive) { _zoomToFitButton.set_sensitive(sensitive); }
	void SetZoomOutSensitive(bool sensitive) { _zoomOutButton.set_sensitive(sensitive); }
	void SetZoomInSensitive(bool sensitive) { _zoomInButton.set_sensitive(sensitive); }
	
	void SetPreviousSensitive(bool sensitive) { _previousButton.set_sensitive(sensitive); }
	void SetNextSensitive(bool sensitive) { _nextButton.set_sensitive(sensitive); }
	void SetReloadSensitive(bool sensitive) { _reloadButton.set_sensitive(sensitive); }
	
	void SetSelectVisualizationSensitive(bool sensitive) { _selectVisualizationButton.set_sensitive(sensitive); }
	
	Gtk::Menu& VisualizationMenu() { return _tfVisualizationMenu; }
	
	void BlockVisualizationSignals() { _blockVisualizationSignals = true; }
	void UnblockVisualizationSignals() { _blockVisualizationSignals = false; }
	
private:
	struct ImgMenuItem {
		Gtk::ImageMenuItem item;
		Gtk::HBox box;
		Gtk::AccelLabel label;
		Gtk::Image image;
	};
	
	void topMenu(Gtk::Menu& menu, Gtk::MenuItem& item, const char* label)
	{
		item.set_submenu(menu);
		item.set_label(label);
		item.set_use_underline(true);
		_menuBar.append(item);
	}
	
	void addItem(Gtk::Menu& menu, Gtk::SeparatorMenuItem& sep)
	{
		menu.append(sep);
	}
	
	template<typename SigType>
	void addItem(Gtk::Menu& menu, Gtk::MenuItem& item, const SigType& sig, const char* label)
	{
		item.set_label(label);
		item.signal_activate().connect(sig);
		menu.append(item);
	}
	
	template<typename SigType>
	void addItem(Gtk::Menu& menu, ImgMenuItem& item, const SigType& sig, const char* label, const char* icon)
	{
		
		//item.box.set_halign(Gtk::ALIGN_START);
		item.image.set_from_icon_name(icon, Gtk::BuiltinIconSize::ICON_SIZE_MENU);
		//item.box.pack_start(item.image);
		//item.label.set_label(label);
		//item.box.pack_start(item.label);
		item.item.set_label(label);
		item.item.set_use_underline(true);
		item.item.set_image(item.image);
		item.item.signal_activate().connect(sig);
		//item.item.add(item.box);
		menu.append(item.item);
	}
	
	void tooltip(ImgMenuItem& item, const char* tooltipStr) { }
	
	void makeFileMenu();
	void makeViewMenu();
	void makePlotMenu();
	void makeBrowseMenu();
	void makeSimulateMenu();
	void makeDataMenu();
	void makeActionsMenu();
	void makeHelpMenu();
	void makeToolbarActions();
	
	Gtk::MenuBar _menuBar;
	Gtk::Toolbar _toolbar;
	
	Gtk::Menu
		_menuFile, _menuView, _menuPlot, _menuBrowse,
		_menuSimulate, _menuData, _menuActions, _menuHelp;
	Gtk::MenuItem
		_miFile, _miView, _miPlot, _miBrowse,
		_miSimulate, _miData, _miActions, _miHelp;
		
	// File menu
	ImgMenuItem _miFileOpen, _miFileOpenDir, _miFileQuit;
	Gtk::MenuItem _miFileOpenSpatial, _miFileOpenST, _miFileSaveBaseline;
	
	// View menu
	Gtk::MenuItem _miViewProperties, _miViewTimeGraph;
	ImgMenuItem _miViewOriginalFlags, _miViewAlternativeFlags;
	Gtk::MenuItem _miViewHighlight;
	Gtk::SeparatorMenuItem _miViewSep1, _miViewSep2, _miViewSep3, _miViewSep4;
	ImgMenuItem _miViewZoomFit, _miViewZoomIn, _miViewZoomOut;
	Gtk::MenuItem _miViewImagePlane, _miViewSetImagePlane, _miViewAddToImagePlane;
	Gtk::MenuItem _miViewStats;

	Glib::RefPtr<Gtk::ActionGroup> _actionGroup;
	Gtk::ToolButton
		_previousButton, _reloadButton, _nextButton,
		_zoomToFitButton, _zoomInButton, _zoomOutButton;
	Gtk::MenuToolButton _selectVisualizationButton;
	Gtk::ToggleToolButton
		_originalFlagsButton, _altFlagsButton,
		_showPPButton, _showPQButton,
		_showQPButton, _showQQButton,
		_backgroundImageButton, _diffImageButton,
		_timeGraphButton, _simFixBandwidthButton,
		_closeExecuteFrameButton;
	bool _blockVisualizationSignals;
	Gtk::RadioToolButton
		_gaussianTestSetsButton, _rayleighTestSetsButton, _zeroTestSetsButton,
		_ncpSetButton, _b1834SetButton, _emptySetButton,
		_sim16ChannelsButton, _sim64ChannelsButton, _sim256ChannelsButton;
	Gtk::Menu _tfVisualizationMenu;
	
};

