#include <boost/python.hpp>
 
#include "data.h"
#include "functions.h"

#include "../structures/polarization.h"
 
BOOST_PYTHON_MODULE(aoflagger)
{
	using namespace boost::python;
	
	class_<aoflagger_python::Data>("Data")
		.def("convert_to_polarization", &aoflagger_python::Data::convert_to_polarization)
		.def("polarizations", &aoflagger_python::Data::polarizations);

	def("sumthreshold", aoflagger_python::sumthreshold);
	def("high_pass_filter", aoflagger_python::high_pass_filter);
	def("set_flag_function", aoflagger_python::set_flag_function);
	
	enum_<PolarizationEnum>("Polarization")
		.value("StokesI", Polarization::StokesI)
		.value("StokesQ", Polarization::StokesQ)
		.value("StokesU", Polarization::StokesU)
		.value("StokesV", Polarization::StokesV)
		.value("RR", Polarization::RR)
		.value("RL", Polarization::RL)
		.value("LR", Polarization::LR)
		.value("LL", Polarization::LL)
		.value("XX", Polarization::XX)
		.value("XY", Polarization::XY)
		.value("YX", Polarization::YX)
		.value("YY", Polarization::YY);
		
	enum_<enum TimeFrequencyData::ComplexRepresentation>("ComplexRepresentation")
		.value("RealPart", TimeFrequencyData::RealPart)
		.value("ImaginaryPart", TimeFrequencyData::ImaginaryPart)
		.value("PhasePart", TimeFrequencyData::PhasePart)
		.value("AmplitudePart", TimeFrequencyData::AmplitudePart)
		.value("ComplexParts", TimeFrequencyData::ComplexParts);
}