#ifndef IINTEGRABLE_HH
#define IINTEGRABLE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:00 GMT

class IIntegrable{
 public:
  virtual IDalitzIntegrator* getIntegrator();
};

#endif
//
