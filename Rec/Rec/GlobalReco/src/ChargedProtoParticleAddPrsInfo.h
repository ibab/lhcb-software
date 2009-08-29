
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddPrsInfo.h
 *
 * Header file for algorithm ChargedProtoParticleAddPrsInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddPrsInfo.h,v 1.1 2009-08-29 20:37:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleAddPrsInfo_H
#define GLOBALRECO_ChargedProtoParticleAddPrsInfo_H 1

// from Gaudi
#include "ChargedProtoParticleCALOBaseAlg.h"

/** @class ChargedProtoParticleAddPrsInfo ChargedProtoParticleAddPrsInfo.h
 *
 *  Updates the CALO 'BREM' information stored in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleAddPrsInfo : public ChargedProtoParticleCALOBaseAlg
{

public:

  /// Standard constructor
  ChargedProtoParticleAddPrsInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleAddPrsInfo( ); ///< Destructor

  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// Load the Calo Prs tables
  bool getPrsData();

  /// Add Calo Prs information to the given ProtoParticle
  bool addPrs( LHCb::ProtoParticle * proto ) const;

private:

  std::string m_protoPath; ///< Location of the ProtoParticles in the TES

  std::string m_inPrsPath ;
  std::string m_prsEPath ;
  std::string m_prsPIDePath ;

  const LHCb::Calo2Track::ITrAccTable*  m_InPrsTable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_PrsETable ;
  const LHCb::Calo2Track::ITrEvalTable* m_dllePrsTable ;

};

#endif // GLOBALRECO_ChargedProtoParticleAddPrsInfo_H
