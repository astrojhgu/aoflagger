#include "strategywizardwindow.h"
#include "interfaces.h"

#include "controllers/rfiguicontroller.h"

#include "../strategy/control/defaultstrategy.h"
#include "../strategy/actions/strategy.h"

#include "../gtkmm-compat.h"

StrategyWizardWindow::StrategyWizardWindow(class RFIGuiController& guiController, class StrategyController& controller) : Window(),
	_strategyController(controller),
	_page(0),
	_telescopeLabel("Telescope:"),
	_telescopeCombo(),
	_finishButton("_Finish", true),
	_nextButton("_Next", true),
	_previousButton("_Previous", true),
	_transientsButton("Transients"), _highTimeResolutionButton("High time resolution"),
	_smallBandwidthButton("Small bandwidth"), _normBandwidthButton("Normal bandwidth"), _largeBandwidthButton("Large bandwidth"),
	_robustConvergenceButton("More iterations (more robust)"),
	_normConvergenceButton("Default number of iterations"),
	_fastConvergenceButton("Fewer iterations (faster)"),
	_insensitiveButton("Insensitive"),
	_normalSensitivityButton("Normal sensitivity"), _sensitiveButton("Sensitive"),
	_useOriginalFlagsButton("Use existing flags"),
	_autoCorrelationButton("Auto-correlation")
{
	set_default_size(500, 250);
	
	initializeTelescopePage(guiController);
	
	initializeOptionPage();
	
	gtkmm_set_image_from_icon_name(_previousButton, "go-previous");
	_previousButton.signal_clicked().connect(
		sigc::mem_fun(*this, &StrategyWizardWindow::onPreviousClicked));
	_buttonBox.pack_start(_previousButton, true, false);
	gtkmm_set_image_from_icon_name(_nextButton, "go-next");
	_nextButton.signal_clicked().connect(
		sigc::mem_fun(*this, &StrategyWizardWindow::onNextClicked));
	_buttonBox.pack_start(_nextButton, true, false);
	gtkmm_set_image_from_icon_name(_finishButton, "gtk-ok");
	_finishButton.signal_clicked().connect(
		sigc::mem_fun(*this, &StrategyWizardWindow::onFinishClicked));
	_buttonBox.pack_end(_finishButton, true, false);
	_finishButton.set_sensitive(false);
	_mainBox.pack_end(_buttonBox, false, false);
	
	add(_mainBox);
	_buttonBox.show_all();
	_mainBox.show();
	
	updateSensitivities();
}

void StrategyWizardWindow::initializeTelescopePage(class RFIGuiController& guiController)
{
	_telescopeSubBox.pack_start(_telescopeLabel);
	_telescopeList = Gtk::ListStore::create(_telescopeListColumns);
	addTelescope("Generic", rfiStrategy::DefaultStrategy::GENERIC_TELESCOPE);
	addTelescope("AARTFAAC", rfiStrategy::DefaultStrategy::AARTFAAC_TELESCOPE);
	addTelescope("Arecibo (305 m single dish, Puerto Rico)", rfiStrategy::DefaultStrategy::ARECIBO_TELESCOPE);
	addTelescope("Bighorns (low-frequency wide-band EoR instrument, Curtin uni, Australia)", rfiStrategy::DefaultStrategy::BIGHORNS_TELESCOPE);
	addTelescope("JVLA (Jansky Very Large Array, New Mexico)", rfiStrategy::DefaultStrategy::JVLA_TELESCOPE);
	addTelescope("LOFAR (Low-Frequency Array, Europe)", rfiStrategy::DefaultStrategy::LOFAR_TELESCOPE);
	addTelescope("MWA (Murchison Widefield Array, Australia)", rfiStrategy::DefaultStrategy::MWA_TELESCOPE);
	addTelescope("Parkes (single dish, Australia)",    rfiStrategy::DefaultStrategy::PARKES_TELESCOPE);
	addTelescope("WSRT (Westerbork Synthesis Radio Telescope, Netherlands)", rfiStrategy::DefaultStrategy::WSRT_TELESCOPE);
	
	_telescopeCombo.set_model(_telescopeList);
	_telescopeCombo.pack_start(_telescopeListColumns.name, false);
	_telescopeCombo.signal_changed().connect(sigc::mem_fun(*this, &StrategyWizardWindow::updateSensitivities));
	_telescopeSubBox.pack_start(_telescopeCombo, false, false);
	
	if(guiController.HasImageSet())
	{
		rfiStrategy::ImageSet& set = guiController.GetImageSet();
		rfiStrategy::DefaultStrategy::TelescopeId telescope;
		unsigned flags;
		double frequency, timeRes, freqRes;
		rfiStrategy::DefaultStrategy::DetermineSettings(set, telescope, flags, frequency, timeRes, freqRes);
		Gtk::TreeModel::Children rows = _telescopeList->children();
		for(Gtk::TreeModel::iterator row=rows.begin(); row!=rows.end(); ++row)
		{
			if((*row)[_telescopeListColumns.val] == telescope)
			{
				_telescopeCombo.set_active(row);
				break;
			}
		}
	}
	
	_telescopeBox.pack_start(_telescopeSubBox, false, false);
	_mainBox.pack_start(_telescopeBox);
	_telescopeBox.show_all();
}

void StrategyWizardWindow::initializeOptionPage()
{
	_optionsLeftBox.pack_start(_transientsButton, true, true);
	_optionsLeftBox.pack_start(_highTimeResolutionButton, true, true);
	
	Gtk::RadioButton::Group bandwidthGroup;
	_optionsRightBox.pack_start(_smallBandwidthButton, true, true);
	_smallBandwidthButton.set_group(bandwidthGroup);
	_optionsRightBox.pack_start(_normBandwidthButton, true, true);
	_normBandwidthButton.set_group(bandwidthGroup);
	_normBandwidthButton.set_active(true);
	_optionsRightBox.pack_start(_largeBandwidthButton, true, true);
	_largeBandwidthButton.set_group(bandwidthGroup);
	
	Gtk::RadioButton::Group convergenceGroup;
	_optionsLeftBox.pack_start(_robustConvergenceButton, true, true);
	_robustConvergenceButton.set_group(convergenceGroup);
	_optionsLeftBox.pack_start(_normConvergenceButton, true, true);
	_normConvergenceButton.set_group(convergenceGroup);
	_normConvergenceButton.set_active(true);
	_optionsLeftBox.pack_start(_fastConvergenceButton, true, true);
	_fastConvergenceButton.set_group(convergenceGroup);
		
	Gtk::RadioButton::Group sensitivityGroup;
	_optionsRightBox.pack_start(_insensitiveButton, true, true);
	_insensitiveButton.set_group(sensitivityGroup);
	_optionsRightBox.pack_start(_normalSensitivityButton, true, true);
	_normalSensitivityButton.set_group(sensitivityGroup);
	_normalSensitivityButton.set_active(true);
	_optionsRightBox.pack_start(_sensitiveButton, true, true);
	_sensitiveButton.set_group(sensitivityGroup);
	
	_optionsLeftBox.pack_start(_useOriginalFlagsButton, true, true);
	_optionsLeftBox.pack_start(_autoCorrelationButton, true, true);
	_optionsBox.pack_start(_optionsLeftBox);
	_optionsBox.pack_start(_optionsRightBox);
	
	_optionsBox.show_all_children();
	_mainBox.pack_start(_optionsBox);
}

void StrategyWizardWindow::addTelescope(const Glib::ustring& name, int val)
{
	Gtk::TreeModel::iterator row = _telescopeList->append();
	(*row)[_telescopeListColumns.name] = name;
	(*row)[_telescopeListColumns.val] = val;
}

void StrategyWizardWindow::onPreviousClicked()
{
	if(_page > 0)
	{
		--_page;
		updatePage();
	}
}

void StrategyWizardWindow::onNextClicked()
{
	if(_page < 1)
	{
		++_page;
		updatePage();
	}
}

void StrategyWizardWindow::onFinishClicked()
{
	const enum rfiStrategy::DefaultStrategy::TelescopeId telescopeId =
		(enum rfiStrategy::DefaultStrategy::TelescopeId) (int) ((*_telescopeCombo.get_active())[_telescopeListColumns.val]);
		
	int flags = rfiStrategy::DefaultStrategy::FLAG_NONE;
	if(_largeBandwidthButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_LARGE_BANDWIDTH;
	if(_smallBandwidthButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_SMALL_BANDWIDTH;
	if(_transientsButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_TRANSIENTS;
	if(_highTimeResolutionButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_HIGH_TIME_RESOLUTION;
	if(_robustConvergenceButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_ROBUST;
	if(_fastConvergenceButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_FAST;
	if(_insensitiveButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_INSENSITIVE;
	if(_sensitiveButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_SENSITIVE;
	if(_useOriginalFlagsButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_USE_ORIGINAL_FLAGS;
	if(_autoCorrelationButton.get_active())
		flags |= rfiStrategy::DefaultStrategy::FLAG_AUTO_CORRELATION;
	
	std::unique_ptr<rfiStrategy::Strategy> strategy(new rfiStrategy::Strategy());
	rfiStrategy::DefaultStrategy::LoadSingleStrategy(*strategy, rfiStrategy::DefaultStrategy::DetermineSetup(telescopeId, flags, 0.0, 0.0, 0.0));
		
	_strategyController.SetStrategy(std::move(strategy));
	_strategyController.NotifyChange();
	
	hide();
	_page = 0;
	updatePage();
}

void StrategyWizardWindow::updateSensitivities()
{
	bool hasTelescope = (_telescopeCombo.get_active_row_number() != -1);
	_previousButton.set_sensitive(_page!=0);
	_nextButton.set_sensitive(_page!=1 && hasTelescope);
	_finishButton.set_sensitive(_page==1 && hasTelescope);
}

void StrategyWizardWindow::updatePage()
{
	_telescopeBox.hide();
	_optionsBox.hide();
	if(_page == 0)
		_telescopeBox.show();
	else
		_optionsBox.show();
	updateSensitivities();
}
