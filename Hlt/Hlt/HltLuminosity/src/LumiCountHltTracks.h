// $Id: LumiCountHltTracks.h,v 1.3 2009-12-23 17:59:49 graven Exp $
#ifndef LUMICOUNTHLTTRACKS_H 
#define LUMICOUNTHLTTRACKS_H 1

// Include files
// from Gaudi
#include <vector>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "HltBase/HltBaseAlg.h"

/** @class LumiCountHltTracks LumiCountHltTracks.h
 *  
 *
 *  Note:

 *  @author Jaap Panman
 *  @date   2008-07-22
 */
class LumiCountHltTracks : public HltBaseAlg {
public: 
  /// Standard constructor
  LumiCountHltTracks( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiCountHltTracks( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  
  Gaudi::StringKey   m_InputSelectionName;
  std::string m_CounterName;
  int m_Counter;

  std::string m_OutputContainerName;

};
#endif // LUMICOUNTHLTTRACKS_H
