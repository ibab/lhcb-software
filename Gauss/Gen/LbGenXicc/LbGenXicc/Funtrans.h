// F. Zhang 01-04-11
#ifndef LBGENXICC_FUNTRANS_H
#define LBGENXICC_FUNTRANS_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall FUNTRANS_ADDRESS(void) ;
}
#else
extern "C" {
  void* funtrans_address__(void) ;
}
#endif

class Funtrans {
public:
  Funtrans();
  ~Funtrans();

  // F. Zhang 01-04-11
  // int& nq2();
  int& npdfu();

  inline void init(); // inlined for speed of access (small function)

private:
  struct FUNTRANS;
  friend struct FUNTRANS;
  
  struct FUNTRANS {
    // F. Zhang 01-04-11
    // int nq2;
    int npdfu;
  };
  int m_dummy;
  double m_realdummy;
  static FUNTRANS* s_funtrans;
};

// Inline implementations for Funtrans
// initialise pointer
#ifdef WIN32
void Funtrans::init(void) {
  if ( 0 == s_funtrans ) s_funtrans = static_cast<FUNTRANS*>(FUNTRANS_ADDRESS());
}
#else
void Funtrans::init(void) {
  if ( 0 == s_funtrans ) s_funtrans = static_cast<FUNTRANS*>(funtrans_address__());
}
#endif
#endif // LBGENXICC_FUNTRANS_H
 
