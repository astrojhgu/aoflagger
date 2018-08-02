#include "sumthresholdmissing.h"

void SumThresholdMissing::Horizontal(const Image2D& input, Mask2D& mask, const Mask2D& missing, Mask2D& scratch, size_t length, num_t threshold)
{
	scratch = mask;
	const size_t width = mask.Width(), height = mask.Height();
	if(length <= width)
	{
		for(size_t y=0;y<height;++y)
		{
			num_t sum = 0.0;

			// Find first non-missing value for the start of the summation interval
			// xLeft points to the first element of the interval, which is marked as non-missing.
			size_t xLeft = 0;
			while(missing.Value(xLeft, y))
				++xLeft;
			
			// xRight points to the last non-missing element of the interval
			size_t
				xRight = xLeft,
				countAdded = 0,
				countTotal = 0;
			while(countTotal+1 < length && xRight != width)
			{
				if(!missing.Value(xRight, y))
				{
					if(!mask.Value(xRight, y))
					{
						sum += input.Value(xRight, y);
						++countAdded;
					}
					++countTotal;
				}
				++xRight;
			}
			
			while(xRight != width)
			{
				// Add a sample at the right
				if(!mask.Value(xRight, y))
				{
					sum += input.Value(xRight, y);
					++countAdded;
				}
				// Check
				if(countAdded>0 && fabs(sum/countAdded) > threshold)
				{
					scratch.SetHorizontalValues(xLeft, y, true, xRight - xLeft + 1);
				}
				// subtract one sample at the left
				if(!mask.Value(xLeft, y))
				{
					sum -= input.Value(xLeft, y);
					--countAdded;
				}
				do {
					++xRight;
				} while(missing.Value(xRight, y) && xRight != width);
				do {
					++xLeft;
				} while(missing.Value(xLeft, y));
			}
		}
	}
	mask = std::move(scratch);
}
