// F. Zhang 01-04-11
#include <complex>
#ifndef LBGENXICC_UPCOM_H
#define LBGENXICC_UPCOM_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall UPCOM_ADDRESS(void) ;
}
#else
extern "C" {
  void* upcom_address__(void) ;
}
#endif

// Lengths of arrays in UPCOM
static const int s_lenPmomup = 5;
static const int s_depthPmomup = 8;
static const int s_lenColmat = 6;  // F. Zhang 01-04-11
static const int s_depthColmat = 64;
static const int s_lenBundamp = 4;
static const int s_lenPmomzero = 5;
static const int s_depthPmomzero = 8;



class Upcom {
public:
  Upcom();
  ~Upcom();
  double& ecm();
  double& pmxicc();
  double& pmb();
  double& pmc();
  double& fxicc();
  double& pmomup(int i, int j);
  std::complex<double>& colmat(int i, int j);
  std::complex<double>& bundamp(int n);
  double& pmomzero(int i, int j);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenPmomup() const {return s_lenPmomup;}
  int depthPmomup() const {return s_depthPmomup;}
  int lenColmat() const {return s_lenColmat;}
  int depthColmat() const {return s_depthColmat;}
  int lenBundamp() const {return s_lenBundamp;}
  int lenPmomzero() const {return s_lenPmomzero;}
  int depthPmomzero() const {return s_depthPmomzero;}

private:
  struct UPCOM;
  friend struct UPCOM;
  
  struct UPCOM {
    double ecm;
    double pmxicc;
    double pmb;
    double pmc;
    double fxicc;
    double pmomup[s_lenPmomup][s_depthPmomup];
    std::complex<double> colmat[s_lenColmat][s_depthColmat];
    std::complex<double> bundamp[s_lenBundamp];
    double pmomzero[s_lenPmomzero][s_depthPmomzero];
  };
  int m_dummy;
  double m_realdummy;
  std::complex<double> m_complexdummy;
  static UPCOM* s_upcom;
};

// Inline implementations for Upcom
// initialise pointer
#ifdef WIN32
void Upcom::init(void) {
  if ( 0 == s_upcom ) s_upcom = static_cast<UPCOM*>(UPCOM_ADDRESS());
}
#else
void Upcom::init(void) {
  if ( 0 == s_upcom ) s_upcom = static_cast<UPCOM*>(upcom_address__());
}
#endif
#endif // LBGENXICC_UPCOM_H //F. Zhang 01-04-11
 
