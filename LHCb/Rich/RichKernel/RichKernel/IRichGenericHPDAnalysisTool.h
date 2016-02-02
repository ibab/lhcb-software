
//-----------------------------------------------------------------------------
/** @file IRichGenericHPDAnalysisTool.h
 *
 *  Header file for tool interface : Rich::IGenericHPDAnalysisTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/10/2009
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IGenericHPDAnalysisTool_H
#define RICHKERNEL_IGenericHPDAnalysisTool_H 1

// STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"

/// Static Interface Identification
static const InterfaceID IID_IRichGenericHPDAnalysisTool( "Rich::IGenericHPDAnalysisTool", 1, 0 );

namespace Rich
{
  //-----------------------------------------------------------------------------
  /** @class IGenericHPDAnalysisTool RichKernel/IRichGenericHPDAnalysisTool.h
   *
   *  Interface for tools that perform any data anlysis on a single HPD data block
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   10/10/2009
   */
  //-----------------------------------------------------------------------------

  class IGenericHPDAnalysisTool : public virtual IAlgTool
  {

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() { return IID_IRichGenericHPDAnalysisTool; }

  public:

    /** @class Result RichKernel/IRichGenericHPDAnalysisTool.h
     *
     *  Results of the HPD analysis
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/10/2009
     */
    class Result
    {
    public:
      LHCb::RichSmartID id; ///< Detector component the result refers to
      std::string message;  ///< Message associated with the result
      StatusCode status;    ///< StatusCode indicating the severity of the report
    };

    /// Type for a list of results
    using Results = std::vector<Result>;

  public:

    /** Applies HPD data analysis to given HPD data
     *
     *  @param hpdID    HPD identifier
     *  @param smartIDs Reference to vector of pixel RichSmartIDs for the given HPD
     *
     *  @return StatusCode indicating if the analysis was successfully run or not
     */
    virtual StatusCode analyse( const LHCb::RichSmartID hpdID,
                                const LHCb::RichSmartID::Vector & smartIDs,
                                IGenericHPDAnalysisTool::Results & results ) const = 0;

  };

}

#endif // RICHKERNEL_IGenericHPDAnalysisTool_H
