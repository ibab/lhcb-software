#ifndef PRFITPARAMETERS_H 
#define PRFITPARAMETERS_H 1

#include <string>
#include <vector>

// Include files
// from Gaudi
#include "GaudiKernel/MsgStream.h"


/** @class PrFitParameters PrFitParameters.h cmt/PrFitParameters.h
 *  Class to store and compute/update parameters for the Forward Tracking
 *
 *  @author Olivier Callot
 *  @date   03/07/2012
 */
class PrFitParameters {
public:
  /// Standard constructor
  PrFitParameters( ); 

  virtual ~PrFitParameters( ); ///< Destructor

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
#endif // PRFITPARAMETERS_H
