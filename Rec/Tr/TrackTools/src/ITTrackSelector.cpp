//-----------------------------------------------------------------------------
/** @file ITTrackSelector.cpp
 *
 *  Implementation file for reconstruction tool : ITTrackSelector
 *
 *  CVS Log :-
 *  $Id: ITTrackSelector.cpp,v 1.1 2010-04-15 09:26:43 wouter Exp $
 *
 *  @author Wouter Hulsbergen
 *  @date   2010
 */
//-----------------------------------------------------------------------------

#include "GaudiAlg/GaudiTool.h"

// STL
#include <string>

#include "TrackSelector.h"

#include "Event/Track.h"

class ITTrackSelector : public TrackSelector
{

public:

  /// constructer
  ITTrackSelector( const std::string& type,
		   const std::string& name,
		   const IInterface* parent );
  
  virtual ~ITTrackSelector();

  /** Returns if the given track is selected or not
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  virtual bool accept ( const LHCb::Track & aTrack ) const;

protected:
  bool computeModuleOverlap( const LHCb::Track & aTrack ) const;

private:
  size_t m_minHitsTop ;
  size_t m_minHitsBottom ;
  size_t m_minHitsA ;
  size_t m_minHitsC ;
  bool   m_requireBoxOverlap ;
  bool   m_requireModuleOverlap ;
};

DECLARE_TOOL_FACTORY( ITTrackSelector )

//-----------------------------------------------------------------------------

ITTrackSelector::ITTrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TrackSelector ( type, name, parent )
{
  declareProperty( "MinHitsTop",     m_minHitsTop = 0 ) ;
  declareProperty( "MinHitsBottom" , m_minHitsBottom = 0 ) ;
  declareProperty( "MinHitsA",       m_minHitsA = 0 ) ;
  declareProperty( "MinHitsC" ,      m_minHitsC = 0 ) ;
  declareProperty( "RequireOverlap" , m_requireBoxOverlap = false ) ;
  declareProperty( "RequireModuleOverlap" , m_requireModuleOverlap = false ) ;
}

ITTrackSelector::~ITTrackSelector() { }

bool ITTrackSelector::accept ( const LHCb::Track& aTrack ) const
{
  size_t numIT[4] = {0,0,0,0} ;
  size_t numOT[2] = {0,0} ;
  for(int i=0; i<4; ++i) numIT[i] = 0 ;
  for( std::vector<LHCb::LHCbID>::const_iterator
	 it = aTrack.lhcbIDs().begin() ;
       it != aTrack.lhcbIDs().end(); ++it ) 
    if( it->isIT() ) {
      ++(numIT[it->stID().detRegion()-1]) ;
    } else if( it->isOT() ) {
      LHCb::OTChannelID otid = it->otID() ;
      ++numOT[ otid.quarter()%2 ] ;
    }
  
  size_t sumit = numIT[0] + numIT[1] + numIT[2] + numIT[3] ;
  size_t sumot = numOT[0] + numOT[1] ;
  size_t sum   = sumot + sumit ;
  //bool acceptitoverlap = !m_requireITOverlap ||
  //  (sumit > numIT[0] && sumit > numIT[1] && sumit > numIT[2] && sumit > numIT[3] ) ;
  //bool acceptotoverlap = !m_requireOTOverlap ||
  //  (sumot > numOT[0] && sumot > numOT[1]) ;
  bool acceptBoxOverlap = 
    (sum > numOT[0] && sum > numOT[1] && 
     sum > numIT[0] && sum > numIT[1] && sum > numIT[2] && sum > numIT[3]) ;
  bool acceptModuleOverlap = m_requireModuleOverlap ? computeModuleOverlap( aTrack) : false ;
  
  return 
    numIT[3] >= m_minHitsTop &&
    numIT[2] >= m_minHitsBottom &&
    numIT[1] >= m_minHitsA &&
    numIT[0] >= m_minHitsC &&
    ( (m_requireBoxOverlap && acceptBoxOverlap ) ||
      (m_requireModuleOverlap && acceptModuleOverlap) ||
      (!m_requireBoxOverlap && !m_requireModuleOverlap ) ) &&
    TrackSelector::accept( aTrack );
}

bool ITTrackSelector::computeModuleOverlap ( const LHCb::Track& aTrack ) const
{
  // this routine checks if this track has hits in two neighbouring
  // modules (to select module overlaps for alignment)

  // this can be written a lot faster
  std::vector< LHCb::STChannelID > ithits ;
  for( std::vector<LHCb::LHCbID>::const_iterator it = aTrack.lhcbIDs().begin() ;
       it != aTrack.lhcbIDs().end(); ++it ) 
    if( it->isIT() ) ithits.push_back( it->stID() ) ;
  
  bool foundoverlap(false) ;
  for( std::vector< LHCb::STChannelID >::const_iterator it = ithits.begin() ;
       it != ithits.end() && !foundoverlap; ++it) {
    unsigned int iuniquelayer = it->detRegion()-1 + 4*( it->layer() -1 + 4* (it->station()-1) ) ;
    for( std::vector< LHCb::STChannelID >::const_iterator jt = ithits.begin() ;
	 jt != it && !foundoverlap ; ++jt ) {
      unsigned int juniquelayer = jt->detRegion()-1 + 4*( jt->layer() -1 + 4* (jt->station()-1) ) ;
      foundoverlap = foundoverlap ||
	( iuniquelayer == juniquelayer &&  it->sector() != jt->sector() ) ;
    }
  }
  return foundoverlap ;
}
