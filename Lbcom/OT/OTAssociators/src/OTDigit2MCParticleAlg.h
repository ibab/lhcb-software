// $Id: OTDigit2MCParticleAlg.h,v 1.1 2003-06-10 09:04:16 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCPARTICLEALG_H
#define OTASSOCIATORS_OTDIGIT2MCPARTICLEALG_H 1

#include "Relations/IAssociator.h" 
#include "Relations/IRelation.h"
#include "Relations/Relation1D.h"

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTDigit2MCHitAsct.h"

/** @class OTDigit2MCParticleAlg OTDigit2MCParticleAlg.h
 *  
 *  Algorithm which makes the association from OTDigits to MCParticles. 
 *  This is used by the associator tool. This algorithm uses the associator
 *  with MCHits.
 *
 *  @author Jeroen van Tilburg
 *  @date   05/06/2003
 */

class OTDigit;
class MCParticle;

class OTDigit2MCParticleAlg : public Algorithm {

  friend class AlgFactory<OTDigit2MCParticleAlg>;
  
public:

  /// Standard constructor
  OTDigit2MCParticleAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~OTDigit2MCParticleAlg(); 

  /// initialize
  virtual StatusCode initialize();    

  /// execute
  virtual StatusCode execute();    

  /// finalize
  virtual StatusCode finalize();    

  // associator function
  virtual StatusCode associateToTruth(const OTDigit* aDigit,
                                      std::vector<MCParticle*>& partVector);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData;   ///< path to put relation table
  std::string m_nameAsct;     ///< name of the associator to MCHits

  // pointer to associator
  OTDigit2MCHitAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTDigit2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTDIGIT2MCPARTICLEALG_H







