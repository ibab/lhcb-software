
#include "RichDet/RichXLocalPosition.h"

RichXLocalPosition::RichXLocalPosition (Rich::RichID rich, 
                                        const int side, 
                                        const double x, 
                                        const double y)
  : m_rich(rich), m_side(side), m_x(x), m_y(y)
{
}


RichXLocalPosition::~RichXLocalPosition() {}

