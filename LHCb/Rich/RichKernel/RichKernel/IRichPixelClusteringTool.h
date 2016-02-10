
//-----------------------------------------------------------------------------
/** @file IRichPixelClusteringTool.h
 *
 *  Header file for tool interface : Rich::DAQ::IPixelClusteringTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRichPixelClusteringTool_H
#define RICHKERNEL_IRichPixelClusteringTool_H 1

// STL
#include <memory>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichPixelCluster.h"

/// Static Interface Identification
static const InterfaceID IID_IRichPixelClusteringTool( "Rich::DAQ::IPixelClusteringTool", 1, 0 );

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class IPixelClusteringTool IRichPixelClusteringTool.h RichKernel/IRichPixelClusteringTool.h
     *
     *  Interface to tools that perform RICH HPD pixel clustering
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

      /** Applies pixel clustering to the given HPD RichSmartIDs
       *
       *  @param smartIDs Reference to vector of pixel RichSmartIDs to cluster
       *
       *  @return A smart unique pointer to the set of clusters for this HPD
       *
       *  @attention The user takes ownership of the returned object. 
       */
      virtual std::unique_ptr<const Rich::HPDPixelClusters>
      findClusters( LHCb::RichSmartID::Vector & smartIDs ) const = 0;
      
    };

  }
}

#endif // RICHKERNEL_IRichHighOccHPDSuppressionTool_H
