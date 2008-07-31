// $Id: LumiCountHltTracks.h,v 1.1 2008-07-31 21:34:47 panmanj Exp $
#ifndef LUMICOUNTHLTTRACKS_H 
#define LUMICOUNTHLTTRACKS_H 1

// Include files
// from Gaudi
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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  std::string m_InputSelectionName;
  const LHCb::Tracks* m_InputContainer;
  std::string m_CounterName;
  int m_Counter;

  // Hlt::Selection& m_sel;

  std::string m_OutputContainerName;
  LHCb::HltLumiSummarys* m_HltLumiSummarys;

};
#endif // LUMICOUNTHLTTRACKS_H
