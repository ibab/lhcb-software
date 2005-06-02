// $Id: CompareMCParticle.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef COMPAREMCPARTICLE_H 
#define COMPAREMCPARTICLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareMCParticle CompareMCParticle.h
 *  Compare two containers of MCParticles
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class CompareMCParticle : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CompareMCParticle( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareMCParticle( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_testName;
};
#endif // COMPAREMCPARTICLE_H
