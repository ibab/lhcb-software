// $Id: Mixevnt.h,v 1.2 2006-05-03 08:24:32 robbep Exp $

#ifndef LBBCVEGPY_MIXEVNT_H
#define LBBCVEGPY_MIXEVNT_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall MIXEVNT_ADDRESS(void) ;
}
#else
extern "C" {
  void* mixevnt_address__(void) ;
}
#endif

// Lengths of arrays in MIXEVNT
static const int s_lenXbcsec = 8;

class Mixevnt {
public:
  Mixevnt();
  ~Mixevnt();
  double& xbcsec(int n);
  int& imix();
  int& imixtype();

  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenXbcsec() const {return s_lenXbcsec;}

private:
  struct MIXEVNT;
  friend struct MIXEVNT;
  
  struct MIXEVNT {
    double xbcsec[s_lenXbcsec];
    int imix;
    int imixtype;
  };
  int m_dummy;
  double m_realdummy;
  static MIXEVNT* s_mixevnt;
};

// Inline implementations for Mixevnt
// initialise pointer
#ifdef WIN32
void Mixevnt::init(void) {
  if ( 0 == s_mixevnt ) s_mixevnt = static_cast<MIXEVNT*>(MIXEVNT_ADDRESS());
}
#else
void Mixevnt::init(void) {
  if ( 0 == s_mixevnt ) s_mixevnt = static_cast<MIXEVNT*>(mixevnt_address__());
}
#endif
#endif // LBBCVEGPY_MIXEVNT_H
 
