// $Id: Pyint2.h,v 1.2 2006-05-03 08:18:07 robbep Exp $

#ifndef LBPYTHIA_PYINT2_H
#define LBPYTHIA_PYINT2_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYINT2_ADDRESS(void);
}
#else
extern "C" {
  void* pyint2_address__(void);
}
#endif

// Lengths of ISET and KFPR COMMONS
static const int s_lenIset = 500;
static const int s_lenKfpr = 500;
static const int s_depthKfpr = 2; 
static const int s_lenCoef = 500;
static const int s_depthCoef = 20; 
static const int s_lenIcol = 40;
static const int s_depthIcol = 4; 
static const int s_widthIcol = 2; 

/** @class Pyint2 Pyint2.h LbPythia/Pyint2.h
 *
 *  Class to access PYINT2 Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pyint2 {
public:
  Pyint2();
  ~Pyint2();
  int& iset(int n);
  int& kfpr(int n, int i);
  double& coef(int n, int i);
  int& icol(int n,int i,int j);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenIset() const {return s_lenIset;}
  int depthKfpr() const {return s_depthKfpr;}
  int lenKfpr() const {return s_lenKfpr;}
  int lenCoef() const {return s_lenCoef;}
  int depthCoef() const {return s_depthCoef;}
  int lenIcol() const {return s_lenIcol;}
  int depthIcol() const {return s_depthIcol;}
  int widthIcol() const {return s_widthIcol;}

private:
  struct PYINT2;
  friend struct PYINT2;
  
  struct PYINT2 {
    int iset[s_lenIset];
    int kfpr[s_depthKfpr][s_lenKfpr];
    double coef[s_depthCoef][s_lenCoef];
    int icol[s_widthIcol][s_depthIcol][s_lenIcol];
  };
  int m_dummy;
  double m_realdummy;
  static PYINT2* s_pyint2;
};

// Inline implementations for Pyint2
// initialise pointer
#ifdef WIN32
void Pyint2::init(void) {
  if ( 0 == s_pyint2 ) s_pyint2 = static_cast<PYINT2*>(PYINT2_ADDRESS());
}
#else
void Pyint2::init(void) {
  if ( 0 == s_pyint2 ) s_pyint2 = static_cast<PYINT2*>(pyint2_address__());
}
#endif
#endif // LBPYTHIA_PYINT2_H
