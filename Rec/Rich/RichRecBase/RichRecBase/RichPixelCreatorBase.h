
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorBase.h
 *
 *  Header file for tool base class : Rich::Rec::PixelCreatorBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/04/2005
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHPIXELCREATORBASE_H
#define RICHRECBASE_RICHPIXELCREATORBASE_H 1

// STL
#include <functional>
#include <vector>

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichKernel/IRichPixelSuppressionTool.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichKernel/IRichPixelClusteringTool.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "RichRecBase/IRichRecGeomTool.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/BoostArray.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Boost
#ifdef __INTEL_COMPILER                     // Disable ICC remark from Boost
  #define BOOST_MULTI_ARRAY_NO_GENERATORS 0 // zero used for undefined preprocessing identifier "BOOST_MULTI_ARRAY_NO_GENERATORS"
#endif
#include "boost/multi_array.hpp"
#ifdef __INTEL_COMPILER
  #undef BOOST_MULTI_ARRAY_NO_GENERATORS
#endif


namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------------
    /** @class PixelCreatorBase RichPixelCreatorBase.h RichRecBase/RichPixelCreatorBase.h
     *
     *  Base class for all RichRecPixel creator tools.
     *
     *  Implements common functionaility needed by all concrete implementations.
     *  Derived classes must implement the methods newPixels and buildPixel using
     *  whatever means appropriate for that implementation.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   20/04/2005
     *
     *  @todo Find a better way to handle the filling of the pixel iterators, that avoids a
     *        seperate loop and if possible the maps
     */
    //---------------------------------------------------------------------------------------

    class PixelCreatorBase : public Rich::Rec::ToolBase,
                             virtual public IPixelCreator,
                             virtual public IIncidentListener
    {

    public:

      /// Standard constructor
      PixelCreatorBase( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

      /// Destructor
      virtual ~PixelCreatorBase( ) {};

      // Initialize method
      virtual StatusCode initialize();

      // Finalize method
      virtual StatusCode finalize();

      // Implement the handle method for the Incident service.
      // This is used to inform the tool of software incidents.
      virtual void handle( const Incident& incident );

    public: // methods from interface

      // Returns a pointer to the RichRecPixels
      LHCb::RichRecPixels * richPixels() const;


      // Access the range for the pixels in the given RICH detector
      IPixelCreator::PixelRange range( const Rich::DetectorType rich ) const;

      // Access the range for the pixels in the given RICH detector and HPD panel
      IPixelCreator::PixelRange range( const Rich::DetectorType rich,
                                       const Rich::Side         panel ) const;

      // Access the begin iterator for the pixels in the given RICH HPD
      IPixelCreator::PixelRange range( const LHCb::RichSmartID hpdID ) const;

      // Form all possible RichRecPixels from RawBuffer
      // The most efficient way to make all RichRecPixel objects in the event.
      StatusCode newPixels() const;

    protected: // methods

      /// Initialise for a new event
      virtual void InitNewEvent();

      /// Finalise current event
      virtual void FinishEvent();

      /// Read the pixels and fill the RICH and panel begin and end iterators
      void fillIterators() const;

      /// Reset all iterators to default values
      void resetIterators() const;

      /// Sort the RichRecPixel container into detector regions
      void sortPixels() const;

      /// Access the final RichRecPixel location in the TES
      const std::string & pixelLocation() const;

      /// Is book keeping to be performed
      bool bookKeep() const;

      /** Check if a given RICH detector is to be used
       *
       *  @param rich The RICH detector type
       *
       *  @return boolean indicating if the given RICH detector is active
       *  @retval true  RICH detector is active
       *  @retval false RICH detector is not in use
       */
      bool useDetector( const Rich::DetectorType rich ) const;

      /** Check the status of the given RICH HPD pixel identifier (RichSmartID)
       *
       *  @param id The HPD pixel RichSmartID to check
       *
       *  @return boolean indicating if the given channel is active
       *  @retval true  channel is active
       *  @retval false channel is not in use
       */
      bool pixelIsOK( const LHCb::RichSmartID id ) const;

      /** Check the status of the given RICH HPD identifier (RichSmartID)
       *
       *  @param id The HPD RichSmartID to check
       *
       *  @return boolean indicating if the given channel is active
       *  @retval true  HPD is active
       *  @retval false HPD is not in use
       */
      bool hpdIsOK( const LHCb::RichSmartID id ) const;

      /** Save a given pixel to the TES container
       *
       *  @param pix Pointer to the RichRecPixel to save
       */
      void savePixel( LHCb::RichRecPixel * pix ) const;

      /** Apply HPD pixel suppression, if configured to do so
       *
       *  @param hpdID    RichSmartID for HPD
       *  @param smartIDs Vector of pixel smartIDs for this HPD
       *
       *  @return boolean indicating if any suppression occured
       *  @retval true Some (or all) pixels have been suppressed. In the case the vector of
       *               pixel RichSmartIDs is changed
       *  @retval false No pixels are suppressed
       */
      bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                  LHCb::RichSmartID::Vector & smartIDs ) const;

      /// Build a new RichRecPixel from an Rich::HPDPixelCluster
      virtual LHCb::RichRecPixel * buildPixel ( const Rich::HPDPixelCluster& cluster ) const;

      /// Build a new RichRecPixel from a single LHCb::RichSmartID
      virtual LHCb::RichRecPixel * buildPixel( const LHCb::RichSmartID & id ) const;

      /// Access the RichSmartIDTool
      inline const ISmartIDTool * smartIDTool() const
      {
        if (!m_idTool) { acquireTool( "RichSmartIDTool", m_idTool, nullptr, true ); }
        return m_idTool;
      }

      /// Access the RichSmartIDDecoder
      inline const Rich::DAQ::IRawBufferToSmartIDsTool * smartIDdecoder() const
      {
        if (!m_decoder) { acquireTool( "RichSmartIDDecoder", m_decoder, nullptr, true ); }
        return m_decoder;
      }

      /// Access the geometry tool
      inline const IGeomTool * geomTool() const
      {
        return m_geomTool;
      } 

    protected: // data

      /// Flag to signify all pixels have been formed
      mutable bool m_allDone = false;

      /// Map between RichSmartID and the associated RichRecPixel
      mutable Rich::HashMap< LHCb::RichSmartID::KeyType, LHCb::RichRecPixel* > m_pixelExists;

      /// Map indicating if a given RichSmartID has been considered already
      mutable Rich::HashMap< LHCb::RichSmartID::KeyType, bool > m_pixelDone;

    private: // helper classes

      /** @class SortByRegion RichPixelCreatorBase.h RichRecBase/RichPixelCreatorBase.h
       *
       *  Class to sort the RichRecPixels according to detector regions
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   20/04/2005
       */
      class SortByRegion
        : std::binary_function<const LHCb::RichRecPixel*,const LHCb::RichRecPixel*,bool>
      {
      public:
        /// Method to return true if RichRecPixel p1 should be listed before p2
        inline bool operator() ( const LHCb::RichRecPixel * p1,
                                 const LHCb::RichRecPixel * p2 ) const
        {
          // only using the first smartID here. what else can we do ?
          return ( p1->hpdPixelCluster().primaryID().dataBitsOnly().key() <
                   p2->hpdPixelCluster().primaryID().dataBitsOnly().key() );
        }
      };

      /** @class PixStats RichPixelCreatorBase.h RichRecBase/RichPixelCreatorBase.h
       *
       *  Simple class to hold pixel/cluster creation statistics
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   20/04/2005
       */
      class PixStats
      {
      public:
        PixStats() : numClusters(0), numPixels(0), rejectedPixels(0) { }
      public:
        unsigned long long numClusters;     ///< Number of selected HPD pixel clusters
        unsigned long long numPixels;       ///< Number of selected raw HPD pixels
        unsigned long long rejectedPixels;  ///< number of rejected HPD pixels
      public:
        /// Has this tally seen some pixels
        bool hasSomeStats() const { return numPixels>0 || rejectedPixels>0 ; }
      };

    private: // data

      /// Pointer to RICH system detector element
      const DeRichSystem * m_richSys = nullptr;

      /// HPD occupancy tools
      mutable std::vector<const Rich::DAQ::IPixelSuppressionTool *> m_hpdOcc;

      /// HPD clustering tools
      mutable std::vector<const Rich::DAQ::IPixelClusteringTool *> m_hpdClus;

      /// Pointer to RichSmartID tool
      mutable const ISmartIDTool * m_idTool = nullptr;

      /// Raw Buffer Decoding tool
      mutable const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder = nullptr;

      /// Geometry tool
      const IGeomTool * m_geomTool = nullptr;

      /// Pointer to RichRecPixels
      mutable LHCb::RichRecPixels * m_pixels = nullptr;

      /// Flag to turn on or off the book keeping features to save cpu time.
      bool m_bookKeep;

      /// Flag to turn on or off the explicit checking of the HPD status
      bool m_hpdCheck;

      /** @brief Flag to turn on/off use of clusters in pixel reconstruction.
       *
       *  If set to true cluster finding will be run, and a single RichRecPixel will be created for each cluster.
       *  If false, cluster finding will STILL be run but each hit will be made into a single RichRecPixel.
       *  In both cases, the associated cluster for each RichRecPixel is still set.
       */
      std::vector<bool> m_clusterHits;

      /** @brief Flag to suppress the finding of clusters entirely (for speed)
       *  If set to true, no clusters will be found and the associated cluster pointers are not set.
       */
      bool m_noClusterFinding;

      /// Flags for which RICH detectors to create pixels for
      std::vector<bool> m_usedDets;

      /// Location of RichRecPixels in TES
      std::string m_richRecPixelLocation;

      /// Begin iterators for various Rich and panel combinations
      mutable boost::multi_array<LHCb::RichRecPixels::iterator,2> m_begins;

      /// End iterators for various Rich and panel combinations
      mutable boost::multi_array<LHCb::RichRecPixels::iterator,2> m_ends;

      /// Begin iterators for each RICH
      mutable boost::array<LHCb::RichRecPixels::iterator,Rich::NRiches> m_richBegin;

      /// End iterators for each RICH
      mutable boost::array<LHCb::RichRecPixels::iterator,Rich::NRiches> m_richEnd;

      /// HPD range map type
      typedef std::pair<LHCb::RichRecPixels::iterator,LHCb::RichRecPixels::iterator> HPDItPair;
      typedef Rich::Map<const LHCb::RichSmartID,HPDItPair> HPDItMap;

      /// iterators for each HPD
      mutable HPDItMap m_hpdIts;

      /// Hit count tally
      mutable boost::array<PixStats, Rich::NRiches> m_hitCount;

      /// Event count
      unsigned long long m_Nevts = 0;

      /// Flag to indicate if the tool has been used in a given event
      mutable bool m_hasBeenCalled = false;

      /// Should HPD occupancy be monitored
      bool m_applyPixelSuppression;

      /// Maximum number of pixels per event
      unsigned int m_maxPixels;

    private: // methods

      /// Printout the pixel creation statistics
      void printStats() const;

      /// returns a pointer to the HPD suppression tool for the given RICH
      const Rich::DAQ::IPixelSuppressionTool * hpdSuppTool( const Rich::DetectorType rich ) const;

      /// returns a pointer to the HPD clustering tool for the given RICH
      const Rich::DAQ::IPixelClusteringTool * hpdClusTool( const Rich::DetectorType rich ) const;
      
    };

    inline void PixelCreatorBase::sortPixels() const
    {
      std::stable_sort( richPixels()->begin(), richPixels()->end(), SortByRegion() );
    }

    inline const std::string & PixelCreatorBase::pixelLocation() const
    {
      return m_richRecPixelLocation;
    }

    inline bool PixelCreatorBase::bookKeep() const
    {
      return m_bookKeep;
    }

    inline bool PixelCreatorBase::useDetector( const Rich::DetectorType rich ) const
    {
      return m_usedDets[rich];
    }

    inline bool PixelCreatorBase::pixelIsOK( const LHCb::RichSmartID id ) const
    {
      return id.isValid();
    }

    inline bool PixelCreatorBase::hpdIsOK( const LHCb::RichSmartID id ) const
    {
      return ( id.isValid() &&                              // Valid HPD ID
               useDetector(id.rich()) &&                    // This RICH is in use
               ( !m_hpdCheck || m_richSys->pdIsActive(id) ) // If required, check HPD is alive
               );
    }

    inline void PixelCreatorBase::savePixel( LHCb::RichRecPixel * pix ) const
    {
      richPixels()->insert( pix );
      ++(m_hitCount[pix->detector()].numClusters);
      m_hitCount[pix->detector()].numPixels += pix->hpdPixelCluster().size();
      m_hasBeenCalled = true;
    }

    inline void PixelCreatorBase::resetIterators() const
    {

      // RICH and panels
      m_begins[Rich::Rich1][Rich::top]    = richPixels()->begin();
      m_begins[Rich::Rich1][Rich::bottom] = richPixels()->begin();
      m_begins[Rich::Rich2][Rich::left]   = richPixels()->begin();
      m_begins[Rich::Rich2][Rich::right]  = richPixels()->begin();
      m_ends[Rich::Rich1][Rich::top]      = richPixels()->begin();
      m_ends[Rich::Rich1][Rich::bottom]   = richPixels()->begin();
      m_ends[Rich::Rich2][Rich::left]     = richPixels()->begin();
      m_ends[Rich::Rich2][Rich::right]    = richPixels()->begin();

      // RICH only
      m_richBegin[Rich::Rich1] = richPixels()->begin();
      m_richBegin[Rich::Rich2] = richPixels()->begin();
      m_richEnd[Rich::Rich1]   = richPixels()->begin();
      m_richEnd[Rich::Rich2]   = richPixels()->begin();

      // HPD
      m_hpdIts.clear();

    }

    inline const Rich::DAQ::IPixelSuppressionTool *
    PixelCreatorBase::hpdSuppTool( const Rich::DetectorType rich ) const
    {
      if ( !m_hpdOcc[rich] )
      {
        acquireTool( "PixelSuppress"+Rich::text(rich), m_hpdOcc[rich], this );
      }
      return m_hpdOcc[rich];
    }

    inline const Rich::DAQ::IPixelClusteringTool *
    PixelCreatorBase::hpdClusTool( const Rich::DetectorType rich ) const
    {
      if ( !m_hpdClus[rich] )
      {
        acquireTool( "PixelCreatorClustering",
                     "PixelClustering"+Rich::text(rich), m_hpdClus[rich], this );
      }
      return m_hpdClus[rich];
    }

    inline bool
    PixelCreatorBase::applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                             LHCb::RichSmartID::Vector & smartIDs ) const
    {
      const unsigned int startSize = smartIDs.size();
      const bool suppressed =
        ( !m_applyPixelSuppression ? false : hpdSuppTool(hpdID.rich())->applyPixelSuppression(hpdID,smartIDs) );
      m_hitCount[hpdID.rich()].rejectedPixels += (startSize-smartIDs.size());
      return suppressed;
    }

  }
} // RICH

  /** Backwards compatibility typedef
   * @todo Remove eventually
   */
typedef Rich::Rec::PixelCreatorBase RichPixelCreatorBase;

#endif // RICHRECBASE_RICHPIXELCREATORBASE_H
