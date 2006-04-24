// $Id: Rconst.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBBCVEGPY_RCONST_H
#define LBBCVEGPY_RCONST_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall RCONST_ADDRESS(void) ;
}
#else
extern "C" {
  void* rconst_address_(void) ;
}
#endif

class Rconst {
public:
  Rconst();
  ~Rconst();
  double& pi();
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
private:
  struct RCONST;
  friend struct RCONST;
  
  struct RCONST {
    double pi;
  };
  int m_dummy;
  double m_realdummy;
  static RCONST* s_rconst;
};

// Inline implementations for Rconst 
// initialise pointer
#ifdef WIN32
void Rconst::init(void) {
  if ( 0 == s_rconst ) s_rconst = static_cast<RCONST*>(RCONST_ADDRESS());
}
#else
void Rconst::init(void) {
  if ( 0 == s_rconst ) s_rconst = static_cast<RCONST*>(rconst_address_());
}
#endif
#endif // LBBCVEGPY_RCONST_H
 
