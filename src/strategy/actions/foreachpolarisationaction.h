#ifndef RFI_FOREACHPOLARISATION_H
#define RFI_FOREACHPOLARISATION_H 

#include "action.h"

#include "../control/actionblock.h"
#include "../control/artifactset.h"

#include "../../structures/timefrequencydata.h"

namespace rfiStrategy {

	class ForEachPolarisationBlock : public ActionBlock
	{
		public:
			ForEachPolarisationBlock() :
				_onXX(true), _onXY(true), _onYX(true), _onYY(true),
				_onStokesI(false), _onStokesQ(false), _onStokesU(false), _onStokesV(false),
				_changeRevised(false)
			{
			}
			virtual ~ForEachPolarisationBlock()
			{
			}
			virtual std::string Description()
			{
				if(selectedPolarizationCount() == 1)
				{
					if(_onXX) return "On XX";
					if(_onXY) return "On XY";
					if(_onYX) return "On YX";
					if(_onYY) return "On YY";
					if(_onStokesI) return "On Stokes I";
					if(_onStokesQ) return "On Stokes Q";
					if(_onStokesU) return "On Stokes U";
					if(_onStokesV) return "On Stokes V";
				}
				return "For each polarisation";
			}
			virtual void Initialize()
			{
			}
			virtual ActionType Type() const { return ForEachPolarisationBlockType; }
			virtual void Perform(ArtifactSet &artifacts, ProgressListener &progress)
			{
				TimeFrequencyData
					oldContaminatedData = artifacts.ContaminatedData(),
					oldRevisedData = artifacts.RevisedData(),
					oldOriginalData = artifacts.OriginalData();
					
				if(oldContaminatedData.Polarisations() != oldOriginalData.Polarisations())
					throw BadUsageException("Contaminated and original do not have equal polarisation, in for each polarisation block");

				if(oldContaminatedData.PolarisationCount() == 1)
				{
					// There is only one polarisation in the contaminated data; just run all childs
					ActionBlock::Perform(artifacts, progress);
				}
				else if(isStokesSelected())
				{
					performStokesIteration(artifacts, progress);
				}
				else {
					bool changeRevised = (oldRevisedData.Polarisations() == oldContaminatedData.Polarisations());
					unsigned count = oldContaminatedData.PolarisationCount();

					for(unsigned polarizationIndex = 0; polarizationIndex < count; ++polarizationIndex)
					{
						if(isPolarizationSelected(oldContaminatedData.GetPolarisation(polarizationIndex)))
						{
							TimeFrequencyData *newContaminatedData =
								oldContaminatedData.CreateTFDataFromPolarisationIndex(polarizationIndex);
							TimeFrequencyData *newOriginalData =
								oldOriginalData.CreateTFDataFromPolarisationIndex(polarizationIndex);

							artifacts.SetContaminatedData(*newContaminatedData);
							artifacts.SetOriginalData(*newOriginalData);
			
							progress.OnStartTask(*this, polarizationIndex, count, newContaminatedData->Description());
			
							delete newOriginalData;
							delete newContaminatedData;
							
							if(changeRevised)
							{
								TimeFrequencyData *newRevised = oldRevisedData.CreateTFDataFromPolarisationIndex(polarizationIndex);
								artifacts.SetRevisedData(*newRevised);
								delete newRevised;
							}
			
							ActionBlock::Perform(artifacts, progress);

							setPolarizationData(polarizationIndex, oldContaminatedData, artifacts.ContaminatedData());
							setPolarizationData(polarizationIndex, oldOriginalData, artifacts.OriginalData());
							if(changeRevised && _changeRevised)
								setPolarizationData(polarizationIndex, oldRevisedData, artifacts.RevisedData());

							progress.OnEndTask(*this);
						}
					}

					artifacts.SetContaminatedData(oldContaminatedData);
					artifacts.SetRevisedData(oldRevisedData);
					artifacts.SetOriginalData(oldOriginalData);
				}
			}

			void SetIterateStokesValues(bool iterateStokesValues)
			{
				_onStokesI = iterateStokesValues;
				_onStokesQ = iterateStokesValues;
				_onStokesU = iterateStokesValues;
				_onStokesV = iterateStokesValues;
			}
			
			bool IterateStokesValues() const
			{
				return _onStokesI && _onStokesQ && _onStokesU && _onStokesV;
			}
			
			void SetOnXX(bool onXX) { _onXX = onXX; }
			void SetOnXY(bool onXY) { _onXY = onXY; }
			void SetOnYX(bool onYX) { _onYX = onYX; }
			void SetOnYY(bool onYY) { _onYY = onYY; }
			void SetOnStokesI(bool onStokesI) { _onStokesI = onStokesI; }
			void SetOnStokesQ(bool onStokesQ) { _onStokesQ = onStokesQ; }
			void SetOnStokesU(bool onStokesU) { _onStokesU = onStokesU; }
			void SetOnStokesV(bool onStokesV) { _onStokesV = onStokesV; }
			
			bool OnXX() const { return _onXX; }
			bool OnXY() const { return _onXY; }
			bool OnYX() const { return _onYX; }
			bool OnYY() const { return _onYY; }
			bool OnStokesI() const { return _onStokesI; }
			bool OnStokesQ() const { return _onStokesQ; }
			bool OnStokesU() const { return _onStokesU; }
			bool OnStokesV() const { return _onStokesV; }
		private:
			bool _onXX, _onXY, _onYX, _onYY, _onStokesI, _onStokesQ, _onStokesU, _onStokesV;
			bool _changeRevised;
			
			bool isPolarizationSelected(PolarizationEnum polarization)
			{
				switch(polarization)
				{
					case Polarization::XX: return _onXX;
					case Polarization::XY: return _onXY;
					case Polarization::YX: return _onYX;
					case Polarization::YY: return _onYY;
					case Polarization::StokesI: return _onStokesI;
					case Polarization::StokesQ: return _onStokesQ;
					case Polarization::StokesU: return _onStokesU;
					case Polarization::StokesV: return _onStokesV;
					default: return false;
				}
			}
			
			bool isStokesSelected() const
			{
				return _onStokesI || _onStokesQ || _onStokesU || _onStokesV;
			}
			
			int selectedPolarizationCount() const
			{
				int count = 0;
				if(_onXX) ++count;
				if(_onXY) ++count;
				if(_onYX) ++count;
				if(_onYY) ++count;
				if(_onStokesI) ++count;
				if(_onStokesQ) ++count;
				if(_onStokesU) ++count;
				if(_onStokesV) ++count;
				return count;
			}

			void setPolarizationData(size_t polarizationIndex, TimeFrequencyData &oldData, TimeFrequencyData &newData)
			{
				oldData.SetPolarizationData(polarizationIndex, newData);
			}

			void performStokesIteration(ArtifactSet &artifacts, ProgressListener &progress)
			{
				TimeFrequencyData
					oldContaminatedData = artifacts.ContaminatedData(),
					oldRevisedData = artifacts.RevisedData(),
					oldOriginalData = artifacts.OriginalData();

				bool changeRevised = (oldRevisedData.Polarisations() == oldContaminatedData.Polarisations());

				Mask2DPtr mask = Mask2D::CreateSetMaskPtr<false>(oldContaminatedData.ImageWidth(), oldContaminatedData.ImageHeight());

				if(_onStokesI)
				{
					performPolarisation(artifacts, progress, Polarization::StokesI, oldContaminatedData, oldOriginalData, oldRevisedData, changeRevised, 0, 4);
					mask->Join(artifacts.ContaminatedData().GetSingleMask());
				}

				if(_onStokesQ)
				{
					performPolarisation(artifacts, progress, Polarization::StokesQ, oldContaminatedData, oldOriginalData, oldRevisedData, changeRevised, 1, 4);
					mask->Join(artifacts.ContaminatedData().GetSingleMask());
				}

				if(_onStokesU)
				{
					performPolarisation(artifacts, progress, Polarization::StokesU, oldContaminatedData, oldOriginalData, oldRevisedData, changeRevised, 2, 4);
					mask->Join(artifacts.ContaminatedData().GetSingleMask());
				}

				if(_onStokesV)
				{
					performPolarisation(artifacts, progress, Polarization::StokesV, oldContaminatedData, oldOriginalData, oldRevisedData, changeRevised, 3, 4);
					mask->Join(artifacts.ContaminatedData().GetSingleMask());
				}
				
				oldContaminatedData.SetGlobalMask(mask);
				artifacts.SetContaminatedData(oldContaminatedData);
				artifacts.SetRevisedData(oldRevisedData);
				artifacts.SetOriginalData(oldOriginalData);
			}

			void performPolarisation(ArtifactSet &artifacts, ProgressListener &progress, PolarizationEnum polarisation, const TimeFrequencyData &oldContaminatedData, const TimeFrequencyData &oldOriginalData, const TimeFrequencyData &oldRevisedData, bool changeRevised, size_t taskNr, size_t taskCount)
			{
				TimeFrequencyData *newContaminatedData =
					oldContaminatedData.CreateTFData(polarisation);
				artifacts.SetContaminatedData(*newContaminatedData);
				progress.OnStartTask(*this, taskNr, taskCount, newContaminatedData->Description());
				delete newContaminatedData;

				TimeFrequencyData *newOriginalData =
					oldOriginalData.CreateTFData(polarisation);
				artifacts.SetOriginalData(*newOriginalData);
				delete newOriginalData;


				if(changeRevised)
				{
					TimeFrequencyData *newRevised = oldRevisedData.CreateTFData(polarisation);
					artifacts.SetRevisedData(*newRevised);
					delete newRevised;
				}

				ActionBlock::Perform(artifacts, progress);

				progress.OnEndTask(*this);
			}
	};

}

#endif // RFI_FOREACHPOLARISATION_H
