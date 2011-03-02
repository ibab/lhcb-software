
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
  : m_errDef     ( 1.   ),
    m_threshold  ( thr  ),
    m_hist       ( hist ),
    m_sf ( hist ? (1.0*hist->GetNbinsX())/(1.0*hist->GetNbinsY()) : 0.0 )
{ }

HPDBoundaryFcn::~HPDBoundaryFcn() {}

unsigned int HPDBoundaryFcn::findBoundary() const
{
  if ( NULL == m_hist ) return 0 ;
  m_boundary.clear() ;

  const int nbins  = m_hist->GetNbinsX()*m_hist->GetNbinsY();
  const double thr = m_threshold*m_hist->Integral()/(1.0*nbins);

  for ( int icol = 0 ; icol < m_hist->GetNbinsX() ; ++icol )
  {
    int ROW0(-1), ROW1(-1);

    for ( int irow = 0; irow <m_hist->GetNbinsY() ; ++irow )
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
      m_boundary.push_back( std::make_pair( icol, ROW0 ) );
    }
    if ( -1 != ROW1 && ROW1 != ROW0 )
    {
      m_boundary.push_back( std::make_pair( icol, ROW1 ) );
    }
  }

  return m_boundary.size() ;
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
        if ( m_hist->GetBinContent( icol+1, irow+1 ) > thr ) return true ;
      }
    }
  }
  return false;
}

double HPDBoundaryFcn::operator()( const std::vector<double>& par ) const
{
  double chi2 = 0.0;
  for ( std::vector< std::pair<int,int> >::const_iterator iter = m_boundary.begin();
        iter != m_boundary.end(); ++iter )
  {
    const double deltaCol = (1.0*iter->first) - par[0];
    const double deltaRow = (m_sf*iter->second) - par[1];
    const double dist = std::sqrt( deltaCol*deltaCol + deltaRow*deltaRow );
    chi2 += ( dist-par[2] )*( dist-par[2] )*12.0;
  }
  return chi2 ;
}
