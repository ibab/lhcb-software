// $Id: Coloct.h,v 1.2 2006-05-03 08:24:31 robbep Exp $

#ifndef LBPYTHIA_COLOCT_H
#define LBPYTHIA_COLOCT_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall COLOCT_ADDRESS(void) ;
}
#else
extern "C" {
  void* coloct_address__(void) ;
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
  if ( 0 == s_coloct ) s_coloct = static_cast<COLOCT*>(coloct_address__());
}
#endif
#endif // LBPYTHIA_COLOCT_H
 
