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
		
		void clear_mask()
		{
			_tfData.SetNoMask();
		}
		
		void join_mask(const Data& other)
		{
			_tfData.JoinMask(other._tfData);
		}
		
		boost::python::list polarizations() const
		{
			const std::vector<PolarizationEnum> pols = _tfData.Polarizations();
			boost::python::list polList;
			for (PolarizationEnum p : pols)
				polList.append(p);
			return polList;
		}
		
		void set_polarization_data(PolarizationEnum polarization, const Data& data)
		{
			size_t polIndex = _tfData.GetPolarizationIndex(polarization);
			_tfData.SetPolarizationData(polIndex, data._tfData);
		}
		
		Data convert_to_polarization(PolarizationEnum polarization)
		{
			return Data(_tfData.Make(polarization));
		}
		
		Data convert_to_complex(enum TimeFrequencyData::ComplexRepresentation complexRepresentation)
		{
			return Data(_tfData.Make(complexRepresentation));
		}
		
		Data make_complex()
		{
			TimeFrequencyData* newTFData = _tfData.CreateTFDataFromComplexCombination(_tfData, _tfData);
			Data newData(*newTFData);
			delete newTFData;
			return newData;
		}
		
		TimeFrequencyData& TFData() { return _tfData; }
		const TimeFrequencyData& TFData() const { return _tfData; }
		
	private:
		TimeFrequencyData _tfData;
	};
}

#endif
