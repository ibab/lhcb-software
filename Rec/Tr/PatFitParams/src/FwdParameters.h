// $Id: FwdParameters.h,v 1.2 2009-08-19 14:16:15 ocallot Exp $
#ifndef FWDPARAMETERS_H 
#define FWDPARAMETERS_H 1

#include <string>
#include <vector>

// Include files
// from Gaudi
#include "GaudiKernel/MsgStream.h"


/** @class FwdParameters FwdParameters.h cmt/FwdParameters.h
 *  Class to store and compute/update parameters for the Forward Tracking
 *
 *  @author Olivier Callot
 *  @date   27/11/2001
 */
class FwdParameters {
public:
  /// Standard constructor
  FwdParameters( ); 

  virtual ~FwdParameters( ); ///< Destructor

  /// Initialise the parameter for computation
  void init ( std::string title, std::vector<double> param );
  
  /// Add an event: specify delta and the functions
  void addEvent( double delta );
  
  /// Solve and update the parameters
  bool updateParameters( MsgStream& log );

  /// Print on cout the parameters in a jobOption format
  void printParams ( std::string prefix );

  /// Print on cout the parameters in a python format
  void printPythonParams ( std::string prefix );

  /// return the specified parameter
  double param( int i ) const  { return m_par[i]; }

  /// set a function
  void setFun( int i, double value ) { m_fun[i] = value; }
  
  /// return the whole sum
  double sum() const {
    double s = 0.;
    for ( int i=0 ; m_nbPar>i ; i++  ) {
      s += m_fun[i] * m_par[i];
    }
    return s;
  }

protected:

private:
  int    m_nbPar;
  double m_par[10];
  double m_dat[10];
  double m_mat[10][10];
  double m_fun[10];
  std::string m_title;
};
#endif // FWDPARAMETERS_H
