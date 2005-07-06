#include"Event/Measurement.h"

bool Measurement::checkType(unsigned int value) const
{
  unsigned int val = (unsigned int)value;  return 0 != ( m_mtype & typeMask & val );
}
