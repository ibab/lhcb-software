
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddMuonInfo.h
 *
 * Header file for algorithm ChargedProtoParticleAddMuonInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddMuonInfo.h,v 1.1 2009-08-29 20:37:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleAddMuonInfo_H
#define GLOBALRECO_ChargedProtoParticleAddMuonInfo_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Muon
#include "Event/MuonPID.h"
#include "Event/ProtoParticle.h"

/** @class ChargedProtoParticleAddMuonInfo ChargedProtoParticleAddMuonInfo.h
 *
 *  Updates the MuonPID information stored in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleAddMuonInfo : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoParticleAddMuonInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleAddMuonInfo( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  /// Load the muon data
  bool getMuonData();

  /// Add (or update) the Muon information for the given ProtoParticle
  void updateMuon( LHCb::ProtoParticle * proto ) const;

private:

  std::string m_muonPath;  ///< Location of MuonPID data objects in the TES
  std::string m_protoPath; ///< Location of the ProtoParticles in the TES

  /// mapping type from Track to MuonPID data objects
  typedef std::map<const LHCb::Track *, const LHCb::MuonPID *> TrackToMuonPID;
  /// mapping from Track to MuonPID data objects
  TrackToMuonPID m_muonMap;

};

#endif // GLOBALRECO_ChargedProtoParticleAddMuonInfo_H
