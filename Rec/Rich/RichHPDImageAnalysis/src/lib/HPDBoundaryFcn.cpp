
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
                                const Params& params )
  : m_hist          ( hist   ),
    m_params        ( params ) 
{ }

void HPDBoundaryFcn::addPixel( const int col, 
                               const int row,
                               Pixel::List & boundary ) const
{
  boundary.push_back( Pixel(col,row,m_hist->GetBinContent(col+1,row+1)) );
  //boundary.push_back( Pixel(col,row,1.0) ); // As in 2010 results
}

void HPDBoundaryFcn::findBoundary( Pixel::List & boundary ) const
{
  if ( m_hist )
  {
    boundary.clear() ;

    const int nbins  = m_hist->GetNbinsX() * m_hist->GetNbinsY();
    const double thr = m_params.cutFraction * m_hist->Integral() / (1.0*nbins);

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
          addPixel( icol, ROW0, boundary );
        }
        if ( -1 != ROW1 && ROW1 != ROW0 )
        {
          addPixel( icol, ROW1, boundary );
        }
      }
    }

    // Then the other way
    // Disable to reproduce 2010 results
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
          addPixel( COL0, irow, boundary );
        }
        if ( -1 != COL1 && COL1 != COL0 )
        {
          addPixel( COL1, irow, boundary );
        }
      }
    }

    // Sort
    std::sort( boundary.begin(), boundary.end() );

    // remove duplicates
    boundary.erase( std::unique(boundary.begin(),boundary.end()), boundary.end() );

    // Enough hits
    if ( boundary.size() < m_params.minBoundary ) { boundary.clear(); }

  }
}

bool HPDBoundaryFcn::hasNeighbour( const int COL,
                                   const int ROW,
                                   const double thr,
                                   const int area ) const
{
  for ( int icol = COL-area; icol <= COL+area ; ++icol )
  {
    for ( int irow = ROW-area; irow <= ROW+area ; ++irow )
    {
      if ( COL == icol && ROW == irow ) { continue ; }
      if ( icol >= 0 && icol < m_hist->GetNbinsX() &&
           irow >= 0 && irow < m_hist->GetNbinsY() )
      {
        if ( m_hist->GetBinContent( icol+1, irow+1 ) > thr ) return true;
      }
    }
  }
  return false;
}


