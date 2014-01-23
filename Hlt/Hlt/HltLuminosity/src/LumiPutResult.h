// $Id: LumiPutResult.h,v 1.2 2009-02-18 13:11:13 panmanj Exp $
#ifndef LUMIPUTRESULT_H
#define LUMIPUTRESULT_H 1

// Include files
#include <memory>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class LumiPutResult LumiPutResult.h
 *
 *
 *  @author Jaap Panman
 *  @date   2008-08-27
 */
class LumiPutResult : public GaudiAlgorithm
{
  public:
    /// Standard constructor
    LumiPutResult( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~LumiPutResult(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode finalize();   ///< Algorithm finalization

  private:
    std::string m_InputContainerName;
    unsigned int m_size;
    std::unique_ptr<double[]> m_means;
    std::unique_ptr<double[]> m_thresholds;
    std::unique_ptr<unsigned int[]> m_infoKeys;
};
#endif // LUMIPUTRESULT_H
