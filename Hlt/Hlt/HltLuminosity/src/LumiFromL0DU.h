// $Id: LumiFromL0DU.h,v 1.4 2010-03-28 12:48:59 graven Exp $
#ifndef LUMIFROML0DU_H
#define LUMIFROML0DU_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class LumiFromL0DU LumiFromL0DU.h
 *
 *
 *  @author Jaap Panman
 *  @date   2008-07-21
 */
class LumiFromL0DU : public GaudiAlgorithm
{
  public:
    /// Standard constructor
    LumiFromL0DU( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~LumiFromL0DU(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution

  private:
    std::string m_InputSelectionName;
    std::string m_OutputContainerName;

    std::map<std::string, std::string> m_CounterValueProperty;
    std::vector<std::pair<int, std::string>> m_CounterValue;
};
#endif // LUMIFROML0DU_H
