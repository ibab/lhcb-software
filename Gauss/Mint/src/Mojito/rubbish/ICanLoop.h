#ifndef ICANLOOP_HH
#define ICANLOOP_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:12 GMT

class ICanLoop{
 protected:
  ICanLoop(){}
 public:
  virtual bool Next()   =0;
  virtual bool Start()  =0;

  virtual ~ICanLoop(){}
};

#endif
//
