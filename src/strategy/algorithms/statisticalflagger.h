#ifndef STATISTICALFLAGGER_H
#define STATISTICALFLAGGER_H

#include <string>

#include "../../structures/mask2d.h"

class StatisticalFlagger{
	public:
		StatisticalFlagger();
		~StatisticalFlagger();
		
		static inline bool SquareContainsFlag(Mask2DCPtr mask, size_t xLeft, size_t yTop, size_t xRight, size_t yBottom);
		static void DilateFlags(Mask2DPtr mask, size_t timeSize, size_t frequencySize)
		{
			DilateFlagsHorizontally(mask, timeSize);
			DilateFlagsVertically(mask, frequencySize);
		}
		static void DilateFlagsHorizontally(Mask2DPtr mask, size_t timeSize);
		static void DilateFlagsVertically(Mask2DPtr mask, size_t frequencySize);
		static void LineRemover(Mask2DPtr mask, size_t maxTimeContamination, size_t maxFreqContamination);
		static void DensityTimeFlagger(Mask2DPtr mask, num_t minimumGoodDataRatio);
		static void DensityFrequencyFlagger(Mask2DPtr mask, num_t minimumGoodDataRatio);
		
	private:
		static void FlagTime(Mask2DPtr mask, size_t x);
		static void FlagFrequency(Mask2DPtr mask, size_t y);
		static void MaskToInts(Mask2DCPtr mask, int **maskAsInt);
		static void SumToLeft(Mask2DCPtr mask, int **sums, size_t width, size_t step, bool reverse);
		static void SumToTop(Mask2DCPtr mask, int **sums, size_t width, size_t step, bool reverse);
		static void ThresholdTime(Mask2DCPtr mask, int **flagMarks, int **sums, int thresholdLevel, int width);
		static void ThresholdFrequency(Mask2DCPtr mask, int **flagMarks, int **sums, int thresholdLevel, int width);
		static void ApplyMarksInTime(Mask2DPtr mask, int **flagMarks);
		static void ApplyMarksInFrequency(Mask2DPtr mask, int **flagMarks);
};

#endif
