// $Id: OTDigit2MCParticleAlg.h,v 1.2 2003-07-15 11:31:07 jvantilb Exp $
#ifndef OTASSOCIATORS_OTDIGIT2MCPARTICLEALG_H
#define OTASSOCIATORS_OTDIGIT2MCPARTICLEALG_H 1

#include "GaudiKernel/Algorithm.h"
#include "OTAssociators/OTDigit2MCHitAsct.h"

class OTDigit;
class MCParticle;

/** @class OTDigit2MCParticleAlg OTDigit2MCParticleAlg.h
 *  
 *  Algorithm which makes the association from OTDigits to MCParticles. 
 *  This is used by the associator tool. This algorithm uses the associator
 *  from OTDigits to MCHits. Since an OTDigit could contain more than one 
 *  tdc-time, an integer is stored in the relation table from OTDigits to 
 *  MCParticles which points to the tdc-time.
 *
 *  @author Jeroen van Tilburg
 *  @date   05/06/2003
 */

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
                                      std::vector<MCParticle*>& partVector,
                                      std::vector<int>& partNumbers);

  /// path to put table
  std::string outputData() const;
 
protected:

private:

  // job options:
  std::string m_outputData;          ///< path to put relation table
  std::string m_nameAsct;            ///< Associator from OTDigits to MCHits

  // pointer to associator
  OTDigit2MCHitAsct::IAsct* m_hAsct; ///< pointer to associator

};

inline std::string OTDigit2MCParticleAlg::outputData() const {
  return  m_outputData;
}


#endif // OTASSOCIATORS_OTDIGIT2MCPARTICLEALG_H







