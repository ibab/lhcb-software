// $Id: $
// Include files 


// local
#include "HPDBoundaryFcn.h"
#include <assert.h>

//-----------------------------------------------------------------------------
// Implementation file for class : HPDBoundaryFcn
//
// 2010-02-03 : Thomas Blake
//-----------------------------------------------------------------------------

namespace Rich 
{
  namespace Mon 
  {

    //=============================================================================
    // Standard constructor, initializes variables
    //=============================================================================
    
    HPDBoundaryFcn::HPDBoundaryFcn( const TH2* hist , const double thr ) :
      m_errDef(1.), 
      m_threshold( thr ), 
      m_hist( hist ){
      m_sf = (1.0*m_hist->GetNbinsX())/(1.0*m_hist->GetNbinsY());
    }

    HPDBoundaryFcn::HPDBoundaryFcn() : 
      m_errDef( 1. ), 
      m_threshold( 0. ), 
      m_hist( NULL ),
      m_sf( 0. ) {} 

    
    //=============================================================================
    // Destructor
    //=============================================================================
    
    HPDBoundaryFcn::~HPDBoundaryFcn() {} 
    
    double HPDBoundaryFcn::nPixels() const {
      if ( NULL == m_hist ) return 0.0 ;
      return m_hist->Integral();
    }
    
    void HPDBoundaryFcn::findBoundary() 
    {
      if ( NULL == m_hist ) return ;
      m_boundary.clear() ;
      
      int nbins  = m_hist->GetNbinsX()*m_hist->GetNbinsY();
      double thr = m_threshold*m_hist->Integral()/(1.0*nbins);
      
      for ( int icol = 0 ; icol < m_hist->GetNbinsX() ; ++icol ){   
        int ROW0 = -1; 
        int ROW1 = -1;
        
        for ( int irow = 0; irow <m_hist->GetNbinsY() ; ++irow ){
          if ( hasNeighbour( m_hist, icol, irow, thr ) && 
               m_hist->GetBinContent( icol+1, irow+1 ) > thr ) {
            ROW0 = irow ;
            break;
          }
        }
        for ( int irow = 0; irow < m_hist->GetNbinsY() ; ++irow ){
          if ( hasNeighbour( m_hist, icol, irow, thr ) && 
               m_hist->GetBinContent( icol+1, m_hist->GetNbinsX()-irow ) > thr ){
            ROW1 = m_hist->GetNbinsX() - irow - 1;
            break;
          } 
        }
        if ( -1 != ROW0 ){
          m_boundary.push_back( std::make_pair( icol, ROW0 ) );
        }
        if ( -1 != ROW1 && ROW1 != ROW0 ) {
          m_boundary.push_back( std::make_pair( icol, ROW1 ) );
        }
      }
      
      return ;
    }
    
    bool HPDBoundaryFcn::hasNeighbour( const TH2* hist, 
                                       const int COL, 
                                       const int ROW, 
                                       const double thr ) const {
      
      for ( int icol = COL-1; icol <= COL+1 ; ++icol ){
        for ( int irow = ROW-1; irow <= ROW+1 ; ++irow ){
          if ( COL == icol && ROW == irow ) continue ;
          else if ( icol >= 0 && icol < hist->GetNbinsX() && 
                    irow >= 0 && irow < hist->GetNbinsY() ){
            if ( hist->GetBinContent( icol+1, irow+1 ) > thr ) return true ;
          }
        }
      }

      return false ;
    }
    
    double HPDBoundaryFcn::operator()( const std::vector<double>& par ) const 
    {
      assert( 3 == par.size() );
      
      double chi2 = 0.0;
      std::vector< std::pair<int,int> >::const_iterator iter ;      

      for ( iter = m_boundary.begin(); iter != m_boundary.end(); ++iter ){
	
        double deltaCol = 1.0*iter->first - par[0];
        double deltaRow = m_sf*iter->second - par[1];
	
        double dist = sqrt( deltaCol*deltaCol + deltaRow*deltaRow );
        
        chi2 += ( dist-par[2] )*( dist-par[2] )*12.0;
      }
      
      return chi2 ;
    }
  }
}

//=============================================================================
	  
