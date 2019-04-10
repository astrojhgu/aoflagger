#ifndef MS_SELECTION_H
#define MS_SELECTION_H

#include <casacore/tables/Tables/ArrayColumn.h>
#include <casacore/tables/Tables/ScalarColumn.h>

#include <casacore/ms/MeasurementSets/MeasurementSet.h>

#include <map>
#include <vector>

class MSSelection
{
public:
	MSSelection(casacore::MeasurementSet& ms,
		size_t intervalStart,
		size_t intervalEnd,
		const std::vector<std::map<double, size_t>>& observationTimes
	) :
		_intervalStart(intervalStart),
		_intervalEnd(intervalEnd),
		_observationTimes(observationTimes),
		_ms(ms)
	{ }
	
	template<typename Function>
	void Process(Function function)
	{
		casacore::ScalarColumn<double> timeColumn(_ms, "TIME");
		casacore::ScalarColumn<int> fieldIdColumn(_ms, "FIELD_ID");
		
		double prevTime = -1.0;
		size_t
			prevFieldId = size_t(-1),
			sequenceId = size_t(-1),
			timeIndexInSequence = size_t(-1),
			timeIndex = size_t(-1);
	
		for(size_t rowIndex = 0; rowIndex != _ms.nrow(); ++rowIndex)
		{
			double time = timeColumn(rowIndex);
			bool newTime = time != prevTime;
			if(newTime)
			{
				prevTime = time;
				++timeIndex;
			}
			if(timeIndex >= _intervalEnd)
				break;
			if(timeIndex >= _intervalStart)
			{
				size_t fieldId = fieldIdColumn(rowIndex);
				if(fieldId != prevFieldId)
				{
					prevFieldId = fieldId;
					sequenceId++;
					newTime = true;
				}
				const std::map<double, size_t>
					&observationTimes = _observationTimes[sequenceId];
				if(newTime)
				{
					timeIndexInSequence = observationTimes.find(time)->second;
				}
				function(rowIndex, sequenceId, timeIndex, timeIndexInSequence);
			}
		}
	}
	
private:
	size_t _intervalStart, _intervalEnd;
	
	std::vector<std::map<double, size_t>> _observationTimes;
	
	casacore::MeasurementSet& _ms;
};

#endif
