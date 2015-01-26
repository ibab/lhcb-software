#ifndef JSONCONVERTER_H 
#define JSONCONVERTER_H 1

#include <string>

// Include files 
// from DaVinci.
#include "Kernel/DaVinciAlgorithm.h"
#include "Event/Particle.h"


// Forward declaration
class ITrackExtrapolator;





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

  /**
   * Find the best vertex for the particle
   */
  const LHCb::VertexBase* findBestVertex(LHCb::Particle *p);
  
protected:

private:
  ITrackExtrapolator *m_trackExtrapolator;
  
};

/**
 * Convertion of Christoph's method.
 * Have to understand why not use the standard one ! 
 */
std::string getParticleName(LHCb::Particle *p);


// Some constants for the detectors
const double HCAL_Z = 14000;
const double MUON_Z = 19000;


#endif // JSONCONVERTER_H
