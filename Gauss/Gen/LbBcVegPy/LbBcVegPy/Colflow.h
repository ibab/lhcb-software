// $Id: Colflow.h,v 1.2 2006-05-03 08:24:31 robbep Exp $

#ifndef LBBCVEGPY_COLFLOW_H
#define LBBCVEGPY_COLFLOW_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall COLFLOW_ADDRESS(void) ;
}
#else
extern "C" {
  void* colflow_address__(void) ;
}
#endif

// Lengths of AMP2CF
static const int s_lenAmp2cf = 10;

class Colflow {
public:
  Colflow();
  ~Colflow();
  double& amp2cf(int n);
  double& smatval();
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenAmp2cf() const {return s_lenAmp2cf;}
private:
  struct COLFLOW;
  friend struct COLFLOW;
  
  struct COLFLOW {
    double amp2cf[s_lenAmp2cf];
    double smatval;
  };
  int m_dummy;
  double m_realdummy;
  static COLFLOW* s_colflow;
};

// Inline implementations for Colflow
// initialise pointer
#ifdef WIN32
void Colflow::init(void) {
  if ( 0 == s_colflow ) s_colflow = static_cast<COLFLOW*>(COLFLOW_ADDRESS());
}
#else
void Colflow::init(void) {
  if ( 0 == s_colflow ) s_colflow = static_cast<COLFLOW*>(colflow_address__());
}
#endif
#endif // LBBCVEGPY_COLFLOW_H
 
