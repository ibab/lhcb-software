
#include <cmath>
#include <iostream>
#include <algorithm>

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

    const int nbins  = m_hist->GetNbinsX() * m_hist->GetNbinsY();
    const double thr = m_threshold*m_hist->Integral()/(1.0*nbins);

    // First one way
    {
      for ( int icol = 0 ; icol < m_hist->GetNbinsX() ; ++icol )
      {
        int ROW0(-1), ROW1(-1);

        for ( int irow = 0; irow < m_hist->GetNbinsY() ; ++irow )
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
          addPixel( icol, ROW0 );
        }
        if ( -1 != ROW1 && ROW1 != ROW0 )
        {
          addPixel( icol, ROW1 );
        }
      }
    }

    // Then the other way
    {
      for ( int irow = 0 ; irow < m_hist->GetNbinsY() ; ++irow )
      {
        int COL0(-1), COL1(-1);

        for ( int icol = 0; icol < m_hist->GetNbinsX() ; ++icol )
        {
          if ( hasNeighbour( icol, irow, thr ) &&
               m_hist->GetBinContent( icol+1, irow+1 ) > thr )
          {
            COL0 = icol ;
            break;
          }
        }
        for ( int icol = 0; icol < m_hist->GetNbinsX() ; ++icol )
        {
          if ( hasNeighbour( icol, irow, thr ) &&
               m_hist->GetBinContent( icol+1, m_hist->GetNbinsX()-irow ) > thr )
          {
            COL1 = m_hist->GetNbinsY() - icol - 1;
            break;
          }
        }
        if ( -1 != COL0 )
        {
          addPixel( COL0, irow );
          
        }
        if ( -1 != COL1 && COL1 != COL0 )
        {
          addPixel( COL1, irow );
        }
      }
    }

    // Sort and remove duplicates
    std::sort  ( m_boundary.begin(), m_boundary.end() );
    std::unique( m_boundary.begin(), m_boundary.end() );

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
        if ( m_hist->GetBinContent( icol+1, irow+1 ) > thr ) return true;
      }
    }
  }
  return false;
}


