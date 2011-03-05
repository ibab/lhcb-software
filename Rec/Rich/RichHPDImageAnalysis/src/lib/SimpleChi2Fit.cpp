
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
  : m_errDef     ( 1.   ),
    m_hist       ( hist ),
    m_sf ( hist ? (1.0*hist->GetNbinsX())/(1.0*hist->GetNbinsY()) : 0.0 ),
    m_boundary   ( list )
{ }

SimpleChi2Fit::~SimpleChi2Fit() {} 

//=============================================================================

double SimpleChi2Fit::operator()( const std::vector<double>& par ) const
{
  double chi2 = 0.0;

  //std::cout << "Params " << par[0] << " " << par[1] << " " << par[2] << std::endl;

  for ( Pixel::List::const_iterator iter = m_boundary.begin();
        iter != m_boundary.end(); ++iter )
  {
    //std::cout << "  Pixel " << iter->col << " " << iter->row << std::endl;

    const double deltaCol = ( 1.0  * iter->col ) - par[0];
    const double deltaRow = ( m_sf * iter->row ) - par[1];
    const double dist = std::sqrt( deltaCol*deltaCol + deltaRow*deltaRow );
    chi2 += std::pow( dist - par[2] , 2 ) * 12.0;

  }

  //std::cout << "Chi^2 = " << chi2 << std::endl;
  return chi2 ;
}
