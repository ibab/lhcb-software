#ifndef LBPYTHIA_PYINT5_H
#define LBPYTHIA_PYINT5_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall PYINT5_ADDRESS(void);
}
#else
extern "C" {
  void* pyint5_address__(void);
}
#endif

// Lengths of ISET and KFPR COMMONS
static const int s_lenNgen = 501 ;
static const int s_lenXsec = 501 ;
static const int s_depthNgen = 3 ; 
static const int s_depthXsec = 3 ;

/** @class Pyint5 Pyint5.h LbPythia/Pyint5.h
 *
 *  Class to access PYINT5 Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pyint5 {
public:
  Pyint5();
  ~Pyint5();
  int& ngen(int n , int i);
  double& xsec(int n, int i);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenNgen() const { return s_lenNgen ; }
  int depthNgen() const { return s_depthNgen ; }
  int lenXsec() const { return s_lenXsec ; }
  int depthXsec() const {return s_depthXsec ; }

private:
  struct PYINT5 ;
  friend struct PYINT5 ;
  
  struct PYINT5 {
    int ngenpd ;
    int ngen[ s_depthNgen ][ s_lenNgen ] ;
    double xsec[ s_depthXsec ][ s_lenXsec ] ;
  };
  int m_dummy ;
  double m_realdummy ;
  static PYINT5 * s_pyint5 ;
};

// Inline implementations for Pyint5
// initialise pointer
#ifdef WIN32
void Pyint5::init( ) {
  if ( 0 == s_pyint5 ) s_pyint5 = static_cast< PYINT5 * >( PYINT5_ADDRESS() ) ;
}
#else
void Pyint5::init( ) {
  if ( 0 == s_pyint5 ) s_pyint5 = static_cast< PYINT5 * >( pyint5_address__() ) ;
}
#endif
#endif // LBPYTHIA_PYINT5_H
