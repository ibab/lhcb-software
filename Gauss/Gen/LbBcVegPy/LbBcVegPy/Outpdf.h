// $Id: Outpdf.h,v 1.1.1.1 2006-04-24 21:45:50 robbep Exp $

#ifndef LBBCVEGPY_OUTPDF_H
#define LBBCVEGPY_OUTPDF_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall OUTPDF_ADDRESS(void) ;
}
#else
extern "C" {
  void* outpdf_address_(void) ;
}
#endif

class Outpdf {
public:
  Outpdf();
  ~Outpdf();

  int& ioutpdf();
  int& ipdfnum();

  inline void init(); // inlined for speed of access (small function)

private:
  struct OUTPDF;
  friend struct OUTPDF;
  
  struct OUTPDF {
    int ioutpdf;
    int ipdfnum;
  };
  int m_dummy;
  double m_realdummy;
  static OUTPDF* s_outpdf;
};

// Inline implementations for Outpdf
// initialise pointer
#ifdef WIN32
void Outpdf::init(void) {
  if ( 0 == s_outpdf ) s_outpdf = static_cast<OUTPDF*>(OUTPDF_ADDRESS());
}
#else
void Outpdf::init(void) {
  if ( 0 == s_outpdf ) s_outpdf = static_cast<OUTPDF*>(outpdf_address_());
}
#endif
#endif // LBBCVEGPY_OUTPDF_H
 
