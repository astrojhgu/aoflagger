#ifndef RFISETFLAGGINGACTION_H
#define RFISETFLAGGINGACTION_H

#include "../control/actioncontainer.h"
#include "../control/artifactset.h"

#include "../../util/progresslistener.h"

namespace rfiStrategy {

	class SetFlaggingAction : public Action
	{
		public:
			enum NewFlagging { None, Everything, FromOriginal, Invert, PolarisationsEqual, FlagZeros, OrOriginal, ToOriginal };

			SetFlaggingAction() : _newFlagging(None) { }

			virtual std::string Description() final override
			{
				switch(_newFlagging)
				{
					default:
					case None:
						return "Set no flags";
					case Everything:
						return "Set everything flagged";
					case FromOriginal:
						return "Restore original flags";
					case ToOriginal:
						return "Change original flags";
					case Invert:
						return "Set inverted flags";
					case PolarisationsEqual:
						return "Apply flags to all polarisations";
					case FlagZeros:
						return "Flag zeros";
					case OrOriginal:
						return "Or flags with original";
				}
			}
			virtual void Perform(class ArtifactSet &artifacts, class ProgressListener &) final override
			{
				if(artifacts.ContaminatedData().IsEmpty())
					throw std::runtime_error("No baseline is loaded! This might mean you forgot to put a For Each Baseline action in front of everything, or you might have forgotten to open an MS.");

				switch(_newFlagging)
				{
					default:
					case None:
						artifacts.ContaminatedData().SetGlobalMask(Mask2D::CreateSetMaskPtr<false>(artifacts.ContaminatedData().ImageWidth(), artifacts.ContaminatedData().ImageHeight()));
						break;
					case Everything:
						artifacts.ContaminatedData().SetGlobalMask(Mask2D::CreateSetMaskPtr<false>(artifacts.ContaminatedData().ImageWidth(), artifacts.ContaminatedData().ImageHeight()));
						break;
					case FromOriginal:
						artifacts.ContaminatedData().SetGlobalMask(artifacts.OriginalData().GetSingleMask());
						break;
					case ToOriginal:
						if(artifacts.OriginalData().MaskCount() == 1)
							artifacts.OriginalData().SetGlobalMask(artifacts.ContaminatedData().GetSingleMask());
						else {
							if(artifacts.ContaminatedData().MaskCount() == 1) {
								for(unsigned i=0;i<artifacts.OriginalData().MaskCount();++i)
									artifacts.OriginalData().SetMask(i, artifacts.ContaminatedData().GetSingleMask());
							} else if(artifacts.ContaminatedData().MaskCount() != artifacts.OriginalData().MaskCount()) {
								throw BadUsageException("Error : can't set flagging to original when polarisations are incompatible");
							} else {
								for(unsigned i=0;i<artifacts.OriginalData().MaskCount();++i)
									artifacts.OriginalData().SetMask(i, artifacts.ContaminatedData().GetMask(i));
							}
						}
						break;
					case Invert: {
						Mask2DPtr mask(new Mask2D(*artifacts.ContaminatedData().GetSingleMask()));
						mask->Invert();
						artifacts.ContaminatedData().SetGlobalMask(mask);
						break;
					}
					case PolarisationsEqual: {
						Mask2DCPtr mask = artifacts.ContaminatedData().GetSingleMask();
						artifacts.ContaminatedData().SetGlobalMask(mask);
						break;
					}
					case FlagZeros: {
						Mask2DPtr mask(new Mask2D(*artifacts.ContaminatedData().GetSingleMask()));
						Image2DCPtr image = artifacts.ContaminatedData().GetSingleImage();
						for(unsigned y=0;y<image->Height();++y) {
							for(unsigned x=0;x<image->Width();++x) {
								if(image->Value(x, y) == 0.0)
									mask->SetValue(x, y, true);
							}
						}
						artifacts.ContaminatedData().SetGlobalMask(mask);
						break;
					}
					case OrOriginal: {
						Mask2DPtr mask(new Mask2D(*artifacts.ContaminatedData().GetSingleMask()));
						mask->Join(*artifacts.OriginalData().GetSingleMask());
						artifacts.ContaminatedData().SetGlobalMask(mask);
						break;
					}
				}
			}
			void SetNewFlagging(enum NewFlagging newFlagging)
			{
				_newFlagging = newFlagging;
			}
			enum NewFlagging NewFlagging() const { return _newFlagging; }
			virtual ActionType Type() const final override { return SetFlaggingActionType; }
		private:
			enum NewFlagging _newFlagging;
	};
}

#endif // RFISETFLAGGINGACTION_H
