// $Id: Pyssmt.h,v 1.2 2006-05-03 08:18:07 robbep Exp $

#ifndef LBPYTHIA_PYSSMT_H
#define LBPYTHIA_PYSSMT_H 1

#ifdef WIN32
extern "C" {
  void*  __stdcall PYSSMT_ADDRESS(void);
}
#else
extern "C" {
  void* pyssmt_address__(void);
}
#endif

static const int s_sizeZmix = 4;
static const int s_sizeUmix = 2;
static const int s_sizeVmix = 2;
static const int s_lenSmz = 4;
static const int s_lenSmw = 2;
static const int s_lenSfmix = 4;
static const int s_widthSfmix = 16;

/** @class Pyssmt Pyssmt.h LbPythia/Pyssmt.h
 *
 *  Class to access PYSSMT Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pyssmt {
public:
  Pyssmt();
  ~Pyssmt();
  const double& zmix(int i, int j) const;
  const double& umix(int i, int j) const;
  const double& vmix(int i, int j) const;
  const double& smz(int i) const;
  const double& smw(int i) const;
  const double& sfmix(int i, int j) const;
  const double& zmixi(int i, int j) const;
  const double& umixi(int i, int j) const;
  const double& vmixi(int i, int j) const;

  inline void init();

private:
  struct PYSSMT;
  friend struct PYSSMT;
  
  struct PYSSMT {
    double zmix[s_sizeZmix][s_sizeZmix];
    double umix[s_sizeUmix][s_sizeUmix];
    double vmix[s_sizeVmix][s_sizeVmix];
    double smz[s_lenSmz];
    double smw[s_lenSmw];
    double sfmix[s_lenSfmix][s_widthSfmix];
    double zmixi[s_sizeZmix][s_sizeZmix] ;
    double umixi[s_sizeUmix][s_sizeUmix] ;
    double vmixi[s_sizeVmix][s_sizeVmix] ;
  };
  
  mutable int m_dummy;
  mutable double m_realdummy;
  
  static PYSSMT const * s_pyssmt;
};

// Inline implementations for Pyssmt
// initialise pointer
#ifdef WIN32
void Pyssmt::init(void) {
  if ( 0 == s_pyssmt ) s_pyssmt = static_cast<PYSSMT*>(PYSSMT_ADDRESS());
}
#else
void Pyssmt::init(void) {
  if ( 0 == s_pyssmt ) s_pyssmt = static_cast<PYSSMT*>(pyssmt_address__());
}
#endif
#endif // LBPYTHIA_PYSSMT_H
