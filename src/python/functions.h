#include "data.h"

#include <cstring>

#include <boost/python/object.hpp>

namespace aoflagger_python
{
	void set_flag_function(PyObject* callable);
	
	boost::python::object get_flag_function();
	
	void sumthreshold(Data& data, double thresholdFactor, bool horizontal, bool vertical);
	
	void high_pass_filter(Data& data, size_t kernelWidth, size_t kernelHeight, double horizontalSigmaSquared, double verticalSigmaSquared);
}
