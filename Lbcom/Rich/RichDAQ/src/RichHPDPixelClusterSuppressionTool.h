
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.h
 *
 *  Header file for tool : RichHPDPixelClusterSuppressionTool
 *
 *  CVS Log :-
 *  $Id: RichHPDPixelClusterSuppressionTool.h,v 1.7 2006-03-23 21:32:33 jonrob Exp $
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
#include "RichKernel/RichSmartIDSorter.h"

namespace PixelInfo
{
  static const int nPixelRowsOrCols = 32;
}

// namespaces
using namespace LHCb; ///< LHCb general namespace
using namespace PixelInfo;

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

  /// Maximum average HPD occupancy to bother with running cluster finding
  unsigned int m_maxAvHPDOcc;

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
    class Cluster : 
      public Rich::BoostMemPoolAlloc<RichHPDPixelClusterSuppressionTool::PixelData::Cluster>
    {

    public: // methods

      /// Constructor
      Cluster( const int id = -1 ) : m_size(0), m_clusterID(id) { }

      /// Add a pixel (row,col) to this cluster
      inline void addPixel() { ++m_size; }

      /// Get cluster ID
      inline int id() const { return m_clusterID; }

      /// read access to number of pixels in this cluster
      inline const unsigned int size() const { return m_size; }

      /// read access to number of pixels in this cluster
      inline unsigned int & size() { return m_size; }

    private:

      unsigned int m_size;       ///< Number of pixels in this cluster
      int          m_clusterID;  ///< Cluster ID

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
    friend inline MsgStream& operator << ( MsgStream& os, const PixelData & data )
    { return data.fillStream(os); }

  private:

    /// Raw input data (false means no hit, true means hit)
    bool m_data[nPixelRowsOrCols][nPixelRowsOrCols];

    /// Assigned cluster for each pixel
    Cluster * m_clusters[nPixelRowsOrCols][nPixelRowsOrCols];

    /// Vector of all created clusters
    std::vector<Cluster*> m_allclus;

  };

};

inline void RichHPDPixelClusterSuppressionTool::
PixelData::setOn( const int row, const int col )
{
  (m_data[row])[col] = true;
}

inline bool RichHPDPixelClusterSuppressionTool::
PixelData::isOn( const int row, const int col ) const
{
  return ( row>=0 && row<nPixelRowsOrCols &&
           col>=0 && col<nPixelRowsOrCols && (m_data[row])[col] );
}

inline RichHPDPixelClusterSuppressionTool::PixelData::Cluster *
RichHPDPixelClusterSuppressionTool::
PixelData::getCluster( const int row, const int col ) const
{
  return ( isOn(row,col) ? (m_clusters[row])[col] : NULL );
}

inline void RichHPDPixelClusterSuppressionTool::
PixelData::setCluster( const int row, const int col, Cluster * clus )
{
  (m_clusters[row])[col] = clus;
  clus->addPixel();
}

inline RichHPDPixelClusterSuppressionTool::
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

inline RichHPDPixelClusterSuppressionTool::
PixelData::~PixelData()
{
  for ( std::vector<Cluster*>::iterator i = m_allclus.begin();
        i != m_allclus.end(); ++i ) { delete *i; }
}

inline RichHPDPixelClusterSuppressionTool::PixelData::Cluster *
RichHPDPixelClusterSuppressionTool::
PixelData::mergeClusters( Cluster * clus1, Cluster * clus2 )
{
  // add pixels to clus1
  clus1->size() += clus2->size();
  // return clus1 as merged cluster
  return clus1;
}

inline RichHPDPixelClusterSuppressionTool::PixelData::Cluster *
RichHPDPixelClusterSuppressionTool::
PixelData::createNewCluster( const int id )
{
  Cluster * clus = new Cluster(id);
  m_allclus.push_back(clus);
  return clus;
}

#endif // RICHDAQ_RichHPDPixelClusterSuppressionTool_H
