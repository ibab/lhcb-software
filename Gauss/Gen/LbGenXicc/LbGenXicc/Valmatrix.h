//F. Zhang 06-04-11
#ifndef LBGENXICC_VALMATRIX_H
#define LBGENXICC_VALMATRIX_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall VALMATRIX_ADDRESS(void) ;
}
#else
extern "C" {
  void* valmatrix_address__(void) ;
}
#endif

class Valmatrix {
public:
  Valmatrix();
  ~Valmatrix();
  double& cmfactor();
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
private:
  struct VALMATRIX;
  friend struct VALMATRIX;
  
  struct VALMATRIX {
    double cmfactor;
  };
  int m_dummy;
  double m_realdummy;
  static VALMATRIX* s_valmatrix;
};

// Inline implementations for Valmatrix 
// initialise pointer
#ifdef WIN32
void Valmatrix::init(void) {
  if ( 0 == s_valmatrix ) s_valmatrix = static_cast<VALMATRIX*>(VALMATRIX_ADDRESS());
}
#else
void Valmatrix::init(void) {
  if ( 0 == s_valmatrix ) s_valmatrix = static_cast<VALMATRIX*>(valmatrix_address__());
}
#endif
#endif // LBGENXICC_VALMATRIX_H
 
