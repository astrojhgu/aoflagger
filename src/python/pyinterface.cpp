#include <boost/python.hpp>

#include <boost/python/numpy.hpp>

#include <numpy/arrayobject.h>

#include "data.h"
#include "functions.h"

#include "../structures/polarization.h"

#include "../interface/aoflagger.h"

#include <iostream>

static boost::python::numpy::ndarray GetImageBuffer(const aoflagger::ImageSet* imageSet, size_t imageIndex)
{
	namespace np = boost::python::numpy;
	const float* values = imageSet->ImageBuffer(imageIndex);
	Py_intptr_t shape[2] = { unsigned(imageSet->Width()), unsigned(imageSet->Height()) };
	np::ndarray result = np::zeros(2, shape, np::dtype::get_builtin<double>());
	std::copy_n(values, imageSet->Width()*imageSet->Height(), reinterpret_cast<double*>(result.get_data()));
	return result;
}

static void SetImageBuffer(aoflagger::ImageSet* imageSet, size_t imageIndex, const boost::python::numpy::ndarray& values)
{
	namespace np = boost::python::numpy;
	if(values.get_dtype() != np::dtype::get_builtin<double>())
		throw std::runtime_error("ImageSet.set_image_buffer(): Invalid type specified for data array; double numpy array required");
	if(values.shape(0) != imageSet->Width() || values.shape(1) != imageSet->Height())
		throw std::runtime_error("ImageSet.set_image_buffer(): dimensions of provided array doesn't match with image set");
	const double *data = reinterpret_cast<const double*>(values.get_data());
	if(!data)
		throw std::runtime_error("Data needs to be provided that is interpretable as a float array");
	float* buffer = imageSet->ImageBuffer(imageIndex);
	for(size_t y=0; y!=imageSet->Height(); ++y)
	{
		float* row = buffer + y * imageSet->HorizontalStride();
		for(size_t x=0; x!=imageSet->Width(); ++x)
		{
			row[x] = *data;
			++data;
		}
	}
}

static boost::python::numpy::ndarray GetBuffer(const aoflagger::FlagMask* flagMask)
{
	namespace np = boost::python::numpy;
	const bool* values = flagMask->Buffer();
	Py_intptr_t shape[2] = { unsigned(flagMask->Width()), unsigned(flagMask->Height()) };
	np::ndarray result = np::zeros(2, shape, np::dtype::get_builtin<bool>());
	std::copy_n(values, flagMask->Width()*flagMask->Height(), reinterpret_cast<bool*>(result.get_data()));
	return result;
}

static void SetBuffer(aoflagger::FlagMask* flagMask, const boost::python::numpy::ndarray& values)
{
	namespace np = boost::python::numpy;
	if(values.get_dtype() != np::dtype::get_builtin<bool>())
		throw std::runtime_error("FlagMask.set_buffer(): Invalid type specified for data array; double numpy array required");
	PyArrayObject *arr = reinterpret_cast<PyArrayObject*>(values.ptr());
	if(PyArray_SHAPE(arr)[0] != flagMask->Width() || PyArray_SHAPE(arr)[1] != flagMask->Height())
		throw std::runtime_error("FlagMask.set_buffer(): dimensions of provided array doesn't match with image set");
	const bool *data = (bool *) PyArray_DATA(arr);
	if(!data)
		throw std::runtime_error("Data needs to be provided that is interpretable as a float array");
	bool* buffer = flagMask->Buffer();
	for(size_t y=0; y!=flagMask->Height(); ++y)
	{
		bool* row = buffer + y * flagMask->HorizontalStride();
		for(size_t x=0; x!=flagMask->Width(); ++x)
		{
			row[x] = *data;
			++data;
		}
	}
}

boost::python::object MakeImageSet1(aoflagger::AOFlagger* flagger, size_t width, size_t height, size_t count)
{ return boost::python::object(flagger->MakeImageSet(width, height, count)); }

boost::python::object MakeImageSet2(aoflagger::AOFlagger* flagger, size_t width, size_t height, size_t count, size_t widthCapacity)
{ return boost::python::object(flagger->MakeImageSet(width, height, count, widthCapacity)); }

boost::python::object MakeImageSet3(aoflagger::AOFlagger* flagger, size_t width, size_t height, size_t count, float initialValue)
{ return boost::python::object(flagger->MakeImageSet(width, height, count, initialValue)); }

boost::python::object MakeImageSet4(aoflagger::AOFlagger* flagger, size_t width, size_t height, size_t count, float initialValue, size_t widthCapacity)
{ return boost::python::object(flagger->MakeImageSet(width, height, count, initialValue, widthCapacity)); }

boost::python::object MakeFlagMask1(aoflagger::AOFlagger* flagger, size_t width, size_t height)
{ return boost::python::object(flagger->MakeFlagMask(width, height)); }

boost::python::object MakeFlagMask2(aoflagger::AOFlagger* flagger, size_t width, size_t height, bool initialValue)
{ return boost::python::object(flagger->MakeFlagMask(width, height, initialValue)); }

boost::python::object MakeStrategy1(aoflagger::AOFlagger* flagger, enum aoflagger::TelescopeId telescopeId)
{ return boost::python::object(flagger->MakeStrategy(telescopeId)); }

boost::python::object MakeStrategy2(aoflagger::AOFlagger* flagger, enum aoflagger::TelescopeId telescopeId, unsigned strategyFlags)
{ return boost::python::object(flagger->MakeStrategy(telescopeId, strategyFlags)); }

boost::python::object MakeStrategy3(aoflagger::AOFlagger* flagger, enum aoflagger::TelescopeId telescopeId, unsigned strategyFlags, double frequency)
{ return boost::python::object(flagger->MakeStrategy(telescopeId, strategyFlags, frequency)); }

boost::python::object MakeStrategy4(aoflagger::AOFlagger* flagger, enum aoflagger::TelescopeId telescopeId, unsigned strategyFlags, double frequency, double timeRes)
{ return boost::python::object(flagger->MakeStrategy(telescopeId, strategyFlags, frequency, timeRes)); }

boost::python::object MakeStrategy5(aoflagger::AOFlagger* flagger, enum aoflagger::TelescopeId telescopeId, unsigned strategyFlags, double frequency, double timeRes, double frequencyRes)
{ return boost::python::object(flagger->MakeStrategy(telescopeId, strategyFlags, frequency, timeRes, frequencyRes)); }

boost::python::object LoadStrategy(aoflagger::AOFlagger* flagger, const std::string& filename)
{ return boost::python::object(flagger->LoadStrategy(filename)); }

boost::python::object Run1(aoflagger::AOFlagger* flagger, aoflagger::Strategy& strategy, const aoflagger::ImageSet& input)
{ return boost::python::object(flagger->Run(strategy, input)); }

boost::python::object Run2(aoflagger::AOFlagger* flagger, aoflagger::Strategy& strategy, const aoflagger::ImageSet& input, const aoflagger::FlagMask& existingFlags)
{ return boost::python::object(flagger->Run(strategy, input, existingFlags)); }

BOOST_PYTHON_MODULE(aoflagger)
{
	using namespace boost::python;
	
	boost::python::numpy::initialize();
	
	class_<aoflagger_python::Data>("Data")
		.def(self - self)
		.def("__copy__", &aoflagger_python::Data::copy)
		.def("clear_mask", &aoflagger_python::Data::clear_mask)
		.def("convert_to_polarization", &aoflagger_python::Data::convert_to_polarization)
		.def("convert_to_complex", &aoflagger_python::Data::convert_to_complex)
		.def("join_mask", &aoflagger_python::Data::join_mask)
		.def("make_complex", &aoflagger_python::Data::make_complex)
		.def("polarizations", &aoflagger_python::Data::polarizations)
		.def("set_image", &aoflagger_python::Data::set_image)
		.def("set_polarization_data", &aoflagger_python::Data::set_polarization_data);

	def("enlarge", aoflagger_python::enlarge);
	def("high_pass_filter", aoflagger_python::high_pass_filter);
	def("low_pass_filter", aoflagger_python::low_pass_filter);
	def("save_heat_map", aoflagger_python::save_heat_map);
	def("print_polarization_statistics", aoflagger_python::print_polarization_statistics);
	def("scale_invariant_rank_operator", aoflagger_python::scale_invariant_rank_operator);
	def("shrink", aoflagger_python::shrink);
	def("sumthreshold", aoflagger_python::sumthreshold);
	def("set_flag_function", aoflagger_python::set_flag_function);
	def("threshold_channel_rms", aoflagger_python::threshold_channel_rms);
	def("threshold_timestep_rms", aoflagger_python::threshold_timestep_rms);
	
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
		
	enum_<enum aoflagger::TelescopeId>("TelescopeId")
		.value("Generic", aoflagger::GENERIC_TELESCOPE)
		.value("AARTFAAC", aoflagger::AARTFAAC_TELESCOPE)
		.value("Arecibo", aoflagger::ARECIBO_TELESCOPE)
		.value("Bighorns", aoflagger::BIGHORNS_TELESCOPE)
		.value("JVLA", aoflagger::JVLA_TELESCOPE)
		.value("LOFAR", aoflagger::LOFAR_TELESCOPE)
		.value("MWA", aoflagger::MWA_TELESCOPE)
		.value("Parkes", aoflagger::PARKES_TELESCOPE)
		.value("WSRT", aoflagger::WSRT_TELESCOPE);

	class_<aoflagger::StrategyFlags>("StrategyFlags", no_init)
		.def_readonly("NONE", &aoflagger::StrategyFlags::NONE)
		.def_readonly("LARGE_BANDWIDTH", &aoflagger::StrategyFlags::LARGE_BANDWIDTH)
		.def_readonly("SMALL_BANDWIDTH", &aoflagger::StrategyFlags::SMALL_BANDWIDTH)
		.def_readonly("TRANSIENTS", &aoflagger::StrategyFlags::TRANSIENTS)
		.def_readonly("ROBUST", &aoflagger::StrategyFlags::ROBUST)
		.def_readonly("FAST", &aoflagger::StrategyFlags::FAST)
		.def_readonly("INSENSITIVE", &aoflagger::StrategyFlags::INSENSITIVE)
		.def_readonly("SENSITIVE", &aoflagger::StrategyFlags::SENSITIVE)
		.def_readonly("USE_ORIGINAL_FLAGS", &aoflagger::StrategyFlags::USE_ORIGINAL_FLAGS)
		.def_readonly("AUTO_CORRELATION", &aoflagger::StrategyFlags::AUTO_CORRELATION)
		.def_readonly("HIGH_TIME_RESOLUTION", &aoflagger::StrategyFlags::HIGH_TIME_RESOLUTION);

	class_<aoflagger::ImageSet>("ImageSet")
		.def("width", &aoflagger::ImageSet::Width)
		.def("height", &aoflagger::ImageSet::Height)
		.def("image_count", &aoflagger::ImageSet::ImageCount)
		.def("horizontal_stride", &aoflagger::ImageSet::HorizontalStride)
		.def("set", &aoflagger::ImageSet::Set)
		.def("get_image_buffer", GetImageBuffer)
		.def("set_image_buffer", SetImageBuffer)
		.def("resize_without_reallocation", &aoflagger::ImageSet::ResizeWithoutReallocation);
	
	class_<aoflagger::FlagMask>("FlagMask")
		.def("width", &aoflagger::FlagMask::Width)
		.def("height", &aoflagger::FlagMask::Height)
		.def("horizontal_stride", &aoflagger::FlagMask::HorizontalStride)
		.def("get_buffer", GetBuffer)
		.def("set_buffer", SetBuffer)
		;
		
	class_<aoflagger::Strategy>("Strategy", no_init);
	
	class_<aoflagger::AOFlagger, boost::noncopyable>("AOFlagger")
		.def("make_image_set", MakeImageSet1)
		.def("make_image_set", MakeImageSet2)
		.def("make_image_set", MakeImageSet3)
		.def("make_image_set", MakeImageSet4)
		.def("make_flag_mask", MakeFlagMask1)
		.def("make_flag_mask", MakeFlagMask2)
		.def("make_strategy", MakeStrategy1)
		.def("make_strategy", MakeStrategy2)
		.def("make_strategy", MakeStrategy3)
		.def("make_strategy", MakeStrategy4)
		.def("make_strategy", MakeStrategy5)
		.def("load_strategy", LoadStrategy)
		.def("run", Run1)
		.def("run", Run2)
		.def("get_version_string", &aoflagger::AOFlagger::GetVersionString).staticmethod("get_version_string")
		.def("get_version_date", &aoflagger::AOFlagger::GetVersionDate).staticmethod("get_version_date");
}
