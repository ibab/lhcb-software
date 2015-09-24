
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusteringTool.h
 *
 *  Header file for tool : Rich::DAQ::HPDPixelClusteringTool
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDPixelClusteringTool_H
#define RICHDAQ_RichHPDPixelClusteringTool_H 1

// STL
#include <functional>

// base class
#include "RichKernel/RichToolBase.h"

// Kernel
//#include "Kernel/MemPoolAlloc.h"
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
     *      info() << mindeltaLL << endmsg;
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     *
     *  @todo Review if it is really need to sort the HPD hits each time (should not)
     */
    //-----------------------------------------------------------------------------

    class HPDPixelClusteringTool : public Rich::ToolBase,
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
      virtual const Rich::HPDPixelClusters *
      findClusters( LHCb::RichSmartID::Vector & smartIDs ) const;

    private:

      /** @class SortALICEIDs RichHPDPixelClusteringTool.h
       *  Class to sort ALICE RichSmartIDs into the correct order for cluster finding
       */
      class SortALICEIDs
        : std::binary_function< const LHCb::RichSmartID, const LHCb::RichSmartID, bool >
      {

      public:

        /** Sort operator for the RichSmartIDs
         *
         *  Sorts ALICE mode pixels into the correct order for cluster finding
         *
         *  @param p1 First RichSmartID
         *  @param p2 Second RichSmartID
         *
         *  @return bool indicating if p1 should be listed before p2
         */
        inline bool operator() ( const LHCb::RichSmartID& p1, const LHCb::RichSmartID& p2 ) const
        {
          return ( 100000*((Rich::DAQ::NumAlicePixelsPerLHCbPixel*p1.pixelRow())+p1.pixelSubRow()) + p1.pixelCol() <
                   100000*((Rich::DAQ::NumAlicePixelsPerLHCbPixel*p2.pixelRow())+p2.pixelSubRow()) + p2.pixelCol() );
        }

      };

    private:

      /// Sort the RichSmartIDs into the correct order for this algorithm
      inline void sortIDs( LHCb::RichSmartID::Vector & smartIDs ) const
      {
        if ( !smartIDs.empty() )
        {
          if ( UNLIKELY( smartIDs.front().pixelSubRowDataIsValid() ) )
          {
            // use own method for ALICE mode
            std::stable_sort( smartIDs.begin(), smartIDs.end(), SortALICEIDs() );
          }
          else
          {
            // LHCb mode is OK to use (faster) helper class
            SmartIDSorter::sortByRegion(smartIDs);
          }
        }
      }
      
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
