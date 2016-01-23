
//---------------------------------------------------------------------------------
/** @file IRichDetectorTool.h
 *
 *  Header file for tool interface : Rich::IDetectorTool
 *
 *  @author Antonis Papanestis  antonis.papanestis@stfc.ac.uk
 *  @date   2012-10-26
 */
//---------------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHDETECTORTOOL_H
#define RICHKERNEL_IRICHDETECTORTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// forward decs
class DeRich;

/// Static Interface Identification
static const InterfaceID IID_IRichDetectorTool( "Rich::IDetectorTool", 1, 0 );

namespace Rich
{

  //---------------------------------------------------------------------------------
  /** @class IDetectorTool IRichDetectorTool.h RichKernel/IRichDetectorTool.h
   *
   *  Interface to tool for the location of the RICH DeRich detector objects
   *
   *  @author Antonis Papanestis  antonis.papanestis@stfc.ac.uk
   *  @date   2012-10-26
   */
  //---------------------------------------------------------------------------------

  class IDetectorTool : virtual public IAlgTool
  {

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() { return IID_IRichDetectorTool; }

    virtual const std::vector<DeRich*>& deRichDetectors() const = 0;

  };

}

#endif // RICHKERNEL_IRICHDETECTORTOOL_H
