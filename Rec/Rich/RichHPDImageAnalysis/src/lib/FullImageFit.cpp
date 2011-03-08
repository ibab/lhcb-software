
#include <cmath>
#include <iostream>

// local
#include "RichHPDImageAnalysis/FullImageFit.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : FullImageFit
//
// 2011-03-05 : Chris Jones
//-----------------------------------------------------------------------------

FullImageFit::FullImageFit( const TH2* hist )
  : m_errDef     ( 1.   ),
    m_hist       ( hist ),
    m_sf ( hist ? (1.0*hist->GetNbinsX())/(1.0*hist->GetNbinsY()) : 0.0 )
{ }

FullImageFit::~FullImageFit() {}

//=============================================================================

double FullImageFit::operator()( const std::vector<double>& par ) const
{
  double chi2 = 0.0;

  // Loop over all bins
  for ( int i = 0; i < m_hist->GetNbinsX(); ++i )
  {
    for ( int  j = 0; j < m_hist->GetNbinsY(); ++j )
    {
      const double deltaCol = ( 1.0  * i ) - par[0];
      const double deltaRow = ( m_sf * j ) - par[1];
      const double dist = std::sqrt( deltaCol*deltaCol + deltaRow*deltaRow );
      chi2 += m_hist->GetBinContent(i,j) * std::pow( dist - par[2] , 2 ) * 12.0;
    }
  }

  return chi2 ;
}
