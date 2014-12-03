#ifndef JSONCONVERTER_H 
#define JSONCONVERTER_H 1

// Include files 
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class JsonConverter JsonConverter.h
 *  
 *  
 *  @author Ben Couturier
 *  @date   2014-12-03
 */
class JsonConverter : public GaudiAlgorithm {
public: 
  /// Standard constructor
  JsonConverter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~JsonConverter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

};
#endif // JSONCONVERTER_H
