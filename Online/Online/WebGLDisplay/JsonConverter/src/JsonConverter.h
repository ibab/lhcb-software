#ifndef JSONCONVERTER_H 
#define JSONCONVERTER_H 1

#include <string>

// Include files 
// from DaVinci.
#include "Kernel/DaVinciAlgorithm.h"
#include "Event/Particle.h"

/** @class JsonConverter JsonConverter.h
 *  
 *  Converts LHCb Events to JSON format.
 *  @author Ben Couturier
 *  @date   2014-12-03
 */
class JsonConverter : public DaVinciAlgorithm {
public: 
  /// Standard constructor
  JsonConverter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~JsonConverter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  const LHCb::VertexBase* findBestVertex(LHCb::Particle *p);

protected:

private:

};

/**
 * Convertion of Christoph's method.
 * Have to understand why not use the standard one ! 
 */
std::string getParticleName(LHCb::Particle *p);

#endif // JSONCONVERTER_H
