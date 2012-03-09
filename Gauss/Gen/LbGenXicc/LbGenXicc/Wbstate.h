// F. Zhang 03-04-11
#ifndef LBGENXICC_WBSTATE_H
#define LBGENXICC_WBSTATE_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall WBSTATE_ADDRESS(void) ;
}
#else
extern "C" {
  void* wbstate_address__(void) ;
}
#endif

class Wbstate {
public:
  Wbstate();
  ~Wbstate();

  double& ratiou();
  double& ratiod();
  double& ratios();
  int& nbound();

  inline void init(); // inlined for speed of access (small function)

private:
  struct WBSTATE;
  friend struct WBSTATE;
  
  struct WBSTATE {
    double ratiou;
    double ratiod;
    double ratios;
    int nbound; 
  };
  int m_dummy;
  double m_realdummy;
  static WBSTATE* s_wbstate;
};

// Inline implementations for Wbstate
// initialise pointer
#ifdef WIN32
void Wbstate::init(void) {
  if ( 0 == s_wbstate ) s_wbstate = static_cast<WBSTATE*>(WBSTATE_ADDRESS());
}
#else
void Wbstate::init(void) {
  if ( 0 == s_wbstate ) s_wbstate = static_cast<WBSTATE*>(wbstate_address__());
}
#endif
#endif // LBGENXICC_WBSTATE_H
 
