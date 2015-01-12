#ifndef OTMONOLAYERALIGNDATA_H
#define OTMONOLAYERALIGNDATA_H

#include <array>
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/DataObject.h"

namespace Al
{
  class OTMonoLayerAlignModuleData
  {
  public:
    //typedef Gaudi::Vector2      Parameters ;
    enum { N = 6 } ; // we split the segment in three pieces
    enum Ambiguity { Left = 0, Right = 1 } ;
    typedef ROOT::Math::SMatrix<double,N,1> Parameters ;
    typedef Gaudi::SymMatrix6x6 Covariance ;
  private:
    Parameters m_halfdchi2dalpha[2] ;
    Covariance m_halfd2chi2dalpha2[2] ;
    Parameters m_halfdchi2dalphaunweighted ;
    size_t m_numhits[2] ;
    double m_sumw ;
    double m_sumy ;
    double m_sumbeta ; // beta is the position along the wire between 0 and 1
    double m_sumwbeta ;
  public:
  OTMonoLayerAlignModuleData() : 
    m_numhits{0,0}, m_sumw{0}, m_sumy{0}, m_sumbeta{0}, m_sumwbeta{0} {}

    // setters and getters
    Parameters& halfdchi2dalpha(int ambiguity) { return m_halfdchi2dalpha[ambiguity] ; }
    const Parameters& halfdchi2dalpha(int ambiguity) const { return m_halfdchi2dalpha[ambiguity] ; }
    Parameters& halfdchi2dalphaunweighted() { return m_halfdchi2dalphaunweighted ; }
    const Parameters& halfdchi2dalphaunweighted() const { return m_halfdchi2dalphaunweighted ; }
    Covariance& halfd2chi2dalpha2(int ambiguity) { return m_halfd2chi2dalpha2[ambiguity] ; }
    const Covariance& halfd2chi2dalpha2(int ambiguity) const { return m_halfd2chi2dalpha2[ambiguity] ; }
    double avy() const { return numhits()>0 ? m_sumy/numhits() : 0  ; }
    double avbeta() const { return numhits()>0 ? m_sumbeta/numhits() : 0  ; }
    double avwbeta() const { return m_sumw>0 ? m_sumwbeta/m_sumw : 0  ; }
    double sumw() const { return m_sumw ; }
    double avw() const { return numhits()>0 ? m_sumw /numhits() : 0 ; }
    size_t numhits() const { return m_numhits[0] + m_numhits[1] ; }
    size_t numhits(int ambiguity) const { return m_numhits[ambiguity] ; }

    /// reset contents
    void reset() ;

    /// add information for counters
    void add(double y, double beta, int ambiguity, double weight = 1 ) ;
    
    /// add another object of this type
    void add( const OTMonoLayerAlignModuleData& rhs ) ;
  } ;

  class OTMonoLayerAlignData : public DataObject
  {
  private:
    enum {N = 432 } ;
    std::array<OTMonoLayerAlignModuleData,N> m_modules ;
    size_t m_nEvents ;
    size_t m_nTracks ;
    size_t m_nTracksWithOTHits ;
    size_t m_nOTHits ;
    size_t m_nOTHitsWithTime ;
    size_t m_nUsedOTHits ;
    double m_totalChisq ;
    size_t m_totalNDoF ;
  public:
    ///
    OTMonoLayerAlignData() ;

    /// get the data for module
    OTMonoLayerAlignModuleData& module( unsigned int index) {
      return m_modules[index] ;
    }

    /// get the data for a module
    const OTMonoLayerAlignModuleData& module( unsigned int index ) const {
      return m_modules[index] ;
    }
    
    size_t nTracks() const { return m_nTracks ; }
    size_t nTracksWithOTHits() const { return m_nTracksWithOTHits ; }
    size_t nEvents() const { return m_nEvents ; }
    size_t nOTHits() const { return m_nOTHits ; }
    size_t nOTHitsWithTime() const { return m_nOTHitsWithTime ; }
    size_t nUsedOTHits() const { return m_nUsedOTHits ; }

    /// reset data contents
    void reset() ; 
    
    /// add another object of this type
    void add( const OTMonoLayerAlignData& rhs ) ;

    /// needs also function with capital because GaudyPthon.Parallel uses that
    void Add(const OTMonoLayerAlignData& rhs) {
      add( rhs ) ;
    }
    
    void addTrack( double chi2, double ndof, bool hasothits ) {
      ++m_nTracks ;
      if( hasothits ) ++m_nTracksWithOTHits ;
      m_totalChisq += chi2 ;
      m_totalNDoF += ndof ;
    }

    void addEvent( size_t nothits,
		   size_t nothitswithtime,
		   size_t nusedothits )
    {
      ++m_nEvents ;
      m_nOTHits += nothits ;
      m_nOTHitsWithTime += nothitswithtime ;
      m_nUsedOTHits += nusedothits ;
    }
    
    // temporary, for debugging:
    size_t hitsFromModules() const {
      size_t sum(0) ;
      for(int i=0; i<N; ++i) sum += m_modules[i].numhits() ;
      return sum ;
    }
  } ;
}


#endif
