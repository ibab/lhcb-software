// $Id: AlignSelTool.h,v 1.6 2007-10-30 11:22:35 lnicolas Exp $
#ifndef ALIGNTRTOOLS_ALIGNSELTOOL_H 
#define ALIGNTRTOOLS_ALIGNSELTOOL_H 1

//===========================================================================
// Includes
//===========================================================================
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/GaudiTool.h"

#include "GaudiKernel/IIncidentListener.h"

#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/RndmGenerators.h"

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
                const IInterface* parent );

  virtual ~AlignSelTool( ); ///< Destructor

  StatusCode AlignSelTool::initialize ( );
  StatusCode AlignSelTool::finalize ( );

  virtual bool accept(const LHCb::Track& aTrack) const;
  
protected:
  
private:

  typedef LHCb::STLiteCluster::STLiteClusters STLiteClusters;

  static const int defValue = -999999;

  mutable Rndm::Numbers m_uniformDist; // Random numbers generator

  ITrackExtrapolator* m_extrapolator; // Interface to track extrapolator

  //===========================================================================
  // Incident listener
  //===========================================================================
  mutable bool m_configured;
  void handle( const Incident& incident );
  void initEvent ( ) const;
  //===========================================================================

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
  std::string m_tracksPath;
  std::string m_itClustersPath;
  std::string m_otTimesPath;
  bool m_fieldOn;
  double m_nStrawsTol;
  double m_nStripsTol;
  //===========================================================================

  //=============================================================================
  // Defining the cuts
  //=============================================================================
  std::string c_trackType;
  int c_modulesToAlign;
  mutable int alignSelectedModules;
  bool c_constOccup;
  int c_maxMulti;
  double c_minP;
  double c_maxChi2PerDoF;
  double c_minChi2Prob;
  int c_maxNHoles;
  int c_maxNSharedHits;
  int c_maxNCloseHits;
  //=============================================================================

  //===========================================================================
  //  Various containers
  //===========================================================================
  mutable const LHCb::Tracks* m_tracks;
  mutable std::vector<LHCb::LHCbID> m_closeHits;
  //===========================================================================

  //===========================================================================
  // Declaring the tracks variables
  //===========================================================================
  LHCb::Track theTrack;
  mutable int m_multiplicity;
  mutable double m_trP;
  mutable double m_entryTX;
  mutable double m_entryTY;
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
  std::vector<LHCb::LHCbID> getCloseHits ( ) const;
  int nNeighbouringHits ( const LHCb::Track& aTrack ) const;
  bool isNeighbouringHit ( LHCb::STChannelID clusID,
                           LHCb::STChannelID hitID ) const;
  bool isNeighbouringHit ( LHCb::OTChannelID timeID,
                           LHCb::OTChannelID hitID ) const;  
  double constOccup_distri ( double constOccup_var ) const;
  void printCutValues ( ) const;
  //=============================================================================

  //=============================================================================
  // Cutting on the variables
  //=============================================================================
  bool selectDefinedModules ( const LHCb::Track& aTrack ) const;
  bool selectBoxOverlaps ( const LHCb::Track& aTrack ) const;
  bool cutConstOccup ( ) const;
  bool cutMultiplicity ( ) const;
  bool cutTrackP ( ) const;
  bool cutTrackChi2PerDoF ( ) const;
  bool cutTrackChi2Prob ( ) const;
  bool cutNHoles ( ) const;
  bool cutNSharedHits ( ) const;
  bool cutNCloseHits ( ) const;
  //=============================================================================

  //=============================================================================
  // for comparing LHCbIDs
  //=============================================================================
  class lessByID {
  public:
    inline bool operator() ( const LHCb::LHCbID& obj1 ,
                             const LHCb::LHCbID& obj2 ) const { 
      return obj1.lhcbID() < obj2.lhcbID() ; 
    }
  };
  //=============================================================================
};
#endif // ALIGNTRTOOLS_ALIGNSELTOOL_H
