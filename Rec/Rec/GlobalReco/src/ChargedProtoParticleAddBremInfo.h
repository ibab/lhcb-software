
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleAddBremInfo.h
 *
 * Header file for algorithm ChargedProtoParticleAddBremInfo
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleAddBremInfo.h,v 1.1 2009-08-29 20:37:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_ChargedProtoParticleAddBremInfo_H
#define GLOBALRECO_ChargedProtoParticleAddBremInfo_H 1

// from Gaudi
#include "ChargedProtoParticleCALOBaseAlg.h"

/** @class ChargedProtoParticleAddBremInfo ChargedProtoParticleAddBremInfo.h
 *
 *  Updates the CALO 'BREM' information stored in the ProtoParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/08/2009
 */

class ChargedProtoParticleAddBremInfo : public ChargedProtoParticleCALOBaseAlg
{

public:

  /// Standard constructor
  ChargedProtoParticleAddBremInfo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedProtoParticleAddBremInfo( ); ///< Destructor

  virtual StatusCode execute();       ///< Algorithm execution

private:

  /// Load the Calo Brem tables
  bool getBremData();

  /// Add Calo Brem information to the given ProtoParticle
  bool addBrem( LHCb::ProtoParticle * proto ) const;

private:

  std::string m_protoPath; ///< Location of the ProtoParticles in the TES

  std::string m_inBremPath ;
  std::string m_bremMatchPath ;
  std::string m_bremChi2Path ;
  std::string m_bremPIDePath ;

  const LHCb::Calo2Track::ITrAccTable*  m_InBremTable ;
  const LHCb::Calo2Track::ITrHypoTable2D* m_bremTrTable ;
  const LHCb::Calo2Track::ITrEvalTable*  m_BremChi2Table ;
  const LHCb::Calo2Track::ITrEvalTable* m_dlleBremTable ;

};

#endif // GLOBALRECO_ChargedProtoParticleAddBremInfo_H
