#ifndef Alignment_ATrackSelector_H
#define Alignment_ATrackSelector_H
//-----------------------------------------------------------------------------
/** @file ATrackSelector.h
 *
 *  Header file for Tstation alignment : ATrackSelector
 *
 *  CVS Log :-
 *  $Id: ATrackSelector.h,v 1.6 2010-02-05 16:47:54 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   31/09/2006
 */
//-----------------------------------------------------------------------------


#include "GaudiAlg/GaudiTool.h"
#include "AlignmentInterfaces/IATrackSelectorTool.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackCaloMatch.h"

#include <string>

// GaudiKernel
#include "GaudiKernel/HashMap.h"
#include "Kernel/OTChannelID.h"

#include "Event/Track.h"
#include "Event/OTMeasurement.h"

// boost
#include "boost/assign/list_of.hpp"
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

class ATrackSelector : public GaudiTool, 
  virtual public IATrackSelectorTool {
  
 public:
  
  /// constructer
   ATrackSelector( const std::string& type,
		   const std::string& name,
		   const IInterface* parent);
   
   virtual ~ATrackSelector(); // Destructor
   
   /** Returns if the given track is selected or not
    *
    *  @param aTrack Reference to the Track to test
    *
    *  @return boolean indicating if the track is selected or not
    *  @retval true  Track is selected
    *  @retval false Track is rejected
    */
   StatusCode initialize();
   StatusCode finalize();
   bool accept ( const LHCb::Track& aTrack );
   int traversesIT( LHCb::Track & aTrack, 
		    int& nOThits, 
		    int& nIThits ) const;
   
   bool countTHits( const LHCb::Track & aTrack,
		    int& nOThits,
		    int& nIThits,
		    int& nTThits);
 private :
   bool uniformTD( const LHCb::OTChannelID & );
   inline int uniqueLayer(const LHCb::OTChannelID& channelID) const {
       return (channelID.station() - 1) * 4 + channelID.layer();
     };
   inline int uniqueQuarter(const LHCb::OTChannelID& channelID) const {
       return uniqueLayer(channelID) * 4 + channelID.quarter();
     };
   inline int uniqueModule(const LHCb::OTChannelID& channelID) const {
       return uniqueQuarter(channelID) * 9 + channelID.module() - 1;
     };
   
 private:
   //Interfaces:
   IATrackSelectorTool* m_trackselector;    
   ITrackCaloMatch *m_trackenergy;    
   
   double m_minChi2Cut; // Min chi^2 cut
   double m_minPCut;    // Min p cut
   double m_minPtCut;    // Min pt cut
   int m_charge;    // select particles with certain charge only
   double m_maxChi2Cut;    // Max chi^2 cut
   double m_maxPCut;    // Max p cut
   double m_maxPtCut;    // Max pt cut
   double m_minITHitCut;    // Min hit cut
   double m_minOTHitCut;    // Min hit cut
   double m_minTTHitCut;    // Min hit cut
   bool m_weights; // attempt to create uniform track distrubution in (OT)
   int m_uniCut; // module IDs larger than m_uniCut will be uniformed.
   double m_energyMinCut;    // Min energy cut
   
   // Track types to accept
   typedef std::vector<std::string> TrackTypes;
   TrackTypes m_trTypes; // List of track types to select
     
   // Mapping type linking track types to selection boolean
   typedef GaudiUtils::HashMap < const LHCb::Track::Types, bool > SelTypes;
   SelTypes m_selTypes;  // Mapping linking track types to selection boolean
   int m_uniform[3][4][4][9];
};

#endif // TRACKTOOLS_JBSelector_H
