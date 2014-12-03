#ifndef JSONCONVERTER_H 
#define JSONCONVERTER_H 1

// Include files 
// from DaVinci.
#include "Kernel/DaVinciAlgorithm.h"


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

protected:

private:

};
#endif // JSONCONVERTER_H
