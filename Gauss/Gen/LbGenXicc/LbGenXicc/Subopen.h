// F. Zhang 03-04-11
#ifndef LBGENXICC_SUBOPEN_H
#define LBGENXICC_SUBOPEN_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall SUBOPEN_ADDRESS(void) ;
}
#else
extern "C" {
  void* subopen_address__(void) ;
}
#endif

class Subopen {
public:
  Subopen();
  ~Subopen();

  double& subfactor();
  double& subenergy();
  int& isubonly();
  int& ichange();
  int& iconsbarnum();

  inline void init(); // inlined for speed of access (small function)

private:
  struct SUBOPEN;
  friend struct SUBOPEN;
  
  struct SUBOPEN {
    double subfactor;
    double subenergy;
    int isubonly; 
    int ichange; 
    int iconsbarnum;
  };
  int m_dummy;
  double m_realdummy;
  static SUBOPEN* s_subopen;
};

// Inline implementations for Subopen
// initialise pointer
#ifdef WIN32
void Subopen::init(void) {
  if ( 0 == s_subopen ) s_subopen = static_cast<SUBOPEN*>(SUBOPEN_ADDRESS());
}
#else
void Subopen::init(void) {
  if ( 0 == s_subopen ) s_subopen = static_cast<SUBOPEN*>(subopen_address__());
}
#endif
#endif // LBGENXICC_SUBOPEN_H
 
