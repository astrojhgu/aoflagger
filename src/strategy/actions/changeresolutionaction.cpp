#include "changeresolutionaction.h"

#include "../control/artifactset.h"

#include <stdexcept>
#include "../algorithms/thresholdtools.h"

namespace rfiStrategy {
	
	void ChangeResolutionAction::Perform(class ArtifactSet &artifacts, class ProgressListener &listener)
	{
		TimeFrequencyData oldContaminated = artifacts.ContaminatedData();

		if(_timeDecreaseFactor != 1)
		{
			ArtifactSet artifactsCopy(artifacts);
			artifactsCopy.SetNoImageSet();
	
			DecreaseTime(artifactsCopy.OriginalData());
			DecreaseTime(artifactsCopy.ContaminatedData());
			DecreaseTime(artifactsCopy.RevisedData());
	
			PerformFrequencyChange(artifactsCopy, listener);
	
			IncreaseTime(artifacts.OriginalData(), artifactsCopy.OriginalData(), false, false);
			IncreaseTime(artifacts.ContaminatedData(), artifactsCopy.ContaminatedData(), _restoreContaminated, _restoreMasks);
			IncreaseTime(artifacts.RevisedData(), artifactsCopy.RevisedData(), _restoreRevised, _restoreMasks);
		} else {
			PerformFrequencyChange(artifacts, listener);
		}
		if(_restoreRevised && !_restoreContaminated)
		{
			oldContaminated.Subtract(artifacts.RevisedData());
			if(_restoreMasks)
				oldContaminated.SetMask(artifacts.ContaminatedData());
			artifacts.SetContaminatedData(oldContaminated);
		}
	}

	void ChangeResolutionAction::PerformFrequencyChange(class ArtifactSet &artifacts, class ProgressListener &listener)
	{
		TimeFrequencyData oldContaminated = artifacts.ContaminatedData();

		if(_frequencyDecreaseFactor != 1)
		{
			ArtifactSet artifactsCopy(artifacts);
			artifactsCopy.SetNoImageSet();
	
			DecreaseFrequency(artifactsCopy.OriginalData());
			DecreaseFrequency(artifactsCopy.ContaminatedData());
			DecreaseFrequency(artifactsCopy.RevisedData());
	
			ActionBlock::Perform(artifactsCopy, listener);
	
			IncreaseFrequency(artifacts.OriginalData(), artifactsCopy.OriginalData(), false, false);
			IncreaseFrequency(artifacts.ContaminatedData(), artifactsCopy.ContaminatedData(), _restoreContaminated, _restoreMasks);
			IncreaseFrequency(artifacts.RevisedData(), artifactsCopy.RevisedData(), _restoreRevised, _restoreMasks);
		} else {
			ActionBlock::Perform(artifacts, listener);
		}

		if(_restoreRevised && !_restoreContaminated)
		{
			oldContaminated.Subtract(artifacts.RevisedData());
			if(_restoreMasks)
				oldContaminated.SetMask(artifacts.ContaminatedData());
			artifacts.SetContaminatedData(oldContaminated);
		}
	}

	void ChangeResolutionAction::DecreaseTime(TimeFrequencyData &timeFrequencyData)
	{
		if(_useMaskInAveraging)
		{
			DecreaseTimeWithMask(timeFrequencyData);
		}
		else {
			size_t imageCount = timeFrequencyData.ImageCount();
			for(size_t i=0;i<imageCount;++i)
			{
				Image2DCPtr image = timeFrequencyData.GetImage(i);
				Image2DPtr newImage(new Image2D(image->ShrinkHorizontally(_timeDecreaseFactor)));
				timeFrequencyData.SetImage(i, std::move(newImage));
			}
			size_t maskCount = timeFrequencyData.MaskCount();
			for(size_t i=0;i<maskCount;++i)
			{
				Mask2DCPtr mask = timeFrequencyData.GetMask(i);
				Mask2DPtr newMask(new Mask2D(mask->ShrinkHorizontally(_timeDecreaseFactor)));
				timeFrequencyData.SetMask(i, std::move(newMask));
			}
		}
	}
	
	void ChangeResolutionAction::DecreaseTimeWithMask(TimeFrequencyData& data)
	{
		size_t polCount = data.PolarizationCount();
		for(size_t i=0;i<polCount;++i)
		{
			TimeFrequencyData polData(data.MakeFromPolarizationIndex(i));
			const Mask2DCPtr mask = polData.GetSingleMask();
			for(unsigned j=0;j<polData.ImageCount();++j)
			{
				const Image2DCPtr image = polData.GetImage(j);
				polData.SetImage(j, ThresholdTools::ShrinkHorizontally(_timeDecreaseFactor, image.get(), mask.get()));
			}
			data.SetPolarizationData(i, std::move(polData));
		}
		size_t maskCount = data.MaskCount();
		for(size_t i=0;i<maskCount;++i)
		{
			Mask2DCPtr mask = data.GetMask(i);
			Mask2DPtr newMask(new Mask2D(mask->ShrinkHorizontallyForAveraging(_timeDecreaseFactor)));
			data.SetMask(i, std::move(newMask));
		}
	}

	void ChangeResolutionAction::DecreaseFrequency(TimeFrequencyData& timeFrequencyData)
	{
		if(_useMaskInAveraging)
		{
			DecreaseFrequencyWithMask(timeFrequencyData);
		}
		else {
      size_t imageCount = timeFrequencyData.ImageCount();
      for(size_t i=0;i<imageCount;++i)
      {
        Image2DCPtr image = timeFrequencyData.GetImage(i);
        Image2DPtr newImage(new Image2D(image->ShrinkVertically(_frequencyDecreaseFactor)));
        timeFrequencyData.SetImage(i, std::move(newImage));
      }
      size_t maskCount = timeFrequencyData.MaskCount();
      for(size_t i=0;i<maskCount;++i)
      {
        Mask2DCPtr mask = timeFrequencyData.GetMask(i);
        Mask2DPtr newMask(new Mask2D(mask->ShrinkVertically(_frequencyDecreaseFactor)));
        timeFrequencyData.SetMask(i, std::move(newMask));
      }
    }
	}
	
	void ChangeResolutionAction::DecreaseFrequencyWithMask(TimeFrequencyData& data)
  {
		size_t polCount = data.PolarizationCount();
		for(size_t i=0;i<polCount;++i)
		{
			TimeFrequencyData polData(data.MakeFromPolarizationIndex(i));
			const Mask2DCPtr mask = polData.GetSingleMask();
			for(unsigned j=0;j<polData.ImageCount();++j)
			{
				const Image2DCPtr image = polData.GetImage(j);
				polData.SetImage(j, ThresholdTools::ShrinkVertically(_frequencyDecreaseFactor, image.get(), mask.get()));
			}
			data.SetPolarizationData(i, std::move(polData));
		}
		size_t maskCount = data.MaskCount();
		for(size_t i=0;i<maskCount;++i)
		{
			Mask2DCPtr mask = data.GetMask(i);
			Mask2DPtr newMask(new Mask2D(mask->ShrinkVerticallyForAveraging(_frequencyDecreaseFactor)));
			data.SetMask(i, std::move(newMask));
		}
  }

	void ChangeResolutionAction::IncreaseTime(TimeFrequencyData &originalData, const TimeFrequencyData &changedData, bool restoreImage, bool restoreMask)
	{
		if(restoreImage)
		{
			size_t imageCount = originalData.ImageCount();
			if(imageCount != changedData.ImageCount())
				throw std::runtime_error("When restoring resolution in change resolution action, original data and changed data do not have the same number of images");
			for(size_t i=0;i<imageCount;++i)
			{
				Image2DCPtr image = changedData.GetImage(i);
				Image2DPtr newImage(new Image2D(image->EnlargeHorizontally(_timeDecreaseFactor, originalData.ImageWidth())));
				originalData.SetImage(i, newImage);
			}
		}
		if(restoreMask)
		{
			originalData.SetMask(changedData);
			size_t maskCount = originalData.MaskCount();
			for(size_t i=0;i<maskCount;++i)
			{
				Mask2DCPtr mask = changedData.GetMask(i);
				Mask2DPtr newMask = Mask2D::CreateUnsetMaskPtr(originalData.ImageWidth(), originalData.ImageHeight());
				newMask->EnlargeHorizontallyAndSet(*mask, _timeDecreaseFactor);
				originalData.SetMask(i, newMask);
			}
		}
	}

	void ChangeResolutionAction::IncreaseFrequency(TimeFrequencyData &originalData, const TimeFrequencyData &changedData, bool restoreImage, bool restoreMask)
	{
		if(restoreImage)
		{
			size_t imageCount = originalData.ImageCount();
			if(imageCount != changedData.ImageCount())
				throw std::runtime_error("When restoring resolution in change resolution action, original data and changed data do not have the same number of images");
			for(size_t i=0;i<imageCount;++i)
			{
				Image2DCPtr image = changedData.GetImage(i);
				Image2DPtr newImage(new Image2D(image->EnlargeVertically(_frequencyDecreaseFactor, originalData.ImageHeight())));
				originalData.SetImage(i, newImage);
			}
		}
		if(restoreMask)
		{
			originalData.SetMask(changedData);
			size_t maskCount = originalData.MaskCount();
			for(size_t i=0;i<maskCount;++i)
			{
				Mask2DCPtr mask = changedData.GetMask(i);
				Mask2DPtr newMask = Mask2D::CreateUnsetMaskPtr(originalData.ImageWidth(), originalData.ImageHeight());
				newMask->EnlargeVerticallyAndSet(*mask, _frequencyDecreaseFactor);
				originalData.SetMask(i, newMask);
			}
		}
	}

}
