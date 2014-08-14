
#include "GaudiAlg/GaudiHistoAlg.h"
#include "AlElementHistos.h"
#include "AlignmentElement.h"
#include "AIDA/IHistogram1D.h"
//#include "AIDA/IHistogram2D.h"
#include "boost/lexical_cast.hpp"

AlElementHistos::AlElementHistos(GaudiHistoAlg& parent, const AlignmentElement& elem, size_t numiter) 
{
  //size_t index_i         = elem.index() ;
  const std::string name = elem.name(); 
  std::string dirname =  "element" + boost::lexical_cast<std::string>( elem.index() ) + "/"; //name + "/" ;
  double resmax = elem.histoResidualMax() ;
  m_resHisto      = parent.book2D(dirname + boost::lexical_cast<std::string>(1000u), 
				  "Residual vs iteration for " + name,
				  -0.5, numiter-0.5, numiter, -resmax , +resmax, 100);
  m_unbiasedResHisto      = parent.book2D(dirname + boost::lexical_cast<std::string>(1001u), 
					  "Unbiased Residual vs iteration for " + name,
					  -0.5, numiter-0.5, numiter, -resmax , +resmax, 100);
  m_pullHisto     = parent.book2D(dirname + boost::lexical_cast<std::string>(2000u),
				  "Pull vs iteration for " + name,
				  -0.5, numiter-0.5, numiter, -5.00 , +5.00, 100);
  m_nHitsHisto    = parent.book1D(dirname + boost::lexical_cast<std::string>(3000u), 
				  "Number of hits vs iteration for " + name, 
				  -0.5, numiter-0.5, numiter);
  //   m_deltaTxHisto  = parent.book1D(dirname + boost::lexical_cast<std::string>(10000u),
  // 				  "Delta Tx vs iteration for " + name,
  // 				  -0.5, numiter-0.5, numiter);
  //   m_deltaTyHisto  = parent.book1D(dirname + boost::lexical_cast<std::string>(20000u),
  // 				  "Delta Ty vs iteration for " + name,
  // 				  -0.5, numiter-0.5, numiter);
  //   m_deltaTzHisto  = parent.book1D(dirname + boost::lexical_cast<std::string>(30000u),
  // 				  "Delta Tz vs iteration for " + name,
  // 				  -0.5, numiter-0.5, numiter);
  //   m_deltaRxHisto  = parent.book1D(dirname + boost::lexical_cast<std::string>(40000u),
  // 				  "Delta Rx vs iteration for " + name,
  // 				  -0.5, numiter-0.5, numiter);
  //   m_deltaRyHisto  = parent.book1D(dirname + boost::lexical_cast<std::string>(50000u),
  // 				  "Delta Ry vs iteration for " + name,
  // 				  -0.5, numiter-0.5, numiter);
  //   m_deltaRzHisto  = parent.book1D(dirname + boost::lexical_cast<std::string>(60000u),
  // 				  "Delta Rz vs iteration for " + name,
  // 				  -0.5, numiter-0.5, numiter);
  m_autoCorrHisto = parent.book2D(dirname + boost::lexical_cast<std::string>(4000u),
				  "hit residual scale factor in " + name + "  vs iteration ",
				  -0.5, numiter-0.5, numiter,-1, 1, 100);
  
  size_t nbins = 50 ;
  double xmax = 5 ;
  m_residualPullHistos[0] = parent.book1D(dirname + "ResPullTx","Residual pull Tx",-xmax,xmax,nbins) ;
  m_residualPullHistos[1] = parent.book1D(dirname + "ResPullTy","Residual pull Ty",-xmax,xmax,nbins) ;
  m_residualPullHistos[2] = parent.book1D(dirname + "ResPullTz","Residual pull Tz",-xmax,xmax,nbins) ;
  m_residualPullHistos[3] = parent.book1D(dirname + "ResPullRx","Residual pull Rx",-xmax,xmax,nbins) ;
  m_residualPullHistos[4] = parent.book1D(dirname + "ResPullRy","Residual pull Ry",-xmax,xmax,nbins) ;
  m_residualPullHistos[5] = parent.book1D(dirname + "ResPullRz","Residual pull Rz",-xmax,xmax,nbins) ;
}

void AlElementHistos::reset()
{
  // only reset histograms that don't go by 'iteration'
  for( size_t ipar=0; ipar<6; ++ipar)
    m_residualPullHistos[ipar]->reset() ;
}
