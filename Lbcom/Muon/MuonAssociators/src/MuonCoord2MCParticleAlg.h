// $Id: MuonCoord2MCParticleAlg.h,v 1.1.1.1 2002-06-28 09:59:33 dhcroft Exp $
#ifndef MUONCOORD2MCPARTICLEALG_H
#define MUONCOORD2MCPARTICLEALG_H 1

#include <string>

#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"
#include "MuonAssociators/MuonCoord2MCParticleAsct.h"

// to get from MuonDigit to MCParticle
#include "MuonAssociators/MuonDigit2MCParticleAsct.h"

/** @class MuonCoord2MCParticleAlg MuonCoord2MCParticleAlg.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   12/06/2002
 */

class MuonCoord;
class MCParticle;

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
  std::string m_nameAsct;

  MuonDigit2MCParticleAsct::IAsct* m_hAsct;

  typedef Relation1D<MuonCoord, MCParticle>    Table;
  // associator function
  virtual StatusCode associateToTruth(const MuonCoord* coord,
                                      Table * aTable );
};

inline std::string MuonCoord2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // MUONCOORD2MCPARTICLEALG_H







