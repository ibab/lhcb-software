#ifndef LBPYTHIA_PYINT6_H
#define LBPYTHIA_PYINT6_H 1

#include <string>

#ifdef WIN32
extern "C" {
  void* __stdcall PYINT6_ADDRESS(void);
}
#else
extern "C" {
  void* pyint6_address__(void);
}
#endif

// Lengths of PROC COMMONS
static const int s_lenProc = 501 ;

/** @class Pyint6 Pyint6.h LbPythia/Pyint6.h
 *
 *  Class to access PYINT6 Pythia Common block from C++ code.
 *  Original code by CDF.
 * 
 *  @author Chris Green,   Purdue University
 *  @author Stan Thompson, Glasgow University  
 *  @date   2003-09-01
 */

class Pyint6 {
public:
  Pyint6();
  ~Pyint6();
  std::string proc(int n);
  inline void init(); // inlined for speed of access (small function)
  // return common array lengths
  int lenProc() const { return s_lenProc ; }

private:
  struct PYINT6;
  friend struct PYINT6;
  
  struct PYINT6 {
    char proc[ 28 * s_lenProc ] ;
  };

  static PYINT6 * s_pyint6 ;
};

// Inline implementations for Pyint6
// initialise pointer
#ifdef WIN32
void Pyint6::init( ) {
  if ( 0 == s_pyint6 ) s_pyint6 = static_cast< PYINT6 * >( PYINT6_ADDRESS() ) ;
}
#else
void Pyint6::init( ) {
  if ( 0 == s_pyint6 ) s_pyint6 = static_cast< PYINT6 * >( pyint6_address__() ) ;
}
#endif
#endif // LBPYTHIA_PYINT6_H
