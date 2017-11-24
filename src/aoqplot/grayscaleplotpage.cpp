#include <limits>

#include "controllers/heatmappagecontroller.h"

#include "grayscaleplotpage.h"

#include "../rfigui/imagepropertieswindow.h"

GrayScalePlotPage::GrayScalePlotPage(HeatMapPageController* controller) :
	_controller(controller),
	_countButton(_statisticGroup, "#"),
	_meanButton(_statisticGroup, "μ"),
	_stdDevButton(_statisticGroup, "σ"),
	_dCountButton(_statisticGroup, "Δ#"),
	_dMeanButton(_statisticGroup, "Δμ"),
	_dStdDevButton(_statisticGroup, "Δσ"),
	_rfiPercentageButton(_statisticGroup, "%"),
	_polXXButton(_polGroup, "XX"),
	_polXYButton(_polGroup, "XY"),
	_polYXButton(_polGroup, "YX"),
	_polYYButton(_polGroup, "YY"),
	_polIButton(_polGroup, "I"),
	_amplitudePhaseButton(_phaseGroup, "A"),
	_phasePhaseButton(_phaseGroup, "ϕ"),
	_realPhaseButton(_phaseGroup, "r"),
	_imaginaryPhaseButton(_phaseGroup, "i"),
	_rangeMinMaxButton(_rangeGroup, "Min to max"),
	_rangeWinsorizedButton(_rangeGroup, "Winsorized"),
	_rangeSpecified(_rangeGroup, "Specified"),
	_logarithmicScaleButton("Logarithmic"),
	_normalizeXAxisButton("Normalize X"),
	_normalizeYAxisButton("Normalize Y"),
	_meanNormButton(_rangeTypeGroup, "Mean"),
	_winsorNormButton(_rangeTypeGroup, "Winsor"),
	_medianNormButton(_rangeTypeGroup, "Median"),
	_plotPropertiesButton("Properties..."),
	_selectStatisticKind(QualityTablesFormatter::StandardDeviationStatistic),
	_imageWidget(&controller->Plot()),
	_imagePropertiesWindow(nullptr)
{
	_imageWidget.Plot().SetCairoFilter(Cairo::FILTER_NEAREST);
	_imageWidget.Plot().SetColorMap(HeatMapPlot::HotColdMap);
	_imageWidget.Plot().SetRange(HeatMapPlot::MinMax);
	_imageWidget.Plot().SetScaleOption(HeatMapPlot::LogScale);
	_imageWidget.Plot().SetZAxisDescription("Statistical value");
	_imageWidget.Plot().SetManualZAxisDescription(true);
	_imageWidget.set_size_request(300, 300);
	
	pack_start(_imageWidget);
	
	controller->Attach(this);
}

GrayScalePlotPage::~GrayScalePlotPage()
{
	if(_imagePropertiesWindow != 0)
		delete _imagePropertiesWindow;
}

void GrayScalePlotPage::InitializeToolbar(Gtk::Toolbar& toolbar)
{
	toolbar.set_toolbar_style(Gtk::TOOLBAR_TEXT);
	initStatisticKinds(toolbar);
	initPolarizations(toolbar);
	initPhaseButtons(toolbar);
	initPlotOptions(toolbar);
}

void GrayScalePlotPage::initStatisticKinds(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator1);
	
	_countButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectCount));
	_countButton.set_tooltip_text("Visibility count");
	toolbar.append(_countButton);
	
	_meanButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectMean));
	_meanButton.set_tooltip_text("Mean value");
	toolbar.append(_meanButton);
	
	_stdDevButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectStdDev));
	_stdDevButton.set_tooltip_text("Standard deviation");
	toolbar.append(_stdDevButton);
	
	//_dCountButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectDCount));
	//_dCountButton.set_tooltip_text("Visibility count in differential statistics");
	//toolbar.append(_dCountButton);
	
	_dMeanButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectDMean));
	_dMeanButton.set_tooltip_text("Frequency-differential (difference between channels) mean value");
	toolbar.append(_dMeanButton);
	
	_dStdDevButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectDStdDev));
	_dStdDevButton.set_tooltip_text("Frequency-differential (difference between channels) standard deviation");
	toolbar.append(_dStdDevButton);
	
	_rfiPercentageButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectRFIPercentage));
	_rfiPercentageButton.set_tooltip_text("Flagged percentage");
	toolbar.append(_rfiPercentageButton);
	
	_stdDevButton.set_active();
}

void GrayScalePlotPage::initPolarizations(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator2);
	
	_polXXButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_polXXButton.set_tooltip_text("XX polarization");
	toolbar.append(_polXXButton);
	
	_polXYButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_polXYButton.set_tooltip_text("XY polarization");
	toolbar.append(_polXYButton);

	_polYXButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_polYXButton.set_tooltip_text("YX polarization");
	toolbar.append(_polYXButton);

	_polYYButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_polYYButton.set_tooltip_text("YY polarization");
	toolbar.append(_polYYButton);

	_polIButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_polIButton.set_tooltip_text("Stokes I polarization");
	toolbar.append(_polIButton);

	_polIButton.set_active();
}

void GrayScalePlotPage::initPhaseButtons(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator3);
	
	_amplitudePhaseButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_amplitudePhaseButton.set_tooltip_text("Amplitude");
	toolbar.append(_amplitudePhaseButton);
	
	_phasePhaseButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_phasePhaseButton.set_tooltip_text("Phase");
	toolbar.append(_phasePhaseButton);
	
	_realPhaseButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_realPhaseButton.set_tooltip_text("Real value");
	toolbar.append(_realPhaseButton);
	
	_imaginaryPhaseButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::updateImage));
	_imaginaryPhaseButton.set_tooltip_text("Imaginary value");
	toolbar.append(_imaginaryPhaseButton);
	
	_amplitudePhaseButton.set_active();
}

void GrayScalePlotPage::initPlotOptions(Gtk::Toolbar& toolbar)
{
	toolbar.append(_separator4);
	
	_rangeMinMaxButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectMinMaxRange));
	toolbar.append(_rangeMinMaxButton);

	_rangeWinsorizedButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectWinsorizedRange));
	toolbar.append(_rangeWinsorizedButton);

	_rangeSpecified.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onSelectSpecifiedRange));
	toolbar.append(_rangeSpecified);
	
	toolbar.append(_separator5);
	
	_logarithmicScaleButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onLogarithmicScaleClicked));
	toolbar.append(_logarithmicScaleButton);
	_logarithmicScaleButton.set_active(true);
	
	_normalizeXAxisButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onNormalizeAxesButtonClicked));
	toolbar.append(_normalizeXAxisButton);
	
	_normalizeYAxisButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onNormalizeAxesButtonClicked));
	toolbar.append(_normalizeYAxisButton);
	
	toolbar.append(_separator6);
	
	_meanNormButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onChangeNormMethod));
	toolbar.append(_meanNormButton);
	
	_winsorNormButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onChangeNormMethod));
	toolbar.append(_winsorNormButton);
	
	_medianNormButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onChangeNormMethod));
	toolbar.append(_medianNormButton);
	
	_plotPropertiesButton.signal_clicked().connect(sigc::mem_fun(*this, &GrayScalePlotPage::onPropertiesClicked));
	toolbar.append(_plotPropertiesButton);
}

void GrayScalePlotPage::updateImage()
{
	_controller->SetKind(getSelectedStatisticKind());
	_controller->SetPolarization(getSelectedPolarization());
	_controller->SetPhase(getSelectedPhase());
	if(_meanNormButton.get_active())
		_controller->SetNormalization(HeatMapPageController::Mean);
	else if(_winsorNormButton.get_active())
		_controller->SetNormalization(HeatMapPageController::Winsorized);
	else // _medianNormButton
		_controller->SetNormalization(HeatMapPageController::Median);
	_controller->UpdateImage();
}

PolarizationEnum GrayScalePlotPage::getSelectedPolarization() const
{
	if(_polXXButton.get_active())
		return Polarization::XX;
	else if(_polXYButton.get_active())
		return Polarization::XY;
	else if(_polYXButton.get_active())
		return Polarization::YX;
	else if(_polYYButton.get_active())
		return Polarization::YY;
	else
		return Polarization::StokesI;
}

enum TimeFrequencyData::ComplexRepresentation GrayScalePlotPage::getSelectedPhase() const
{
	if(_amplitudePhaseButton.get_active())
		return TimeFrequencyData::AmplitudePart;
	else if(_phasePhaseButton.get_active())
		return TimeFrequencyData::PhasePart;
	else if(_realPhaseButton.get_active())
		return TimeFrequencyData::RealPart;
	else if(_imaginaryPhaseButton.get_active())
		return TimeFrequencyData::ImaginaryPart;
	else
		return TimeFrequencyData::AmplitudePart;
}

void GrayScalePlotPage::onPropertiesClicked()
{
	if(_imagePropertiesWindow == 0)
		_imagePropertiesWindow = new ImagePropertiesWindow(_imageWidget, "Plotting properties");
	_imagePropertiesWindow->show();
	_imagePropertiesWindow->raise();
}

void GrayScalePlotPage::Redraw()
{
	_imageWidget.Update();
}
