// $Id: Wavezero.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_WAVEZERO_H
#define LBPYTHIA_WAVEZERO_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall WAVEZERO_ADDRESS(void) ;
}
#else
extern "C" {
  void* wavezero_address_(void) ;
}
#endif

class Wavezero {
public:
  Wavezero();
  ~Wavezero();

  double& fbc();

  inline void init(); // inlined for speed of access (small function)


private:
  struct WAVEZERO;
  friend struct WAVEZERO;
  
  struct WAVEZERO {
    double fbc;
  };
  int m_dummy;
  double m_realdummy;
  static WAVEZERO* s_wavezero;
};

// Inline implementations for Wavezero
// initialise pointer
#ifdef WIN32
void Wavezero::init(void) {
  if ( 0 == s_wavezero ) s_wavezero = static_cast<WAVEZERO*>(WAVEZERO_ADDRESS());
}
#else
void Wavezero::init(void) {
  if ( 0 == s_wavezero ) s_wavezero = static_cast<WAVEZERO*>(wavezero_address_());
}
#endif
#endif // LBPYTHIA_WAVEZERO_H
 
