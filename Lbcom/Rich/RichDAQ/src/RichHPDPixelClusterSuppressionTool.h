
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.h
 *
 *  Header file for tool : RichHPDPixelClusterSuppressionTool
 *
 *  CVS Log :-
 *  $Id: RichHPDPixelClusterSuppressionTool.h,v 1.2 2006-03-22 09:51:52 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichHPDPixelClusterSuppressionTool_H
#define RICHDAQ_RichHPDPixelClusterSuppressionTool_H 1

// base class
#include "RichHighOccHPDSuppressionTool.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------
/** @class RichHPDPixelClusterSuppressionTool RichHPDPixelClusterSuppressionTool.h
 *
 *  Tool for monitoring high occupancy HPDs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   21/03/2006
 */
//-----------------------------------------------------------------------------

class RichHPDPixelClusterSuppressionTool : public RichHighOccHPDSuppressionTool
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichHPDPixelClusterSuppressionTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

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
  /** @class PixelData RichHPDPixelClusterSuppressionTool.h
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
    /** @class Cluster RichHPDPixelClusterSuppressionTool.h
     *
     *  Utility class representing a cluster of HPD pixels
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   21/03/2006
     */
    //-----------------------------------------------------------------------------
    class Cluster : public Rich::BoostMemPoolAlloc<Cluster>
    {

    public: // definitions

      /// data describing a single pixel
      typedef std::pair<unsigned int,unsigned int> Pixel;

      /// a vector of pixels
      typedef std::vector< Pixel >                 Data;

    public: // methods

      /// Constructor
      Cluster( const int id = -1 ) : m_clusterID(id) { }

      /// Add a pixel (row,col) to this cluster
      inline void addPixel( const unsigned int row, const unsigned int col )
      { m_cluster.push_back( Pixel(row,col) ); }

      /// Add a pixel (row,col) to this cluster
      inline void addPixel( const Pixel & pixel )
      { m_cluster.push_back( pixel ); }

      /// Get cluster ID
      inline int id() const { return m_clusterID; }

      /// Get read access to cluster data
      inline const Cluster::Data & pixels() const
      { return m_cluster; }

    private:

      Data m_cluster;  ///< pixels in this cluster
      int m_clusterID; ///< Cluster ID

    };

  public: // definitions

    /// 32 by 32 array of raw data
    typedef std::vector< std::vector<bool> >     RawDataArray;

    /// 32 by 32 array assigning each pixel to a Cluster
    typedef std::vector< std::vector<Cluster*> > ClusterArray;

  public: // methods

    /// Constructor from a list of RichSmartIDs
    PixelData ( const LHCb::RichSmartID::Vector & smartIDs );

    /// Destructor
    ~PixelData();

    /// Set given col and row on
    void setOn( const unsigned int row, const unsigned int col );

    /// Check if given row and col is on
    bool isOn( const int row, const int col ) const;

    /// Get cluster for given pixel
    Cluster * getCluster( const int row, const int col ) const;

    /// Set cluster for given pixel
    void setCluster( const int row, const int col, Cluster * clus );

    /// Create a new cluster with given ID
    Cluster * createNewCLuster( const int id );

    /// Create a new cluster with given ID
    Cluster * mergeClusters( Cluster * clus1, Cluster * clus2 );

    /// Create a new vector of suppressed RichSmartIDs
    void suppressedIDs( const LHCb::RichSmartID hID,
                        LHCb::RichSmartID::Vector & smartIDs,
                        const unsigned int maxSize );

    /// Print in a human readable way
    MsgStream& fillStream( MsgStream& os ) const;

    /// Overload output to ostream
    friend inline MsgStream& operator << ( MsgStream& os, const PixelData & data )
    { return data.fillStream(os); }

  private:

    RawDataArray m_data;     ///< Raw input data
    ClusterArray m_clusters; ///< Assigned clusters
    std::vector<Cluster*> m_allclus;   ///< List of all created clusters

  };

};

inline void RichHPDPixelClusterSuppressionTool::
PixelData::setOn( const unsigned int row, const unsigned int col )
{
  (m_data[row])[col] = true;
}

inline bool RichHPDPixelClusterSuppressionTool::
PixelData::isOn( const int row, const int col ) const
{
  return ( row>=0 && row<32 && col>=0 && col<32 && (m_data[row])[col] );
}

inline RichHPDPixelClusterSuppressionTool::PixelData::Cluster *
RichHPDPixelClusterSuppressionTool::
PixelData::getCluster( const int row, const int col ) const
{
  return (m_clusters[row])[col];
}

inline void RichHPDPixelClusterSuppressionTool::
PixelData::setCluster( const int row, const int col, Cluster * clus )
{
  (m_clusters[row])[col] = clus;
  if (clus) clus->addPixel(row,col);
}

inline RichHPDPixelClusterSuppressionTool::PixelData::Cluster *
RichHPDPixelClusterSuppressionTool::
PixelData::createNewCLuster( const int id )
{
  Cluster * clus = new Cluster(id);
  m_allclus.push_back(clus);
  return clus;
}

inline RichHPDPixelClusterSuppressionTool::PixelData::Cluster *
RichHPDPixelClusterSuppressionTool::
PixelData::mergeClusters( Cluster * clus1, Cluster * clus2 )
{
  for ( Cluster::Data::const_iterator i = clus2->pixels().begin();
        i != clus2->pixels().end(); ++i )
  {
    clus1->addPixel( *i );
    setCluster( (*i).first, (*i).second, clus1 );
  }
  return clus1;
}

inline RichHPDPixelClusterSuppressionTool::
PixelData::PixelData( const LHCb::RichSmartID::Vector & smartIDs )
  : m_data     ( 32, std::vector<bool>(32,false) ),
    m_clusters ( 32, std::vector<Cluster*>(32)   )
{
  m_allclus.reserve(5);
  for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
        iS != smartIDs.end(); ++iS )
  {
    setOn( (*iS).pixelRow(), (*iS).pixelCol() );
  }
}

inline RichHPDPixelClusterSuppressionTool::
PixelData::~PixelData()
{
  for ( std::vector<Cluster*>::iterator i = m_allclus.begin();
        i != m_allclus.end(); ++i ) { delete *i; }
}

inline void RichHPDPixelClusterSuppressionTool::
PixelData::suppressedIDs( const LHCb::RichSmartID hID,
                          LHCb::RichSmartID::Vector & smartIDs,
                          const unsigned int maxSize )
{
  smartIDs.clear();
  for ( unsigned int row = 0; row < 32; ++row )
  {
    for ( unsigned int col = 0; col < 32; ++col )
    {
      if ( isOn(row,col) && getCluster(row,col)->pixels().size() <= maxSize )
      {
        RichSmartID id(hID);
        id.setPixelRow(row);
        id.setPixelCol(col);
        smartIDs.push_back(id);
      }
    }
  }
}

#endif // RICHDAQ_RichHPDPixelClusterSuppressionTool_H
