
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>
#include <algorithm>

#include "boost/assign/list_of.hpp"

// local
#include "RichHPDImageAnalysis/CleanHPDImage.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : CleanHPDImage
//
// 2011-03-07 : Chris Jones
//-----------------------------------------------------------------------------

TH2D* Clean::filter() const
{
  TH2D * cleanedHist(NULL);

  // reset
  reset();

  if ( m_inHist )
  {

    // construct unique ID each time ...
    // nonsense to keep ROOT happy
    static unsigned int iH(0);
    std::ostringstream id;
    id << m_inHist->GetName() << "_Cleaned_" << ++iH;

    // make a new histogram for the cleaned image
    cleanedHist = new TH2D ( id.str().c_str(),
                             (std::string(m_inHist->GetTitle())+"_Cleaned").c_str(),
                             m_inHist->GetNbinsX(),
                             m_inHist->GetXaxis()->GetXmin(),
                             m_inHist->GetXaxis()->GetXmax(),
                             m_inHist->GetNbinsY(),
                             m_inHist->GetYaxis()->GetXmin(),
                             m_inHist->GetYaxis()->GetXmax() );

    // Get total content
    const double totalSum = m_inHist->Integral();
    if ( totalSum > 0 )
    {

      // Loop once to fill info on column/row occupancies
      std::map<unsigned int,double> xCount,yCount;
      double cleanedSum(0);
      for ( int i = 0; i < m_inHist->GetNbinsX(); ++i )
      {
        for ( int  j = 0; j < m_inHist->GetNbinsY(); ++j )
        {
          const double bin_cont = m_inHist->GetBinContent(i+1,j+1);
          if ( bin_cont > m_params.minBinContent )
          {
            // Exclude 'hot' pixels
            if ( m_nEvents == 0 ||
                 (double)bin_cont/(double)m_nEvents < m_params.maxEventOcc )
            {
              xCount[i]  += bin_cont;
              yCount[j]  += bin_cont;
              cleanedSum += bin_cont;
            }
            else
            {
              excludePixel(i,j);
            }
          }
        }
      }

      // bin content if totally averaged out
      m_avPixCont = ( cleanedSum /
                      (double)( m_inHist->GetNbinsX() * m_inHist->GetNbinsY() ) );

      // First Loop to detect bad pixels to exclude
      for ( int i = 0; i < m_inHist->GetNbinsX(); ++i )
      {
        for ( int  j = 0; j < m_inHist->GetNbinsY(); ++j )
        {
          // Is Pixel OK
          bool pixOK = !isExcluded(i,j);
          if ( !pixOK ) { continue; }

          // Bin content
          const double bin_cont = m_inHist->GetBinContent(i+1,j+1);

          // Skip 'hot' pixels
          pixOK &= ( bin_cont/m_avPixCont < m_params.hotBinFactor );
          if ( pixOK && m_nEvents > 0 )
          {
            // use number of events to clean
            const double frac = (double)bin_cont/(double)m_nEvents;
            pixOK &= ( frac < m_params.maxEventOcc );
          }

          // skip dead columns
          pixOK &= ( xCount[i] > 0 && yCount[j] > 0 );

          // skip very hot columns/rows
          pixOK &= ( xCount[i]/cleanedSum < m_params.hotRowColFraction );
          pixOK &= ( yCount[j]/cleanedSum < m_params.hotRowColFraction );

          // Skip hot 'bi' columns or rows
          pixOK &= ( (xCount[i]+xCount[i+1])/cleanedSum < m_params.hotBiRowColFraction );
          pixOK &= ( (yCount[j]+yCount[j+1])/cleanedSum < m_params.hotBiRowColFraction );

          // ignore centre region
          pixOK &= ( std::sqrt(std::pow(i-15.5,2)+std::pow(j-15.5,2)) > m_params.centreRegionSize );

          // update flags to excluded bad pixels from averages etc.
          if ( !pixOK ) { excludePixel(i,j); }

        }
      }

      // Second bad pixel loop, that requires avFromNeighbours
      for ( int i = 0; i < m_inHist->GetNbinsX(); ++i )
      {
        for ( int  j = 0; j < m_inHist->GetNbinsY(); ++j )
        {
          // Is Pixel OK
          bool pixOK = !isExcluded(i,j);
          if ( !pixOK ) { continue; }

          // Bin content
          const double bin_cont = m_inHist->GetBinContent(i+1,j+1);

          // dead pixels (empty, but neighbours have lots of hits)
          if ( bin_cont < m_params.minBinContent )
          {
            const double neigh_cont = avFromNeighbours(i,j);
            if ( neigh_cont > m_avPixCont * m_params.neighbourFracForDeadPix ) { pixOK = false; }
          }

          // update flags to excluded bad pixels from averages etc.
          if ( !pixOK ) { excludePixel(i,j); }

        }
      }

      // Final loop to fill the cleaned histogram
      for ( int i = 0; i < m_inHist->GetNbinsX(); ++i )
      {
        for ( int  j = 0; j < m_inHist->GetNbinsY(); ++j )
        {
          if ( !isExcluded(i,j) )
          {
            cleanedHist->Fill( i, j, m_inHist->GetBinContent(i+1,j+1) );
          }
          else if ( m_params.giveRemovedPixAvCont )
          {
            cleanedHist->Fill( i, j, avFromNeighbours(i,j) );
          }
        }
      }

    }

  }

  return cleanedHist;
}

double Clean::avFromNeighbours(  const int COL,
                                 const int ROW ) const
{
  double av(0);
  unsigned int nPixels(0);
  for ( int icol = COL-1; icol <= COL+1 ; ++icol )
  {
    for ( int irow = ROW-1; irow <= ROW+1 ; ++irow )
    {
      // skip pixel average is being computed for
      if ( COL == icol && ROW == irow ) { continue; }
      // skip excluded pixels
      if ( isExcluded(icol,irow)      ) { continue; }
      // add this pix to the average calculation
      av += m_inHist->GetBinContent( icol+1, irow+1 );
      ++nPixels;
    }
  }
  return ( nPixels > 0 ? av / (double)nPixels : 0.0 );
}
