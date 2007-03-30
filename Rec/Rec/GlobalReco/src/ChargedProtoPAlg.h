//-----------------------------------------------------------------------------
/** @file ChargedProtoPAlg.h
 *
 * Header file for algorithm ChargedProtoPAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoPAlg.h,v 1.25 2007-03-30 07:16:55 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_CHARGEDPROTOPALG_H
#define GLOBALRECO_CHARGEDPROTOPALG_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/HashMap.h"

// interfaces
#include "TrackInterfaces/ITrackSelector.h"
#include "TrackInterfaces/ITrackVelodEdxCharge.h"

// from CaloUtils
#include "CaloUtils/ICaloElectron.h"
#include "CaloUtils/Calo2Track.h"
#include "Event/CaloDataFunctor.h"
#include "CaloDet/DeCalorimeter.h"

// Relations
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted2D.h"

// Event
#include "Event/Track.h"
#include "Event/RichPID.h"
#include "Event/MuonPID.h"
#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoPAlg ChargedProtoPAlg.h
 *
 *  Algorithm to build charged ProtoParticles from the results of the reconstruction
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 29/03/2006
 */
//-----------------------------------------------------------------------------

class ChargedProtoPAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoPAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoPAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Access to the ProtoParticle container
  inline LHCb::ProtoParticles * protos() { return m_protos; }

  /// Load the RichPIDs and build reverse mappings
  StatusCode getRichData();

  /// Load the MuonPIDs and build reverse mappings
  StatusCode getMuonData();

  /// Load the Calo tables 
  StatusCode getCaloData();
  // Add extra info from CaloDigits (Spd+Prs)
  double CaloSpd       ( const LHCb::CaloHypo*  hypo  )  const ;
  double CaloPrs       ( const LHCb::CaloHypo*  hypo  )  const ;
  double CaloEcal      ( const LHCb::CaloHypo*  hypo  )  const ;

  /// Add Rich information to the given ProtoParticle
  bool addRich( LHCb::ProtoParticle * proto ) const;

  /// Add Muon information to the given ProtoParticle
  bool addMuon( LHCb::ProtoParticle * proto ) const;

  /// Add Calo information to the given ProtoParticle
  bool addCalo( LHCb::ProtoParticle * proto ) const;

  /// Add Velo dE/dx information to the given ProtoParticle
  bool addVelodEdx( LHCb::ProtoParticle * proto ) const;

private: // data

  std::string m_tracksPath;   ///< Location in TES of input Tracks
  std::string m_richPath;     ///< Location in TES of input RichPIDs
  std::string m_muonPath;     ///< Location in TES of input MuonPIDs
  std::string m_protoPath;    ///< Location in TES of output ProtoParticles
  bool m_PrsPID,m_SpdPID,m_EcalPID,m_HcalPID,m_BremPID; // Calo PIDs

  LHCb::ProtoParticles * m_protos; ///< Pointer to current ProtoParticle container

  /// Track selector tool
  ITrackSelector * m_trSel;

  /// Velo dE/dx charge tool
  ITrackVelodEdxCharge * m_velodEdx;

  /// CaloElectron tool
  ICaloElectron * m_electron;

  /// mapping type from Track to RichPID data objects
  typedef std::map<const LHCb::Track *, const LHCb::RichPID *> TrackToRichPID;
  /// mapping from Track to RichPID data objects
  TrackToRichPID m_richMap;

  /// mapping type from Track to MuonPID data objects
  typedef std::map<const LHCb::Track *, const LHCb::MuonPID *> TrackToMuonPID;
  /// mapping from Track to RichPID data objects
  TrackToMuonPID m_muonMap;

  // Track->Calo (Cluster/Hypo/Id/Acc) relation tables
  const LHCb::Calo2Track::IClusTrTable2D* m_clusTrTable ;
  const LHCb::Calo2Track::ITrHypoTable2D* m_elecTrTable ;
  const LHCb::Calo2Track::ITrHypoTable2D* m_bremTrTable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dlleEcalTable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dllePrsTable  ;
  const LHCb::Calo2Track::ITrEvalTable* m_dlleHcalTable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dlleBremTable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dllmuHcalTable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dllmuEcalTable ;
  const LHCb::Calo2Track::ITrAccTable*  m_InSpdTable  ;
  const LHCb::Calo2Track::ITrAccTable*  m_InPrsTable  ;
  const LHCb::Calo2Track::ITrAccTable*  m_InEcalTable ;
  const LHCb::Calo2Track::ITrAccTable*  m_InHcalTable ;
  const LHCb::Calo2Track::ITrAccTable*  m_InBremTable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_SpdETable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_PrsETable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_HcalETable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_EcalETable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_ClusChi2Table ;
  const LHCb::Calo2Track::ITrEvalTable*  m_BremChi2Table ;
  const LHCb::Calo2Track::ITrEvalTable*  m_EcalChi2Table ;

  // tallies

  /// Event count
  unsigned long m_nEvts;

  /// Simple utility tally class
  class TrackTally
  {
  public:
    /// Default constructor
    TrackTally() : totTracks(0), selTracks(0), caloTracks(0), 
                   richTracks(0), muonTracks(0), velodEdxTracks(0) { }
    unsigned long totTracks;      ///< Number of considered tracks
    unsigned long selTracks;      ///< Number of tracks selected to creaste a ProtoParticle from
    unsigned long caloTracks;     ///< Number of ProtoParticles created with CALO info
    unsigned long richTracks;     ///< Number of ProtoParticles created with RICH info
    unsigned long muonTracks;     ///< Number of ProtoParticles created with MUON info
    unsigned long velodEdxTracks; ///< Number of ProtoParticles created with VELO dE/dx info
  };

  /// Map type containing tally for various track types
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, TrackTally > TrackMap;
  /// Total number of tracks considered and selected
  TrackMap m_nTracks;


};

#endif // GLOBALRECO_CHARGEDPROTOPALG_H
