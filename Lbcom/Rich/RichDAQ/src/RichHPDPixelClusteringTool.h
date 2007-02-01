
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusteringTool.h
 *
 *  Header file for tool : Rich::DAQ::HPDPixelClusteringTool
 *
 *  CVS Log :-
 *  $Id: RichHPDPixelClusteringTool.h,v 1.1 2007-02-01 17:42:29 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDPixelClusteringTool_H
#define RICHDAQ_RichHPDPixelClusteringTool_H 1

// base class
#include "RichHighOccHPDSuppressionTool.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"
#include "RichKernel/RichSmartIDSorter.h"

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

    namespace PixClusInfo
    {
      static const int nPixelRowsOrCols = 32;
    }

    //-----------------------------------------------------------------------------
    /** @class HPDPixelClusteringTool RichHPDPixelClusteringTool.h
     *
     *  Tool for monitoring high occupancy HPDs
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     */
    //-----------------------------------------------------------------------------

    class HPDPixelClusteringTool : public HighOccHPDSuppressionTool
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      HPDPixelClusteringTool( const std::string& type,
                              const std::string& name,
                              const IInterface* parent );

      // Initialization of the tool after creation
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Monitor the occupancy of the given HPD
      bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                  LHCb::RichSmartID::Vector & smartIDs ) const;

    private: // private data

      /// Maximum allowable pixel cluster size
      unsigned int m_maxPixClusterSize;

      /// Minimum HPD occupancy to bother with running cluster finding
      unsigned int m_minHPDocc;

    private: // utility classes

      //-----------------------------------------------------------------------------
      /** @class PixelData RichHPDPixelClusteringTool.h
       *
       *  Utility class representing a single HPD pixel data in a form useful
       *  for clustering
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   21/03/2006
       */
      //-----------------------------------------------------------------------------
      class PixelData
      {

      public:

        //-----------------------------------------------------------------------------
        /** @class Cluster RichHPDPixelClusteringTool.h
         *
         *  Utility class representing a cluster of HPD pixels
         *
         *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
         *  @date   21/03/2006
         */
        //-----------------------------------------------------------------------------
        class Cluster :
          public Rich::BoostMemPoolAlloc<HPDPixelClusteringTool::PixelData::Cluster>
        {

        public: // definitions

          /// data describing a single pixel
          typedef std::pair<unsigned int,unsigned int> Pixel;

          /// a vector of pixels
          typedef std::vector< Pixel >                 Data;


        public: // methods

          /// Constructor
          Cluster( const int id = -1 ) : m_clusterID(id)
          { m_cluster.reserve(10); }

          /// Get cluster ID
          inline int id() const { return m_clusterID; }

          /// Add a pixel (row,col) to this cluster
          inline void addPixel( const unsigned int row,
                                const unsigned int col )
          { m_cluster.push_back( Pixel(row,col) ); }

          /// Add a pixel (row,col) to this cluster
          inline void addPixel( const Pixel & pixel )
          { m_cluster.push_back( pixel ); }

          /// Get read access to cluster data
          inline const Cluster::Data & pixels() const
          { return m_cluster; }

          /// Return cluster size
          inline unsigned int size() const
          { return pixels().size(); }

        private:

          int  m_clusterID;  ///< Cluster ID
          Data m_cluster;    ///< list of pixels in this cluster

        };

      public: // methods

        /// Constructor from a list of RichSmartIDs
        PixelData ( const LHCb::RichSmartID::Vector & smartIDs );

        /// Destructor
        ~PixelData();

        /// Set given col and row on
        void setOn( const int row, const int col );

        /// Check if given row and col is on
        bool isOn( const int row, const int col ) const;

        /// Get cluster for given pixel
        Cluster * getCluster( const int row, const int col ) const;

        /// Set cluster for given pixel
        void setCluster( const int row, const int col, Cluster * clus );

        /// Create a new cluster with given ID
        Cluster * createNewCluster( const int id );

        /// Create a new cluster with given ID
        Cluster * mergeClusters( Cluster * clus1, Cluster * clus2 );

        /// Create a new vector of suppressed RichSmartIDs
        void suppressIDs( LHCb::RichSmartID::Vector & smartIDs,
                          const unsigned int maxSize );

        /// Print in a human readable way
        MsgStream& fillStream( MsgStream& os ) const;

        /// Overload output to ostream
        friend inline MsgStream& operator << ( MsgStream& os,
                                               const PixelData & data )
        { return data.fillStream(os); }

      private:

        /// Raw input data (false means no hit, true means hit)
        bool m_data[PixClusInfo::nPixelRowsOrCols][PixClusInfo::nPixelRowsOrCols];

        /// Assigned cluster for each pixel
        Cluster * m_clusters[PixClusInfo::nPixelRowsOrCols][PixClusInfo::nPixelRowsOrCols];

        /// Vector of all created clusters
        std::vector<Cluster*> m_allclus;

      };

    };

    inline void HPDPixelClusteringTool::
    PixelData::setOn( const int row, const int col )
    {
      (m_data[row])[col] = true;
    }

    inline bool HPDPixelClusteringTool::
    PixelData::isOn( const int row, const int col ) const
    {
      return ( row>=0 && row<PixClusInfo::nPixelRowsOrCols &&
               col>=0 && col<PixClusInfo::nPixelRowsOrCols && (m_data[row])[col] );
    }

    inline HPDPixelClusteringTool::PixelData::Cluster *
    HPDPixelClusteringTool::
    PixelData::getCluster( const int row, const int col ) const
    {
      return ( isOn(row,col) ? (m_clusters[row])[col] : NULL );
    }

    inline void HPDPixelClusteringTool::
    PixelData::setCluster( const int row, const int col, Cluster * clus )
    {
      (m_clusters[row])[col] = clus;
      clus->addPixel(row,col);
    }

    inline HPDPixelClusteringTool::
    PixelData::PixelData( const LHCb::RichSmartID::Vector & smartIDs )
    {
      // initialise the c arrays
      memset ( m_data,     0, sizeof(m_data)     );
      memset ( m_clusters, 0, sizeof(m_clusters) );
      m_allclus.reserve(10);
      // set the hit pixels as "on"
      for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
            iS != smartIDs.end(); ++iS )
      {
        setOn( (*iS).pixelRow(), (*iS).pixelCol() );
      }
    }

    inline HPDPixelClusteringTool::
    PixelData::~PixelData()
    {
      for ( std::vector<Cluster*>::iterator i = m_allclus.begin();
            i != m_allclus.end(); ++i ) { delete *i; }
    }

    inline HPDPixelClusteringTool::PixelData::Cluster *
    HPDPixelClusteringTool::
    PixelData::mergeClusters( Cluster * clus1, Cluster * clus2 )
    {
      // add pixels to clus1
      for ( Cluster::Data::const_iterator i = clus2->pixels().begin();
            i != clus2->pixels().end(); ++i )
      {
        clus1->addPixel( *i );
        setCluster( (*i).first, (*i).second, clus1 );
      }
      // return clus1 as merged cluster
      return clus1;
    }

    inline HPDPixelClusteringTool::PixelData::Cluster *
    HPDPixelClusteringTool::
    PixelData::createNewCluster( const int id )
    {
      Cluster * clus = new Cluster(id);
      m_allclus.push_back(clus);
      return clus;
    }

  }
}

#endif // RICHDAQ_RichHPDPixelClusteringTool_H
