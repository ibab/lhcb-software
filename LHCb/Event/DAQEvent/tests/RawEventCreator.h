// $Id: RawEventCreator.h,v 1.1 2005-10-14 12:48:50 cattanem Exp $
#ifndef TESTS_RAWEVENTCREATOR_H 
#define TESTS_RAWEVENTCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class RawEventCreator RawEventCreator.h tests/RawEventCreator.h
 *  Creates and fills dummy RawEvent  
 *
 *  @author Markus Frank
 *  @date   2005-10-13
 */
class RawEventCreator : public GaudiAlgorithm {
public: 
  /// Standard constructor
  RawEventCreator( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RawEventCreator( );     ///< Destructor
  virtual StatusCode execute();    ///< Algorithm execution

protected:

private:

};
#endif // TESTS_RAWEVENTCREATOR_H
