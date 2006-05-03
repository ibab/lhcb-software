// $Id: Pymssm.h,v 1.2 2006-05-03 08:18:07 robbep Exp $

#ifndef LBPYTHIA_PYMSSM_H
#define LBPYTHIA_PYMSSM_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYMSSM_ADDRESS(void);
}
#else
extern "C" {
  void* pymssm_address__(void);
}
#endif

static const int s_lenImss = 100;
static const int s_lenRmss = 100;

/** @class Pymssm Pymssm.h LbPythia/Pymssm.h
 *
 *  Class to access PYMSSM Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pymssm {
public:
  Pymssm();
  ~Pymssm();
  
  int& imss(int n);
  double& rmss(int n);
  
  int lenImss() const {return s_lenImss;}
  int lenRmss() const {return s_lenRmss;}
  
  inline void init();
  
private:
  struct PYMSSM;
  friend struct PYMSSM;
  
  struct PYMSSM {
    int imss[s_lenImss];
    double rmss[s_lenRmss];
  };
  
  int m_dummy;
  double m_realdummy;
  
  static PYMSSM* s_pymssm;
};

// Inline implementations for Pymssm
// initialise pointer
#ifdef WIN32
void Pymssm::init(void) {
  if ( 0 == s_pymssm ) s_pymssm = static_cast<PYMSSM*>(PYMSSM_ADDRESS());
}
#else
void Pymssm::init(void) {
  if ( 0 == s_pymssm ) s_pymssm = static_cast<PYMSSM*>(pymssm_address__());
}
#endif
#endif // LBPYTHIA_PYMSSM_H
