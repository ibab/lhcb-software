
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinderLookUpTable.h
 *
 *  Header file for tool : Rich::MirrorSegFinderLookUpTable
 *
 *  @author Chris Jones
 *  @date   2015-02-01
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RichMirrorSegFinderLookUpTable_H
#define RICHTOOLS_RichMirrorSegFinderLookUpTable_H 1

// STL
#include <sstream>
#include <cmath>
#include <vector>
#include <functional>

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichMirrorSegFinder.h"

// LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// RichKernel
#include "RichKernel/BoostArray.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichSphMirror.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class MirrorSegFinderLookUpTable RichMirrorSegFinderLookUpTable.h
   *
   *  Tool to find the appropriate mirror segment for a given reflection point.
   *  Implementation using a 2D look up table.
   *
   *  @author Chris Jones
   *  @date   2015-02-01
   */
  //-----------------------------------------------------------------------------

  class MirrorSegFinderLookUpTable : public Rich::ToolBase,
                                     virtual public IMirrorSegFinder
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    MirrorSegFinderLookUpTable( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

    virtual ~MirrorSegFinderLookUpTable( ); ///< Destructor

    // Initialization of the tool after creation
    virtual StatusCode initialize();

    // Finalization of the tool before deletion
    virtual StatusCode finalize  ();

  public: // methods (and doxygen comments) inherited from public interface

    /// Locates the spherical mirror Segment given a reflection point,
    /// RICH identifier and panel
    const DeRichSphMirror* findSphMirror( const Rich::DetectorType rich,
                                          const Rich::Side side,
                                          const Gaudi::XYZPoint& reflPoint ) const;

    /// Locates the secondary mirror Segment given a reflection point,
    /// RICH identifier and panel
    const DeRichSphMirror* findSecMirror( const Rich::DetectorType rich,
                                          const Rich::Side side,
                                          const Gaudi::XYZPoint& reflPoint ) const;

  private: // classes

    //-----------------------------------------------------------------------------
    /** @class MirrorFinder RichMirrorSegFinderLookUpTable.h
     *
     *  Helper object implementing the mirror search for a single region.
     *
     *  @author Chris Jones
     *  @date   2015-02-01
     */
    //-----------------------------------------------------------------------------
    class MirrorFinder
    {
    public:  
      /// Default constructor
      MirrorFinder() { reset(); }
      /// Reset this object
      void reset();
      /// Initialise this object
      void init( const unsigned int nBins, const double extraSize );
      /// Find the mirror for a given position
      inline const DeRichSphMirror* find( const Gaudi::XYZPoint& p ) const
      {
        return m_lookupTable.get( xIndex(p.x()), yIndex(p.y()) );
      }
      /// Get the mirror closest to a given (x,y) point
      const DeRichSphMirror* closestXY( const double& x, const double& y ) const;
      /// Returns the 2D (x,y) distance^2 between a given point and mirror centre
      static inline double distance2( const double& x, const double& y,
                                      const DeRichSphMirror* M )
      {
        return ( gsl_pow_2( x - M->mirrorCentre().X() ) + 
                 gsl_pow_2( y - M->mirrorCentre().Y() ) );
      }
    public:
      /// Vector of all mirrors in this panel
      std::vector<const DeRichSphMirror*> mirrors;
    private:
      typedef std::vector<const DeRichSphMirror*> MirrorVector;
      typedef std::vector<MirrorVector> Mirror2DArray;
       /** @class LookupTable RichMirrorSegFinderLookUpTable.h
        *  2D (x,y) Lookup table for RICH mirrors */
      class LookupTable : public Mirror2DArray
      {
      public:
        /// Constructor with bin sizes
        LookupTable( const unsigned int xBins = 0, 
                     const unsigned int yBins = 0 )
          : Mirror2DArray( xBins, MirrorVector(yBins,nullptr) ) { }
        /// Access the mirror for a given set of indices
        inline const DeRichSphMirror* get( const unsigned int ix,
                                           const unsigned int iy ) const
        {
          return ((*this)[ix])[iy];
        }
        /// Set the mirror for a given bin
        void set( const unsigned int ix,
                  const unsigned int iy,
                  const DeRichSphMirror * m ) 
        {
          ((*this)[ix])[iy] = m;
        }
      };
    private:
      /// Get x for a given index value
      inline double binX( const unsigned int i ) const
      {
        return m_minX + (i/m_incX) + (0.5/m_incX);
      }
      /// Get y for a given index value
      inline double binY( const unsigned int i ) const
      {
        return m_minY + (i/m_incY) + (0.5/m_incY);
      }
      /// Get the x index
      inline unsigned int xIndex( const double& x ) const
      {
        const unsigned int i = ( x > m_minX ? (unsigned int)((x-m_minX)*m_incX) : 0 );
        return ( i < m_nXBins ? i : m_nXBins-1 );
      }
      /// Get the y index
      inline unsigned int yIndex( const double& y ) const
      {
        const unsigned int i = ( y > m_minY ? (unsigned int)((y-m_minY)*m_incY) : 0 );
        return ( i < m_nYBins ? i : m_nYBins-1 );
      }
    private:
      double m_minX{ 9e9}; ///< Minimum X
      double m_maxX{-9e9}; ///< Maximum X
      double m_minY{ 9e9}; ///< Minimum Y
      double m_maxY{-9e9}; ///< Maximum Y
      double m_incX{0}; ///< 1 / Increment in X
      double m_incY{0}; ///< 1 / Increment in Y
      unsigned int m_nXBins{0}; ///< Number of bins in X for look up table
      unsigned int m_nYBins{0}; ///< Number of bins in Y for look up table
      LookupTable m_lookupTable; ///< The lookup table
    };

  private: // methods

    /// Update Mirror information on Condition changes
    StatusCode mirrorUpdate();

    /// Load mirrors for the given RICH detector
    StatusCode loadMirrors( const Rich::DetectorType rich );

    /// Returns the side for a given mirror and Rich Detector
    inline Rich::Side side ( const DeRichSphMirror * mirror, 
                             const Rich::DetectorType rich ) const
    {
      return ( Rich::Rich1 == rich ? 
               mirror->mirrorCentre().y() > 0.0 ? Rich::top  : Rich::bottom :
               mirror->mirrorCentre().x() > 0.0 ? Rich::left : Rich::right  );
    }

    /// Debug printout of the mirrors
    void printMirrors() const;

    // /// test mirror from lookup table
    // void testFinding( const MirrorFinder& finder,
    //                   const Rich::DetectorType rich,
    //                   const Rich::Side side,
    //                   const Gaudi::XYZPoint& reflPoint ) const;

  private: // data

    /// Enumeration for flat and spherical mirror types
    enum mirrorType { sph = 0, sec = 1 };

    /// Pointers to the spherical mirror detector elements
    MirrorFinder m_sphMirrors[Rich::NRiches][Rich::NPDPanelsPerRICH];

    /// Pointers to the secondary (spherical) mirror detector elements
    MirrorFinder m_secMirrors[Rich::NRiches][Rich::NPDPanelsPerRICH];

    /// job option to turn on testing of the mirror finding.
    //bool m_testFinding;

    /// Flag for the first update
    bool m_firstUpdate{true};

    /// Look table sizes for RICH1 and RICH2
    std::vector<unsigned int> m_tableSize;

    /// Boundary sizes to add to lookup table ranges, to take mirror segment extents into account
    std::vector<double> m_extraSize;

    /// For tests, save the max error on the found mirror distance
    mutable std::vector<double> m_maxDistDiff;

    /// For test, save the fail rate
    mutable std::vector< std::pair<unsigned long long, unsigned long long> > m_failRate;

  };

}

#endif // RICHTOOLS_RichMirrorSegFinderLookUpTable_H
