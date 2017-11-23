#include "rfiguicontroller.h"

#include "../../strategy/actions/strategy.h"

#include "../../strategy/algorithms/fringestoppingfitter.h"
#include "../../strategy/algorithms/mitigationtester.h"
#include "../../strategy/algorithms/svdmitigater.h"

#include "../../strategy/plots/rfiplots.h"
#include "../../strategy/control/defaultstrategy.h"

#include "../../strategy/imagesets/imageset.h"
#include "../../strategy/imagesets/msimageset.h"
#include "../../strategy/imagesets/joinedspwset.h"
#include "../../strategy/imagesets/spatialmsimageset.h"
#include "../../strategy/imagesets/spatialtimeimageset.h"

#include "../../quality/histogramcollection.h"

#include "../../util/multiplot.h"

#include "../plot/plotmanager.h"

#include "../../structures/spatialmatrixmetadata.h"

#include "../rfiguiwindow.h"

#include "imagecomparisoncontroller.h"

#include <gtkmm/messagedialog.h>

RFIGuiController::RFIGuiController() :
	_showOriginalFlags(true), _showAlternativeFlags(true),
	_showPP(true), _showPQ(false), _showQP(false), _showQQ(true),
	_rfiGuiWindow(nullptr), _strategyController(nullptr),
	_tfController()
{
	_plotManager = new class PlotManager();
}

RFIGuiController::~RFIGuiController()
{
	delete _plotManager;
}

bool RFIGuiController::IsImageLoaded() const
{
	return _tfController.Plot().HasImage();
}

TimeFrequencyData RFIGuiController::ActiveData() const
{
	return _tfController.GetActiveData();
}

TimeFrequencyData RFIGuiController::OriginalData() const
{
	return _tfController.OriginalData();
}

TimeFrequencyData RFIGuiController::RevisedData() const
{
	return _tfController.RevisedData();
}

TimeFrequencyData RFIGuiController::ContaminatedData() const
{
	return _tfController.ContaminatedData();
}

TimeFrequencyMetaDataCPtr RFIGuiController::SelectedMetaData() const
{
	return _tfController.Plot().GetSelectedMetaData();
}

void RFIGuiController::plotMeanSpectrum(bool weight)
{
	if(IsImageLoaded())
	{
		std::string title = weight ? "Sum spectrum" : "Mean spectrum";
		Plot2D &plot = _plotManager->NewPlot2D(title);

		TimeFrequencyData data = ActiveData();
		Mask2DCPtr mask =
			Mask2D::CreateSetMaskPtr<false>(data.ImageWidth(), data.ImageHeight());
		Plot2DPointSet &beforeSet = plot.StartLine("Without flagging");
		if(weight)
			RFIPlots::MakeMeanSpectrumPlot<true>(beforeSet, data, mask, SelectedMetaData());
		else
			RFIPlots::MakeMeanSpectrumPlot<false>(beforeSet, data, mask, SelectedMetaData());

		mask.reset(new Mask2D(*data.GetSingleMask()));
		if(!mask->AllFalse())
		{
			Plot2DPointSet &afterSet = plot.StartLine("Flagged");
			if(weight)
				RFIPlots::MakeMeanSpectrumPlot<true>(afterSet, data, mask, SelectedMetaData());
			else
				RFIPlots::MakeMeanSpectrumPlot<false>(afterSet, data, mask, SelectedMetaData());
		}
		
		_plotManager->Update();
	}
}

void RFIGuiController::PlotDist()
{
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("Distribution");

		TimeFrequencyData activeData = ActiveData();
		Image2DCPtr image = activeData.GetSingleImage();
		Mask2DPtr mask =
			Mask2D::CreateSetMaskPtr<false>(image->Width(), image->Height());
		Plot2DPointSet &totalSet = plot.StartLine("Total");
		RFIPlots::MakeDistPlot(totalSet, image, mask);

		Plot2DPointSet &uncontaminatedSet = plot.StartLine("Uncontaminated");
		mask.reset(new Mask2D(*activeData.GetSingleMask()));
		RFIPlots::MakeDistPlot(uncontaminatedSet, image, mask);

		mask->Invert();
		Plot2DPointSet &rfiSet = plot.StartLine("RFI");
		RFIPlots::MakeDistPlot(rfiSet, image, mask);

		_plotManager->Update();
	}
}

void RFIGuiController::PlotLogLogDist()
{
	if(IsImageLoaded())
	{
		TimeFrequencyData activeData = ActiveData();
		HistogramCollection histograms(activeData.PolarizationCount());
		for(unsigned p=0;p!=activeData.PolarizationCount();++p)
		{
			TimeFrequencyData polData(activeData.MakeFromPolarizationIndex(p));
			Image2DCPtr image = polData.GetSingleImage();
			Mask2DCPtr mask = std::make_shared<Mask2D>(*polData.GetSingleMask());
			histograms.Add(0, 1, p, image, mask);
		}
		_rfiGuiWindow->ShowHistogram(histograms);
	}
}

void RFIGuiController::PlotPowerSpectrum()
{
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("Power spectrum");
		plot.SetLogarithmicYAxis(true);

		TimeFrequencyData data = ActiveData();
		Image2DCPtr image = data.GetSingleImage();
		Mask2DPtr mask =
			Mask2D::CreateSetMaskPtr<false>(image->Width(), image->Height());
		Plot2DPointSet &beforeSet = plot.StartLine("Before");
		RFIPlots::MakePowerSpectrumPlot(beforeSet, image, mask, SelectedMetaData());

		mask = std::make_shared<Mask2D>(*data.GetSingleMask());
		if(!mask->AllFalse())
		{
			Plot2DPointSet &afterSet = plot.StartLine("After");
			RFIPlots::MakePowerSpectrumPlot(afterSet, image, mask, SelectedMetaData());
		}
		
		_plotManager->Update();
	}
}

void RFIGuiController::PlotPowerSpectrumComparison()
{
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("Power spectrum comparison");

		TimeFrequencyData data = OriginalData();
		Image2DCPtr image = data.GetSingleImage();
		Mask2DCPtr mask = data.GetSingleMask();
		Plot2DPointSet &originalSet = plot.StartLine("Original");
		RFIPlots::MakePowerSpectrumPlot(originalSet, image, mask, SelectedMetaData());

		data = ContaminatedData();
		image = data.GetSingleImage();
		mask = data.GetSingleMask();
		Plot2DPointSet &alternativeSet = plot.StartLine("Alternative");
		RFIPlots::MakePowerSpectrumPlot(alternativeSet, image, mask, SelectedMetaData());
	
		_plotManager->Update();
	}
}

void RFIGuiController::PlotFrequencyScatter()
{
	if(IsImageLoaded())
	{
		MultiPlot plot(_plotManager->NewPlot2D("Frequency scatter"), 4);
		RFIPlots::MakeFrequencyScatterPlot(plot, ActiveData(), SelectedMetaData());
		plot.Finish();
		_plotManager->Update();
	}
}

void RFIGuiController::PlotPowerRMS()
{
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("Spectrum RMS");
		plot.SetLogarithmicYAxis(true);

		TimeFrequencyData activeData = ActiveData();
		Image2DCPtr image = activeData.GetSingleImage();
		Mask2DPtr mask =
			Mask2D::CreateSetMaskPtr<false>(image->Width(), image->Height());
		Plot2DPointSet &beforeSet = plot.StartLine("Before");
		RFIPlots::MakeRMSSpectrumPlot(beforeSet, image, mask);

		mask = std::make_shared<Mask2D>(*activeData.GetSingleMask());
		if(!mask->AllFalse())
		{
			Plot2DPointSet &afterSet = plot.StartLine("After");
			RFIPlots::MakeRMSSpectrumPlot(afterSet, image, mask);
	
			//mask->Invert();
			//Plot2DPointSet &rfiSet = plot.StartLine("RFI");
			//RFIPlots::MakeRMSSpectrumPlot(rfiSet, _timeFrequencyWidget.Image(), mask);
		}

		_plotManager->Update();
	}
}

void RFIGuiController::PlotPowerSNR()
{
	Image2DCPtr
		image = ActiveData().GetSingleImage(),
		model = RevisedData().GetSingleImage();
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("SNR spectrum");
		plot.SetLogarithmicYAxis(true);

		Mask2DPtr mask =
			Mask2D::CreateSetMaskPtr<false>(image->Width(), image->Height());
		Plot2DPointSet &totalPlot = plot.StartLine("Total");
		RFIPlots::MakeSNRSpectrumPlot(totalPlot, image, model, mask);

		mask = std::make_shared<Mask2D>(*ActiveData().GetSingleMask());
		if(!mask->AllFalse())
		{
			Plot2DPointSet &uncontaminatedPlot = plot.StartLine("Uncontaminated");
			RFIPlots::MakeSNRSpectrumPlot(uncontaminatedPlot, image, model, mask);
	
			mask->Invert();
			Plot2DPointSet &rfiPlot = plot.StartLine("RFI");
			RFIPlots::MakeSNRSpectrumPlot(rfiPlot, image, model, mask);
		}

		_plotManager->Update();
	}
}

void RFIGuiController::PlotPowerTime()
{
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("Power over time");
		plot.SetLogarithmicYAxis(true);

		TimeFrequencyData activeData = ActiveData();
		Image2DCPtr image = activeData.GetSingleImage();
		Mask2DPtr mask =
			Mask2D::CreateSetMaskPtr<false>(image->Width(), image->Height());
		Plot2DPointSet &totalPlot = plot.StartLine("Total");
		RFIPlots::MakePowerTimePlot(totalPlot, image, mask, SelectedMetaData());

		mask = std::make_shared<Mask2D>(*activeData.GetSingleMask());
		if(!mask->AllFalse())
		{
			Plot2DPointSet &uncontaminatedPlot = plot.StartLine("Uncontaminated");
			RFIPlots::MakePowerTimePlot(uncontaminatedPlot, image, mask, SelectedMetaData());
	
			mask->Invert();
			Plot2DPointSet &rfiPlot = plot.StartLine("RFI");
			RFIPlots::MakePowerTimePlot(rfiPlot, image, mask, SelectedMetaData());
		}

		_plotManager->Update();
	}
}

void RFIGuiController::PlotPowerTimeComparison()
{
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("Time comparison");

		TimeFrequencyData data = OriginalData();
		Mask2DCPtr mask = data.GetSingleMask();
		Image2DCPtr image = data.GetSingleImage();
		Plot2DPointSet &originalPlot = plot.StartLine("Original");
		RFIPlots::MakePowerTimePlot(originalPlot, image, mask, SelectedMetaData());

		data = ContaminatedData();
		mask = data.GetSingleMask();
		image = data.GetSingleImage();
		Plot2DPointSet &alternativePlot = plot.StartLine("Original");
		plot.StartLine("Alternative");
		RFIPlots::MakePowerTimePlot(alternativePlot, image, mask, SelectedMetaData());

		_plotManager->Update();
	}
}

void RFIGuiController::PlotTimeScatter()
{
	if(IsImageLoaded())
	{
		MultiPlot plot(_plotManager->NewPlot2D("Time scatter"), 4);
		RFIPlots::MakeTimeScatterPlot(plot, ActiveData(), SelectedMetaData());
		plot.Finish();
		_plotManager->Update();
	}
}

void RFIGuiController::PlotTimeScatterComparison()
{
	if(IsImageLoaded())
	{
		MultiPlot plot(_plotManager->NewPlot2D("Time scatter comparison"), 8);
		RFIPlots::MakeTimeScatterPlot(plot, OriginalData(), SelectedMetaData(), 0);
		RFIPlots::MakeTimeScatterPlot(plot, ContaminatedData(), SelectedMetaData(), 4);
		plot.Finish();
		_plotManager->Update();
	}
}

void RFIGuiController::PlotSingularValues()
{
	if(IsImageLoaded())
	{
		Plot2D &plot = _plotManager->NewPlot2D("Singular values");

		SVDMitigater::CreateSingularValueGraph(ActiveData(), plot);
		_plotManager->Update();
	}
}

void RFIGuiController::Open(const std::string& filename, BaselineIOMode ioMode, bool readUVW, const std::string& dataColumn, bool subtractModel, size_t polCountToRead, bool loadStrategy, bool combineSPW)
{
	Logger::Info << "Opening " << filename << '\n';
	try
	{
		std::unique_ptr<rfiStrategy::ImageSet> imageSet(rfiStrategy::ImageSet::Create(filename, ioMode));
		rfiStrategy::MSImageSet* msImageSet =
			dynamic_cast<rfiStrategy::MSImageSet*>(imageSet.get());
		if(msImageSet != 0)
		{
			msImageSet->SetSubtractModel(subtractModel);
			msImageSet->SetDataColumnName(dataColumn);
	
			if(polCountToRead == 1)
				msImageSet->SetReadStokesI();
			else if(polCountToRead == 2)
				msImageSet->SetReadDipoleAutoPolarisations();
			else
				msImageSet->SetReadAllPolarisations();
	
			msImageSet->SetReadUVW(readUVW);
			
			if(combineSPW)
			{
				msImageSet->Initialize();
				imageSet.release();
				std::unique_ptr<rfiStrategy::MSImageSet> msImageSetPtr(msImageSet);
				imageSet.reset(new rfiStrategy::JoinedSPWSet(std::move(msImageSetPtr)));
			}
		}
		imageSet->Initialize();
		
		if(loadStrategy && _strategyController!=nullptr)
		{
			rfiStrategy::DefaultStrategy::TelescopeId telescopeId;
			unsigned flags;
			double frequency, timeResolution, frequencyResolution;
			rfiStrategy::DefaultStrategy::DetermineSettings(*imageSet, telescopeId, flags, frequency, timeResolution, frequencyResolution);
			_strategyController->Strategy().RemoveAll();
			rfiStrategy::DefaultStrategy::LoadStrategy(
				_strategyController->Strategy(),
				telescopeId,
				flags | rfiStrategy::DefaultStrategy::FLAG_GUI_FRIENDLY,
				frequency,
				timeResolution,
				frequencyResolution
			);
			_strategyController->NotifyChange();
		}
	
		SetImageSet(std::move(imageSet));
		
	} catch(std::exception &e)
	{
		if(_rfiGuiWindow != nullptr)
		{
			Gtk::MessageDialog dialog(*_rfiGuiWindow, e.what(), false, Gtk::MESSAGE_ERROR);
			dialog.run();
		}
		else {
			Logger::Error << e.what() << '\n';
		}
	}
}

void RFIGuiController::OpenTestSet(unsigned index, bool gaussianTestSets)
{
	unsigned width = 1024*16, height = 1024;
	if(IsImageLoaded())
	{
		TimeFrequencyData activeData = ActiveData();
		width = activeData.ImageWidth();
		height = activeData.ImageHeight();
	}
	Mask2DPtr rfi = Mask2D::CreateSetMaskPtr<false>(width, height);
	Image2DPtr testSetReal(MitigationTester::CreateTestSet(index, rfi, width, height, gaussianTestSets));
	Image2DPtr testSetImaginary(MitigationTester::CreateTestSet(2, rfi, width, height, gaussianTestSets));
	TimeFrequencyData data(Polarization::StokesI, testSetReal, testSetImaginary);
	data.SetGlobalMask(rfi);
	
	_tfController.SetNewData(data, SelectedMetaData());
	_rfiGuiWindow->GetTimeFrequencyWidget().Update();
}

void RFIGuiController::ExecutePythonStrategy()
{
	TimeFrequencyData data = OriginalData(); 
	_pythonStrategy.Execute(data);
	_tfController.SetContaminatedData(data);
	_rfiGuiWindow->GetTimeFrequencyWidget().Update();
}

void RFIGuiController::SetImageSet(std::unique_ptr<rfiStrategy::ImageSet> newImageSet)
{
	_imageSetIndex = newImageSet->StartIndex();
	_imageSet = std::move(newImageSet);
	if((_rfiGuiWindow!=nullptr) && dynamic_cast<rfiStrategy::IndexableSet*>(_imageSet.get()) != 0)
	{
		_rfiGuiWindow->OpenGotoWindow();
	} else {
		LoadCurrentTFData();
	}
}

void RFIGuiController::SetImageSetIndex(std::unique_ptr<rfiStrategy::ImageSetIndex> newImageSetIndex)
{
	_imageSetIndex = std::move(newImageSetIndex);
	LoadCurrentTFData();
}

void RFIGuiController::LoadCurrentTFData()
{
	if(HasImageSet()) {
		std::unique_lock<std::mutex> lock(_ioMutex);
		_imageSet->AddReadRequest(*_imageSetIndex);
		_imageSet->PerformReadRequests();
		std::unique_ptr<rfiStrategy::BaselineData> baseline = _imageSet->GetNextRequested();
		lock.unlock();
		
		_tfController.SetNewData(baseline->Data(), baseline->MetaData());
		baseline.reset();
		
		_spatialMetaData.reset();
		rfiStrategy::SpatialMSImageSet* smsImageSet =
			dynamic_cast<rfiStrategy::SpatialMSImageSet*>(_imageSet.get());
		if(smsImageSet != nullptr)
		{
			_spatialMetaData.reset(new SpatialMatrixMetaData(smsImageSet->SpatialMetaData(GetImageSetIndex())));
		}
		
		// We store these seperate, as they might access the measurement set. This is
		// not only faster (the names are used in the onMouse.. events) but also less dangerous,
		// since the set can be simultaneously accessed by another thread. (thus the io mutex should
		// be locked before calling below statements).
		std::string name, description;
		lock.lock();
		name = GetImageSet().Name();
		description = GetImageSetIndex().Description();
		lock.unlock();
		
		_tfController.Plot().SetTitleText(description);
		
		if(_rfiGuiWindow != nullptr)
		{
			// Disable forward/back buttons when only one baseline is available
			std::unique_ptr<rfiStrategy::ImageSetIndex> firstIndex = _imageSet->StartIndex();
			firstIndex->Next();
			bool multipleBaselines = firstIndex->IsValid();
			firstIndex.reset();
			
			_rfiGuiWindow->SetBaselineInfo(multipleBaselines, name, description);
		}
	}
}

void RFIGuiController::SetRevisedData(const TimeFrequencyData& data)
{
	_tfController.SetRevisedData(data);
}

void RFIGuiController::LoadPath(const std::string& filename)
{
	std::unique_lock<std::mutex> lock(_ioMutex);
	std::unique_ptr<rfiStrategy::ImageSet> imageSet(rfiStrategy::ImageSet::Create(filename, DirectReadMode));
	imageSet->Initialize();
	lock.unlock();
	
	rfiStrategy::DefaultStrategy::TelescopeId telescopeId;
	unsigned flags;
	double frequency, timeResolution, frequencyResolution;
	rfiStrategy::DefaultStrategy::DetermineSettings(*imageSet, telescopeId, flags, frequency, timeResolution, frequencyResolution);
	_strategyController->Strategy().RemoveAll();
	rfiStrategy::DefaultStrategy::LoadStrategy(
		_strategyController->Strategy(),
		telescopeId,
		flags | rfiStrategy::DefaultStrategy::FLAG_GUI_FRIENDLY,
		frequency,
		timeResolution,
		frequencyResolution
	);
	_strategyController->NotifyChange();
	
	SetImageSet(std::move(imageSet));
}

void RFIGuiController::LoadSpatial(const std::string& filename)
{
	std::unique_lock<std::mutex> lock(_ioMutex);
	std::unique_ptr<rfiStrategy::SpatialMSImageSet> imageSet(new rfiStrategy::SpatialMSImageSet(filename));
	imageSet->Initialize();
	lock.unlock();
	SetImageSet(std::move(imageSet));
}

void RFIGuiController::LoadSpatialTime(const std::string& filename)
{
	std::unique_lock<std::mutex> lock(_ioMutex);
	std::unique_ptr<rfiStrategy::SpatialTimeImageSet> imageSet(new rfiStrategy::SpatialTimeImageSet(filename));
	imageSet->Initialize();
	lock.unlock();
	SetImageSet(std::move(imageSet));
}
