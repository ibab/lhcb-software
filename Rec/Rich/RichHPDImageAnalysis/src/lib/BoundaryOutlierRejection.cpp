
#include <cmath>

#include "RichHPDImageAnalysis/BoundaryOutlierRejection.h"

using namespace Rich::HPDImage;

//-----------------------------------------------------------------------------
// Implementation file for class : BoundaryOutlierRejection
//
// 2011-03-07 : Chris Jones
//-----------------------------------------------------------------------------

BoundaryOutlierRejection::BoundaryOutlierRejection( const TH2* hist,
                                                    const Pixel::List& list,
                                                    const FitResult& result,
                                                    const Params& params )
  : m_boundary ( list   ),
    m_result   ( result ),
    m_params   ( params ),
    m_sf ( hist ? (1.0*hist->GetNbinsX())/(1.0*hist->GetNbinsY()) : 0.0 )
{ }

void BoundaryOutlierRejection::reject( Pixel::List & outliersRemoved ) const
{
  outliersRemoved.clear();
  outliersRemoved.reserve(m_boundary.size());

  // Loop over input list and save those that pass
  for ( Pixel::List::const_iterator iter = m_boundary.begin();
        iter != m_boundary.end(); ++iter )
  {
    const double deltaCol = ( 1.0  * iter->col ) - m_result.col;
    const double deltaRow = ( m_sf * iter->row ) - m_result.row;
    const double dist = std::fabs( m_result.radius -
                                   std::sqrt( deltaCol*deltaCol + deltaRow*deltaRow ) );
    if ( dist < m_params.maxDistance )
    {
      outliersRemoved.push_back(*iter);
    }
  }

}
