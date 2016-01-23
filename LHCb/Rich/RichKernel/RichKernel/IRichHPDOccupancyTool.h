
//-----------------------------------------------------------------------------
/** @file IRichHPDOccupancyTool.h
 *
 *  Header file for tool interface : Rich::DAQ::IHPDOccupancyTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRichHPDOccupancyTool_H
#define RICHKERNEL_IRichHPDOccupancyTool_H 1

// STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"

/// Static Interface Identification
static const InterfaceID IID_IRichHPDOccupancyTool( "Rich::DAQ::IHPDOccupancyTool", 1, 0 );

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class IHPDOccupancyTool RichKernel/IRichHPDOccupancyTool.h
   *
   *  Interface for tool which determines the occupancy in each HPD
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   15/03/2002
   */
  //-----------------------------------------------------------------------------

  class IHPDOccupancyTool : public virtual IAlgTool
  {

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() { return IID_IRichHPDOccupancyTool; }

    /** Returns the average occupancy for the given HPD
     *
     *  @param hpdID HPD identifier
     *  @param smartIDs RichSmartIDS for the given HPD for the current event
     *
     *  @return The average HPD occupancy
     */
    virtual double averageOccupancy( const LHCb::RichSmartID hpdID ) const = 0;

  };

}

#endif // RICHKERNEL_IRichHPDOccupancyTool_H
