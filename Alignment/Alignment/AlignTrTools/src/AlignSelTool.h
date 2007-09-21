// $Id: AlignSelTool.h,v 1.4 2007-09-21 15:45:52 lnicolas Exp $
#ifndef ALIGNTRTOOLS_ALIGNSELTOOL_H 
#define ALIGNTRTOOLS_ALIGNSELTOOL_H 1

//===========================================================================
// Includes
//===========================================================================
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

// Interface
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

// Event
#include "Event/Track.h"
#include "Event/OTTime.h"
#include "Event/STLiteCluster.h"
//===========================================================================

//===========================================================================
// Forward declarations
class DeOTDetector;
class DeSTDetector;
class DeITDetector;
//===========================================================================

/** @class AlignSelTool AlignSelTool.h
 *  
 *
 *  @author Louis Nicolas
 *  @date   2006-12-01
 */
class AlignSelTool : public GaudiTool, 
                     virtual public ITrackSelector,
                     virtual public IIncidentListener {

public: 
  /// Standard constructor
  AlignSelTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~AlignSelTool( ); ///< Destructor

  StatusCode AlignSelTool::initialize();

  virtual bool accept(const LHCb::Track& aTrack) const;
  
  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   **/
  void handle( const Incident& incident );
  
protected:
  
private:

  static const int defValue = -999999;

  //===========================================================================
  // Constants picked up from the geometry
  //===========================================================================
  int m_nStations;

  int m_nQuarters;
  int m_nModules;
  int m_nStraws;

  int m_nBoxes;
  int m_nLadders;
  int m_nStrips;
  //===========================================================================

  //===========================================================================
  // More Properties
  //===========================================================================
  bool m_fieldOn;
  double m_nStrawsTol;
  double m_nStripsTol;
  int alignSelectedModules;
  mutable bool m_configured;
  void initEvent() const;
  ITrackExtrapolator* m_extrapolator; ///< Interface to track extrapolator
  //===========================================================================

  //=============================================================================
  // Defining the cuts
  //=============================================================================
  std::string c_trackType;
  int c_modulesToAlign;
  int c_maxMulti;
  double c_minP;
  double c_maxChi2PerDoF;
  double c_minChi2Prob;
  int c_maxNHoles;
  int c_maxNSharedHits;
  int c_maxNCloseHits;
  //=============================================================================

  //===========================================================================
  //  The objects in use
  //===========================================================================
  LHCb::Track theTrack;
  std::string m_tracksPath;
  mutable const LHCb::Tracks* m_tracks;
  std::string m_itClustersPath;
  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;
  mutable STLiteClusters* m_itClusters;
  std::string m_otTimesPath;
  mutable const LHCb::OTTimes* m_otTimes;
  //===========================================================================

  //===========================================================================
  // Declaring the tracks variables
  //===========================================================================
  mutable int m_multiplicity;
  mutable double m_trP;
  mutable double m_trChi2PerDoF;
  mutable double m_trChi2Prob;
  mutable int m_nHoles;
  mutable int m_nSharedHits;
  mutable int m_nCloseHits;
  //===========================================================================

  //=============================================================================
  // Getting the tracks variables
  //=============================================================================
  int getAllVariables ( const LHCb::Track& aTrack ) const;
  bool isSharedHit ( const LHCb::Track& aTrack,
                     const LHCb::Node* aNode ) const;
  int nNeighbouringHits ( const LHCb::Track& aTrack ) const;
  bool isNeighbouringHit ( LHCb::OTChannelID timeID,
                           LHCb::OTChannelID hitID,
                           unsigned int hitStraw ) const;  
  //=============================================================================

  //=============================================================================
  // Cutting on the variables
  //=============================================================================
  bool selectDefinedModules ( const LHCb::Track& aTrack ) const;
  bool selectBoxOverlaps ( const LHCb::Track& aTrack ) const;
  bool cutMultiplicity ( ) const;
  bool cutTrackP ( ) const;
  bool cutTrackChi2PerDoF ( ) const;
  bool cutTrackChi2Prob ( ) const;
  bool cutNHoles ( ) const;
  bool cutNSharedHits ( ) const;
  bool cutNCloseHits ( ) const;
  //=============================================================================
};
#endif // ALIGNTRTOOLS_ALIGNSELTOOL_H
