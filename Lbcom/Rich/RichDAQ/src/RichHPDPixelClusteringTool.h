
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusteringTool.h
 *
 *  Header file for tool : Rich::DAQ::HPDPixelClusteringTool
 *
 *  CVS Log :-
 *  $Id: RichHPDPixelClusteringTool.h,v 1.6 2007-03-01 19:39:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDPixelClusteringTool_H
#define RICHDAQ_RichHPDPixelClusteringTool_H 1

// base class
#include "RichKernel/RichToolBase.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"
#include "RichKernel/RichSmartIDSorter.h"

// interface
#include "RichKernel/IRichPixelClusteringTool.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class HPDPixelClusteringTool RichHPDPixelClusteringTool.h
     *
     *  An implementation of the HPD pixel clustering tool interface.
     *
     *  This implementation forms simple clusters of connected (direct neighbours) pixels
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     *
     *  @todo Review if it is really need to sort the HPD hits each time (should not)
     */
    //-----------------------------------------------------------------------------

    class HPDPixelClusteringTool : public RichToolBase,
                                   virtual public IPixelClusteringTool
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      HPDPixelClusteringTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent );

      /// Initalisation
      virtual StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      /// Applies a pixel clusering algorithm to the given HPD RichSmartIDs
      virtual const Rich::HPDPixelClusters::ConstSharedPtn
      findClusters( LHCb::RichSmartID::Vector & smartIDs ) const;

    private: // data members

      /// Minimum cluster size
      unsigned int m_minClusSize;

      /// Maximum cluster size
      unsigned int m_maxClusSize;

      /// Allow pixels to be clustered across diagonals
      bool m_allowDiags;

    };

  }
}

#endif // RICHDAQ_RichHPDPixelClusteringTool_H
