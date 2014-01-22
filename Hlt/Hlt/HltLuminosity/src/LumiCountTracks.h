// $Id: LumiCountTracks.h,v 1.3 2008-08-25 10:59:04 panmanj Exp $
#ifndef LUMICOUNTTRACKS_H 
#define LUMICOUNTTRACKS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from LHCB
#include "Event/Track.h"

/** @class LumiCountTracks LumiCountTracks.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2008-07-21
 */
class LumiCountTracks : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiCountTracks( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~LumiCountTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  std::string m_InputSelectionName;
  std::string m_CounterName;
  std::string m_OutputContainerName;
  int m_Counter;
};
#endif // LUMICOUNTTRACKS_H
