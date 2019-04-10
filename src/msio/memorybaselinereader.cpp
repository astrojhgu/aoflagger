#include "memorybaselinereader.h"

#include "../structures/system.h"

#include "../util/logger.h"
#include "../util/stopwatch.h"

#include <casacore/ms/MeasurementSets/MeasurementSet.h>
#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>

#include <vector>

using namespace casacore;

void MemoryBaselineReader::PerformReadRequests()
{
	if(!_isRead)
	{
		readSet();
		_isRead = true;
	}
	
	for(size_t i=0;i!=_readRequests.size();++i)
	{
		const ReadRequest &request = _readRequests[i];
		BaselineID id(request.antenna1, request.antenna2, request.spectralWindow, request.sequenceId);
		std::map<BaselineID, std::unique_ptr<Result>>::const_iterator
			requestedBaselineIter = _baselines.find(id);
		if(requestedBaselineIter == _baselines.end())
		{
			std::ostringstream errorStr;
			errorStr <<
				"Exception in PerformReadRequests(): requested baseline is not available in measurement set "
				"(antenna1=" << request.antenna1 << ", antenna2=" << request.antenna2 << ", "
				"spw=" << request.spectralWindow << ", sequenceId=" << request.sequenceId << ")";
			throw std::runtime_error(errorStr.str());
		}
		else
			_results.push_back(*requestedBaselineIter->second);
	}
	
	_readRequests.clear();
}

void MemoryBaselineReader::readSet()
{
	Stopwatch watch(true);
	
	initializeMeta();

	casacore::Table& table = *Table();
	
	ScalarColumn<int>
		ant1Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA1)),
		ant2Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA2)),
		dataDescIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::DATA_DESC_ID)),
		fieldIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FIELD_ID));
	ScalarColumn<double>
		timeColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::TIME));
	ArrayColumn<casacore::Complex>
		dataColumn(table, DataColumnName());
	ArrayColumn<bool>
		flagColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FLAG));
	ArrayColumn<double>
		uvwColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::UVW));
	
	size_t
		antennaCount = MetaData().AntennaCount(),
		polarizationCount = Polarizations().size(),
		bandCount = MetaData().BandCount(),
		sequenceCount = MetaData().SequenceCount(),
		intStart = IntervalStart(),
		intEnd = IntervalEnd();
	
	std::vector<size_t> dataIdToSpw;
	MetaData().GetDataDescToBandVector(dataIdToSpw);
	
	std::vector<BandInfo> bandInfos(bandCount);
	for(size_t b=0; b!=bandCount; ++b)
		bandInfos[b] = MetaData().GetBandInfo(b);
	
	// Initialize the look-up matrix
	// to quickly access the elements (without the map-lookup)
	typedef std::unique_ptr<Result> MatrixElement;
	typedef std::vector<MatrixElement> MatrixRow;
	typedef std::vector<MatrixRow> BaselineMatrix;
	typedef std::vector<BaselineMatrix> BaselineCube;
	
	BaselineCube baselineCube(sequenceCount * bandCount);
	
	for(size_t s=0; s!=sequenceCount; ++s)
	{
		for(size_t b=0; b!=bandCount; ++b)
		{
			BaselineMatrix& matrix = baselineCube[s*bandCount + b];
			matrix.resize(antennaCount);
			
			BandInfo band = MetaData().GetBandInfo(0);
			for(size_t a1=0;a1!=antennaCount;++a1)
			{
				matrix[a1].resize(antennaCount);
				for(size_t a2=0;a2!=antennaCount;++a2)
					matrix[a1][a2] = 0;
			}
		}
	}
	
	// The actual reading of the data
	Logger::Debug << "Reading the data (interval={" << intStart << "..." << intEnd << "})...\n";
	
	double prevTime = -1.0;
	size_t timeIndexInSequence = size_t(0);
	size_t rowCount = table.nrow();
	
	casacore::Array<casacore::Complex> dataArray;
	casacore::Array<bool> flagArray;
	size_t
		prevFieldId = size_t(-1),
		sequenceId = size_t(-1),
		timeIndex = size_t(-1);
	for(size_t rowIndex = 0; rowIndex != rowCount; ++rowIndex)
	{
		double time = timeColumn(rowIndex);
		bool newTime = time != prevTime;
		if(newTime)
		{
			prevTime = time;
			++timeIndex;
		}
		if(timeIndex >= intEnd)
			break;
		if(timeIndex >= intStart)
		{
			size_t fieldId = fieldIdColumn(rowIndex);
			if(fieldId != prevFieldId)
			{
				prevFieldId = fieldId;
				sequenceId++;
				newTime = true;
			}
			const std::map<double, size_t>
				&observationTimes = ObservationTimes(sequenceId);
			if(newTime)
			{
				timeIndexInSequence = observationTimes.find(time)->second;
			}
			size_t ant1 = ant1Column(rowIndex);
			size_t ant2 = ant2Column(rowIndex);
			size_t spw = dataIdToSpw[dataDescIdColumn(rowIndex)];
			size_t spwFieldIndex = spw + sequenceId * bandCount;
			if(ant1 > ant2) std::swap(ant1, ant2);
			
			std::unique_ptr<Result>& result = baselineCube[spwFieldIndex][ant1][ant2];
			if(result == nullptr)
			{
				const size_t timeStepCount = observationTimes.size();
				const size_t nFreq = MetaData().FrequencyCount(spw);
				result.reset(new Result());
				for(size_t p=0;p!=polarizationCount;++p) {
					result->_realImages.emplace_back(Image2D::CreateZeroImagePtr(timeStepCount, nFreq));
					result->_imaginaryImages.emplace_back(Image2D::CreateZeroImagePtr(timeStepCount, nFreq));
					result->_flags.emplace_back(Mask2D::CreateSetMaskPtr<true>(timeStepCount, nFreq));
				}
				result->_bandInfo = bandInfos[spw];
				result->_uvw.resize(timeStepCount);
			}
			
			dataArray = dataColumn.get(rowIndex);
			flagArray = flagColumn.get(rowIndex);
			
			Array<double> uvwArray = uvwColumn.get(rowIndex);
			Array<double>::const_iterator uvwPtr = uvwArray.begin();
			UVW uvw;
			uvw.u = *uvwPtr; ++uvwPtr;
			uvw.v = *uvwPtr; ++uvwPtr;
			uvw.w = *uvwPtr;
			result->_uvw[timeIndexInSequence] = uvw;
			
			for(size_t p=0; p!=polarizationCount; ++p)
			{
				Array<Complex>::const_contiter dataPtr = dataArray.cbegin();
				Array<bool>::const_contiter flagPtr = flagArray.cbegin();
			
				Image2D& real = *result->_realImages[p];
				Image2D& imag = *result->_imaginaryImages[p];
				Mask2D& mask = *result->_flags[p];
				const size_t imgStride = real.Stride();
				const size_t mskStride = mask.Stride();
				num_t* realOutPtr = real.ValuePtr(timeIndexInSequence, 0);
				num_t* imagOutPtr = imag.ValuePtr(timeIndexInSequence, 0);
				bool* flagOutPtr = mask.ValuePtr(timeIndexInSequence, 0);
				
				for(size_t i=0;i!=p;++i)
				{
					++dataPtr;
					++flagPtr;
				}
				size_t frequencyCount = bandInfos[spw].channels.size();
				for(size_t ch=0;ch!=frequencyCount;++ch)
				{
					*realOutPtr = dataPtr->real();
					*imagOutPtr = dataPtr->imag();
					*flagOutPtr = *flagPtr;
					
					realOutPtr += imgStride;
					imagOutPtr += imgStride;
					flagOutPtr += mskStride;
					
					for(size_t i=0;i!=polarizationCount;++i) {
						++dataPtr;
						++flagPtr;
					}
				}
			}
		}
	}
	
	// Store elements in matrix in the baseline map.
	for(size_t s=0; s!=sequenceCount; ++s)
	{
		for(size_t b=0; b!=bandCount; ++b)
		{
			size_t fbIndex = s*bandCount + b;
			for(size_t a1=0; a1!=antennaCount; ++a1)
			{
				for(size_t a2=a1; a2!=antennaCount; ++a2)
				{
					std::unique_ptr<Result>& result = baselineCube[fbIndex][a1][a2];
					if(result)
					{
						_baselines.emplace(BaselineID(a1, a2, b, s), std::move(result));
					}
				}
			}
		}
	}
	_areFlagsChanged = false;
	
	Logger::Debug << "Reading toke " << watch.ToString() << ".\n";
}

void MemoryBaselineReader::PerformFlagWriteRequests()
{
	if(!_isRead)
	{
		readSet();
		_isRead = true;
	}
	
	for(size_t i=0; i!=_writeRequests.size(); ++i)
	{
		const FlagWriteRequest &request = _writeRequests[i];
		BaselineID id(request.antenna1, request.antenna2, request.spectralWindow, request.sequenceId);
		std::unique_ptr<Result>& result = _baselines[id];
		if(result->_flags.size() != request.flags.size())
			throw std::runtime_error("Polarizations do not match");
		for(size_t p=0;p!=result->_flags.size();++p)
			result->_flags[p].reset(new Mask2D(*request.flags[p]));
	}
	_areFlagsChanged = true;
	
	_writeRequests.clear();
}

void MemoryBaselineReader::writeFlags()
{
	casacore::Table &table = *Table();
	table.reopenRW();
	
	ScalarColumn<int>
		ant1Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA1)),
		ant2Column(table, casacore::MeasurementSet::columnName(MSMainEnums::ANTENNA2)),
		dataDescIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::DATA_DESC_ID)),
		fieldIdColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FIELD_ID));
	ScalarColumn<double>
		timeColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::TIME));
	ArrayColumn<bool>
		flagColumn(table, casacore::MeasurementSet::columnName(MSMainEnums::FLAG));
	std::vector<size_t> dataIdToSpw;
	MetaData().GetDataDescToBandVector(dataIdToSpw);
	
	const size_t polarizationCount = Polarizations().size();
		
	Logger::Debug << "Flags have changed, writing them back to the set...\n";
	
	double prevTime = -1.0;
	unsigned rowCount = table.nrow();
	size_t
		timeIndex = size_t(-1),
		prevFieldId = size_t(-1),
		sequenceId = size_t(-1),
		intStart = IntervalStart(),
		intEnd = IntervalEnd(),
		timeIndexInSequence = 0;
	for(unsigned rowIndex = 0; rowIndex != rowCount; ++rowIndex)
	{
		double time = timeColumn(rowIndex);
		bool newTime = time != prevTime;
		if(newTime)
		{
			prevTime = time;
			++timeIndex;
		}
		if(timeIndex >= intEnd)
			break;
		if(timeIndex >= intStart)
		{
			size_t fieldId = fieldIdColumn(rowIndex);
			if(fieldId != prevFieldId)
			{
				prevFieldId = fieldId;
				sequenceId++;
				newTime = true;
			}
			const std::map<double, size_t>
				&observationTimes = ObservationTimes(sequenceId);
			if(newTime)
			{
				timeIndexInSequence = observationTimes.find(time)->second;
			}
			
			size_t ant1 = ant1Column(rowIndex);
			size_t ant2 = ant2Column(rowIndex);
			size_t spw = dataIdToSpw[dataDescIdColumn(rowIndex)];
			if(ant1 > ant2) std::swap(ant1, ant2);
			
			size_t frequencyCount = MetaData().FrequencyCount(spw);
			IPosition flagShape = IPosition(2);
			flagShape[0] = polarizationCount;
			flagShape[1] = frequencyCount;
			casacore::Array<bool> flagArray(flagShape);
			
			BaselineID baselineID(ant1, ant2, spw, sequenceId);
			std::map<BaselineID, std::unique_ptr<Result>>::iterator
				resultIter = _baselines.find(baselineID);
			std::unique_ptr<Result>& result = resultIter->second;
			
			Array<bool>::contiter flagPtr = flagArray.cbegin();
			
			std::vector<Mask2D*> masks(polarizationCount);
			for(size_t p=0;p!=polarizationCount;++p)
				masks[p] = result->_flags[p].get();
			
			for(size_t ch=0;ch!=frequencyCount;++ch)
			{
				for(size_t p=0;p!=polarizationCount;++p)
				{
					*flagPtr = masks[p]->Value(timeIndexInSequence, ch);
					++flagPtr;
				}
			}
			
			flagColumn.put(rowIndex, flagArray);
		}
	}
	
	_areFlagsChanged = false;
}

bool MemoryBaselineReader::IsEnoughMemoryAvailable(const std::string &filename)
{
	uint64_t size = MeasurementSetDataSize(filename);
	uint64_t totalMem = System::TotalMemory();
	
	if(size * 2 >= totalMem)
	{
		Logger::Warn
			<< (size/1000000) << " MB required, but " << (totalMem/1000000) << " MB available.\n"
			"Because this is not at least twice as much, direct read mode (slower!) will be used.\n";
		return false;
	} else {
		Logger::Debug
			<< (size/1000000) << " MB required, " << (totalMem/1000000)
			<< " MB available: will use memory read mode.\n";
		return true;
	}
}
