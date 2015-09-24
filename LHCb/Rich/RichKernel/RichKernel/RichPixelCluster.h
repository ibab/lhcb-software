
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

// Boost
#include <boost/shared_ptr.hpp>

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
    typedef LHCb::RichSmartID::Vector SmartIDVector;

  public:

    /// Default Constructor
    HPDPixelCluster( ) { }
    
    /// Constructor with reserved size
    explicit HPDPixelCluster( const size_t resSize ) { m_ids.reserve(resSize); }

    /// Constructor from a single channel (one pixel cluster)
    explicit HPDPixelCluster( const LHCb::RichSmartID & id ) : m_ids(1,id) { }

    /// Constructor from a vector of RichSmartIDs
    explicit HPDPixelCluster( const SmartIDVector & ids ) : m_ids(ids) { }

    // Copy constructor
    HPDPixelCluster( const HPDPixelCluster & clus ) : m_ids(clus.smartIDs()) { }

    /// Destructor
    ~HPDPixelCluster( ) { }

  public:

    /// const access to the list of HPD pixels
    inline const SmartIDVector & smartIDs() const { return m_ids; }

    /// The primary (seed) channel ID
    inline const LHCb::RichSmartID& primaryID() const 
    {
      static const LHCb::RichSmartID defaultID;
      return ( !smartIDs().empty() ? smartIDs().front() : defaultID ); 
    }

    /// The RICH detector for this cluster
    inline Rich::DetectorType rich() const { return primaryID().rich(); }

    /// The RICH panel for this cluster
    inline LHCb::RichSmartID panel() const { return primaryID().panelID(); }

    /// The RICH HPD for this cluster
    inline LHCb::RichSmartID hpd()   const { return primaryID().pdID(); }

    /// Number of channels in this cluster
    inline unsigned int size()       const { return smartIDs().size(); }

  public:

    /// Add a channel to this cluster
    inline void addChannel( const LHCb::RichSmartID& id ) { m_ids.push_back(id); }

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
   *  @todo Update how row and column data is handled to not need to create
   *        ALICE mode sized array when LHCb mode is in use
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   21/03/2006
   */
  //-----------------------------------------------------------------------------
  class HPDPixelClusters : public LHCb::MemPoolAlloc<Rich::HPDPixelClusters>
  {

  public:

    /// const Shared pointer to a HPDPixelClusters
    typedef boost::shared_ptr<const HPDPixelClusters> ConstSharedPtn;

    /// Shared pointer to a HPDPixelClusters
    typedef boost::shared_ptr<HPDPixelClusters>       SharedPtn;

  public:

    /** Are we in LHCb or ALICE mode ?
     *  @return Boolean indicating data mode
     *  @retval TRUE  ALICE mode
     *  @retval FALSE LHCb mode
     */
    inline bool aliceMode() const { return m_aliceMode; }

    /** Set the mode (LHCb or ALICE)
     *  @param mode Boolean indicating if we are in ALICE(true) or LHCb(false) mode
     */
    inline void setAliceMode( const bool mode ) { m_aliceMode = mode; }

    /// Maximum number of pixel columns
    inline int nPixelCols() const { return Rich::DAQ::NumPixelColumns; }

    /// Number of pixel rows (either 32 for LHCbmode or 256 for ALICE mode)
    inline int nPixelRows() const 
    { 
      return ( aliceMode() ? Rich::DAQ::MaxDataSizeALICE : Rich::DAQ::MaxDataSize );
    }

    /// Returns the 'correct' row number for the given RichSmartID (either LHCb or ALICE mode)
    inline int rowNumber( const LHCb::RichSmartID& id ) const
    {
      return ( !aliceMode() ? id.pixelRow() :
               ((Rich::DAQ::NumAlicePixelsPerLHCbPixel*id.pixelRow())+id.pixelSubRow()) );
    }

    /// Returns the 'correct' column number for the given RichSmartID (either LHCb or ALICE mode)
    inline int colNumber( const LHCb::RichSmartID& id ) const
    {
      return id.pixelCol();
    }

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
      typedef LHCb::Boost::PoolAllocList<Cluster*> PtnVector;
      //typedef LHCb::Boost::PoolAllocVector<Cluster*> PtnVector;

    public: // methods

      /// Constructor
      Cluster( const int id = -1 ) 
        : m_clusterID(id) 
        , m_cluster(3) // guess a reserve size for pixels in cluster
      { }
      
      /// Get cluster ID
      inline int id() const
      {
        return m_clusterID;
      }

      /// Add a pixel to this cluster
      inline void addPixel( const LHCb::RichSmartID& id )
      {
        m_cluster.addChannel( id );
      }

      /// Get read access to cluster data
      inline const Rich::HPDPixelCluster & pixels() const
      {
        return m_cluster;
      }

      /// Shortcut to the cluster size
      inline unsigned int size() const
      {
        return pixels().smartIDs().size();
      }

    private:

      int  m_clusterID;          ///< Cluster ID
      HPDPixelCluster m_cluster; ///< list of pixels in this cluster

    };

  public: // methods

    /// Constructor from a list of RichSmartIDs
    HPDPixelClusters ( const HPDPixelCluster::SmartIDVector & smartIDs );

    /// Destructor
    ~HPDPixelClusters()
    {
      for ( auto * i : m_allclus ) { delete i; }
    }

    /// Create a new cluster with given ID
    Cluster * createNewCluster();

    /// Create a new cluster with given ID
    Cluster * mergeClusters( Cluster * clus1, Cluster * clus2 );

    /// Read access to the vector of clusters
    inline const Cluster::PtnVector & clusters() const { return m_allclus; }

    /// Create a new vector of suppressed RichSmartIDs
    void suppressIDs( HPDPixelCluster::SmartIDVector & smartIDs,
                      const unsigned int maxSize ) const;

    /// Split the given clusters up into single channel clusters
    void splitClusters( const Cluster::PtnVector & clusters );

    /// Get cluster for given pixel
    Cluster * getCluster( const int row, const int col ) const;

    /// Set cluster for given pixel
    void setCluster( const int row, const int col, Cluster * clus );

    /// Freeze these clusters
    inline void freeze() { m_frozen = true; }

  public:

    /// Print in a human readable way
    MsgStream& fillStream( MsgStream& os ) const;

    /// Overload output to MsgStream
    friend inline MsgStream& operator << ( MsgStream& os,
                                           const HPDPixelClusters & data )
    { return data.fillStream(os); }

  private: // methods

    /// Set given col and row on
    void setOn( const int row, const int col );

    /// Check if given row and col is on
    bool isOn( const int row, const int col ) const;

    /// Remove a cluster
    void removeCluster( Cluster * clus );

  private: // data

    /// HPD ID
    LHCb::RichSmartID m_hpdID;

    /** Raw input data (row,col) (false means no hit, true means hit)
     *  @attention Hardcoding number of rows here to ALICE mode
     */
    static bool m_data[Rich::DAQ::MaxDataSizeALICE][Rich::DAQ::NumPixelColumns];

    /** Assigned cluster for each pixel
     *  @attention Hardcoding number of rows here to ALICE mode
     */
    static Cluster * m_clusters[Rich::DAQ::MaxDataSizeALICE][Rich::DAQ::NumPixelColumns];

    /// Vector of all created clusters
    Cluster::PtnVector m_allclus;

    /// working variable, storing the last used cluster ID
    unsigned int m_lastID;

    /// Are we in ALICE mode ?
    bool m_aliceMode;  

    /// Freeze these clusters (prevent access to m_clusters and m_data)
    bool m_frozen;

  };

  inline void HPDPixelClusters::setOn( const int row, const int col )
  {
    (m_data[row])[col] = true;
  }

  inline bool HPDPixelClusters::isOn( const int row, const int col ) const
  {
    return ( row>=0 && row<nPixelRows() &&
             col>=0 && col<nPixelCols() && (m_data[row])[col] );
  }

  inline HPDPixelClusters::Cluster *
  HPDPixelClusters::getCluster( const int row, const int col ) const
  {
    return ( isOn(row,col) && !m_frozen ? (m_clusters[row])[col] : NULL );
  }

  inline HPDPixelClusters::Cluster *
  HPDPixelClusters::createNewCluster()
  {
    m_allclus.push_back( new Cluster(++m_lastID) );
    return m_allclus.back();
  }

  inline void HPDPixelClusters::removeCluster( Cluster * clus )
  {
    auto iF = std::find( m_allclus.begin(), m_allclus.end(), clus );
    if ( iF != m_allclus.end() )
    {
      m_allclus.erase( iF );
      delete clus;
    }
  }

}

#endif // RICHKERNEL_RichPixelCluster_H
