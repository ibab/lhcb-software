
#include <cmath>
#include <iostream>

// local
#include "RichHPDImageAnalysis/SimpleChi2Fit.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : SimpleChi2Fit
//
// 2011-03-05 : Chris Jones
//-----------------------------------------------------------------------------

SimpleChi2Fit::SimpleChi2Fit( const TH2* hist ,
                              const Pixel::List& list )
  : m_errDef     ( 1.    ),
    m_sf ( hist ? (1.0*hist->GetNbinsX())/(1.0*hist->GetNbinsY()) : 0.0 ),
    m_boundary   ( &list )
{ }

SimpleChi2Fit::~SimpleChi2Fit() {} 

//=============================================================================

double SimpleChi2Fit::operator()( const std::vector<double>& par ) const
{
  double chi2 = 0.0;

  for ( Pixel::List::const_iterator iter = m_boundary->begin();
        iter != m_boundary->end(); ++iter )
  {
    const double deltaCol = ( 1.0  * iter->col ) - par[0];
    const double deltaRow = ( m_sf * iter->row ) - par[1];
    const double dist = std::sqrt( deltaCol*deltaCol + deltaRow*deltaRow );
    chi2 += iter->intensity * std::pow( dist - par[2] , 2 ) * 12.0;
  }

  return chi2 ;
}
