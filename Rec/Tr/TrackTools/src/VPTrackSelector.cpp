//-----------------------------------------------------------------------------
/** @file VPTrackSelector.cpp
 *
 *  Implementation file for reconstruction tool : VPTrackSelector
 *
 *  CVS Log :-
 *  $Id: VeloTrackSelector.cpp,v 1.1 2015-01-22 09:25:40 chombach Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Christoph Hombach Christoph.Hombach@hep.manchester.ac.uk 
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiAlg/GaudiTool.h"

// STL
#include <string>

#include "TrackSelector.h"

class VPTrackSelector : public TrackSelector
{

public:

  /// constructer
  VPTrackSelector( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );
  
  virtual ~VPTrackSelector();

  /** Returns if the given track is selected or not
   *
   *  @param aTrack Reference to the Track to test
   *
   *  @return boolean indicating if the track is selected or not
   *  @retval true  Track is selected
   *  @retval false Track is rejected
   */
  virtual bool accept ( const LHCb::Track & aTrack ) const;

private:
  size_t m_minHitsASide ;
  size_t m_minHitsCSide ;
};

DECLARE_TOOL_FACTORY( VPTrackSelector )

//-----------------------------------------------------------------------------

VPTrackSelector::VPTrackSelector( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : TrackSelector ( type, name, parent )
{
  declareInterface<ITrackSelector>(this);
  declareProperty( "MinHitsASide" , m_minHitsASide = 0 ) ;
  declareProperty( "MinHitsCSide" , m_minHitsCSide = 0 ) ;
}

VPTrackSelector::~VPTrackSelector() { }

bool VPTrackSelector::accept ( const LHCb::Track& aTrack ) const
{
  size_t numHits[2] = {0,0} ;
  for( std::vector<LHCb::LHCbID>::const_iterator 
	 it = aTrack.lhcbIDs().begin() ;
       it != aTrack.lhcbIDs().end(); ++it ) 
    if( it->isVP() ) {
      LHCb::VPChannelID vpid = it->vpID() ;
      unsigned int side    = (vpid.sensor()%2) ; 
      ++numHits[side] ;
    }
  return 
    numHits[0] >= m_minHitsASide &&
    numHits[1] >= m_minHitsCSide && 
    TrackSelector::accept(aTrack) ;
}

