// $Id: Totcross.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_TOTCROSS_H
#define LBPYTHIA_TOTCROSS_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall TOTCROSS_ADDRESS(void) ;
}
#else
extern "C" {
  void* totcross_address_(void) ;
}
#endif

class Totcross {
public:
  Totcross();
  ~Totcross();

  double& appcross();

  inline void init(); // inlined for speed of access (small function)


private:
  struct TOTCROSS;
  friend struct TOTCROSS;
  
  struct TOTCROSS {
    double appcross;
  };
  int m_dummy;
  double m_realdummy;
  static TOTCROSS* s_totcross;
};

// Inline implementations for Totcross
// initialise pointer
#ifdef WIN32
void Totcross::init(void) {
  if ( 0 == s_totcross ) s_totcross = static_cast<TOTCROSS*>(TOTCROSS_ADDRESS());
}
#else
void Totcross::init(void) {
  if ( 0 == s_totcross ) s_totcross = static_cast<TOTCROSS*>(totcross_address_());
}
#endif
#endif // LBPYTHIA_TOTCROSS_H
 
