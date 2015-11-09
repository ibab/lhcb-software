#ifndef LUMISTORECONSTANT_H
#define LUMISTORECONSTANT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class LumiStoreConstant LumiStoreConstant.h
 *
 *
 *  @author Rosen Matev
 *  @date   2015-11-09
 */
class LumiStoreConstant : public GaudiAlgorithm
{
  public:
    /// Standard constructor
    LumiStoreConstant( const std::string& name, ISvcLocator* pSvcLocator );

    ~LumiStoreConstant() override = default; ///< Destructor

    StatusCode initialize() override; ///< Algorithm initialization
    StatusCode execute() override;    ///< Algorithm execution
    StatusCode finalize() override;   ///< Algorithm finalization

  private:
    std::string m_OutputContainerName;

    std::string m_CounterName;
    int m_Counter;
    int m_Value;
};
#endif // LUMISTORECONSTANT_H
