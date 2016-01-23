
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.h
 *
 *  Header file for tool : Rich::DAQ::HPDPixelClusterSuppressionTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDPixelClusterSuppressionTool_H
#define RICHDAQ_RichHPDPixelClusterSuppressionTool_H 1

// base class
#include "RichHighOccHPDSuppressionTool.h"

// Kernel
//#include "Kernel/MemPoolAlloc.h"

// interfaces
#include "RichKernel/IRichPixelClusteringTool.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class HPDPixelClusterSuppressionTool RichHPDPixelClusterSuppressionTool.h
     *
     *  Tool for monitoring high occupancy HPDs
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     */
    //-----------------------------------------------------------------------------

    class HPDPixelClusterSuppressionTool : public HighOccHPDSuppressionTool
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      HPDPixelClusterSuppressionTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

      // Initialization of the tool after creation
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Monitor the occupancy of the given HPD
      bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                  LHCb::RichSmartID::Vector & smartIDs ) const;

    private: // private data

      /// Pointer to the clustering tool
      const IPixelClusteringTool * m_clustTool = nullptr;

      /// Maximum allowable pixel cluster size
      unsigned int m_maxPixClusterSize;

      /// Minimum HPD occupancy to bother with running cluster finding
      unsigned int m_minHPDocc;

    };

  }
}

#endif // RICHDAQ_RichHPDPixelClusterSuppressionTool_H
