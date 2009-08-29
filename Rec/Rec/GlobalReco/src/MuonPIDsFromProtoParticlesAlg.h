
//-----------------------------------------------------------------------------
/** @file MuonPIDsFromProtoParticlesAlg.h
 *
 * Header file for algorithm MuonPIDsFromProtoParticlesAlg
 *
 * CVS Log :-
 * $Id: MuonPIDsFromProtoParticlesAlg.h,v 1.4 2009-08-29 20:37:19 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_MuonPIDsFromProtoParticlesAlg_H 
#define GLOBALRECO_MuonPIDsFromProtoParticlesAlg_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/ProtoParticle.h"
#include "Event/MuonPID.h"

//-----------------------------------------------------------------------------
/** @class MuonPIDsFromProtoParticlesAlg MuonPIDsFromProtoParticlesAlg.h
 *  
 *  Simple algorithm to recreate MuonPID data objects from the data
 *  stored in the ProtoParticles
 *
 *  @author Chris Jones
 *  @date   2006-08-01
 */
//-----------------------------------------------------------------------------

class MuonPIDsFromProtoParticlesAlg : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  MuonPIDsFromProtoParticlesAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonPIDsFromProtoParticlesAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// Input location of ProtoParticles in TES
  std::string m_protoPloc;

  /// Output location of RichPIDs in TES
  std::string m_muonPIDloc;

  /// Location of Muon Tracks in TES
  std::string m_muonTrackLoc;

};

#endif // GLOBALRECO_MuonPIDsFromProtoParticlesAlg_H
