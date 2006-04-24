// $Id: Qqbar.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_QQBAR_H
#define LBPYTHIA_QQBAR_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall QQBAR_ADDRESS(void) ;
}
#else
extern "C" {
  void* qqbar_address_(void) ;
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
  if ( 0 == s_qqbar ) s_qqbar = static_cast<QQBAR*>(qqbar_address_());
}
#endif
#endif // LBPYTHIA_QQBAR_H
 
