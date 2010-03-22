// $Id: $
#ifndef HPDBOUNDARYFCN_H 
#define HPDBOUNDARYFCN_H 1

// Include files

/** @class HPDBoundaryFcn HPDBoundaryFcn.h
 *  
 *
 *  @author Thomas Blake
 *  @date   2010-02-04
 */

#include "Minuit2/FCNBase.h"
#include "TH2.h"
#include <vector>
#include <cmath>
namespace Rich {
  namespace Mon {
    
    class HPDBoundaryFcn : public ROOT::Minuit2::FCNBase  {
      
    public: 
      /// Standard constructor
      HPDBoundaryFcn( const TH2* hist, const double threshold ); 
      HPDBoundaryFcn();
      virtual double operator()(const std::vector<double>& par ) const ;
      virtual double Up() const{  return m_errDef; }
      void setErrDef( const double def ){ m_errDef = def; }
      
      void findBoundary() ;
      double nPixels() const ;
      
      virtual ~HPDBoundaryFcn( ); ///< Destructor
      
    protected:
      
      bool hasNeighbour( const TH2* hist, const int COL, const int ROW, const double thr ) const ;
      
      
    private:
      double m_errDef ;
      double m_threshold ;
      const TH2* m_hist ;
      
      double m_sf ;
      
      std::vector< std::pair< int, int > > m_boundary ;
      
    };
  } 
}

#endif // HPDBOUNDARYFCN_H
