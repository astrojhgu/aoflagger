#ifdef __SSE__
#define USE_INTRINSICS
#endif

#ifdef USE_INTRINSICS

#include <stdint.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#include "../../structures/image2d.h"

#include "combinatorialthresholder.h"

#include "../../util/logger.h"

/**
 * The SSE version of the Vertical SumThreshold algorithm using intrinsics.
 *
 * The SumThreshold algorithm is applied on 4 time steps at a time. Since the SSE has
 * instructions that operate on 4 floats at a time, this could in theory speed up the
 * processing with a factor of 4. However, a lot of time is lost shuffling the data in the
 * right order/registers, and since a timestep consists of 1 byte booleans and 4 byte
 * floats, there's a penalty. Finally, since the 4 timesteps have to be processed exactly
 * the same way, conditional branches had to be replaced by conditional moves.
 *
 * The average profit of SSE intrinsics vs no SSE seems to be about a factor of 1.5 to 2-3,
 * depending on the Length parameter, but also depending on the number of flags (With
 * Length=256, it can make a factor of 3 difference). It might also vary on different
 * processors; e.g. on my Desktop Xeon with older gcc, the profit was less pronounced,
 * while my Intel i5 at home showed an avg factor of over 2 difference.
 *
 * The algorithm works with Length=1, but since that is a normal thresholding operation,
 * computing a sumthreshold has a lot of overhead, hence is not optimal at that size.
 */
template<size_t Length>
void CombinatorialThresholder::VerticalSumThresholdLargeSSE(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold)
{
	*scratch = *mask;
	const size_t width = mask->Width(), height = mask->Height();
	const __m128 zero4 = _mm_set1_ps(0.0);
	const __m128i zero4i = _mm_set1_epi32(0);
	const __m128i ones4 = _mm_set1_epi32(1);
	const __m128 threshold4Pos = _mm_set1_ps(threshold);
	const __m128 threshold4Neg = _mm_set1_ps(-threshold); 
	if(Length <= height)
	{
		for(size_t x=0;x<width;x += 4)
		{
			__m128 sum4 = _mm_set1_ps(0.0);
			__m128i count4 = _mm_set1_epi32(0);
			size_t yBottom;
			
			for(yBottom=0;yBottom+1<Length;++yBottom)
			{
				const bool *rowPtr = mask->ValuePtr(x, yBottom);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m128 conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(rowPtr[3], rowPtr[2], rowPtr[1], rowPtr[0]),
													zero4i));
				
				// Conditionally increment counters
				count4 = _mm_add_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Add values with conditional move
				__m128 m = _mm_and_ps(_mm_load_ps(input->ValuePtr(x, yBottom)), conditionMask);
				sum4 = _mm_add_ps(sum4, _mm_or_ps(m, _mm_andnot_ps(conditionMask, zero4)));
			}
			
			size_t yTop = 0;
			while(yBottom < height)
			{
				// ** Add the 4 sample at the bottom **
				
				// get a ptr
				const bool *rowPtr = mask->ValuePtr(x, yBottom);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m128 conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(rowPtr[3], rowPtr[2], rowPtr[1], rowPtr[0]),
													zero4i));
				
				// Conditionally increment counters
				count4 = _mm_add_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Add values with conditional move
				sum4 = _mm_add_ps(sum4,
					_mm_or_ps(_mm_and_ps(_mm_load_ps(input->ValuePtr(x, yBottom)), conditionMask),
										_mm_andnot_ps(conditionMask, zero4)));
				
				// ** Check sum **
				
				// if sum/count > threshold || sum/count < -threshold
				__m128 avg4 = _mm_div_ps(sum4, _mm_cvtepi32_ps(count4));
				const unsigned flagConditions =
					_mm_movemask_ps(_mm_cmpgt_ps(avg4, threshold4Pos)) |
					_mm_movemask_ps(_mm_cmplt_ps(avg4, threshold4Neg));
				// | _mm_movemask_ps(_mm_cmplt_ps(count4, zero4i));
				
				// The assumption is that most of the values are actually not thresholded, hence, if
				// this is the case, we circumvent the whole loop at the cost of one extra comparison:
				if(flagConditions != 0)
				{
					union
					{
						bool theChars[4];
						uint32_t theInt;
					} outputValues = { {
						(flagConditions&1)!=0,
						(flagConditions&2)!=0,
						(flagConditions&4)!=0,
						(flagConditions&8)!=0 } };

					for(size_t i=0;i<Length;++i)
					{
						uint32_t *outputPtr = reinterpret_cast<uint32_t*>(scratch->ValuePtr(x, yTop + i));
						*outputPtr |= outputValues.theInt;
					}
				}
				
				// ** Subtract the sample at the top **
				
				// get a ptr
				const bool *tRowPtr = mask->ValuePtr(x, yTop);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(tRowPtr[3], tRowPtr[2], tRowPtr[1], tRowPtr[0]),
													zero4i));
				
				// Conditionally decrement counters
				count4 = _mm_sub_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Subtract values with conditional move
				sum4 = _mm_sub_ps(sum4,
					_mm_or_ps(_mm_and_ps(_mm_load_ps(input->ValuePtr(x, yTop)), conditionMask),
										_mm_andnot_ps(conditionMask, zero4)));
				
				// ** Next... **
				++yTop;
				++yBottom;
			}
		}
	}
	std::swap(*mask, *scratch);
}

template<size_t Length>
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold)
{
	// The idea of the horizontal SSE version is to read four ('y') rows and
	// process them simultaneously. 
	
	// Currently, this SSE horizontal version is not significant faster
	// (less than ~3%) than the
	// Non-SSE horizontal version. This has probably to do with 
	// rather randomly reading through the set (first (0,0)-(0,3), then (1,0)-(1,3), etc)
	// this introduces cache misses and/or many smaller reading requests
	
	*scratch = *mask;
	const size_t width = mask->Width(), height = mask->Height();
	const __m128 zero4 = _mm_set1_ps(0.0);
	const __m128i zero4i = _mm_set1_epi32(0);
	const __m128i ones4 = _mm_set1_epi32(1);
	const __m128 threshold4Pos = _mm_set1_ps(threshold);
	const __m128 threshold4Neg = _mm_set1_ps(-threshold); 
	if(Length <= width)
	{
		for(size_t y=0;y<height;y += 4)
		{
			__m128 sum4 = _mm_set1_ps(0.0);
			__m128i count4 = _mm_set1_epi32(0);
			size_t xRight;
			
			const bool
				*rFlagPtrA = mask->ValuePtr(0, y+3),
				*rFlagPtrB = mask->ValuePtr(0, y+2),
				*rFlagPtrC = mask->ValuePtr(0, y+1),
				*rFlagPtrD = mask->ValuePtr(0, y);
			const num_t
				*rValPtrA = input->ValuePtr(0, y+3),
				*rValPtrB = input->ValuePtr(0, y+2),
				*rValPtrC = input->ValuePtr(0, y+1),
				*rValPtrD = input->ValuePtr(0, y);
				
			for(xRight=0;xRight+1<Length;++xRight)
			{
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m128 conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(*rFlagPtrA, *rFlagPtrB, *rFlagPtrC, *rFlagPtrD),
													zero4i));
				
				// Conditionally increment counters (nr unflagged samples)
				count4 = _mm_add_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Load 4 samples
				__m128 v = _mm_set_ps(*rValPtrA,
															*rValPtrB,
															*rValPtrC,
															*rValPtrD);
				
				// Add values with conditional move
				sum4 = _mm_add_ps(sum4, _mm_or_ps(_mm_and_ps(v, conditionMask),
																					_mm_andnot_ps(conditionMask, zero4)));
				
				++rFlagPtrA;
				++rFlagPtrB;
				++rFlagPtrC;
				++rFlagPtrD;

				++rValPtrA;
				++rValPtrB;
				++rValPtrC;
				++rValPtrD;
			}
			
			size_t xLeft = 0;
			const bool
				*lFlagPtrA = mask->ValuePtr(0, y+3),
				*lFlagPtrB = mask->ValuePtr(0, y+2),
				*lFlagPtrC = mask->ValuePtr(0, y+1),
				*lFlagPtrD = mask->ValuePtr(0, y);
			const num_t
				*lValPtrA = input->ValuePtr(0, y+3),
				*lValPtrB = input->ValuePtr(0, y+2),
				*lValPtrC = input->ValuePtr(0, y+1),
				*lValPtrD = input->ValuePtr(0, y);
				
			while(xRight < width)
			{
				// ** Add the sample at the right **
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m128 conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(*rFlagPtrA, *rFlagPtrB, *rFlagPtrC, *rFlagPtrD),
													zero4i));
				
				// Conditionally increment counters
				count4 = _mm_add_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Load 4 samples
				__m128 v = _mm_set_ps(*rValPtrA,
															*rValPtrB,
															*rValPtrC,
															*rValPtrD);
				
				// Add values with conditional move (sum4 += (v & m) | (m & ~0) ).
				sum4 = _mm_add_ps(sum4, _mm_or_ps(_mm_and_ps(v, conditionMask), 
																					_mm_andnot_ps(conditionMask, zero4)));
				
				// ** Check sum **
				
				// if sum/count > threshold || sum/count < -threshold
				__m128 count4AsSingle = _mm_cvtepi32_ps(count4);
				const unsigned flagConditions =
					_mm_movemask_ps(_mm_cmpgt_ps(_mm_div_ps(sum4, count4AsSingle), threshold4Pos)) |
					_mm_movemask_ps(_mm_cmplt_ps(_mm_div_ps(sum4, count4AsSingle), threshold4Neg));
				
				if((flagConditions & 1) != 0)
					scratch->SetHorizontalValues(xLeft, y, true, Length);
				if((flagConditions & 2) != 0)
					scratch->SetHorizontalValues(xLeft, y+1, true, Length);
				if((flagConditions & 4) != 0)
					scratch->SetHorizontalValues(xLeft, y+2, true, Length);
				if((flagConditions & 8) != 0)
					scratch->SetHorizontalValues(xLeft, y+3, true, Length);
				
				// ** Subtract the sample at the left **
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(*lFlagPtrA, *lFlagPtrB, *lFlagPtrC, *lFlagPtrD),
													zero4i));
				
				// Conditionally decrement counters
				count4 = _mm_sub_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Load 4 samples
				v = _mm_set_ps(*lValPtrA,
											 *lValPtrB,
											 *lValPtrC,
											 *lValPtrD);
				
				// Subtract values with conditional move
				sum4 = _mm_sub_ps(sum4,
					_mm_or_ps(_mm_and_ps(v, conditionMask), _mm_andnot_ps(conditionMask, zero4)));
				
				// ** Next... **
				++xLeft;
				++xRight;
				
				++rFlagPtrA;
				++rFlagPtrB;
				++rFlagPtrC;
				++rFlagPtrD;
				
				++lFlagPtrA;
				++lFlagPtrB;
				++lFlagPtrC;
				++lFlagPtrD;

				++rValPtrA;
				++rValPtrB;
				++rValPtrC;
				++rValPtrD;
				
				++lValPtrA;
				++lValPtrB;
				++lValPtrC;
				++lValPtrD;
			}
		}
	}
	std::swap(*mask, *scratch);
}

template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<2>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<4>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<8>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<16>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<32>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<64>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<128>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeSSE<256>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);

template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<2>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<4>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<8>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<16>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<32>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<64>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<128>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::HorizontalSumThresholdLargeSSE<256>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);

#endif

#ifdef __AVX2__

#include <immintrin.h>

template<size_t Length>
void CombinatorialThresholder::VerticalSumThresholdLargeAVX(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold)
{
	*scratch = *mask;
	const size_t width = mask->Width(), height = mask->Height();
	const __m256 zero8 = _mm256_set1_ps(0.0);
	const __m256i zero8i = _mm256_set1_epi32(0);
	const __m256i ones8 = _mm256_set1_epi32(1);
	const __m256 threshold8Pos = _mm256_set1_ps(threshold);
	const __m256 threshold8Neg = _mm256_set1_ps(-threshold); 
	if(Length <= height)
	{
		size_t x=0;
		for(;x+4<width;x += 8)
		{
			__m256 sum8 = _mm256_set1_ps(0.0);
			__m256i count8 = _mm256_set1_epi32(0);
			size_t yBottom;
			
			for(yBottom=0;yBottom+1<Length;++yBottom)
			{
				const bool *rowPtr = mask->ValuePtr(x, yBottom);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m256 conditionMask = _mm256_castsi256_ps(
					_mm256_cmpeq_epi32(_mm256_set_epi32(
						rowPtr[7], rowPtr[6], rowPtr[5], rowPtr[4],
						rowPtr[3], rowPtr[2], rowPtr[1], rowPtr[0]),
						zero8i));
				
				// Conditionally increment counters
				count8 = _mm256_add_epi32(count8, _mm256_and_si256(_mm256_castps_si256(conditionMask), ones8));
				
				// Add values with conditional move
				__m256 m = _mm256_and_ps(_mm256_load_ps(input->ValuePtr(x, yBottom)), conditionMask);
				sum8 = _mm256_add_ps(sum8, _mm256_or_ps(m, _mm256_andnot_ps(conditionMask, zero8)));
			}
			
			size_t yTop = 0;
			while(yBottom < height)
			{
				// ** Add the 8 sample at the bottom **
				
				// get a ptr
				const bool *rowPtr = mask->ValuePtr(x, yBottom);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m256 conditionMask = _mm256_castsi256_ps(
					_mm256_cmpeq_epi32(_mm256_set_epi32(
						rowPtr[7], rowPtr[6], rowPtr[5], rowPtr[4],
						rowPtr[3], rowPtr[2], rowPtr[1], rowPtr[0]),
						zero8i));
				
				// Conditionally increment counters
				count8 = _mm256_add_epi32(count8, _mm256_and_si256(_mm256_castps_si256(conditionMask), ones8));
				
				// Add values with conditional move
				sum8 = _mm256_add_ps(sum8,
					_mm256_or_ps(_mm256_and_ps(_mm256_load_ps(
						input->ValuePtr(x, yBottom)), conditionMask),
					_mm256_andnot_ps(conditionMask, zero8)));
				
				// ** Check sum **
				
				// if sum/count > threshold || sum/count < -threshold
				__m256 avg8 = _mm256_div_ps(sum8, _mm256_cvtepi32_ps(count8));
				//float a[8];
				//_mm256_storeu_ps(a, avg8);
				//std::cout << a[0] << '\n';
				const int flagConditions = 
					_mm256_movemask_ps(_mm256_cmp_ps(avg8, threshold8Pos, _CMP_GT_OQ)) | _mm256_movemask_ps(_mm256_cmp_ps(avg8, threshold8Neg, _CMP_LT_OQ));
				
				// The assumption is that most of the values are actually not thresholded, hence, if
				// this is the case, we circumvent the whole loop at the cost of one extra comparison:
				if(flagConditions != 0)
				{
					//std::cout << "y=" << yTop << ", flagConditions = " << flagConditions << "\n";
					union
					{
						bool theChars[8];
						uint64_t theInt;
					} outputValues = { {
						(flagConditions&1)!=0,
						(flagConditions&2)!=0,
						(flagConditions&4)!=0,
						(flagConditions&8)!=0,
						(flagConditions&16)!=0,
						(flagConditions&32)!=0,
						(flagConditions&64)!=0,
						(flagConditions&128)!=0 
					} };

					for(size_t i=0;i<Length;++i)
					{
						uint64_t *outputPtr = reinterpret_cast<uint64_t*>(scratch->ValuePtr(x, yTop + i));
						*outputPtr |= outputValues.theInt;
					}
				}
				
				// ** Subtract the sample at the top **
				
				// get a ptr
				const bool *tRowPtr = mask->ValuePtr(x, yTop);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				conditionMask = _mm256_castsi256_ps(
					_mm256_cmpeq_epi32(_mm256_set_epi32(
						tRowPtr[7], tRowPtr[6], tRowPtr[5], tRowPtr[4],
						tRowPtr[3], tRowPtr[2], tRowPtr[1], tRowPtr[0]),
					zero8i));
				
				// Conditionally decrement counters
				count8 = _mm256_sub_epi32(count8, _mm256_and_si256(_mm256_castps_si256(conditionMask), ones8));
				
				// Subtract values with conditional move
				sum8 = _mm256_sub_ps(sum8,
					_mm256_or_ps(_mm256_and_ps(_mm256_load_ps(input->ValuePtr(x, yTop)), conditionMask),
					_mm256_andnot_ps(conditionMask, zero8)));
				
				// ** Next... **
				++yTop;
				++yBottom;
			}
		}
		
		// =============================================================================
		// Since images are aligned on 4 values and we are stepping with 8 values, there
		// might be 4 values left. These are done with the 'SSE' algorithm.
		// (this code is a literal copy of the above SSE algorithm)
		// =============================================================================
		if(x < width)
		{
			const __m128i zero4i = _mm_set1_epi32(0);
			const __m128i ones4 = _mm_set1_epi32(1);
			const __m128 zero4 = _mm_set1_ps(0.0);
			const __m128 threshold4Pos = _mm_set1_ps(threshold);
			const __m128 threshold4Neg = _mm_set1_ps(-threshold); 
			__m128 sum4 = _mm_set1_ps(0.0);
			__m128i count4 = _mm_set1_epi32(0);
			size_t yBottom;
			
			for(yBottom=0;yBottom+1<Length;++yBottom)
			{
				const bool *rowPtr = mask->ValuePtr(x, yBottom);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m128 conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(rowPtr[3], rowPtr[2], rowPtr[1], rowPtr[0]),
													zero4i));
				
				// Conditionally increment counters
				count4 = _mm_add_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Add values with conditional move
				__m128 m = _mm_and_ps(_mm_load_ps(input->ValuePtr(x, yBottom)), conditionMask);
				sum4 = _mm_add_ps(sum4, _mm_or_ps(m, _mm_andnot_ps(conditionMask, zero4)));
			}
			
			size_t yTop = 0;
			while(yBottom < height)
			{
				// ** Add the 4 sample at the bottom **
				
				// get a ptr
				const bool *rowPtr = mask->ValuePtr(x, yBottom);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				__m128 conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(rowPtr[3], rowPtr[2], rowPtr[1], rowPtr[0]),
													zero4i));
				
				// Conditionally increment counters
				count4 = _mm_add_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Add values with conditional move
				sum4 = _mm_add_ps(sum4,
					_mm_or_ps(_mm_and_ps(_mm_load_ps(input->ValuePtr(x, yBottom)), conditionMask),
										_mm_andnot_ps(conditionMask, zero4)));
				
				// ** Check sum **
				
				// if sum/count > threshold || sum/count < -threshold
				__m128 avg4 = _mm_div_ps(sum4, _mm_cvtepi32_ps(count4));
				const unsigned flagConditions =
					_mm_movemask_ps(_mm_cmpgt_ps(avg4, threshold4Pos)) |
					_mm_movemask_ps(_mm_cmplt_ps(avg4, threshold4Neg));
				// | _mm_movemask_ps(_mm_cmplt_ps(count4, zero4i));
				
				// The assumption is that most of the values are actually not thresholded, hence, if
				// this is the case, we circumvent the whole loop at the cost of one extra comparison:
				if(flagConditions != 0)
				{
					union
					{
						bool theChars[4];
						unsigned theInt;
					} outputValues = { {
						(flagConditions&1)!=0,
						(flagConditions&2)!=0,
						(flagConditions&4)!=0,
						(flagConditions&8)!=0 } };

					for(size_t i=0;i<Length;++i)
					{
						unsigned *outputPtr = reinterpret_cast<unsigned*>(scratch->ValuePtr(x, yTop + i));
						
						*outputPtr |= outputValues.theInt;
					}
				}
				
				// ** Subtract the sample at the top **
				
				// get a ptr
				const bool *tRowPtr = mask->ValuePtr(x, yTop);
				
				// Assign each integer to one bool in the mask
				// Convert true to 0xFFFFFFFF and false to 0
				conditionMask = _mm_castsi128_ps(
					_mm_cmpeq_epi32(_mm_set_epi32(tRowPtr[3], tRowPtr[2], tRowPtr[1], tRowPtr[0]),
													zero4i));
				
				// Conditionally decrement counters
				count4 = _mm_sub_epi32(count4, _mm_and_si128(_mm_castps_si128(conditionMask), ones4));
				
				// Subtract values with conditional move
				sum4 = _mm_sub_ps(sum4,
					_mm_or_ps(_mm_and_ps(_mm_load_ps(input->ValuePtr(x, yTop)), conditionMask),
										_mm_andnot_ps(conditionMask, zero4)));
				
				// ** Next... **
				++yTop;
				++yBottom;
			}
		}
	}
	std::swap(*mask, *scratch);
}

#else
#warning "AVX2 optimization of SumThreshold algorithm is not used"
#endif

template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<2>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<4>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<8>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<16>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<32>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<64>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<128>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
template
void CombinatorialThresholder::VerticalSumThresholdLargeAVX<256>(const Image2D* input, Mask2D* mask, Mask2D* scratch, num_t threshold);
