// $Id: MuonCoord2MCParticleAlg.h,v 1.2 2002-07-03 09:32:12 dhcroft Exp $
#ifndef MUONCOORD2MCPARTICLEALG_H
#define MUONCOORD2MCPARTICLEALG_H 1

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "MuonAssociators/MuonCoord2MCParticleAsct.h"

class MuonCoord;

/** @class MuonCoord2MCParticleAlg MuonCoord2MCParticleAlg.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   12/06/2002
 */

class MuonCoord2MCParticleAlg : public Algorithm {
    friend AlgFactory<MuonCoord2MCParticleAlg>;
  
public:

  /// Standard constructor
  MuonCoord2MCParticleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~MuonCoord2MCParticleAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  std::string m_outputData;

  // associator function
  StatusCode associateToTruth(const MuonCoord* coord,
                              MuonCoord2MCParticleAsct::Table * aTable );
};

inline std::string MuonCoord2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // MUONCOORD2MCPARTICLEALG_H







