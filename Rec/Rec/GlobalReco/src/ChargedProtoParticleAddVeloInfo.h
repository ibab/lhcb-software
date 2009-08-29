
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddVeloInfo.h
 *
 * Header file for algorithm ChargedProtoParticleAddVeloInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddVeloInfo.h,v 1.1 2009-08-29 20:37:19 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleAddVeloInfo_H
#define GLOBALRECO_ChargedProtoParticleAddVeloInfo_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/ProtoParticle.h"

// interfaces
#include "TrackInterfaces/ITrackVelodEdxCharge.h"

/** @class ChargedProtoParticleAddVeloInfo ChargedProtoParticleAddVeloInfo.h
 *
 *  Updates the VELO PID (dE/dx) information stored in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleAddVeloInfo : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ChargedProtoParticleAddVeloInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleAddVeloInfo( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// Add Velo dE/dx information to the given ProtoParticle
  bool addVelodEdx( LHCb::ProtoParticle * proto ) const;

private:

  std::string m_protoPath; ///< Location of the ProtoParticles in the TES

  /// Velo dE/dx charge tool
  ITrackVelodEdxCharge * m_velodEdx;

};

#endif // GLOBALRECO_ChargedProtoParticleAddVeloInfo_H
