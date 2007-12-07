// $Id: RawEventCreator.h,v 1.1 2007-12-07 14:34:05 marcocle Exp $
#ifndef TESTS_RAWEVENTCREATOR_H 
#define TESTS_RAWEVENTCREATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

class IEventTimeDecoder;

namespace DAQEventTests {
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
  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute();    ///< Algorithm execution

protected:

private:
  IEventTimeDecoder *m_eventTimeDecoder;
  
};

} // namespace DAQEventTests

#endif // TESTS_RAWEVENTCREATOR_H
