#ifndef SUMTHRESHOLD_MISSING_H
#define SUMTHRESHOLD_MISSING_H

#include "../../structures/image2d.h"
#include "../../structures/mask2d.h"

class SumThresholdMissing
{
public:
	static void Horizontal(const Image2D& input, Mask2D& mask, const Mask2D& missing, Mask2D& scratch, size_t length, num_t threshold);
	
private:
	SumThresholdMissing() = delete;
};

#endif
