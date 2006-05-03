// $Id: Pydat3.h,v 1.2 2006-05-03 08:18:07 robbep Exp $

#ifndef LBPYTHIA_PYDAT3_H
#define LBPYTHIA_PYDAT3_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYDAT3_ADDRESS(void);
}
#else
extern "C" {
  void* pydat3_address__(void);
}
#endif

// Lengths of array in PYDAT2 common
static const int s_lenMdcy = 500;
static const int s_depthMdcy = 3;
static const int s_lenMdme = 8000;
static const int s_depthMdme = 2;
static const int s_lenBrat = 8000;
static const int s_lenKfdp = 8000;
static const int s_depthKfdp = 5;

/** @class Pydat3 Pydat3.h LbPythia/Pydat3.h
 *
 *  Class to access PYDAT3 Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pydat3 {
public:
  Pydat3();
  ~Pydat3();

  int& mdcy(int kc, int i);
  int& mdme(int idc, int i);
  double& brat(int idc);
  int& kfdp(int idc, int kf);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMdcy() const {return s_lenMdcy;}
  int depthMdcy() const {return s_depthMdcy;}
  int lenMdme() const {return s_lenMdme;}
  int depthMdme() const {return s_depthMdme;}
  int lenBrat() const {return s_lenBrat;}
  int lenKfdp() const {return s_lenKfdp;}
  int depthKfdp() const {return s_depthKfdp;}

private:
  struct PYDAT3;
  friend struct PYDAT3;
  struct PYDAT3 {
    int mdcy[s_depthMdcy][s_lenMdcy];
    int mdme[s_depthMdme][s_lenMdme];
    double brat[s_lenBrat];
    int kfdp[s_depthKfdp][s_lenKfdp];
  };
  int m_dummy;
  double m_realdummy;
  static PYDAT3* s_pydat3;
};

// Inline implementations for Pydat3
// initialise pointer
#ifdef WIN32
void Pydat3::init(void) {
  if ( 0 == s_pydat3 ) s_pydat3 = static_cast<PYDAT3*>(PYDAT3_ADDRESS());
}
#else
void Pydat3::init(void) {
  if ( 0 == s_pydat3 ) s_pydat3 = static_cast<PYDAT3*>(pydat3_address__());
}
#endif
#endif // LBPYTHIA_PYDAT3_H
