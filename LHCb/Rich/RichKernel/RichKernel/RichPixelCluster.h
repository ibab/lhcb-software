
//--------------------------------------------------------------------------------
/** @file RichPixelCluster.h
 *
 *  Header file for pixel clustering class Rich::DAQ::PixelCluster
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   02/02/2007
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RichPixelCluster_H
#define RICHKERNEL_RichPixelCluster_H 1

// STL
#include <vector>
#include <list>
#include <ostream>

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SerializeSTL.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/MemPoolAlloc.h"
#include "Kernel/FastAllocList.h"
#include "Kernel/FastAllocVector.h"

// RichKernel
#include "RichKernel/RichDAQDefinitions.h"

namespace Rich
{
  // Needed to get vector printing to work
  using GaudiUtils::operator<<;

  //-----------------------------------------------------------------------------------------------
  /** @class HPDPixelCluster RichPixelCluster.h RichKernel/RichPixelCluster.h
   *
   *  Utility class used to represent a single cluster of HPD pixels
   *
   *  @author Chris Jones
   *  @date   07/02/2007
   */
  //-----------------------------------------------------------------------------------------------
  class HPDPixelCluster : public LHCb::MemPoolAlloc<Rich::HPDPixelCluster>
  {

  public:

    /// RichSmartID vector type
    using SmartIDVector = LHCb::RichSmartID::Vector;

  public:

    /// Default Constructor
    HPDPixelCluster() = default;

    /// Default destructor
    ~HPDPixelCluster() = default;

    /// Default Copy Constructor
    HPDPixelCluster( const HPDPixelCluster& ) = default;

    /// Default Copy operator
    HPDPixelCluster& operator=( const HPDPixelCluster& ) = default;

    /// Default Move Constructor
    HPDPixelCluster( HPDPixelCluster&& ) = default;

    /// Default Move operator
    HPDPixelCluster& operator=( HPDPixelCluster&& ) = default;

  public:

    /// Constructor with reserved size
    explicit HPDPixelCluster( const SmartIDVector::size_type resSize ) 
    { m_ids.reserve(resSize); }

    /// Constructor from a single channel (one pixel cluster)
    explicit HPDPixelCluster( const LHCb::RichSmartID & id ) : m_ids(1,id) { }

    /// Copy Constructor from a vector of RichSmartIDs
    explicit HPDPixelCluster( const SmartIDVector & ids ) : m_ids(ids) { }

    /// Move Constructor from a vector of RichSmartIDs
    explicit HPDPixelCluster( SmartIDVector && ids ) : m_ids(std::move(ids)) { }

  public:

    /// const access to the list of HPD pixels
    inline const SmartIDVector & smartIDs() const  & noexcept { return m_ids; }

    /// move access to the list of HPD pixels
    inline       SmartIDVector&& smartIDs()       && noexcept { return std::move(m_ids); }

    /// The primary (seed) channel ID
    inline LHCb::RichSmartID primaryID() const
    {
      return ( !smartIDs().empty() ? smartIDs().front() : LHCb::RichSmartID() );
    }

    /// The RICH detector for this cluster
    inline Rich::DetectorType rich() const noexcept { return primaryID().rich(); }

    /// The RICH panel for this cluster
    inline LHCb::RichSmartID panel() const noexcept { return primaryID().panelID(); }

    /// The RICH HPD for this cluster
    inline LHCb::RichSmartID hpd()   const noexcept { return primaryID().pdID(); }

    /// Number of channels in this cluster
    inline SmartIDVector::size_type size() const noexcept { return smartIDs().size(); }

  public:

    /// Add a channel to this cluster
    inline void addChannel( const LHCb::RichSmartID& id ) { m_ids.emplace_back(id); }

    /// Add a channel to this cluster
    inline void addChannel( LHCb::RichSmartID&& id ) { m_ids.emplace_back(std::move(id)); }

  public:

    /// overload printout to ostream operator <<
    friend inline std::ostream& operator << ( std::ostream& s,
                                              const HPDPixelCluster & cluster )
    {
      return s << "[ " << cluster.smartIDs() << " ]";
    }

  private:

    /// The vector of RichSmartIDs for this cluster
    SmartIDVector m_ids;

  };

  //-----------------------------------------------------------------------------
  /** @class HPDPixelClusters RichPixelCluster.h
   *
   *  Utility class representing a set of clusters for a single HPD
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   21/03/2006
   */
  //-----------------------------------------------------------------------------
  class HPDPixelClusters : public LHCb::MemPoolAlloc<Rich::HPDPixelClusters>
  {

  public:

    //-----------------------------------------------------------------------------
    /** @class Cluster RichPixelCluster.h
     *
     *  Utility class representing a cluster of HPD pixels together with a unique
     *  (within an HPD) cluster ID
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     */
    //-----------------------------------------------------------------------------
    class Cluster : public LHCb::MemPoolAlloc<Rich::HPDPixelClusters::Cluster>
    {

    public: // definitions

      /// collection of cluster pointers
      using PtnVector = LHCb::Boost::PoolAllocList<Cluster*>;

    public: // methods

      /// Default Constructor
      Cluster() = default;

      /// Default destructor
      ~Cluster() = default;
      
      /// Default Copy Constructor
      Cluster( const Cluster& ) = default;
      
      /// Default Copy operator
      Cluster& operator=( const Cluster& ) = default;
      
      /// Default Move Constructor
      Cluster( Cluster&& ) = default;

      /// Default Move operator
      Cluster& operator=( Cluster&& ) = default;

      /// Constructor from ID
      Cluster( const int id ) : m_clusterID(id) { }

      /// Get cluster ID
      inline int id() const noexcept
      {
        return m_clusterID;
      }

      /// Add a pixel to this cluster
      inline void addPixel( const LHCb::RichSmartID& id )
      {
        m_cluster.addChannel( id );
      }

      /// Get read access to cluster data
      inline const Rich::HPDPixelCluster & pixels() const noexcept
      {
        return m_cluster;
      }

      /// Shortcut to the cluster size
      inline LHCb::RichSmartID::Vector::size_type size() const noexcept
      {
        return pixels().smartIDs().size();
      }

    private:

      /// Cluster ID
      int m_clusterID{-1};  

      /// list of pixels in this cluster. Initialize with reserved size 3 (rough guess).
      HPDPixelCluster m_cluster{3}; 

    };

  public: // methods

    /// Constructor
    HPDPixelClusters( ) = default;

    /// Destructor
    ~HPDPixelClusters() { for ( auto * i : m_allclus ) { delete i; } }

    /// Read access to the vector of clusters
    inline const Cluster::PtnVector & clusters() const { return m_allclus; }

    /// Write access to the vector of clusters
    inline       Cluster::PtnVector & clusters()       { return m_allclus; }

    /// Create a new vector of suppressed RichSmartIDs
    void suppressIDs( HPDPixelCluster::SmartIDVector & smartIDs,
                      const unsigned int maxSize ) const;

    /// Get the cluster for a given HPD ID
    const Cluster * getCluster( const LHCb::RichSmartID & id ) const;

  public:

    /// Print in a human readable way
    MsgStream& fillStream( MsgStream& os ) const;

    /// Overload output to MsgStream
    friend inline MsgStream& operator << ( MsgStream& os,
                                           const HPDPixelClusters & clus )
    { return clus.fillStream(os); }

  public:

    /// Add a new cluster
    inline void addCluster( Cluster * clus ) { m_allclus.push_back(clus); }

  private: // data

    /// Vector of all created clusters
    Cluster::PtnVector m_allclus;

  };

  //-----------------------------------------------------------------------------
  /** @class HPDPixelClustersBuilder RichPixelCluster.h
   *
   *  Utility class used to build a set of clusters
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   21/03/2006
   */
  //-----------------------------------------------------------------------------
  class HPDPixelClustersBuilder : public LHCb::MemPoolAlloc<Rich::HPDPixelClustersBuilder>
  {

  public:

    /// Default Constructor
    HPDPixelClustersBuilder( )
    { 
      memset ( m_data,     0, sizeof(m_data)     );
      memset ( m_clusters, 0, sizeof(m_clusters) );
    }

  public:

    /// Initialise for a new HPD
    void initialise( HPDPixelClusters * clus,
                     const HPDPixelCluster::SmartIDVector & smartIDs );

  public:

    /// Maximum number of pixel columns
    inline int nPixelCols() const noexcept { return Rich::DAQ::NumPixelColumns; }

    /// Number of pixel rows (either 32 for LHCbmode or 256 for ALICE mode)
    inline int nPixelRows() const noexcept
    {
      return ( aliceMode() ? Rich::DAQ::MaxDataSizeALICE : Rich::DAQ::MaxDataSize );
    }

    /// Returns the 'correct' row number for the given RichSmartID (either LHCb or ALICE mode)
    inline int rowNumber( const LHCb::RichSmartID& id ) const noexcept
    {
      return ( !aliceMode() ? id.pixelRow() :
               ((Rich::DAQ::NumAlicePixelsPerLHCbPixel*id.pixelRow())+id.pixelSubRow()) );
    }

    /// Returns the 'correct' column number for the given RichSmartID (either LHCb or ALICE mode)
    inline int colNumber( const LHCb::RichSmartID& id ) const noexcept
    {
      return id.pixelCol();
    }

  public:

    /// Split the given clusters up into single channel clusters
    void splitClusters( const HPDPixelClusters::Cluster::PtnVector & clusters );

    /// Get cluster for given pixel
    HPDPixelClusters::Cluster * getCluster( const int row, const int col ) const;

    /// Set cluster for given pixel
    void setCluster( const LHCb::RichSmartID & id,
                     const int row, const int col, 
                     HPDPixelClusters::Cluster * clus );

    /// Create a new cluster with given ID
    HPDPixelClusters::Cluster * createNewCluster();

    /// Create a new cluster with given ID
    HPDPixelClusters::Cluster * mergeClusters( HPDPixelClusters::Cluster * clus1, 
                                               HPDPixelClusters::Cluster * clus2 );

    /** Are we in LHCb or ALICE mode ?
     *  @return Boolean indicating data mode
     *  @retval TRUE  ALICE mode
     *  @retval FALSE LHCb mode
     */
    inline bool aliceMode() const noexcept { return m_aliceMode; }

    /** Set the mode (LHCb or ALICE)
     *  @param mode Boolean indicating if we are in ALICE(true) or LHCb(false) mode
     */
    inline void setAliceMode( const bool mode ) noexcept { m_aliceMode = mode; }

  public:

    /// Print in a human readable way
    MsgStream& fillStream( MsgStream& os ) const;

    /// Overload output to MsgStream
    friend inline MsgStream& operator << ( MsgStream& os,
                                           const HPDPixelClustersBuilder & b )
    { return b.fillStream(os); }

  private: // methods

    /// Set given col and row on
    void setOn( const int row, const int col );

    /// Check if given row and col is on
    bool isOn( const int row, const int col ) const;

    /// Remove a cluster
    void removeCluster( HPDPixelClusters::Cluster * clus );

  private:

    /// The list of clusters to fill
    HPDPixelClusters * m_hpdClus{nullptr};

    /// working variable, storing the last used cluster ID
    unsigned int m_lastID{0};

    /// Are we in ALICE mode ?
    bool m_aliceMode{false};

    /** Raw input data (row,col) (false means no hit, true means hit)
     *  @attention Hardcoding number of rows here to ALICE mode
     */
    bool m_data[Rich::DAQ::MaxDataSizeALICE][Rich::DAQ::NumPixelColumns];

    /** Assigned cluster for each pixel
     *  @attention Hardcoding number of rows here to ALICE mode
     */
    HPDPixelClusters::Cluster * m_clusters[Rich::DAQ::MaxDataSizeALICE][Rich::DAQ::NumPixelColumns];

  };

  inline 
  void
  HPDPixelClustersBuilder::setOn( const int row, const int col )
  {
    (m_data[row])[col] = true;
  }

  inline
  bool
  HPDPixelClustersBuilder::isOn( const int row, const int col ) const
  {
    return ( row>=0 && row<nPixelRows() &&
             col>=0 && col<nPixelCols() && (m_data[row])[col] );
  }

  inline 
  HPDPixelClusters::Cluster *
  HPDPixelClustersBuilder::getCluster( const int row, const int col ) const
  {
    return ( isOn(row,col) ? (m_clusters[row])[col] : nullptr );
  }

  inline 
  HPDPixelClusters::Cluster *
  HPDPixelClustersBuilder::createNewCluster()
  {
    m_hpdClus->addCluster( new HPDPixelClusters::Cluster(++m_lastID) );
    return m_hpdClus->clusters().back();
  }

  inline 
  void 
  HPDPixelClustersBuilder::removeCluster( HPDPixelClusters::Cluster * clus )
  {
    const auto iF = std::find( m_hpdClus->clusters().begin(), 
                               m_hpdClus->clusters().end(), clus );
    if ( iF != m_hpdClus->clusters().end() )
    {
      m_hpdClus->clusters().erase( iF );
      delete clus;
    }
  }

  inline 
  void 
  HPDPixelClustersBuilder::setCluster( const LHCb::RichSmartID & id,
                                       const int row, const int col, 
                                       HPDPixelClusters::Cluster * clus )
  {
    // Set the pointer to the cluster for this (row,col)
    (m_clusters[row])[col] = clus;
    // save this hit to the list of pixels for this cluster
    clus->addPixel(id);
  }

}

#endif // RICHKERNEL_RichPixelCluster_H
