// F. Zhang 06-04-11
#ifndef LBGENXICC_VEGASBIN_H
#define LBGENXICC_VEGASBIN_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall VEGASBIN_ADDRESS(void) ;
}
#else
extern "C" {
  void* vegasbin_address__(void) ;
}
#endif

class Vegasbin {
public:
  Vegasbin();
  ~Vegasbin();
  //F. Zhang 06-04-11 double& pi();
  int& nvbin();
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
private:
  struct VEGASBIN;
  friend struct VEGASBIN;
  
  struct VEGASBIN {
    // F. Zhang 06-04-11 double pi;
    int nvbin;
  };
  int m_dummy;
  double m_realdummy;
  static VEGASBIN* s_vegasbin;
};

// Inline implementations for Vegasbin 
// initialise pointer
#ifdef WIN32
void Vegasbin::init(void) {
  if ( 0 == s_vegasbin ) s_vegasbin = static_cast<VEGASBIN*>(VEGASBIN_ADDRESS());
}
#else
void Vegasbin::init(void) {
  if ( 0 == s_vegasbin ) s_vegasbin = static_cast<VEGASBIN*>(vegasbin_address__());
}
#endif
#endif // LBGENXICC_VEGASBIN_H
 
