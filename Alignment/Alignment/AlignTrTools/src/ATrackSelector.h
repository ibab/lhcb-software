#ifndef Alignment_ATrackSelector_H
#define Alignment_ATrackSelector_H
//-----------------------------------------------------------------------------
/** @file ATrackSelector.h
 *
 *  Header file for Tstation alignment : ATrackSelector
 *
 *  CVS Log :-
 *  $Id: ATrackSelector.h,v 1.9 2010-04-22 09:47:25 jblouw Exp $
 *
 *  @author J. Blouw johan.blouw@cern.ch
 *  @date   31/09/2006
 */
//-----------------------------------------------------------------------------


#include "GaudiAlg/GaudiTool.h"
#include "AlignmentInterfaces/IATrackSelectorTool.h"
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrackCaloMatch.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

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
   StatusCode Reset();

   void PrintUniformTD();

 private :
   bool uniformTD( const LHCb::OTChannelID & );
   bool yCut( const LHCb::Track & );
   inline int uniqueLayer(const LHCb::OTChannelID& channelID) const {
       return (channelID.station() - 1) * 4 + channelID.layer();
     };
   inline int uniqueQuarter(const LHCb::OTChannelID& channelID) const {
       return uniqueLayer(channelID) * 4 + channelID.quarter();
     };
   inline int uniqueModule(const LHCb::OTChannelID& channelID) const {
       return uniqueQuarter(channelID) * 9 + channelID.module() - 1;
     };
   void PrintSummary();
 private:
   //Interfaces:
   IATrackSelectorTool* m_trackselector;    
   ITrackExtrapolator* m_extrapolator;
   ITrackCaloMatch *m_trackenergy;    

   bool Unify( const LHCb::Track&  );
   bool uniformCut( int & );
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
   double m_yMaxCut;
   double m_yMinCut;
   bool m_weights; // attempt to create uniform track distrubution in (OT)
   int m_uniCut; // module IDs larger than m_uniCut will be uniformed.
   double m_energyMinCut;    // Min energy cut

   int m_wrong_charge, m_total, m_bad_chi2, m_bad_p, m_bad_pt;
   int m_few_hits, m_bad_energy, m_not_uniform;
   
   // Track types to accept
   typedef std::vector<std::string> TrackTypes;
   TrackTypes m_trTypes; // List of track types to select
     
   // Mapping type linking track types to selection boolean
   typedef GaudiUtils::HashMap < const LHCb::Track::Types, bool > SelTypes;
   SelTypes m_selTypes;  // Mapping linking track types to selection boolean
   int m_uniform[3][4][4][9];
   int m_HitMatrix[9][9];
};

#endif // TRACKTOOLS_JBSelector_H
