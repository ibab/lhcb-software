#ifndef CCPCCSYS_H
#define CCPCCSYS_H
#include "CCPCSubSys.h"

class CCPCCSys : public CCPCSubSys
{
//friend class CCPCHisto;
protected:
  CCPCCSys();
  ~CCPCCSys();
public:
  void *Allocate(int);
  static CCPCCSys& m_instance();
  void setup(char *n);
};

#endif
