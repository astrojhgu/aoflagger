#include "functions.h"

#include "../strategy/algorithms/thresholdconfig.h"
#include "../strategy/algorithms/highpassfilter.h"

#include <boost/python.hpp>

using namespace boost::python;

namespace aoflagger_python
{

static object flagFunction;
	
void set_flag_function(PyObject* callable)
{
	flagFunction = object(boost::python::handle<>(boost::python::borrowed(callable)));
}

object get_flag_function()
{
	return flagFunction;
}
	
void sumthreshold(Data& data, double thresholdFactor, bool horizontal, bool vertical)
{
	ThresholdConfig thresholdConfig;
	thresholdConfig.InitializeLengthsDefault();
	thresholdConfig.InitializeThresholdsFromFirstThreshold(6.0L, ThresholdConfig::Rayleigh);
	if(!horizontal)
		thresholdConfig.RemoveHorizontalOperations();
	if(!vertical)
		thresholdConfig.RemoveVerticalOperations();
	
	if(data.TFData().PolarizationCount() != 1)
		throw std::runtime_error("Input data in sum_threshold has wrong format");
	
	Mask2DPtr mask = Mask2D::CreateCopy(data.TFData().GetSingleMask());
	Image2DCPtr image = data.TFData().GetSingleImage();
	thresholdConfig.Execute(image, mask, false, thresholdFactor);
	data.TFData().SetGlobalMask(mask);
}

void high_pass_filter(Data& data, size_t kernelWidth, size_t kernelHeight, double horizontalSigmaSquared, double verticalSigmaSquared)
{
	if(data.TFData().PolarizationCount() != 1)
		throw std::runtime_error("High-pass filtering needs single polarization");
	HighPassFilter filter;
	filter.SetHWindowSize(kernelWidth);
	filter.SetVWindowSize(kernelHeight);
	filter.SetHKernelSigmaSq(horizontalSigmaSquared);
	filter.SetVKernelSigmaSq(verticalSigmaSquared);
	Mask2DCPtr mask = data.TFData().GetSingleMask();
	size_t imageCount = data.TFData().ImageCount();
	
	for(size_t i=0;i<imageCount;++i)
		data.TFData().SetImage(i, filter.ApplyHighPass(data.TFData().GetImage(i), mask));
}
	
}
