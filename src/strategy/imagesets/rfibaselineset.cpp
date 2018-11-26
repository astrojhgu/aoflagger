#include "rfibaselineset.h"

#include "../../msio/singlebaselinefile.h"

#include <fstream>

namespace rfiStrategy {

std::unique_ptr<BaselineData> RFIBaselineSet::Read()
{
	SingleBaselineFile file;
	std::ifstream str(_path);
	file.Read(str);
	return std::unique_ptr<BaselineData>(new BaselineData(
		file.data,
		TimeFrequencyMetaDataPtr(new TimeFrequencyMetaData(file.metaData))
	));
}

}
