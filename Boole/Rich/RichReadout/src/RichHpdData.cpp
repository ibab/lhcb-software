
#include "RichReadout/RichHpdData.h"
#include "RichReadout/Pixel.h"


RichHpdData::RichHpdData(const Pixel* id, int isize, int value)
  :mypixel(*id)
{
  sz = isize ;
  for (unsigned int i=0 ; i<sz ; i++) DataFrame[i] = value ;
}

RichHpdData::RichHpdData(const Pixel* id, const std::vector<int>& df)
  :mypixel(*id)
{
  sz = df.size() ;
  for (unsigned int i=0 ; i<sz ; i++) DataFrame[i] = df[i] ;
}
