// $Id: FwdFitPolinomial.h,v 1.1.1.1 2007-10-09 18:50:43 smenzeme Exp $
#ifndef FWDFITPOLINOMIAL_H 
#define FWDFITPOLINOMIAL_H 1

// Include files
#include <vector>

/** @class FwdFitPolinomial FwdFitPolinomial.h
 *  Fit a polinomial of specified degree
 *
 *  @author Olivier Callot
 *  @date   2004-07-29
 */
class FwdFitPolinomial {
public: 
  /// Standard constructor
  FwdFitPolinomial( int degree ); 

  virtual ~FwdFitPolinomial( ); ///< Destructor

  void fill( double x, double z );
  
  bool solve();

  double param( int i ) { return m_param[i]; }

protected:

private:
  int m_degree;
  std::vector<double> m_sumx;
  std::vector<double> m_sumz;
  std::vector<double> m_param;
 
};
#endif // FWDFITPOLINOMIAL_H
