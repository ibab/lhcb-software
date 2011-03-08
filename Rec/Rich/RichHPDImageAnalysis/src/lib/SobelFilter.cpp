
#include <iostream>
#include <cmath>
#include <sstream>

// local
#include "RichHPDImageAnalysis/SobelFilter.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : SobelFilter
//
// 2011-03-07 : Chris Jones
//-----------------------------------------------------------------------------

TH2D* SobelFilter::filter() const
{
  TH2D * sobelHist(NULL);

  if ( m_inHist )
  {

    // construct unique ID each time ...
    // nonsense to keep ROOT happy
    static unsigned int iH(0);
    std::ostringstream id;
    id << m_inHist->GetName() << "_SobelFiltered_" << ++iH;

    // make a new histogram for the Sobel filtered image
    sobelHist = new TH2D ( id.str().c_str(),
                           (std::string(m_inHist->GetTitle())+"_SobelFiltered").c_str(),
                           m_inHist->GetNbinsX(),
                           m_inHist->GetXaxis()->GetXmin(),
                           m_inHist->GetXaxis()->GetXmax(),
                           m_inHist->GetNbinsY(),
                           m_inHist->GetYaxis()->GetXmin(),
                           m_inHist->GetYaxis()->GetXmax() );

    for ( int i = 1; i < m_inHist->GetNbinsX()-1; ++i )
    {
      for ( int j = 1; j < m_inHist->GetNbinsY()-1; ++j )
      {

        // ignore centre region
        if ( std::sqrt(std::pow(i-15.5,2)+std::pow(j-15.5,2)) > m_params.centreRegionSize )
        {
          double array[3][3];
          array[0][0] = m_inHist->GetBinContent(i  , j+2);    // top left
          array[0][1] = m_inHist->GetBinContent(i+1, j+2);    // above
          array[0][2] = m_inHist->GetBinContent(i+2, j+2);    // top right
          array[1][0] = m_inHist->GetBinContent(i  , j+1);    // left
          array[1][1] = m_inHist->GetBinContent(i+1, j+1);    // centre
          array[1][2] = m_inHist->GetBinContent(i+2, j+1);    // right
          array[2][0] = m_inHist->GetBinContent(i  , j  );    // bottom left
          array[2][1] = m_inHist->GetBinContent(i+1, j  );    // bottom
          array[2][2] = m_inHist->GetBinContent(i+2, j  );    // bottom right
          const double new_intensity_x = array[0][0] + 2*array[0][1] + array[0][2] - array[2][0] - 2*array[2][1] - array[2][2];
          const double new_intensity_y = array[0][2] + 2*array[1][2] + array[2][2] - array[0][0] - 2*array[1][0] - array[2][0];
          const double new_intensity   = std::sqrt( std::pow(new_intensity_x,2) + std::pow(new_intensity_y,2) );
          sobelHist->Fill( i, j, new_intensity );
        }

      }
    }

  }
  return sobelHist;
}
