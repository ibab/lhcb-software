#ifndef LUMICOUNTVELOWITHZRCUTS_H 
#define LUMICOUNTVELOWITHZRCUTS_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class LumiCountVeloWithZRCuts LumiCountVeloWithZRCuts.h
 *
 *  @author Vladislav Balagura
 *  @date   2011-09-20
 *
 *  (copied from LumiCountMuons)
 */
class LumiCountVeloWithZRCuts : public GaudiAlgorithm {
public: 
  LumiCountVeloWithZRCuts( std::string name, ISvcLocator* pSvcLocator );
  virtual ~LumiCountVeloWithZRCuts( );

  virtual StatusCode initialize();
  virtual StatusCode execute   ();
protected:
  std::string m_TrackCounterName, m_VertexCounterName;
  std::string m_TrackInputSelectionName, m_VertexInputSelectionName;
  std::string m_OutputContainerName;
  double m_AbsZCut, m_RCut;
  int m_TrackCounter, m_VertexCounter;
};
#endif
