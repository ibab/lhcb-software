// $Id: Pypars.h,v 1.2 2006-05-03 08:18:07 robbep Exp $

#ifndef LBPYTHIA_PYPARS_H
#define LBPYTHIA_PYPARS_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYPARS_ADDRESS(void);
}
#else
extern "C" {
  void* pypars_address__(void);
}
#endif

// Lengths of MSTU and PARU COMMONS
static const int s_lenMstp = 200;
static const int s_lenParp = 200;
static const int s_lenMsti = 200;
static const int s_lenPari = 200;

/** @class Pypars Pypars.h LbPythia/Pypars.h
 *
 *  Class to access PYPARS Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pypars {
public:
  Pypars();
  ~Pypars();
  int& mstp(int n);
  double& parp(int n);
  int& msti(int n);
  double& pari(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMstp() const {return s_lenMstp;}
  int lenParp() const {return s_lenParp;}
  int lenMsti() const {return s_lenMsti;}
  int lenPari() const {return s_lenPari;}

private:
  struct PYPARS;
  friend struct PYPARS;
  struct PYPARS {
    int mstp[s_lenMstp];
    double parp[s_lenParp];
    int msti[s_lenMsti];
    double pari[s_lenPari];
  };
  int m_dummy;
  double m_realdummy;
  static PYPARS* s_pypars;
};

// Inline implementations for Pypars
// initialise pointer
#ifdef WIN32
void Pypars::init(void) {
  if ( 0 == s_pypars ) s_pypars = static_cast<PYPARS*>(PYPARS_ADDRESS());
}
#else
void Pypars::init(void) {
  if ( 0 == s_pypars ) s_pypars = static_cast<PYPARS*>(pypars_address__());
}
#endif
#endif // LBPYTHIA_PYPARS_H
