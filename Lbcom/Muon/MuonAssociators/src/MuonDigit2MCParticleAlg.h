// $Id: MuonDigit2MCParticleAlg.h,v 1.3 2003-01-17 14:14:55 sponce Exp $
#ifndef MUONDIGIT2MCPARTICLEALG_H
#define MUONDIGIT2MCPARTICLEALG_H 1

#include <string>

#include "GaudiKernel/Algorithm.h"
#include "MuonAssociators/MuonDigit2MCParticleAsct.h"

/** @class MuonDigit2MCParticleAlg MuonDigit2MCParticleAlg.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   12/06/2002
 */

class MuonDigit;

class MuonDigit2MCParticleAlg : public Algorithm {

  friend class AlgFactory<MuonDigit2MCParticleAlg>;
  
public:

  /// Standard constructor
  MuonDigit2MCParticleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~MuonDigit2MCParticleAlg(); 

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
  StatusCode associateToTruth(const MuonDigit* digit,
                              MuonDigit2MCParticleAsct::Table * table );

};

inline std::string MuonDigit2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // MUONDIGIT2MCPARTICLEALG_H







