#ifndef LBPYTHIA_HEPEUP_H
#define LBPYTHIA_HEPEUP_H 1

#ifdef WIN32
extern "C" {
  void* __stdcall HEPEUP_ADDRESS(void) ;
}
#else
extern "C" {
  void* hepeup_address__(void) ;
}
#endif

// Lengths of MSTU and PARU COMMONS
static const int s_lenHepeup = 500;

/** @class Hepeup Hepeup.h LbPythia/Hepeup.h
 *
 *  Class to access HEPEUP Pythia Common block from C++ code.
 * 
 *  @author Patrick Robbe
 *  @date   2012-07-17
 */

class Hepeup {
public:
  Hepeup();
  ~Hepeup();
  int& nup();
  int& idprup() ;
  int& idup( int n ) ;
  int& istup( int n ) ;
  int& mothup( int i , int n ) ;
  int& icolup( int i , int n ) ;
   
  double& xwgtup() ;
  double& scalup() ;
  double& aqedup() ;
  double& aqcdup() ;
  double& pup( int i , int n ) ;
  double& vtimup( int n ) ;
  double& spinup( int n ) ;

  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenHepeup() const {return s_lenHepeup;}

private:
  struct HEPEUP;
  friend struct HEPEUP;
  
  struct HEPEUP {
    int nup ;
    int idprup ;
    double xwgtup ;
    double scalup ;
    double aqedup ;
    double aqcdup ;
    int idup[ s_lenHepeup ] ;
    int istup[ s_lenHepeup ] ;
    int mothup[ s_lenHepeup ][ 2 ] ;
    int icolup[ s_lenHepeup ][ 2 ] ;
    double pup[ s_lenHepeup ][ 5 ] ;
    double vtimup[ s_lenHepeup ] ;
    double spinup[ s_lenHepeup ] ;
  };
  int m_dummy;
  double m_realdummy;
  static HEPEUP* s_hepeup;
};

// Inline implementations for Hepeup
// initialise pointer
#ifdef WIN32
void Hepeup::init(void) {
  if ( 0 == s_hepeup ) s_hepeup = static_cast<HEPEUP*>(HEPEUP_ADDRESS());
}
#else
void Hepeup::init(void) {
  if ( 0 == s_hepeup ) s_hepeup = static_cast<HEPEUP*>(hepeup_address__());
}
#endif
#endif // LBPYTHIA_HEPEUP_H
 
