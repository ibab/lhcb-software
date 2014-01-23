// $Id: LumiPrepareResult.h,v 1.1 2008-08-29 07:52:45 panmanj Exp $
#ifndef LUMIPREPARERESULT_H
#define LUMIPREPARERESULT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class LumiPrepareResult LumiPrepareResult.h
 *
 *
 *  @author Jaap Panman
 *  @date   2008-08-27
 */
class LumiPrepareResult : public GaudiAlgorithm
{
  public:
    /// Standard constructor
    LumiPrepareResult( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~LumiPrepareResult(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution

  private:
    std::string m_OutputContainerName;
};
#endif // LUMIPREPARERESULT_H
