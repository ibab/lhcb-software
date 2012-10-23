#ifndef PRCHEATEDVELO_H 
#define PRCHEATEDVELO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PrCheatedVelo PrCheatedVelo.h
 *  Cheated Pattern for the Velo
 *
 *  @author Olivier Callot
 *  @date   2012-07-26
 */
class PrCheatedVelo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PrCheatedVelo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrCheatedVelo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
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
//-----------------------------------------------------------------------------
// Implementation file for class : PrFitPolinomial
//
// 2004-07-29 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrFitPolinomial::PrFitPolinomial( int degree ) {
  m_degree = degree;
  for ( int k1 = 0 ; degree >= k1 ; k1++ ) {
     m_sumx.push_back( 0. );
     m_sumz.push_back( 0. );
     m_sumz.push_back( 0. );
     m_param.push_back( 0. );
  }
}
//=============================================================================
// Destructor
//=============================================================================
PrFitPolinomial::~PrFitPolinomial() {}

//=========================================================================
//  
//=========================================================================
void PrFitPolinomial::fill( double x, double z ) {
  double valZ = 1.;
  double valX = x;
  for ( int kk = 0 ; m_degree >= kk ; kk++ ) {
    m_sumx[kk] += valX;
    valX *= z;
    m_sumz[2*kk] += valZ;
    valZ *= z;
    m_sumz[2*kk+1] += valZ;
    valZ *= z;
  }
}

//=========================================================================
//  
//=========================================================================
bool PrFitPolinomial::solve ( ) {
  
  double mat[10][10];
  double vect[10];

  for ( int kk = 0; m_degree >= kk; kk++ ){
    vect[kk] = m_sumx[kk];
    for ( int ll = 0; m_degree >= ll; ll++ ) {
      mat[ll][kk] = m_sumz[kk+ll];
    }
  }
  int i, j;
  int im, jm;
  double den;

  int Dim = m_degree+1;
  
  for ( im = 0 ; Dim > im  ; im++  ) {
    for ( jm = Dim-1 ; 0 <= jm ; jm-- ) {
      for ( i=0 ; Dim > i ; i++ ) {
        if ( 0 != mat[i][i] ) {
          den = mat[i][i];
          vect[i] /= den;
          for ( j=0 ; Dim > j ; j++ ) {
            mat[i][j] /= den;
          }
        }
      }

      if ( im == jm ) continue;
      if ( (0 == mat[jm][im]) &&
           (0 == mat[im][jm])    ) continue;

      double den = mat[im][im]*mat[jm][jm] - mat[im][jm]*mat[jm][im];
      double c1, c2;

      if ( 0 != den ) {
        c1 =   mat[jm][jm]/den;
        c2 = - mat[im][jm]/den;
        vect[Dim] = vect[im]*c1 + vect[jm]*c2;
        for ( i=0 ; Dim > i ; i++ ) {
          mat[Dim][i] = mat[im][i]*c1 + mat[jm][i]*c2;
        }
        c1 = - mat[jm][im]/den;
        c2 =   mat[im][im]/den;
        vect[jm] = vect[im]*c1 + vect[jm]*c2;
        for ( i=0 ; Dim > i ; i++ ) {
          mat[jm][i] = mat[im][i]*c1 + mat[jm][i]*c2;
        }
        mat[jm][im] = 0.;
        mat[Dim][jm] = 0.;

        vect[im] = vect[Dim];
        for ( i=0 ; Dim > i ; i++ ) {
          mat[im][i] = mat[Dim][i];
        }
      } else {
        return false;
      }
    }
  }
  for ( i=0 ; Dim > i ; i++ ) {
    m_param[i] = vect[i];
  }
  return true;
}
//=============================================================================
#endif // PRFITPOLINOMIAL_H
#endif // PRCHEATEDVELO_H
