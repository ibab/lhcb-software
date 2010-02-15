#ifndef CCPCHSYS
#define CCPCHSYS
#include "CCPCSubSys.h"

class CCPCHSys : public CCPCSubSys
{
friend class CCPCHisto;
protected:
  CCPCHSys();
  ~CCPCHSys();
public:
  void *Allocate(int);
  static CCPCHSys& m_instance();
  void setup(char *n);
};

#endif
