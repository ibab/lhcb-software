// $Id: Pydat2.h,v 1.2 2006-05-03 08:18:07 robbep Exp $
#ifndef LBPYTHIA_PYDAT2_H
#define LBPYTHIA_PYDAT2_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYDAT2_ADDRESS(void);
}
#else
extern "C" {
  void* pydat2_address__(void);
}
#endif

// Lengths of array in PYDAT2 common
static const int s_lenPmas   = 500;
static const int s_depthPmas = 4;
static const int s_depthKchg = 4;
static const int s_lenParf   = 2000;
static const int s_lenVckm = 4;

/** @class Pydat2 Pydat2.h LbPythia/Pydat2.h
 *
 *  Class to access PYDAT2 Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pydat2 {
public:
  Pydat2();
  ~Pydat2();
  
  int& kchg(int kc, int i);
  double& pmas(int kc, int i);
  double& parf(int n);
  double& vckm(int i, int j);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int depthKchg() const {return s_depthKchg;}
  int lenPmas() const {return s_lenPmas;}
  int depthPmas() const {return s_depthPmas;}
  int lenParf() const {return s_lenParf;}
  int lenVckm() const {return s_lenVckm;}

private:
  struct PYDAT2;
  friend struct PYDAT2;
  struct PYDAT2 {
    int kchg[s_depthKchg][s_lenPmas];
    double pmas[s_depthPmas][s_lenPmas];
    double parf[s_lenParf];
    double vckm[s_lenVckm][s_lenVckm];
  };
  int m_dummy;
  double m_realdummy;
  static PYDAT2* s_pydat2;
};

// Inline implementations for Pydat2
#ifdef WIN32
// initialise pointer
void Pydat2::init(void) {
  if ( 0 == s_pydat2 ) s_pydat2 = static_cast<PYDAT2*>(PYDAT2_ADDRESS());
}
#else
// initialise pointer
void Pydat2::init(void) {
  if ( 0 == s_pydat2 ) s_pydat2 = static_cast<PYDAT2*>(pydat2_address__());
}
#endif
#endif // LBPYTHIA_PYDAT2_H
