// $Id: Hepeup.h,v 1.2 2006-05-03 08:24:31 robbep Exp $

#ifndef LBBCVEGPY_HEPEUP_H
#define LBBCVEGPY_HEPEUP_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall HEPEUP_ADDRESS(void) ;
}
#else
extern "C" {
  void* hepeup_address__(void) ;
}
#endif

// Lengths of MSTU and PARU COMMONS
static const int s_lenIdup = 500;
static const int s_lenIstup= 500;
static const int s_lenMothup = 2;
static const int s_depthMothup = 500;
static const int s_lenIcolup = 2;
static const int s_depthIcolup = 500;
static const int s_lenPup = 5;
static const int s_depthPup = 500;
static const int s_lenVtimup = 500;
static const int s_lenSpinup = 500;

class Hepeup {
public:
  Hepeup();
  ~Hepeup();

  int& nup();
  int& idprup();
  double& xwgtup();
  double& scalup();
  double& aqedup();
  double& aqcdup();
  int& idup(int n);
  int& istup(int n);
  int& mothup(int i, int j);
  int& icolup(int i, int j);
  double& pup(int i, int j);
  double& vtimup(int n);
  double& spinup(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenIdup() const {return s_lenIdup;}
  int lenIstup() const {return s_lenIstup;}
  int lenMothup() const {return s_lenMothup;}
  int depthMothup() const {return s_depthMothup;}
  int lenIcolup() const {return s_lenIcolup;}
  int depthIcolup() const {return s_depthIcolup;}
  int lenPup() {return s_lenPup;}
  int depthPup() {return s_depthPup;}
  int lenVtimup() {return s_lenVtimup;}
  int lenSpinup() {return s_lenSpinup;}

private:
  struct HEPEUP;
  friend struct HEPEUP;
  
  struct HEPEUP {
    int nup;
    int idprup;
    double xwgtup;
    double scalup;
    double aqedup;
    double aqcdup;
    int idup[s_lenIdup];
    int istup[s_lenIstup];
    int mothup[s_lenMothup][s_depthMothup];
    int icolup[s_lenIcolup][s_depthIcolup];
    double pup[s_lenPup][s_depthPup];
    double vtimup[s_lenVtimup];
    double spinup[s_lenSpinup]; 
  };
  int m_dummy;
  double m_realdummy;
  static HEPEUP* s_hepeup;
};

// Inline implementations for Hepeup
// initialise pointer
#ifdef WIN32
void Hepeup::init(void) {
  if ( 0 == s_hepeup ) s_hepeup = static_cast<HEPEUP*>(HEPEUP_ADDRESS());
}
#else
void Hepeup::init(void) {
  if ( 0 == s_hepeup ) s_hepeup = static_cast<HEPEUP*>(hepeup_address__());
}
#endif
#endif // LBBCVEGPY_HEPEUP_H
 
