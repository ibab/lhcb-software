// $Id: Qqbar.h,v 1.2 2006-05-03 08:24:32 robbep Exp $

#ifndef LBBCVEGPY_QQBAR_H
#define LBBCVEGPY_QQBAR_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall QQBAR_ADDRESS(void) ;
}
#else
extern "C" {
  void* qqbar_address__(void) ;
}
#endif

class Qqbar {
public:
  Qqbar();
  ~Qqbar();

  int& iqqbar();
  int& iqcode();

  inline void init(); // inlined for speed of access (small function)

private:
  struct QQBAR;
  friend struct QQBAR;
  
  struct QQBAR {
    int iqqbar;
    int iqcode;
  };
  int m_dummy;
  double m_realdummy;
  static QQBAR* s_qqbar;
};

// Inline implementations for Qqbar
// initialise pointer
#ifdef WIN32
void Qqbar::init(void) {
  if ( 0 == s_qqbar ) s_qqbar = static_cast<QQBAR*>(QQBAR_ADDRESS());
}
#else
void Qqbar::init(void) {
  if ( 0 == s_qqbar ) s_qqbar = static_cast<QQBAR*>(qqbar_address__());
}
#endif
#endif // LBBCVEGPY_QQBAR_H
 
