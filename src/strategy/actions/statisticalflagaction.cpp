#include "../../util/progresslistener.h"

#include "statisticalflagaction.h"

#include "../algorithms/statisticalflagger.h"
#include "../algorithms/siroperator.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

	void StatisticalFlagAction::Perform(ArtifactSet &artifacts, class ProgressListener &)
	{
		TimeFrequencyData &data = artifacts.ContaminatedData();
			
		Mask2DPtr mask = Mask2D::CreateCopy(data.GetSingleMask());
		
		StatisticalFlagger::DilateFlags(mask, _enlargeTimeSize, _enlargeFrequencySize);
		SIROperator::OperateHorizontally(mask, _minimumGoodTimeRatio);
		SIROperator::OperateVertically(mask, _minimumGoodFrequencyRatio);
		
		for(size_t y=0; y!=mask->Height(); ++y)
		{
			bool* rowPtr = mask->ValuePtr(0, y);
			
			size_t count = 0;
			for(size_t x=0; x!=mask->Width(); ++x)
			{
				if(rowPtr[x]) ++count;
			}
			if(count > mask->Width() * (1.0-_minAvailableTimesRatio))
				mask->SetAllHorizontally<true>(y);
		}
		
		for(size_t x=0; x!=mask->Width(); ++x)
		{
			size_t count = 0;
			for(size_t y=0; y!=mask->Height(); ++y)
			{
				if(mask->Value(x, y)) ++count;
			}
			if(count > mask->Height() * (1.0-_minAvailableFrequenciesRatio))
				mask->SetAllVertically<true>(x);
		}
		
		if(mask->GetCount<true>() > mask->Width()*mask->Height()*(1.0-_minAvailableTFRatio))
			mask->SetAll<true>();
		
		data.SetGlobalMask(mask);
	}

} // namespace rfiStrategy
