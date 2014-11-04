#ifndef TurboMIAB_H 
#define TurboMIAB_H 1

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include <algorithm>
//

/** @class TurboMIAB TurboMIAB.h
 * 
 *
 */
class TurboMIAB : public GaudiAlgorithm 
{

public: 

	// Standard constructor
  	TurboMIAB( const std::string& name, ISvcLocator* pSvcLocator );

  	virtual ~TurboMIAB( ); ///< Destructor

  	StatusCode initialize();    ///< Algorithm initialization
  	StatusCode execute   ();    ///< Algorithm execution
 
protected:

private:
};
#endif // TurboMIAB_H
