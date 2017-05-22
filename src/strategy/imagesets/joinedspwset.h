
#ifndef JOINED_SPW_SET_H
#define JOINED_SPW_SET_H

#include "imageset.h"
#include "msimageset.h"

#include <vector>
#include <list>
#include <map>

namespace rfiStrategy {
	
	struct JoinedSequence
	{
		unsigned antenna1, antenna2;
		unsigned sequenceId;
		unsigned fieldId;
		
		JoinedSequence(unsigned _antenna1, unsigned _antenna2, unsigned _sequenceId, unsigned _fieldId) :
			antenna1(_antenna1), antenna2(_antenna2),
			sequenceId(_sequenceId),
			fieldId(_fieldId)
			{ }
		bool operator<(const JoinedSequence &rhs) const
		{
			if(antenna1 < rhs.antenna1) return true;
			else if(antenna1 == rhs.antenna1)
			{
				if(antenna2 < rhs.antenna2) return true;
				else if(antenna2 == rhs.antenna2)
				{
					return sequenceId < rhs.sequenceId;
				}
			}
			return false;
		}
		bool operator==(const JoinedSequence &rhs) const
		{
			return antenna1==rhs.antenna1 && antenna2==rhs.antenna2 &&
				sequenceId==rhs.sequenceId;
		}
	};
	
	class JoinedSPWSetIndex : public ImageSetIndex {
		public:
			friend class JoinedSPWSet;
			
			explicit JoinedSPWSetIndex(class JoinedSPWSet &set);
			virtual ~JoinedSPWSetIndex() override { }
			virtual void Previous() override final;
			virtual void Next() override final;
			virtual std::string Description() const override final;
			virtual bool IsValid() const override final { return _isValid; }
			virtual JoinedSPWSetIndex *Copy() const override final
			{ return new JoinedSPWSetIndex(*this); }
			
		private:
			std::map<JoinedSequence, std::vector<std::pair<size_t, size_t>>>::const_iterator _iterator;
			bool _isValid;
	};
	
	class JoinedSPWSet : public ImageSet
	{
	public:
		/**
		 * @param msImageSet An already initialized image set of which ownership is transferred to
		 * this class.
		 */
		explicit JoinedSPWSet(MSImageSet* msImageSet) : _msImageSet(msImageSet)
		{
			const std::vector<MeasurementSet::Sequence>& sequences = _msImageSet->Sequences();
			for(size_t sequenceIndex = 0; sequenceIndex!=sequences.size(); ++sequenceIndex)
			{
				const MeasurementSet::Sequence& s = sequences[sequenceIndex];
				JoinedSequence js(s.antenna1, s.antenna2, s.sequenceId, s.fieldId);
				// TODO Central frequency instead of spw id is a better idea
				_joinedSequences[js].push_back(std::make_pair(s.spw, sequenceIndex));
			}
			for(auto& js : _joinedSequences)
				std::sort(js.second.begin(), js.second.end());
		}
		
		virtual ~JoinedSPWSet() override { };
		virtual ImageSet *Copy() override final
		{
			JoinedSPWSet* newSet = new JoinedSPWSet();
			newSet->_msImageSet.reset( _msImageSet->Copy() );
			newSet->_joinedSequences = _joinedSequences;
			return newSet;
		}

		virtual ImageSetIndex *StartIndex() override final
		{
			return new JoinedSPWSetIndex(*this);
		}
		
		virtual void Initialize() override final { }
		virtual std::string Name() override final
		{ return _msImageSet->Name() + " (SPWs joined)"; }
		virtual std::string File() override final
		{ return _msImageSet->File(); }
		
		virtual void AddReadRequest(const ImageSetIndex &index) override final
		{
			const std::vector<std::pair<size_t /*spw*/, size_t /*seq*/>>& indexInformation =
				static_cast<const JoinedSPWSetIndex&>(index)._iterator->second;
			
			for(const std::pair<size_t, size_t>& spwAndSeq : indexInformation)
			{
				MSImageSetIndex msIndex(*_msImageSet, spwAndSeq.second);
				_msImageSet->AddReadRequest(msIndex);
			}
			_requests.push_back(static_cast<const JoinedSPWSetIndex&>(index)._iterator);
		}
		
		virtual void PerformReadRequests() override final
		{
			_msImageSet->PerformReadRequests();
			for(auto& request : _requests)
			{
				const std::vector<std::pair<size_t /*spw*/, size_t /*seq*/>>& indexInformation =
					request->second;
				std::vector<std::unique_ptr<BaselineData>> data;
				size_t totalHeight = 0;
				for(size_t i=0; i!=indexInformation.size(); ++i)
				{
					data.emplace_back(_msImageSet->GetNextRequested());
					totalHeight += data.back()->Data().ImageHeight();
				}
				AOLogger::Info << "SPW combined: " << totalHeight << " channels.\n";
				
				// Combine the images
				TimeFrequencyData tfData(data[0]->Data());
				size_t width = tfData.ImageWidth();
				for(size_t imgIndex=0; imgIndex!=tfData.ImageCount(); ++imgIndex)
				{
					size_t chIndex = 0;
					Image2DPtr img = Image2D::CreateUnsetImagePtr(width, totalHeight);
					for(size_t i=0; i!=data.size(); ++i)
					{
						Image2DCPtr src = data[i]->Data().GetImage(imgIndex);
						for(size_t y=0; y!=src->Height(); ++y)
						{
							num_t* destPtr = img->ValuePtr(0, y+chIndex);
							const num_t* srcPtr = src->ValuePtr(0, y);
							for(size_t x=0; x!=src->Width(); ++x)
								destPtr[x] = srcPtr[x];
						}
						chIndex += data[i]->Data().ImageHeight();
					}
					tfData.SetImage(imgIndex, img);
				}
				
				// Combine the masks
				for(size_t maskIndex=0; maskIndex!=tfData.MaskCount(); ++maskIndex)
				{
					size_t chIndex = 0;
					Mask2DPtr mask = Mask2D::CreateUnsetMaskPtr(width, totalHeight);
					for(size_t i=0; i!=data.size(); ++i)
					{
						Mask2DCPtr src = data[i]->Data().GetMask(maskIndex);
						for(size_t y=0; y!=src->Height(); ++y)
						{
							bool* destPtr = mask->ValuePtr(0, y+chIndex);
							const bool* srcPtr = src->ValuePtr(0, y);
							for(size_t x=0; x!=src->Width(); ++x)
								destPtr[x] = srcPtr[x];
						}
						chIndex += data[i]->Data().ImageHeight();
					}
					tfData.SetMask(maskIndex, mask);
				}
				
				// Combine the metadata
				TimeFrequencyMetaDataPtr metaData(new TimeFrequencyMetaData(*data[0]->MetaData()));
				BandInfo band = metaData->Band();
				size_t chIndex = band.channels.size();
				band.channels.resize(totalHeight);
				for(size_t i=1; i!=data.size(); ++i)
				{
					const BandInfo& srcBand = data[i]->MetaData()->Band();
					for(size_t ch=0; ch!=srcBand.channels.size(); ++ch)
						band.channels[ch + chIndex] = srcBand.channels[ch];
					chIndex += srcBand.channels.size();
				}
				
				metaData->SetBand(band);
				
				BaselineData combinedData(tfData, metaData);
				_baselineData.push_back(combinedData);
			}
			_requests.clear();
		}
		
		virtual BaselineData *GetNextRequested() override final
		{
			BaselineData* data = new BaselineData(_baselineData.front());
			_baselineData.pop_front();
			return data;
		}
		
		virtual void AddWriteFlagsTask(const ImageSetIndex &index, std::vector<Mask2DCPtr> &flags) override final
		{
			// TODO
		}
		virtual void PerformWriteFlagsTask() override final
		{
			// TODO
		}
		
		const std::map<JoinedSequence, std::vector<std::pair<size_t, size_t>>>& JoinedSequences() const { return _joinedSequences; }
		
		MSImageSet& msImageSet() { return *_msImageSet; }
	private:
		JoinedSPWSet() { }
		
		std::unique_ptr<MSImageSet> _msImageSet;
		std::map<JoinedSequence, std::vector<std::pair<size_t, size_t>>> _joinedSequences;
		std::vector<std::map<JoinedSequence, std::vector<std::pair<size_t, size_t>>>::const_iterator> _requests;
		std::list<BaselineData> _baselineData;
	};
	
	JoinedSPWSetIndex::JoinedSPWSetIndex(class JoinedSPWSet &set) :
		ImageSetIndex(set),
		_iterator(static_cast<JoinedSPWSet&>(imageSet()).JoinedSequences().begin()),
		_isValid(true)
	{
	}
	
	void JoinedSPWSetIndex::Next()
	{
		++_iterator;
		if(_iterator == static_cast<JoinedSPWSet&>(imageSet()).JoinedSequences().end())
		{
			_iterator = static_cast<JoinedSPWSet&>(imageSet()).JoinedSequences().begin();
			_isValid = false;
		}
	}

	void JoinedSPWSetIndex::Previous()
	{
		if(_iterator == static_cast<JoinedSPWSet&>(imageSet()).JoinedSequences().begin())
		{
			_iterator = static_cast<JoinedSPWSet&>(imageSet()).JoinedSequences().end();
			_isValid = false;
		}
		--_iterator;
	}

	std::string JoinedSPWSetIndex::Description() const
	{
		MSImageSet& msImageSet = static_cast<JoinedSPWSet&>(imageSet()).msImageSet();
		
		const JoinedSequence &sequence = _iterator->first;
		size_t
			antenna1 = sequence.antenna1,
			antenna2 = sequence.antenna2,
			sequenceId = sequence.sequenceId;
		AntennaInfo info1 = msImageSet.GetAntennaInfo(antenna1);
		AntennaInfo info2 = msImageSet.GetAntennaInfo(antenna2);
		std::stringstream sstream;
		sstream
			<< info1.station << ' ' << info1.name << " x " << info2.station << ' ' << info2.name
			<< " (joined spws)";
		if(static_cast<class MSImageSet&>(imageSet()).SequenceCount() > 1)
		{
			sstream
				<< ", seq " << sequenceId;
		}
		return sstream.str();
	}
}

#endif
