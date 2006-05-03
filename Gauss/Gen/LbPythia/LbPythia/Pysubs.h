// $Id: Pysubs.h,v 1.2 2006-05-03 08:18:07 robbep Exp $

#ifndef LBPYTHIA_PYSUBS_H
#define LBPYTHIA_PYSUBS_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYSUBS_ADDRESS(void);
}
#else
extern "C" {
  void* pysubs_address__(void);
}
#endif

// Lengths of array in PYSUBS common
static const int s_lenMsub = 500;
static const int s_lenCkin = 200;
static const int s_lenKfin = 81;
static const int s_depthKfin = 2;

/** @class Pysubs Pysubs.h LbPythia/Pysubs.h
 *
 *  Class to access PYDAT1 Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pysubs {
public:
  Pysubs();
  ~Pysubs();
  int& msel(); 
  int& msub(int n);
  int& kfin(int iside, int jflav);
  double& ckin(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenMsub() const {return s_lenMsub;}
  int lenCkin() const {return s_lenCkin;}
  int lenKfin() const {return s_lenKfin;}
  int depthKfin() const {return s_depthKfin;}

private:
  struct PYSUBS;
  friend struct PYSUBS;
  struct PYSUBS {
    int msel;
    int padding;
    int msub[s_lenMsub];
    int kfin[s_lenKfin][s_depthKfin];
    double ckin[s_lenCkin];
  };
  int m_dummy;
  double m_realdummy;
  static PYSUBS* s_pysubs;
};

// Inline implementations for Pysubs
// initialise pointer
#ifdef WIN32
void Pysubs::init(void) {
  if ( 0 == s_pysubs ) s_pysubs = static_cast<PYSUBS*>(PYSUBS_ADDRESS());
}
#else
void Pysubs::init(void) {
  if ( 0 == s_pysubs ) s_pysubs = static_cast<PYSUBS*>(pysubs_address__());
}
#endif
#endif // LBPYTHIA_PYSUBS_H
