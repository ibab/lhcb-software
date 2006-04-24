// $Id: Loggrade.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_LOGGRADE_H
#define LBPYTHIA_LOGGRADE_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall LOGGRADE_ADDRESS(void) ;
}
#else
extern "C" {
  void* loggrade_address_(void) ;
}
#endif

class Loggrade {
public:
  Loggrade();
  ~Loggrade();

  int& ievntdis();
  int& igenerate();
  int& ivegasopen();
  int& igrade();

  inline void init(); // inlined for speed of access (small function)

private:
  struct LOGGRADE;
  friend struct LOGGRADE;
  
  struct LOGGRADE {
    int ievntdis;
    int igenerate;
    int ivegasopen;
    int igrade;
  };
  int m_dummy;
  double m_realdummy;
  static LOGGRADE* s_loggrade;
};

// Inline implementations for Loggrade
// initialise pointer
#ifdef WIN32
void Loggrade::init(void) {
  if ( 0 == s_loggrade ) s_loggrade = static_cast<LOGGRADE*>(LOGGRADE_ADDRESS());
}
#else
void Loggrade::init(void) {
  if ( 0 == s_loggrade ) s_loggrade = static_cast<LOGGRADE*>(loggrade_address_());
}
#endif
#endif // LBPYTHIA_LOGGRADE_H
 
