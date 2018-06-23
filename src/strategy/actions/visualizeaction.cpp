#include "visualizeaction.h"

#include "../control/artifactset.h"

namespace rfiStrategy {

void VisualizeAction::Perform(ArtifactSet& artifacts, ProgressListener&)
{
	std::ostringstream str;
	str << _label << " (T=" << artifacts.Sensitivity() << ")";
	switch(_source)
	{
		case FromOriginal:
			artifacts.AddVisualization(str.str(), artifacts.OriginalData());
			break;
		case FromRevised:
			artifacts.AddVisualization(str.str(), artifacts.RevisedData());
			break;
		case FromContaminated:
			artifacts.AddVisualization(str.str(), artifacts.ContaminatedData());
			break;
	}
}

}
