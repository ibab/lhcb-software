
#include <cmath>
#include <iostream>

// local
#include "RichHPDImageAnalysis/HPDBoundaryFcn.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : HPDBoundaryFcn
//
// 2011-03-02 : Chris Jones
//-----------------------------------------------------------------------------

HPDBoundaryFcn::HPDBoundaryFcn( const TH2* hist ,
                                const double thr )
  : m_threshold     ( thr  ),
    m_hist          ( hist ),
    m_boundaryFound ( false )
{ }

HPDBoundaryFcn::~HPDBoundaryFcn() {}

const HPDBoundaryFcn::Boundary& HPDBoundaryFcn::boundary() const
{
  if ( !m_boundaryFound ) findBoundary();
  return m_boundary;
}

void HPDBoundaryFcn::findBoundary() const
{
  if ( m_hist )
  {
    m_boundary.clear() ;

    const int nbins  = m_hist->GetNbinsX()*m_hist->GetNbinsY();
    const double thr = m_threshold*m_hist->Integral()/(1.0*nbins);

    for ( int icol = 0 ; icol < m_hist->GetNbinsX() ; ++icol )
    {
      int ROW0(-1), ROW1(-1);

      for ( int irow = 0; irow <m_hist->GetNbinsY() ; ++irow )
      {
        if ( hasNeighbour( icol, irow, thr ) &&
             m_hist->GetBinContent( icol+1, irow+1 ) > thr )
        {
          ROW0 = irow ;
          break;
        }
      }
      for ( int irow = 0; irow < m_hist->GetNbinsY() ; ++irow )
      {
        if ( hasNeighbour( icol, irow, thr ) &&
             m_hist->GetBinContent( icol+1, m_hist->GetNbinsX()-irow ) > thr )
        {
          ROW1 = m_hist->GetNbinsX() - irow - 1;
          break;
        }
      }
      if ( -1 != ROW0 )
      {
        m_boundary.push_back( Pixel( icol, ROW0 ) );
      }
      if ( -1 != ROW1 && ROW1 != ROW0 )
      {
        m_boundary.push_back( Pixel( icol, ROW1 ) );
      }
    }

    m_boundaryFound = true;
  }
}

bool HPDBoundaryFcn::hasNeighbour( const int COL,
                                   const int ROW,
                                   const double thr ) const
{
  for ( int icol = COL-1; icol <= COL+1 ; ++icol )
  {
    for ( int irow = ROW-1; irow <= ROW+1 ; ++irow )
    {
      if ( COL == icol && ROW == irow ) { continue ; }
      else if ( icol >= 0 && icol < m_hist->GetNbinsX() &&
                irow >= 0 && irow < m_hist->GetNbinsY() )
      {
        if ( m_hist->GetBinContent( icol+1, irow+1 ) > thr ) return true ;
      }
    }
  }
  return false;
}


