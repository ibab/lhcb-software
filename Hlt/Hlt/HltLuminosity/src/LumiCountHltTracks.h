// $Id: LumiCountHltTracks.h,v 1.4 2009-12-24 14:13:20 graven Exp $
#ifndef LUMICOUNTHLTTRACKS_H 
#define LUMICOUNTHLTTRACKS_H 1

// Include files
// from Gaudi
#include <vector>
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "GaudiKernel/StringKey.h"
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

private:
  
  const Hlt::Selection* m_input;
  Gaudi::StringKey   m_InputSelectionName;
  std::string m_CounterName;
  std::string m_OutputContainerName;
  int m_Counter;


};
#endif // LUMICOUNTHLTTRACKS_H
