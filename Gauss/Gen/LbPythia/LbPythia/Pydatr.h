// $Id: Pydatr.h,v 1.2 2006-05-03 08:18:07 robbep Exp $

#ifndef LBPYTHIA_PYDATR_H
#define LBPYTHIA_PYDATR_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYDATR_ADDRESS(void);
}
#else
extern "C" {
  void* pydatr_address__(void);
}
#endif

static const int s_lenMrpy = 6;
static const int s_lenRrpy = 100;

/** @class Pydatr Pydatr.h LbPythia/Pydatr.h
 *
 *  Class to access PYDATR Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pydatr {
public:
  Pydatr();
  ~Pydatr();
  int& mrpy(int n);
  double& rrpy(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMrpy() const {return s_lenMrpy;}
  int lenRrpy() const {return s_lenRrpy;}

private:
  // Lengths of array in PYDATR common
  struct PYDATR;
  friend struct PYDATR;
  struct PYDATR {
    int mrpy[s_lenMrpy];
    double rrpy[s_lenRrpy];
  };
  int m_dummy;
  double m_realdummy;
  static PYDATR* s_pydatr;
};


// Inline implementations for Pydatr
// initialise pointer
#ifdef WIN32
void Pydatr::init(void) {
  if ( 0 == s_pydatr ) s_pydatr = static_cast<PYDATR*>(PYDATR_ADDRESS());
}
#else 
void Pydatr::init(void) {
  if ( 0 == s_pydatr ) s_pydatr = static_cast<PYDATR*>(pydatr_address__());
}
#endif
#endif // LBPYTHIA_PYDATR_H
