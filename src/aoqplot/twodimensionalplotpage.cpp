#include <limits>

#include <boost/bind.hpp>

#include "datawindow.h"
#include "twodimensionalplotpage.h"

#include "../quality/statisticscollection.h"
#include "../quality/statisticsderivator.h"

#include "../plot/plotpropertieswindow.h"

TwoDimensionalPlotPage::TwoDimensionalPlotPage(AOQPlotPageController* controller) :
	_controller(controller),
	_countButton("#"),
	_meanButton("μ"),
	_stdDevButton("σ"),
	_varianceButton("σ²"),
	_dCountButton("Δ#"),
	_dMeanButton("Δμ"),
	_dStdDevButton("Δσ"),
	_rfiPercentageButton("%"),
	_polXXButton("XX"),
	_polXYButton("XY"),
	_polYXButton("YX"),
	_polYYButton("YY"),
	_polIButton("I"),
	_amplitudeButton("A"),
	_phaseButton("ϕ"),
	_realButton("r"),
	_imaginaryButton("i"),
	_logarithmicButton("Log"),
	_zeroAxisButton("0"),
	_plotPropertiesButton("P"),
	_dataExportButton("D"),
	_plotPropertiesWindow(nullptr),
	_customButtonsCreated(false)
{
	_plotWidget.SetPlot(_controller->Plot());
	pack_start(_plotWidget, Gtk::PACK_EXPAND_WIDGET);
	
	show_all_children();
	
	_dataWindow = new DataWindow();
	
	_controller->Attach(this);
}

TwoDimensionalPlotPage::~TwoDimensionalPlotPage()
{
	delete _dataWindow;
	if(_plotPropertiesWindow != 0)
		delete _plotPropertiesWindow;
}

void TwoDimensionalPlotPage::updatePlotConfig()
{
	_controller->Plot().SetIncludeZeroYAxis(_zeroAxisButton.get_active());
	_controller->Plot().SetLogarithmicYAxis(_logarithmicButton.get_active());
	_plotWidget.Update();
}

std::set<QualityTablesFormatter::StatisticKind> TwoDimensionalPlotPage::GetSelectedKinds() const
{
	std::set<QualityTablesFormatter::StatisticKind> kinds;
	if(_countButton.get_active())
		kinds.insert(QualityTablesFormatter::CountStatistic);
	if(_meanButton.get_active())
		kinds.insert(QualityTablesFormatter::MeanStatistic);
	if(_stdDevButton.get_active())
		kinds.insert(QualityTablesFormatter::StandardDeviationStatistic);
	if(_varianceButton.get_active())
		kinds.insert(QualityTablesFormatter::VarianceStatistic);
	if(_dCountButton.get_active())
		kinds.insert(QualityTablesFormatter::DCountStatistic);
	if(_dMeanButton.get_active())
		kinds.insert(QualityTablesFormatter::DMeanStatistic);
	if(_dStdDevButton.get_active())
		kinds.insert(QualityTablesFormatter::DStandardDeviationStatistic);
	if(_rfiPercentageButton.get_active())
		kinds.insert(QualityTablesFormatter::RFIPercentageStatistic);
	return kinds;
}

std::set<std::pair<unsigned int, unsigned int> > TwoDimensionalPlotPage::GetSelectedPolarizations() const
{
	std::set<std::pair<unsigned, unsigned> > pols;
	if(_polXXButton.get_active())
		pols.insert(std::make_pair(0, 0));
	if(_polXYButton.get_active())
		pols.insert(std::make_pair(1, 1));
	if(_polYXButton.get_active())
		pols.insert(std::make_pair(2, 2));
	if(_polYYButton.get_active())
		pols.insert(std::make_pair(3, 3));
	if(_polIButton.get_active())
		pols.insert(std::make_pair(0, 3));
	return pols;
}

std::set<AOQPlotPageController::PhaseType> TwoDimensionalPlotPage::GetSelectedPhases() const
{
	std::set<AOQPlotPageController::PhaseType> phases;
	if(_amplitudeButton.get_active())
		phases.insert(AOQPlotPageController::AmplitudePhaseType);
	if(_phaseButton.get_active())
		phases.insert(AOQPlotPageController::PhasePhaseType);
	if(_realButton.get_active())
		phases.insert(AOQPlotPageController::RealPhaseType);
	if(_imaginaryButton.get_active())
		phases.insert(AOQPlotPageController::ImaginaryPhaseType);
	return phases;
}

void TwoDimensionalPlotPage::InitializeToolbar(Gtk::Toolbar& toolbar)
{
	toolbar.set_toolbar_style(Gtk::TOOLBAR_TEXT);
	initStatisticKindButtons(toolbar);
	initPolarizationButtons(toolbar);
	initPhaseButtons(toolbar);
	initPlotButtons(toolbar);
	
	if(!_customButtonsCreated)
	{
		addCustomPlotButtons(toolbar);
		_customButtonsCreated = true;
	}
}

void TwoDimensionalPlotPage::initStatisticKindButtons(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator1);
	
	_countButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_countButton.set_tooltip_text("Visibility count");
	toolbar.append(_countButton);
	
	_meanButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_meanButton.set_tooltip_text("Mean value");
	toolbar.append(_meanButton);
	
	_stdDevButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_stdDevButton.set_active(true);
	_stdDevButton.set_tooltip_text("Standard deviation");
	toolbar.append(_stdDevButton);
	
	_varianceButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	toolbar.append(_varianceButton);
	
	//_dCountButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	//toolbar.append(_dCountButton);
	
	_dMeanButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_dMeanButton.set_tooltip_text("Frequency-differential (difference between channels) mean value");
	toolbar.append(_dMeanButton);
	
	_dStdDevButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_dStdDevButton.set_tooltip_text("Frequency-differential (difference between channels) standard deviation");
	toolbar.append(_dStdDevButton);
	
	_rfiPercentageButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_rfiPercentageButton.set_tooltip_text("Flagged percentage");
	toolbar.append(_rfiPercentageButton);
}

void TwoDimensionalPlotPage::initPolarizationButtons(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator2);
	
	_polXXButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_polXXButton.set_tooltip_text("XX polarization");
	toolbar.append(_polXXButton);
	
	_polXYButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_polXYButton.set_tooltip_text("XY polarization");
	toolbar.append(_polXYButton);
	
	_polYXButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_polYXButton.set_tooltip_text("YX polarization");
	toolbar.append(_polYXButton);
	
	_polYYButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_polYYButton.set_tooltip_text("YY polarization");
	toolbar.append(_polYYButton);
	
	_polIButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_polIButton.set_active(true);
	_polIButton.set_tooltip_text("Stokes I polarization");
	toolbar.append(_polIButton);
}

void TwoDimensionalPlotPage::initPhaseButtons(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator3);
	
	_amplitudeButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_amplitudeButton.set_active(true);
	_amplitudeButton.set_tooltip_text("Amplitude");
	toolbar.append(_amplitudeButton);
	
	_phaseButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_phaseButton.set_tooltip_text("Phase");
	toolbar.append(_phaseButton);
	
	_realButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_realButton.set_tooltip_text("Real value");
	toolbar.append(_realButton);
	
	_imaginaryButton.signal_clicked().connect(sigc::mem_fun(*_controller, &AOQPlotPageController::UpdatePlot));
	_imaginaryButton.set_tooltip_text("Imaginary value");
	toolbar.append(_imaginaryButton);
}

void TwoDimensionalPlotPage::initPlotButtons(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator4);
	
	_logarithmicButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::onLogarithmicClicked));
	toolbar.append(_logarithmicButton);
	
	_zeroAxisButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::updatePlotConfig));
	_zeroAxisButton.set_active(true);
	toolbar.append(_zeroAxisButton);
	_controller->Plot().SetIncludeZeroYAxis(true);
	
	_plotPropertiesButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::onPlotPropertiesClicked));
	toolbar.append(_plotPropertiesButton);

	_dataExportButton.signal_clicked().connect(sigc::mem_fun(*this, &TwoDimensionalPlotPage::onDataExportClicked));
	toolbar.append(_dataExportButton);
}

void TwoDimensionalPlotPage::onPlotPropertiesClicked()
{
	if(_plotPropertiesWindow == 0)
	{
		_plotPropertiesWindow = new PlotPropertiesWindow(_controller->Plot(), "Plot properties");
		_plotPropertiesWindow->OnChangesApplied = boost::bind(&AOQPlotPageController::UpdatePlot, _controller);
	}
	
	_plotPropertiesWindow->show();
	_plotPropertiesWindow->raise();
}

void TwoDimensionalPlotPage::updateDataWindow()
{
	if(_dataWindow->get_visible())
		_dataWindow->SetData(_controller->Plot());
}

void TwoDimensionalPlotPage::onDataExportClicked()
{
	_dataWindow->show();
	_dataWindow->raise();
	updateDataWindow();
}
void TwoDimensionalPlotPage::Redraw()
{
	_plotWidget.Update();
	
	if(_dataWindow->get_visible())
	{
		updateDataWindow();
	}
}
