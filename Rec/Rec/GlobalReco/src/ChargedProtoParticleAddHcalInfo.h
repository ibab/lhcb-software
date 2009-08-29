
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddHcalInfo.h
 *
 * Header file for algorithm ChargedProtoParticleAddHcalInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddHcalInfo.h,v 1.1 2009-08-29 20:37:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleAddHcalInfo_H
#define GLOBALRECO_ChargedProtoParticleAddHcalInfo_H 1

// from Gaudi
#include "ChargedProtoParticleCALOBaseAlg.h"

/** @class ChargedProtoParticleAddHcalInfo ChargedProtoParticleAddHcalInfo.h
 *
 *  Updates the CALO HCAL information stored in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleAddHcalInfo : public ChargedProtoParticleCALOBaseAlg
{

public:

  /// Standard constructor
  ChargedProtoParticleAddHcalInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleAddHcalInfo( ); ///< Destructor

  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// Load the Calo Hcal tables
  bool getHcalData();

  /// Add Calo Hcal information to the given ProtoParticle
  bool addHcal( LHCb::ProtoParticle * proto ) const;

private:

  std::string m_protoPath; ///< Location of the ProtoParticles in the TES

  std::string m_inHcalPath ;
  std::string m_hcalEPath ;
  std::string m_hcalPIDePath ;
  std::string m_hcalPIDmuPath ;

  const LHCb::Calo2Track::ITrAccTable*  m_InHcalTable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_HcalETable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dlleHcalTable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dllmuHcalTable ;

};

#endif // GLOBALRECO_ChargedProtoParticleAddHcalInfo_H
