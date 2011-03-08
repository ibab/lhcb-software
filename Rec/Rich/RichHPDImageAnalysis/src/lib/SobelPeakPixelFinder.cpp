
#include <cmath>
#include <iostream>
#include <algorithm>

// local
#include "RichHPDImageAnalysis/SobelPeakPixelFinder.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : SobelPeakPixelFinder
//
// 2011-03-02 : Chris Jones
//-----------------------------------------------------------------------------

SobelPeakPixelFinder::SobelPeakPixelFinder( const TH2* hist ,
                                            const Params& params )
  : m_hist          ( hist   ),
    m_params        ( params ) { }

SobelPeakPixelFinder::~SobelPeakPixelFinder() {}

void SobelPeakPixelFinder::findBoundary( Pixel::List & boundary ) const
{
  if ( m_hist )
  {
    boundary.clear() ;

    const int nbins         = m_hist->GetNbinsX()  * m_hist->GetNbinsY();
    const double global_thr = m_params.cutFraction * m_hist->Integral() / (1.0*nbins);

    for ( int icol = 0 ; icol < m_hist->GetNbinsX() ; ++icol )
    {
      for ( int irow = 0; irow < m_hist->GetNbinsY() ; ++irow )
      {
        //const double local_thr = localThreshold(icol,irow);
        if ( isPeak( icol, irow, global_thr ) )
        {
          boundary.push_back( Pixel(icol,irow,m_hist->GetBinContent(icol+1,irow+1)) );
        }
      }
    }

    // Sort
    std::sort ( boundary.begin(), boundary.end() );

    // Enough hits
    if ( boundary.size() < m_params.minBoundary )
    {
      boundary.clear();
    }

  }
}

double SobelPeakPixelFinder::localThreshold( const int COL, 
                                             const int ROW,
                                             const int area ) const
{
  double threshold(0);
  unsigned int sum(0);
  for ( int icol = COL-area; icol <= COL+area ; ++icol )
  {
    for ( int irow = ROW-area; irow <= ROW+area ; ++irow )
    {
      if ( COL == icol && ROW == irow ) { continue ; }
      if ( icol >= 0 && icol < m_hist->GetNbinsX() &&
           irow >= 0 && irow < m_hist->GetNbinsY() )
      {
        threshold += m_hist->GetBinContent(icol+1,irow+1);
        ++sum;
      }
    }
  }
  return ( sum>0 ? m_params.cutFraction*threshold/(double)sum : 0.0 );
}

bool SobelPeakPixelFinder::isPeak( const int COL,
                                   const int ROW,
                                   const double thr ) const
{
  const double binC = m_hist->GetBinContent(COL+1,ROW+1);
  if ( binC < thr ) return false;

  bool peak = true;
  for ( int icol = COL-1; icol <= COL+1 ; ++icol )
  {
    if ( COL == icol ) { continue ; }
    if ( icol >= 0 && icol < m_hist->GetNbinsX() )
    {
      if ( m_hist->GetBinContent( icol+1, ROW+1 ) > binC )
      {
        peak = false;
        break;
      }
    }
  }

  if ( !peak )
  {
    peak = true;
    for ( int irow = ROW-1; irow <= ROW+1 ; ++irow )
    {
      if ( ROW == irow ) { continue ; }
      if ( irow >= 0 && irow < m_hist->GetNbinsY() )
      {
        if ( m_hist->GetBinContent( COL+1, irow+1 ) > binC )
        {
          peak = false;
          break;
        }
      }
    }
  }

  return peak;
}


