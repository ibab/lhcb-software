// F. Zhang 06-04-11
#ifndef LBGENXICC_MTYPEOFXI_H
#define LBGENXICC_MTYPEOFXI_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall MTYPEOFXI_ADDRESS(void) ;
}
#else
extern "C" {
  void* mtypeofxi_address__(void) ;
}
#endif

class Mtypeofxi {
public:
  Mtypeofxi();
  ~Mtypeofxi();
  //F. Zhang 06-04-11 double& pi();
  int& mgenxi();
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
private:
  struct MTYPEOFXI;
  friend struct MTYPEOFXI;
  
  struct MTYPEOFXI {
    // F. Zhang 06-04-11 double pi;
    int mgenxi;
  };
  int m_dummy;
  double m_realdummy;
  static MTYPEOFXI* s_mtypeofxi;
};

// Inline implementations for Mtypeofxi 
// initialise pointer
#ifdef WIN32
void Mtypeofxi::init(void) {
  if ( 0 == s_mtypeofxi ) s_mtypeofxi = static_cast<MTYPEOFXI*>(MTYPEOFXI_ADDRESS());
}
#else
void Mtypeofxi::init(void) {
  if ( 0 == s_mtypeofxi ) s_mtypeofxi = static_cast<MTYPEOFXI*>(mtypeofxi_address__());
}
#endif
#endif // LBGENXICC_MTYPEOFXI_H
 
