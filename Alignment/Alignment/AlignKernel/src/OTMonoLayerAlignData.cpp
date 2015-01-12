#include "AlignKernel/OTMonoLayerAlignData.h"

namespace Al
{
  void OTMonoLayerAlignModuleData::reset()
  {
    m_halfdchi2dalpha[0] = Parameters() ;
    m_halfdchi2dalpha[1] = Parameters() ;
    m_halfd2chi2dalpha2[0] = Covariance() ;
    m_halfd2chi2dalpha2[1] = Covariance() ;
    m_halfdchi2dalphaunweighted = Parameters() ;
    m_numhits[0] = m_numhits[1] = 0 ;
    m_sumy = m_sumbeta = m_sumwbeta = m_sumw = 0 ;
  }
  
  void OTMonoLayerAlignModuleData::add(double y, double beta, int ambiguity, double weight)
  {
    m_numhits[ambiguity] += 1 ;
    m_sumy += y ;
    m_sumbeta += beta ;
    m_sumwbeta += weight * beta ;
    m_sumw += weight ;
  }
  
  void OTMonoLayerAlignModuleData::add( const OTMonoLayerAlignModuleData& rhs )
  {
    m_halfdchi2dalpha[0] += rhs.m_halfdchi2dalpha[0] ;
    m_halfdchi2dalpha[1] += rhs.m_halfdchi2dalpha[1] ;
    m_halfd2chi2dalpha2[0] += rhs.m_halfd2chi2dalpha2[0] ;
    m_halfd2chi2dalpha2[1] += rhs.m_halfd2chi2dalpha2[1] ;
    m_halfdchi2dalphaunweighted += rhs.m_halfdchi2dalphaunweighted ;
    m_numhits[0] += rhs.m_numhits[0] ;
    m_numhits[1] += rhs.m_numhits[1] ;
    m_sumw += rhs.m_sumw ;			      
    m_sumy += rhs.m_sumy ;
    m_sumbeta += rhs.m_sumbeta ;
    m_sumwbeta += rhs.m_sumwbeta ;
  }

  OTMonoLayerAlignData::OTMonoLayerAlignData()
  {
    reset() ;
  }
  
  void OTMonoLayerAlignData::reset() 
  {
    for(unsigned int i=0; i<N; ++i)
      m_modules[i].reset() ;
    m_nEvents = 0 ;
    m_nTracks = 0 ;
    m_nTracksWithOTHits = 0 ;
    m_nOTHits = 0 ;
    m_nOTHitsWithTime = 0 ;
    m_nUsedOTHits = 0 ;
    m_totalChisq = 0.0 ;
    m_totalNDoF = 0 ;
  }
  
  void OTMonoLayerAlignData::add( const OTMonoLayerAlignData& rhs ) 
  {
    for(unsigned int i=0; i<N; ++i)
      m_modules[i].add( rhs.m_modules[i] ) ;
    m_nEvents += rhs.m_nEvents ;
    m_nTracks += rhs.m_nTracks ;
    m_nTracksWithOTHits += rhs.m_nTracksWithOTHits ;
    m_nOTHits += rhs.m_nOTHits ;
    m_nOTHitsWithTime += rhs.m_nOTHitsWithTime ;
    m_nUsedOTHits += rhs.m_nUsedOTHits ;
    m_totalChisq += rhs.m_totalChisq ;
    m_totalNDoF += rhs.m_totalNDoF ;
  }
}


