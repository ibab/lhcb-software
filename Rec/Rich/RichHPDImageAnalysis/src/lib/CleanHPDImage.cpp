
#include <iostream>
#include <cmath>
#include <map>
#include <sstream>

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

  if ( m_inHist )
  {

    // construct unique ID each time ...
    // nonsense to keep ROOT happy
    static unsigned int iH(0);
    std::ostringstream id;
    id << m_inHist->GetName() << "_Cleaned_" << iH++; 

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

      // bin content if totally averaged out
      const double avBinCont = totalSum / (double) ( m_inHist->GetNbinsX() * m_inHist->GetNbinsY() );

      // Loop once to fill info on column/row occupancies
      std::map<unsigned int,unsigned int> xCount,yCount;
      for ( int i = 0; i < m_inHist->GetNbinsX(); ++i )
      {
        for ( int  j = 0; j < m_inHist->GetNbinsY(); ++j )
        {
          const double bin_cont = m_inHist->GetBinContent(i+1,j+1);
          if ( bin_cont > m_params.minBinContent )
          {
            ++xCount[i]; ++yCount[j];
          }
        }
      }

      // Loop over original bins
      for ( int i = 0; i < m_inHist->GetNbinsX(); ++i )
      {
        for ( int  j = 0; j < m_inHist->GetNbinsY(); ++j )
        {
          // Bin content
          const double bin_cont = m_inHist->GetBinContent(i+1,j+1);

          // Is Pixel OK by default.
          bool pixOK = true;

          // Skip 'hot' pixels
          pixOK &= ( bin_cont/avBinCont < m_params.hotBinFractor );

          // skip dead columns
          pixOK &= ( xCount[i] > 0 && yCount[j] > 0 );

          // skip very hot columns/rows
          pixOK &= ( xCount[i]/totalSum < m_params.hotRowColFraction &&
                     yCount[j]/totalSum < m_params.hotRowColFraction );

          // ignore centre region 
          pixOK &= ( std::sqrt(std::pow(i-15.5,2)+std::pow(j-15.5,2)) > m_params.centreRegionSize );

          // dead pixels (empty, but neighbours have lots of hits)
          if ( pixOK && bin_cont < m_params.minBinContent )
          {
            const double neigh_cont = avFromNeighbours(i,j);
            if ( neigh_cont > avBinCont * m_params.neighbourFracForDeadPix )
            {
              // pixel is dead Jim
              pixOK = false;
            }
          }
 
          // Fill into final plot
          if ( pixOK ) 
          {
            cleanedHist->Fill( i, j, bin_cont );
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
      if ( COL == icol && ROW == irow ) { continue ; }
      av += m_inHist->GetBinContent( icol+1, irow+1 );
      ++nPixels;
    }
  }
  return ( nPixels > 0 ? av / (double)nPixels : 0.0 );
}
