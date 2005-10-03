// $Id: Pythia.h,v 1.1 2005-10-03 10:33:00 robbep Exp $
#ifndef LBPYTHIA_PYTHIA_H 
#define LBPYTHIA_PYTHIA_H 1

// Include files
#include <string>
#include "LbPythia/Pydat1.h"
#include "LbPythia/Pydat2.h"
#include "LbPythia/Pydat3.h"
#include "LbPythia/Pysubs.h"
#include "LbPythia/Pypars.h"
#include "LbPythia/Pydatr.h"
#include "LbPythia/Pyint2.h"
#include "LbPythia/Pymssm.h"

/** @class Pythia Pythia.h LbPythia/Pythia.h
 * 
 *  Utility class to access Pythia Fortran Functions 
 *
 *  @author Patrick Robbe
 *  @date   2005-08-19
 */
class Pythia {
public:
  static Pysubs & pysubs() { return s_pysubs ; }
  static Pydat1 & pydat1() { return s_pydat1 ; }
  static Pypars & pypars() { return s_pypars ; }
  static Pydat2 & pydat2() { return s_pydat2 ; }
  static Pydat3 & pydat3() { return s_pydat3 ; }
  static Pydatr & pydatr() { return s_pydatr ; }
  static Pyint2 & pyint2() { return s_pyint2 ; }
  static Pymssm & pymssm() { return s_pymssm ; }

  static void PdfLib_Init( ) ;
  static void SetUserProcess( int val ) ;
  static void SetBeam( double p1x, double p1y, double p1z ,
                       double p2x, double p2y, double p2z ) ;
  static void InitPyBlock( int unit, const std::string fileName ) ;
  static void PyInit( const std::string frame, const std::string beam , 
                      const std::string target , double win ) ;
  static void PyList( int val ) ;
  static void PyEvnt( ) ;
  static int LunHep( int val ) ;
  static int PyComp( int val ) ;
  static void PdfLib_End( ) ;
  static void PyStat( int val ) ;

protected:

private:
	static Pysubs s_pysubs ; ///< PYSUBS Common Block data
	static Pydat1 s_pydat1 ; ///< PYDAT1 Common Block data
	static Pypars s_pypars ; ///< PYPARS Common Block data
	static Pydat2 s_pydat2 ; ///< PYDAT2 Common Block data
	static Pydat3 s_pydat3 ; ///< PYDAT3 Common Block data
	static Pydatr s_pydatr ; ///< PYDATR Common Block data
	static Pyint2 s_pyint2 ; ///< PYINT2 Common Block data
  static Pymssm s_pymssm ; ///< PYMSSM Common Block data
};
#endif // LBPYTHIA_PYTHIA_H
