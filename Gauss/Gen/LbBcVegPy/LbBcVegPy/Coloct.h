// $Id: Coloct.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_COLOCT_H
#define LBPYTHIA_COLOCT_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall COLOCT_ADDRESS(void) ;
}
#else
extern "C" {
  void* coloct_address_(void) ;
}
#endif

class Coloct {
public:
  Coloct();
  ~Coloct();

  int& ioctet();

  inline void init(); // inlined for speed of access (small function)


private:
  struct COLOCT;
  friend struct COLOCT;
  
  struct COLOCT {
    int ioctet;
  };
  int m_dummy;
  double m_realdummy;
  static COLOCT* s_coloct;
};

// Inline implementations for Coloct
// initialise pointer
#ifdef WIN32
void Coloct::init(void) {
  if ( 0 == s_coloct ) s_coloct = static_cast<COLOCT*>(COLOCT_ADDRESS());
}
#else
void Coloct::init(void) {
  if ( 0 == s_coloct ) s_coloct = static_cast<COLOCT*>(coloct_address_());
}
#endif
#endif // LBPYTHIA_COLOCT_H
 
