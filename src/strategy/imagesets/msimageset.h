#ifndef MSIMAGESET_H
#define MSIMAGESET_H

#include <set>
#include <string>
#include <stdexcept>

#include "../../structures/antennainfo.h"
#include "../../structures/timefrequencydata.h"
#include "../../structures/timefrequencymetadata.h"
#include "../../structures/measurementset.h"

#include "../../msio/baselinereader.h"

#include "indexableset.h"

#include "../../util/aologger.h"

namespace rfiStrategy {

	class MSImageSetIndex : public ImageSetIndex {
		public:
			friend class MSImageSet;
			
			explicit MSImageSetIndex(class rfiStrategy::ImageSet &set) : ImageSetIndex(set), _sequenceIndex(0), _isValid(true) { }
			
			MSImageSetIndex(class rfiStrategy::ImageSet &set, size_t sequenceIndex) : ImageSetIndex(set), _sequenceIndex(sequenceIndex), _isValid(true) { }
			
			virtual void Previous();
			virtual void Next();
			virtual std::string Description() const;
			virtual bool IsValid() const { return _isValid; }
			virtual MSImageSetIndex *Copy() const
			{
				MSImageSetIndex *index = new MSImageSetIndex(imageSet());
				index->_sequenceIndex = _sequenceIndex;
				index->_isValid = _isValid;
				return index;
			}
			size_t SequenceIndex() const { return _sequenceIndex; }
		private:
			size_t _sequenceIndex;
			bool _isValid;
	};
	
	class MSImageSet : public IndexableSet {
		public:
			MSImageSet(const std::string &location, BaselineIOMode ioMode) :
				_msFile(location),
				_set(location),
				_reader(),
				_dataColumnName("DATA"), 
				_subtractModel(false),
				_readDipoleAutoPolarisations(true),
				_readDipoleCrossPolarisations(true),
				_readStokesI(false),
				_bandCount(0),
				_fieldCount(0),
				_sequencesPerBaselineCount(0),
				_scanCountPartOverlap(100),
				_readFlags(true),
				_readUVW(false),
				_ioMode(ioMode)
			{
			}
			
			~MSImageSet()
			{
			}

			virtual MSImageSet *Copy() override final
			{
				MSImageSet *newSet = new MSImageSet(_set.Path(), _ioMode);
				newSet->_reader = _reader;
				newSet->_dataColumnName = _dataColumnName;
				newSet->_subtractModel = _subtractModel;
				newSet->_sequences = _sequences;
				newSet->_bandCount = _bandCount;
				newSet->_fieldCount = _fieldCount;
				newSet->_readDipoleAutoPolarisations = _readDipoleAutoPolarisations;
				newSet->_readDipoleCrossPolarisations = _readDipoleCrossPolarisations;
				newSet->_readStokesI = _readStokesI;
				newSet->_scanCountPartOverlap = _scanCountPartOverlap;
				newSet->_readFlags = _readFlags;
				newSet->_readUVW = _readUVW;
				newSet->_ioMode = _ioMode;
				return newSet;
			}
	
			virtual std::string Name() override final { return _set.Path(); }
			virtual std::string File() override final { return _set.Path(); }
			
			virtual void AddReadRequest(const ImageSetIndex &index) override final;
			virtual void PerformReadRequests() override final;
			virtual BaselineData *GetNextRequested() override final;

			virtual void AddWriteFlagsTask(const ImageSetIndex &index, std::vector<Mask2DCPtr> &flags) override final;
			virtual void PerformWriteFlagsTask() override final;

			virtual void Initialize() override final;
	
			virtual ImageSetIndex *StartIndex() override final { return new MSImageSetIndex(*this); }

			virtual void PerformWriteDataTask(const ImageSetIndex &index, std::vector<Image2DCPtr> realImages, std::vector<Image2DCPtr> imaginaryImages) override final
			{
				const MSImageSetIndex &msIndex = static_cast<const MSImageSetIndex&>(index);
				_reader->PerformDataWriteTask(realImages, imaginaryImages, GetAntenna1(msIndex), GetAntenna2(msIndex), GetBand(msIndex), GetSequenceId(msIndex));
			}
			
			virtual BaselineReaderPtr Reader() override final { return _reader; }
			
			virtual size_t GetAntenna1(const ImageSetIndex &index) override final {
				return _sequences[static_cast<const MSImageSetIndex&>(index)._sequenceIndex].antenna1;
			}
			virtual size_t GetAntenna2(const ImageSetIndex &index) override final {
				return _sequences[static_cast<const MSImageSetIndex&>(index)._sequenceIndex].antenna2;
			}
			virtual size_t GetBand(const ImageSetIndex &index) override final {
				return _sequences[static_cast<const MSImageSetIndex&>(index)._sequenceIndex].spw;
			}
			virtual size_t GetField(const ImageSetIndex &index) override final {
				return _sequences[static_cast<const MSImageSetIndex&>(index)._sequenceIndex].fieldId;
			}
			virtual size_t GetSequenceId(const ImageSetIndex &index) override final {
				return _sequences[static_cast<const MSImageSetIndex&>(index)._sequenceIndex].sequenceId;
			}
	
			virtual MSImageSetIndex *Index(size_t antenna1, size_t antenna2, size_t bandIndex, size_t sequenceId) final override
			{
				MSImageSetIndex *index = new MSImageSetIndex(*this);
				index->_sequenceIndex = FindBaselineIndex(antenna1, antenna2, bandIndex, sequenceId);
				return index;
			}
			
			const std::string &DataColumnName() const { return _dataColumnName; }
			void SetDataColumnName(const std::string &name) {
				if(_reader != 0)
					throw std::runtime_error("Trying to set data column after creating the reader!");
				_dataColumnName = name;
			}

			bool SubtractModel() const { return _subtractModel; }
			void SetSubtractModel(bool subtractModel) {
				if(_reader != 0)
					throw std::runtime_error("Trying to set model subtraction after creating the reader!");
				_subtractModel = subtractModel;
			}

			void SetReadAllPolarisations()
			{
				if(_reader != 0)
					throw std::runtime_error("Trying to set polarization to read after creating the reader!");
				_readDipoleAutoPolarisations = true;
				_readDipoleCrossPolarisations = true;
				_readStokesI = false;
			}
			void SetReadDipoleAutoPolarisations()
			{
				if(_reader != 0)
					throw std::runtime_error("Trying to set polarization to read after creating the reader!");
				_readDipoleAutoPolarisations = true;
				_readDipoleCrossPolarisations = false;
				_readStokesI = false;
			}
			void SetReadStokesI()
			{
				if(_reader != 0)
					throw std::runtime_error("Trying to set polarization to read after creating the reader!");
				_readStokesI = true;
				_readDipoleAutoPolarisations = false;
				_readDipoleCrossPolarisations = false;
			}

			virtual size_t BandCount() const final override { return _bandCount; }
			virtual class ::AntennaInfo GetAntennaInfo(unsigned antennaIndex) final override { return _set.GetAntennaInfo(antennaIndex); }
			virtual class ::BandInfo GetBandInfo(unsigned bandIndex) final override
			{
				return _set.GetBandInfo(bandIndex);
			}
			virtual size_t SequenceCount() const final override { return _sequencesPerBaselineCount; }
			
			size_t AntennaCount() { return _set.AntennaCount(); }
			virtual class ::FieldInfo GetFieldInfo(unsigned fieldIndex) override final
			{
				return _set.GetFieldInfo(fieldIndex);
			}
			std::vector<double> ObservationTimesVector(const ImageSetIndex &index);
			size_t FieldCount() const { return _fieldCount; }
			void SetReadFlags(bool readFlags) { _readFlags = readFlags; }
			void SetReadUVW(bool readUVW)
			{
				_readUVW = readUVW;
			}
			const std::vector<MeasurementSet::Sequence>& Sequences() const {
				return _sequences;
			}
		private:
			friend class MSImageSetIndex;
			MSImageSet(const std::string &location, BaselineReaderPtr reader) :
				_msFile(location), _set(location), _reader(reader),
				_dataColumnName("DATA"), _subtractModel(false),
				_readDipoleAutoPolarisations(true),
				_readDipoleCrossPolarisations(true),
				_readStokesI(false),
				_scanCountPartOverlap(100),
				_readFlags(true),
				_readUVW(false),
				_ioMode(AutoReadMode)
			{ }
			size_t StartIndex(const MSImageSetIndex &index);
			size_t EndIndex(const MSImageSetIndex &index);
			void initReader();
			size_t FindBaselineIndex(size_t antenna1, size_t antenna2, size_t band, size_t sequenceId);
			TimeFrequencyMetaDataCPtr createMetaData(const ImageSetIndex &index, std::vector<UVW> &uvw);

			const std::string _msFile;
			MeasurementSet _set;
			BaselineReaderPtr _reader;
			std::string _dataColumnName;
			bool _subtractModel;
			bool _readDipoleAutoPolarisations, _readDipoleCrossPolarisations, _readStokesI;
			std::vector<MeasurementSet::Sequence> _sequences;
			size_t _bandCount, _fieldCount, _sequencesPerBaselineCount;
			size_t _scanCountPartOverlap;
			bool _readFlags, _readUVW;
			BaselineIOMode _ioMode;
			std::vector<BaselineData> _baselineData;
	};

}
	
#endif
