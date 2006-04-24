// $Id: Vegasinf.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_VEGASINF_H
#define LBPYTHIA_VEGASINF_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall VEGASINF_ADDRESS(void) ;
}
#else
extern "C" {
  void* vegasinf_address_(void) ;
}
#endif

class Vegasinf {
public:
  Vegasinf();
  ~Vegasinf();

  int& number();
  int& nitmx();

  inline void init(); // inlined for speed of access (small function)

private:
  struct VEGASINF;
  friend struct VEGASINF;
  
  struct VEGASINF {
    int number;
    int nitmx;
  };
  int m_dummy;
  double m_realdummy;
  static VEGASINF* s_vegasinf;
};

// Inline implementations for Vegasinf
// initialise pointer
#ifdef WIN32
void Vegasinf::init(void) {
  if ( 0 == s_vegasinf ) s_vegasinf = static_cast<VEGASINF*>(VEGASINF_ADDRESS());
}
#else
void Vegasinf::init(void) {
  if ( 0 == s_vegasinf ) s_vegasinf = static_cast<VEGASINF*>(vegasinf_address_());
}
#endif
#endif // LBPYTHIA_VEGASINF_H
 
