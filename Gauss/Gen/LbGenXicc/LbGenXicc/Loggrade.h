// F. Zhang 03-04-11
#ifndef LBGENXICC_LOGGRADE_H
#define LBGENXICC_LOGGRADE_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall LOGGRADE_ADDRESS(void) ;
}
#else
extern "C" {
  void* loggrade_address__(void) ;
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
  int& iusecurdir(); 

  inline void init(); // inlined for speed of access (small function)

private:
  struct LOGGRADE;
  friend struct LOGGRADE;
  
  struct LOGGRADE {
    int ievntdis;
    int igenerate;
    int ivegasopen;
    int igrade;
    int iusecurdir; 
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
  if ( 0 == s_loggrade ) s_loggrade = static_cast<LOGGRADE*>(loggrade_address__());
}
#endif
#endif // LBGENXICC_LOGGRADE_H
 
