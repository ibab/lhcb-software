// $Id: Usertran.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBPYTHIA_USERTRAN_H
#define LBPYTHIA_USERTRAN_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall USERTRAN_ADDRESS(void) ;
}
#else
extern "C" {
  void* usertran_address_(void) ;
}
#endif

class Usertran {
public:
  Usertran();
  ~Usertran();

  int& ishower();
  int& idpp();

  inline void init(); // inlined for speed of access (small function)

private:
  struct USERTRAN;
  friend struct USERTRAN;
  
  struct USERTRAN {
    int ishower;
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
  if ( 0 == s_usertran ) s_usertran = static_cast<USERTRAN*>(usertran_address_());
}
#endif
#endif // LBPYTHIA_USERTRAN_H
 
