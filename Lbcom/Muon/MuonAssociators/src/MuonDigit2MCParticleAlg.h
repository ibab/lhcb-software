// $Id: MuonDigit2MCParticleAlg.h,v 1.1.1.1 2002-06-28 09:59:33 dhcroft Exp $
#ifndef MUONDIGIT2MCPARTICLEALG_H
#define MUONDIGIT2MCPARTICLEALG_H 1

#include <string>

#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"
#include "MuonAssociators/MuonDigit2MCParticleAsct.h"

/** @class MuonDigit2MCParticleAlg MuonDigit2MCParticleAlg.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   12/06/2002
 */

class MuonDigit;
class MCParticle;

class MuonDigit2MCParticleAlg : public Algorithm {
    friend AlgFactory<MuonDigit2MCParticleAlg>;
  
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
  std::string m_nameAsct;

  typedef Relation1D<MuonDigit, MCParticle>    Table;
  // associator function
  virtual StatusCode associateToTruth(const MuonDigit* digit,
                                      Table * table );

};

inline std::string MuonDigit2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // MUONDIGIT2MCPARTICLEALG_H







