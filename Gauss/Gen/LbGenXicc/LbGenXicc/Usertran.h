// F. Zhang 01-04-11
#ifndef LBGENXICC_USERTRAN_H
#define LBGENXICC_USERTRAN_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall USERTRAN_ADDRESS(void) ;
}
#else
extern "C" {
  void* usertran_address__(void) ;
}
#endif

class Usertran {
public:
  Usertran();
  ~Usertran();

  //int& ishower(); F. Zhang 01-04-11
  int& idpp();

  inline void init(); // inlined for speed of access (small function)

private:
  struct USERTRAN;
  friend struct USERTRAN;
  
  struct USERTRAN {
    //int ishower; F. Zhang 01-04-11
    int idpp;
  };
  int m_dummy;
  double m_realdummy;
  static USERTRAN* s_usertran;
};

// Inline implementations for Usertran
// initialise pointer
#ifdef WIN32
void Usertran::init(void) {
  if ( 0 == s_usertran ) s_usertran = static_cast<USERTRAN*>(USERTRAN_ADDRESS());
}
#else
void Usertran::init(void) {
  if ( 0 == s_usertran ) s_usertran = static_cast<USERTRAN*>(usertran_address__());
}
#endif
#endif // LBGENXICC_USERTRAN_H
 
