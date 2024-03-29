#ifndef AOFLAGGER_STATISTICALFLAGGERTEST_H
#define AOFLAGGER_STATISTICALFLAGGERTEST_H

#include "../../testingtools/asserter.h"
#include "../../testingtools/unittest.h"

#include "../../../structures/mask2d.h"

#include "../../../strategy/algorithms/morphologicalflagger.h"

class StatisticalFlaggerTest : public UnitTest {
	public:
		StatisticalFlaggerTest() : UnitTest("Statistical flagger")
		{
			AddTest(TestTimeDilation(), "Time dilation");
			AddTest(TestFrequencyDilation(), "Frequency dilation");
			AddTest(TestTimeDilationSpeed(), "Time dilation speed");
		}
		
	private:
		struct TestTimeDilation : public Asserter
		{
			void operator()();
		};
		struct TestFrequencyDilation : public Asserter
		{
			void operator()();
		};
		struct TestTimeDilationSpeed : public Asserter
		{
			void operator()();
		};
		
		static std::string maskToString(Mask2DCPtr mask)
		{
			std::stringstream s;
			for(unsigned y=0;y<mask->Height();++y)
			{
				for(unsigned x=0;x<mask->Width();++x)
				{
					s << (mask->Value(x, y) ? 'x' : ' ');
				}
			}
			return s.str();
		}
		
		static void setMask(Mask2DPtr mask, const std::string &str)
		{
			std::string::const_iterator i = str.begin();
			for(unsigned y=0;y<mask->Height();++y)
			{
				for(unsigned x=0;x<mask->Width();++x)
				{
					mask->SetValue(x, y, (*i) == 'x');
					++i;
				}
			}
		}
};

inline void StatisticalFlaggerTest::TestTimeDilation::operator()()
{
	Mask2DPtr mask = Mask2D::CreateSetMaskPtr<false>(10, 1);
	setMask(mask, "     x    ");
	
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.0);
	AssertEquals(maskToString(mask), "     x    ", "Min=0.0, single center flagged, no enlarge");
	
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.5);
	AssertEquals(maskToString(mask), "     x    ", "Min=0.5, single center flagged, no enlarge");
	
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.6);
	AssertEquals(maskToString(mask), "    xx    ", "Min=0.6, from one to two samples");

	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.0);
	AssertEquals(maskToString(mask), "    xx    ");
	
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.5);
	AssertEquals(maskToString(mask), "   xxxx   ");

	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.6);
	AssertEquals(maskToString(mask), "xxxxxxxxxx");

	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 1.0);
	AssertEquals(maskToString(mask), "xxxxxxxxxx");
	
	setMask(mask, "xx xx     ");

	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.0);
	AssertEquals(maskToString(mask), "xx xx     ");

	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.5);
	AssertTrue(mask->Value(2,0), "Fill hole");
	
	mask = Mask2D::CreateSetMaskPtr<false>(40, 1);
	//             0    5    0    5    0    5    0    5    
	setMask(mask, "     xxxxxx xx xx x x xxx xxxxx         ");
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.2);
	AssertEquals(maskToString(mask), "    xxxxxxxxxxxxx x xxxxxxxxxxxx        ");

	setMask(mask, "     xxxxxx xx xx x x xxx xxxxx         ");
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.3);
	AssertEquals(maskToString(mask), "   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx       ");

	setMask(mask, "     xxxxxx xx xx x x xxx xxxxx         ");
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.5);
	AssertEquals(maskToString(mask), "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  ");

	setMask(mask, "xxxxxxxxxxxxxxx       xxxxxxxxxxxxxxxxxx");
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.3);
	AssertEquals(maskToString(mask), "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
}

inline void StatisticalFlaggerTest::TestFrequencyDilation::operator()()
{
	Mask2DPtr mask = Mask2D::CreateSetMaskPtr<false>(1, 10);
	setMask(mask, "     x    ");
	
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.0);
	AssertEquals(maskToString(mask), "     x    ", "Min=0.0, single center flagged, no enlarge");
	
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.5);
	AssertEquals(maskToString(mask), "     x    ", "Min=0.5, single center flagged, no enlarge");
	
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.6);
	AssertEquals(maskToString(mask), "    xx    ", "Min=0.6, from one to two samples");

	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.0);
	AssertEquals(maskToString(mask), "    xx    ");
	
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.5);
	AssertEquals(maskToString(mask), "   xxxx   ");

	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.6);
	AssertEquals(maskToString(mask), "xxxxxxxxxx");

	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 1.0);
	AssertEquals(maskToString(mask), "xxxxxxxxxx");
	
	setMask(mask, "xx xx     ");

	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.0);
	AssertEquals(maskToString(mask), "xx xx     ");

	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.5);
	AssertTrue(mask->Value(0,2), "Fill hole");
	
	mask = Mask2D::CreateSetMaskPtr<false>(1, 40);
	//             0    5    0    5    0    5    0    5    
	setMask(mask, "     xxxxxx xx xx x x xxx xxxxx         ");
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.2);
	AssertEquals(maskToString(mask), "    xxxxxxxxxxxxx x xxxxxxxxxxxx        ");

	setMask(mask, "     xxxxxx xx xx x x xxx xxxxx         ");
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.3);
	AssertEquals(maskToString(mask), "   xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx       ");

	setMask(mask, "     xxxxxx xx xx x x xxx xxxxx         ");
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.5);
	AssertEquals(maskToString(mask), "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  ");

	setMask(mask, "xxxxxxxxxxxxxxx       xxxxxxxxxxxxxxxxxx");
	MorphologicalFlagger::DensityFrequencyFlagger(mask.get(), 0.3);
	AssertEquals(maskToString(mask), "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
}

inline void StatisticalFlaggerTest::TestTimeDilationSpeed::operator()()
{
	const unsigned flagsSize = 10000;
	const unsigned channels = 256;
	Mask2DPtr mask = Mask2D::CreateSetMaskPtr<false>(flagsSize, channels);
	for(unsigned y=0;y<channels;++y)
	{
		for(unsigned i=0;i<flagsSize; ++i)
		{
			mask->SetValue(i, 0, (RNG::Uniform() >= 0.2));
		}
	}
	MorphologicalFlagger::DensityTimeFlagger(mask.get(), 0.1);
}

#endif
