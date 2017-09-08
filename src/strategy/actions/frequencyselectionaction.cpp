#include "frequencyselectionaction.h"

#include "../../structures/image2d.h"
#include "../../structures/samplerow.h"

#include "../control/artifactset.h"

namespace rfiStrategy {
	
void FrequencySelectionAction::Perform(ArtifactSet &artifacts, class ProgressListener &)
{
	const Image2D* image = artifacts.ContaminatedData().GetSingleImage().get();
	SampleRowPtr channels = SampleRow::CreateEmpty(image->Height());
	Mask2DPtr mask(new Mask2D(*artifacts.ContaminatedData().GetSingleMask()));
	for(size_t y=0;y<image->Height();++y)
	{
		SampleRowPtr row = SampleRow::CreateFromRowWithMissings(image, mask.get(), y);
		channels->SetValue(y, row->RMSWithMissings());
	}
	bool change;
	do {
		num_t median = channels->MedianWithMissings();
		num_t stddev = channels->StdDevWithMissings(median);
		change = false;
		double effectiveThreshold = _threshold * stddev * artifacts.Sensitivity();
		for(size_t y=0;y<channels->Size();++y)
		{
			if(!channels->ValueIsMissing(y) && (channels->Value(y) - median > effectiveThreshold || (_clipDown && median - channels->Value(y) > effectiveThreshold)))
			{
				mask->SetAllHorizontally<true>(y);
				channels->SetValueMissing(y);
				change = true;
			}
		}
	} while(change);
	artifacts.ContaminatedData().SetGlobalMask(mask);
}

} // namespace
