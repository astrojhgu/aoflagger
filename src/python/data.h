#ifndef PYTHON_DATA_H
#define PYTHON_DATA_H

#include "../structures/timefrequencydata.h"

#include <boost/python/list.hpp>

namespace aoflagger_python
{
	class Data
	{
	public:
		Data() = default;
		
		Data(const TimeFrequencyData& tfData) : _tfData(tfData)
		{ }
		
		boost::python::list polarizations() const
		{
			const std::vector<PolarizationEnum> pols = _tfData.Polarisations();
			boost::python::list polList;
			for (PolarizationEnum p : pols)
				polList.append(p);
			return polList;
		}
		
		Data convert_to_polarization(PolarizationEnum polarization)
		{
			return Data(_tfData.Make(polarization));
		}
		
		Data convert_to_complex_representation(enum TimeFrequencyData::ComplexRepresentation complexRepresentation)
		{
			return Data(_tfData.Make(complexRepresentation));
		}
		
		TimeFrequencyData& TFData() { return _tfData; }
		const TimeFrequencyData& TFData() const { return _tfData; }
		
	private:
		TimeFrequencyData _tfData;
	};
}

#endif
