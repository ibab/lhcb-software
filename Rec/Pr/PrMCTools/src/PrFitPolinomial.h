#ifndef PRFITPOLINOMIAL_H 
#define PRFITPOLINOMIAL_H 1

// Include files
#include <vector>

/** @class PrFitPolinomial PrFitPolinomial.h
 *  Fit a polinomial of specified degree
 *
 *  @author Olivier Callot
 *  @date   2012-07-03
 */
class PrFitPolinomial {
public: 
  /// Standard constructor
  PrFitPolinomial( int degree ); 
  
  virtual ~PrFitPolinomial( ); ///< Destructor
  
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
#endif
