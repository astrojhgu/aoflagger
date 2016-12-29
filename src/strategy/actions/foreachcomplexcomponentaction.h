#ifndef FOR_EACH_COMPLEX_COMPONENT_ACTION_H
#define FOR_EACH_COMPLEX_COMPONENT_ACTION_H

#include "action.h"

#include "../../util/progresslistener.h"

#include "../control/artifactset.h"
#include "../control/actionblock.h"

namespace rfiStrategy {

	class ForEachComplexComponentAction : public ActionBlock
	{
		public:
			ForEachComplexComponentAction() : ActionBlock(), _onAmplitude(false), _onPhase(false), _onReal(true), _onImaginary(true), _restoreFromAmplitude(false)
			{
			}
			virtual std::string Description()
			{
				if(IterationCount() == 1)
				{
					if(_onAmplitude) {
						if(_restoreFromAmplitude)
							return "On amplitude (restore)";
						else
							return "On amplitude";
					}
					if(_onPhase) return "On phase";
					if(_onReal) return "On real";
					if(_onImaginary) return "On imaginary";
				}
				return "For each complex component";
			}
			virtual ActionType Type() const { return ForEachComplexComponentActionType; }
			virtual void Perform(ArtifactSet &artifacts, class ProgressListener &listener)
			{
				size_t taskCount = 0;
				if(_onAmplitude) ++taskCount;
				if(_onPhase) ++taskCount;
				if(_onReal) ++taskCount;
				if(_onImaginary) ++taskCount;
				
				size_t taskIndex = 0;
				
				if(_onAmplitude) {
					listener.OnStartTask(*this, taskIndex, taskCount, "On amplitude");
					performOnAmplitude(artifacts, listener);
					listener.OnEndTask(*this);
					++taskIndex;
				}
				if(_onPhase) {
					listener.OnStartTask(*this, taskIndex, taskCount, "On phase");
					performOnPhaseRepresentation(artifacts, listener, TimeFrequencyData::PhasePart);
					listener.OnEndTask(*this);
					++taskIndex;
				}
				if(_onReal) {
					listener.OnStartTask(*this, taskIndex, taskCount, "On real");
					performOnPhaseRepresentation(artifacts, listener, TimeFrequencyData::RealPart);
					listener.OnEndTask(*this);
					++taskIndex;
				}
				if(_onImaginary) {
					listener.OnStartTask(*this, taskIndex, taskCount, "On imaginary");
					performOnPhaseRepresentation(artifacts, listener, TimeFrequencyData::ImaginaryPart);
					listener.OnEndTask(*this);
					++taskIndex;
				}
			}
			unsigned IterationCount() const {
				unsigned count = 0;
				if(_onAmplitude) ++count;
				if(_onPhase) ++count;
				if(_onReal) ++count;
				if(_onImaginary) ++count;
				return count;
			}
			void SetRestoreFromAmplitude(bool restoreFromAmplitude)
			{
				_restoreFromAmplitude = restoreFromAmplitude;
			}
			bool RestoreFromAmplitude() const
			{
				return _restoreFromAmplitude;
			}
			void SetOnAmplitude(bool onAmplitude)
			{
				_onAmplitude = onAmplitude;
			}
			bool OnAmplitude() const
			{
				return _onAmplitude;
			}
			void SetOnPhase(bool onPhase)
			{
				_onPhase = onPhase;
			}
			bool OnPhase() const
			{
				return _onPhase;
			}
			void SetOnReal(bool onReal)
			{
				_onReal = onReal;
			}
			bool OnReal() const
			{
				return _onReal;
			}
			void SetOnImaginary(bool onImaginary)
			{
				_onImaginary = onImaginary;
			}
			bool OnImaginary() const
			{
				return _onImaginary;
			}
		private:
			void performOnAmplitude(ArtifactSet &artifacts, class ProgressListener &listener)
			{
				enum TimeFrequencyData::ComplexRepresentation contaminatedRepresentation = 
					artifacts.ContaminatedData().ComplexRepresentation();
				enum TimeFrequencyData::ComplexRepresentation revisedRepresentation = 
					artifacts.RevisedData().ComplexRepresentation();
				enum TimeFrequencyData::ComplexRepresentation originalRepresentation = 
					artifacts.OriginalData().ComplexRepresentation();

				if(contaminatedRepresentation == TimeFrequencyData::ComplexParts)
				{
					TimeFrequencyData *newContaminatedData =
						artifacts.ContaminatedData().CreateTFData(TimeFrequencyData::AmplitudePart);
					artifacts.SetContaminatedData(*newContaminatedData);
					delete newContaminatedData;
				}
				if(revisedRepresentation == TimeFrequencyData::ComplexParts)
				{
					TimeFrequencyData *newRevisedData =
						artifacts.RevisedData().CreateTFData(TimeFrequencyData::AmplitudePart);
					artifacts.SetRevisedData(*newRevisedData);
					delete newRevisedData;
				}
				if(originalRepresentation == TimeFrequencyData::ComplexParts)
				{
					TimeFrequencyData *newOriginalData =
						artifacts.OriginalData().CreateTFData(TimeFrequencyData::AmplitudePart);
					artifacts.SetOriginalData(*newOriginalData);
					delete newOriginalData;
				}

				ActionBlock::Perform(artifacts, listener);

				if(contaminatedRepresentation == TimeFrequencyData::ComplexParts)
				{
					TimeFrequencyData *newContaminatedData =
						TimeFrequencyData::CreateTFDataFromComplexCombination(artifacts.ContaminatedData(), artifacts.ContaminatedData());
					if(_restoreFromAmplitude)
						newContaminatedData->MultiplyImages(1.0L/M_SQRT2);
					newContaminatedData->SetMask(artifacts.ContaminatedData());
					artifacts.SetContaminatedData(*newContaminatedData);
					delete newContaminatedData;
				}
				if(revisedRepresentation == TimeFrequencyData::ComplexParts)
				{
					TimeFrequencyData *newRevisedData =
						TimeFrequencyData::CreateTFDataFromComplexCombination(artifacts.RevisedData(), artifacts.RevisedData());
					if(_restoreFromAmplitude)
						newRevisedData->MultiplyImages(1.0L/M_SQRT2);
					newRevisedData->SetMask(artifacts.RevisedData());
					artifacts.SetRevisedData(*newRevisedData);
					delete newRevisedData;
				}
				if(originalRepresentation == TimeFrequencyData::ComplexParts)
				{
					TimeFrequencyData *newOriginalData =
						TimeFrequencyData::CreateTFDataFromComplexCombination(artifacts.OriginalData(), artifacts.OriginalData());
					if(_restoreFromAmplitude)
						newOriginalData->MultiplyImages(1.0L/M_SQRT2);
					newOriginalData->SetMask(artifacts.OriginalData());
					artifacts.SetOriginalData(*newOriginalData);
					delete newOriginalData;
				}
			}
			
			void performOnPhaseRepresentation(ArtifactSet &artifacts, class ProgressListener &listener, enum TimeFrequencyData::ComplexRepresentation phaseRepresentation)
			{
				enum TimeFrequencyData::ComplexRepresentation
					contaminatedPhase = artifacts.ContaminatedData().ComplexRepresentation(),
					revisedPhase = artifacts.RevisedData().ComplexRepresentation(),
					originalPhase = artifacts.OriginalData().ComplexRepresentation();
					
				TimeFrequencyData
					prevContaminated = artifacts.ContaminatedData(),
					prevRevised = artifacts.RevisedData(),
					prevOriginal = artifacts.OriginalData();
					
				bool phaseRepresentationIsAvailable = false;

				if(contaminatedPhase == TimeFrequencyData::ComplexParts || contaminatedPhase == phaseRepresentation)
				{
					TimeFrequencyData *newContaminatedData =
						artifacts.ContaminatedData().CreateTFData(phaseRepresentation);
					artifacts.SetContaminatedData(*newContaminatedData);
					delete newContaminatedData;
					phaseRepresentationIsAvailable = true;
				}
				if(revisedPhase == TimeFrequencyData::ComplexParts || revisedPhase == phaseRepresentation)
				{
					TimeFrequencyData *newRevisedData =
						artifacts.RevisedData().CreateTFData(phaseRepresentation);
					artifacts.SetRevisedData(*newRevisedData);
					delete newRevisedData;
					phaseRepresentationIsAvailable = true;
				}
				if(originalPhase == TimeFrequencyData::ComplexParts || originalPhase == phaseRepresentation)
				{
					TimeFrequencyData *newOriginalData =
						artifacts.OriginalData().CreateTFData(phaseRepresentation);
					artifacts.SetOriginalData(*newOriginalData);
					delete newOriginalData;
					phaseRepresentationIsAvailable = true;
				}

				if(phaseRepresentationIsAvailable)
				{
					ActionBlock::Perform(artifacts, listener);

					if(phaseRepresentation != TimeFrequencyData::PhasePart)
					{
						if(contaminatedPhase == TimeFrequencyData::ComplexParts)
							setPart(artifacts.ContaminatedData(), prevContaminated);
						if(revisedPhase == TimeFrequencyData::ComplexParts)
							setPart(artifacts.RevisedData(), prevRevised);
						if(originalPhase == TimeFrequencyData::ComplexParts)
							setPart(artifacts.OriginalData(), prevOriginal);
					}
				}
			}
			void setPart(TimeFrequencyData &changedData, TimeFrequencyData &prevData)
			{
				TimeFrequencyData *newData, *otherPart;
				switch(changedData.ComplexRepresentation())
				{
					default:
					case TimeFrequencyData::RealPart:
						otherPart = prevData.CreateTFData(TimeFrequencyData::ImaginaryPart);
						newData = TimeFrequencyData::CreateTFDataFromComplexCombination(changedData, *otherPart);
						break;
					case TimeFrequencyData::ImaginaryPart:
						otherPart = prevData.CreateTFData(TimeFrequencyData::RealPart);
						newData = TimeFrequencyData::CreateTFDataFromComplexCombination(*otherPart, changedData);
						break;
				}
				changedData = *newData;
				changedData.SetMask(prevData);
				delete newData;
				delete otherPart;
			}
			
			bool _onAmplitude, _onPhase, _onReal, _onImaginary;
			bool _restoreFromAmplitude;
		};

} // namespace

#endif // FOR_EACH_COMPLEX_COMPONENT_ACTION_H
