#ifndef MUONCOORD2MCPARTICLELINK_H 
#define MUONCOORD2MCPARTICLELINK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/MuonDigit.h"
#include "Event/MuonCoord.h"
#include "Event/MCParticle.h"

#include "Linker/LinkedTo.h"
#include "MuonDAQ/IMuonRawBuffer.h"

/** @class MuonCoord2MCParticleLink MuonCoord2MCParticleLink.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2005-12-30
 */
class MuonCoord2MCParticleLink : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MuonCoord2MCParticleLink( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MuonCoord2MCParticleLink( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  StatusCode associateToTruth(LHCb::MuonCoord *coord,
                              std::map<const LHCb::MCParticle*,double>& 
                              partMap);
protected:

private:
  //  bool m_associateAll;
  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit>* m_digitLink;
  std::vector<LHCb::MuonTileID> list_digit;
  IMuonRawBuffer* m_muonBuffer;  

};
#endif // MUONCOORD2MCPARTICLELINK_H
