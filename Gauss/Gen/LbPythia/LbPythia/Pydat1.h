// $Id: Pydat1.h,v 1.2 2006-05-03 08:18:06 robbep Exp $

#ifndef LBPYTHIA_PYDAT1_H
#define LBPYTHIA_PYDAT1_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYDAT1_ADDRESS(void) ;
}
#else
extern "C" {
  void* pydat1_address__(void) ;
}
#endif

// Lengths of MSTU and PARU COMMONS
static const int s_lenMstu = 200;
static const int s_lenParu = 200;
static const int s_lenMstj = 200;
static const int s_lenParj = 200;

/** @class Pydat1 Pydat1.h LbPythia/Pydat1.h
 *
 *  Class to access PYDAT1 Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pydat1 {
public:
  Pydat1();
  ~Pydat1();
  int& mstu(int n);
  double& paru(int n);
  int& mstj(int n);
  double& parj(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMstu() const {return s_lenMstu;}
  int lenParu() const {return s_lenParu;}
  int lenMstj() const {return s_lenMstj;}
  int lenParj() const {return s_lenParj;}

private:
  struct PYDAT1;
  friend struct PYDAT1;
  
  struct PYDAT1 {
    int mstu[s_lenMstu];
    double paru[s_lenParu];
    int mstj[s_lenMstj];
    double parj[s_lenParj];
  };
  int m_dummy;
  double m_realdummy;
  static PYDAT1* s_pydat1;
};

// Inline implementations for Pydat1
// initialise pointer
#ifdef WIN32
void Pydat1::init(void) {
  if ( 0 == s_pydat1 ) s_pydat1 = static_cast<PYDAT1*>(PYDAT1_ADDRESS());
}
#else
void Pydat1::init(void) {
  if ( 0 == s_pydat1 ) s_pydat1 = static_cast<PYDAT1*>(pydat1_address__());
}
#endif
#endif // LBPYTHIA_PYDAT1_H
 
