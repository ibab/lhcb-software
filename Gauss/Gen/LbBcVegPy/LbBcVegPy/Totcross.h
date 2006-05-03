// $Id: Totcross.h,v 1.2 2006-05-03 08:24:32 robbep Exp $

#ifndef LBBCVEGPY_TOTCROSS_H
#define LBBCVEGPY_TOTCROSS_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall TOTCROSS_ADDRESS(void) ;
}
#else
extern "C" {
  void* totcross_address__(void) ;
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
  if ( 0 == s_totcross ) s_totcross = static_cast<TOTCROSS*>(totcross_address__());
}
#endif
#endif // LBBCVEGPY_TOTCROSS_H
 
