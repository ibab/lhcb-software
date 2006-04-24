// $Id: Vegcross.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBBCVEGPY_VEGCROSS_H
#define LBBCVEGPY_VEGCROSS_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall VEGCROSS_ADDRESS(void) ;
}
#else
extern "C" {
  void* vegcross_address_(void) ;
}
#endif

class Vegcross {
public:
  Vegcross();
  ~Vegcross();
  double& vegsec();
  double& vegerr();
  int& iveggrade();
  inline void init(); // inlined for speed of access (small function)

private:
  struct VEGCROSS;
  friend struct VEGCROSS;
  
  struct VEGCROSS {
    double vegsec;
    double vegerr;
    int    iveggrade;
  };
  int m_dummy;
  double m_realdummy;
  static VEGCROSS* s_vegcross;
};

// Inline implementations for Vegcross
// initialise pointer
#ifdef WIN32
void Vegcross::init(void) {
  if ( 0 == s_vegcross ) s_vegcross = static_cast<VEGCROSS*>(VEGCROSS_ADDRESS());
}
#else
void Vegcross::init(void) {
  if ( 0 == s_vegcross ) s_vegcross = static_cast<VEGCROSS*>(vegcross_address_());
}
#endif
#endif // LBBCVEGPY_VEGCROSS_H
 
