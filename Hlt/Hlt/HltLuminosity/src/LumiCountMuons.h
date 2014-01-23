#ifndef LUMICOUNTMUONS_H
#define LUMICOUNTMUONS_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class LumiCountMuons LumiCountMuons.h
 *
 *  @author Vladislav Balagura
 *  @date   2011-03-01
 *
 *  (copied from LumiCountTracks)
 */
class LumiCountMuons : public GaudiAlgorithm
{
  public:
    LumiCountMuons( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~LumiCountMuons();

    virtual StatusCode initialize();
    virtual StatusCode execute();

  protected:
    std::string m_CounterName;
    std::string m_InputSelectionName;
    std::string m_OutputContainerName;
    double m_Threshold;
    int m_Counter;
};
#endif
