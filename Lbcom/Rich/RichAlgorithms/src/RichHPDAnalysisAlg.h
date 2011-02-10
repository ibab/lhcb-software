
//-----------------------------------------------------------------------------
/** @file RichHPDAnalysisAlg.h
 *
 *  Header file for tool : Rich::HPDAnalysisAlg
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2008-10-14
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECQC_RichHPDAnalysisAlg_H
#define RICHRECQC_RichHPDAnalysisAlg_H 1

// STL
#include <string>
#include <vector>

// RichKernel
#include "RichKernel/RichAlgBase.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichKernel/IRichGenericHPDAnalysisTool.h"

// RichDet
#include "RichDet/DeRichSystem.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class HPDAnalysisAlg RichHPDAnalysisAlg.h
   *
   *  Simple monitor to look for hot HPD pixels
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2008-10-14
   */
  //-----------------------------------------------------------------------------
  class HPDAnalysisAlg : public Rich::AlgBase
  {

  public:

    /// Standard constructor
    HPDAnalysisAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HPDAnalysisAlg( ); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution

  private:

    /// Process an individual TAE event
    StatusCode processTAEEvent( const std::string & taeEvent );

  private:

    /// Raw Buffer Decoding tool
    const Rich::DAQ::IRawBufferToSmartIDsTool * m_SmartIDDecoder;

    ///< The TAE location(s) to monitor
    std::vector<std::string> m_taeEvents;

    typedef std::vector<std::string> HPDToolNames;
    /// HPD analysis tool names
    HPDToolNames m_toolNames;

    typedef std::vector<const Rich::IGenericHPDAnalysisTool*> HPDTools;
    /// Pointers to HPD tools
    HPDTools m_tools;

  };

}

#endif // RICHRECQC_RichHPDAnalysisAlg_H
