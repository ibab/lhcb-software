
//--------------------------------------------------------------------------------
/** @file RichPixelCluster.h
 *
 *  Header file for pixel clustering class Rich::DAQ::PixelCluster
 *
 *  CVS Log :-
 *  $Id: RichPixelCluster.h,v 1.1 2007-02-02 17:40:14 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   02/02/2007
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RichPixelCluster_H
#define RICHKERNEL_RichPixelCluster_H 1

// STL
#include <vector>
#include <ostream>

// Gaudi
#include "GaudiKernel/MsgStream.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/BoostMemPoolAlloc.h"

// Boost
#include <boost/shared_ptr.hpp>

namespace Rich
{

  namespace
  {
    /// Number of HPD pixel rows or columns
    static const int nPixelRowsOrCols = 32;
  }

  //-----------------------------------------------------------------------------
  /** @class HPDPixelClusters RichPixelCluster.h
   *
   *  Utility class representing a set of clusters for a single HPD
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   21/03/2006
   */
  //-----------------------------------------------------------------------------
  class HPDPixelClusters : public Rich::BoostMemPoolAlloc<HPDPixelClusters>
  {

  public:

    /// const Shared pointer to a HPDPixelClusters
    typedef boost::shared_ptr<const HPDPixelClusters> ConstSharedPtn;

    /// Shared pointer to a HPDPixelClusters
    typedef boost::shared_ptr<HPDPixelClusters> SharedPtn;

  public:

    //-----------------------------------------------------------------------------
    /** @class Cluster RichPixelCluster.h
     *
     *  Utility class representing a cluster of HPD pixels
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     */
    //-----------------------------------------------------------------------------
    class Cluster : public Rich::BoostMemPoolAlloc<HPDPixelClusters::Cluster>
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
    HPDPixelClusters ( const LHCb::RichSmartID::Vector & smartIDs );

    /// Destructor
    ~HPDPixelClusters();

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
                      const unsigned int maxSize ) const;

    /// Print in a human readable way
    MsgStream& fillStream( MsgStream& os ) const;

    /// Overload output to MsgStream
    friend inline MsgStream& operator << ( MsgStream& os,
                                           const HPDPixelClusters & data )
    { return data.fillStream(os); }

  private:

    /// Raw input data (false means no hit, true means hit)
    bool m_data[nPixelRowsOrCols][nPixelRowsOrCols];

    /// Assigned cluster for each pixel
    Cluster * m_clusters[nPixelRowsOrCols][nPixelRowsOrCols];

    /// Vector of all created clusters
    std::vector<Cluster*> m_allclus;

  };

  inline void HPDPixelClusters::setOn( const int row, const int col )
  {
    (m_data[row])[col] = true;
  }

  inline bool HPDPixelClusters::isOn( const int row, const int col ) const
  {
    return ( row>=0 && row<nPixelRowsOrCols &&
             col>=0 && col<nPixelRowsOrCols && (m_data[row])[col] );
  }

  inline HPDPixelClusters::Cluster *
  HPDPixelClusters::getCluster( const int row, const int col ) const
  {
    return ( isOn(row,col) ? (m_clusters[row])[col] : NULL );
  }

  inline void HPDPixelClusters::setCluster( const int row, const int col, Cluster * clus )
  {
    (m_clusters[row])[col] = clus;
    clus->addPixel(row,col);
  }

  inline HPDPixelClusters::Cluster *
  HPDPixelClusters::createNewCluster( const int id )
  {
    m_allclus.push_back( new Cluster(id) );
    return m_allclus.back();
  }

}

#endif // RICHKERNEL_RichPixelCluster_H
