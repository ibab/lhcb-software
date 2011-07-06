#ifndef ILOOPABLE_HH
#define ILOOPABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:54 GMT

namespace MINT{

class ILoopable{
 protected:
  ILoopable(){}
 public:
  virtual bool Next()   =0;
  virtual bool Start()  =0;

  virtual ~ILoopable(){}
};
}//namespace MINT

#endif
//
