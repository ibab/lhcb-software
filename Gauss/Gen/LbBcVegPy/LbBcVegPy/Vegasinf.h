// $Id: Vegasinf.h,v 1.2 2006-05-03 08:24:32 robbep Exp $

#ifndef LBBCVEGPY_VEGASINF_H
#define LBBCVEGPY_VEGASINF_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall VEGASINF_ADDRESS(void) ;
}
#else
extern "C" {
  void* vegasinf_address__(void) ;
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
  if ( 0 == s_vegasinf ) s_vegasinf = static_cast<VEGASINF*>(vegasinf_address__());
}
#endif
#endif // LBBCVEGPY_VEGASINF_H
 
