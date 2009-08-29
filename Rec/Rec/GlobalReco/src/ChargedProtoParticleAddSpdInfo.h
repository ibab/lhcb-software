
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddSpdInfo.h
 *
 * Header file for algorithm ChargedProtoParticleAddSpdInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddSpdInfo.h,v 1.1 2009-08-29 20:37:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleAddSpdInfo_H
#define GLOBALRECO_ChargedProtoParticleAddSpdInfo_H 1

// from Gaudi
#include "ChargedProtoParticleCALOBaseAlg.h"

/** @class ChargedProtoParticleAddSpdInfo ChargedProtoParticleAddSpdInfo.h
 *
 *  Updates the CALO SPD information stored in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleAddSpdInfo : public ChargedProtoParticleCALOBaseAlg
{

public:

  /// Standard constructor
  ChargedProtoParticleAddSpdInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleAddSpdInfo( ); ///< Destructor

  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// Load the Calo Spd tables
  bool getSpdData();

  /// Add Calo Spd information to the given ProtoParticle
  bool addSpd( LHCb::ProtoParticle * proto ) const;

private:

  std::string m_protoPath; ///< Location of the ProtoParticles in the TES

  std::string m_inSpdPath ;
  std::string m_spdEPath ;

  const LHCb::Calo2Track::ITrAccTable*  m_InSpdTable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_SpdETable ;

};

#endif // GLOBALRECO_ChargedProtoParticleAddSpdInfo_H
