
//-----------------------------------------------------------------------------
/** @file IRichPixelClusteringTool.h
 *
 *  Header file for tool interface : Rich::DAQ::IPixelClusteringTool
 *
 *  CVS Log :-
 *  $Id: IRichPixelClusteringTool.h,v 1.1 2007-02-01 17:41:46 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRichPixelClusteringTool_H
#define RICHKERNEL_IRichPixelClusteringTool_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"

/// Static Interface Identification
static const InterfaceID IID_IRichPixelClusteringTool( "Rich::DAQ::IPixelClusteringTool", 1, 0 );

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace DAQ
   *
   *  namespace for RICH DAQ software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class IPixelClusteringTool IRichPixelClusteringTool.h RichKernel/IRichPixelClusteringTool.h
     *
     *  Interface for tool for apply suppression to HPD pixels
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class IPixelClusteringTool : public virtual IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichPixelClusteringTool; }

      /** Applies a pixel clusering algorithm to the given HPD data
       *
       *  @param hpdID    HPD identifier
       *  @param smartIDs Reference to vector of pixel RichSmartIDs for the given HPD
       *
       *  @return 
       */
      virtual bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                          LHCb::RichSmartID::Vector & smartIDs ) const = 0;

    };

  }
}

#endif // RICHKERNEL_IRichHighOccHPDSuppressionTool_H
