
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleRemovePIDInfo.h
 *
 * Header file for algorithm ChargedProtoParticleRemovePIDInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleRemovePIDInfo.h,v 1.1 2010-03-15 17:17:01 jonrob Exp $
 *
 *  @author Dmitry Golubkov
 *  @date 13/03/2010
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleRemovePIDInfo_H
#define GLOBALRECO_ChargedProtoParticleRemovePIDInfo_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/ProtoParticle.h"

/** @class ChargedProtoParticleRemovePIDInfo ChargedProtoParticleRemovePIDInfo.h
 *
 *  Remove the Muon and Rich PID information stored in the ProtoParticles
 *
 *  @author Dmitry Golubkov
 *  @date 13/03/2010
 */

class ChargedProtoParticleRemovePIDInfo : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoParticleRemovePIDInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleRemovePIDInfo( ); ///< Destructor

  virtual StatusCode execute();    ///< Algorithm execution

private:

  std::string m_protoPath;        ///< Location of the ProtoParticles in the TES
  bool m_RemoveRichPID;           ///< instruct algorith to remove the RichPID info from PP
  bool m_RemoveMuonPID;           ///< instruct algorith to remove the MuonPID info from PP
  bool m_RemoveCombPID;           ///< instruct algorith to remove the CombPID info from PP
};

#endif // GLOBALRECO_ChargedProtoParticleRemovePIDInfo_H
