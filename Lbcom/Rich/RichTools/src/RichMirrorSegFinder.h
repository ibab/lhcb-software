
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.h
 *
 *  Header file for tool : Rich::MirrorSegFinder
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHMIRRORSEGFINDER_H
#define RICHTOOLS_RICHMIRRORSEGFINDER_H 1

// STL
#include <sstream>
#include <cmath>
#include <vector>
#include <functional>

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

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class MirrorSegFinder RichMirrorSegFinder.h
   *
   *  Tool to find the appropriate mirror segment for a given reflection point
   *
   *  @author Antonis Papanestis
   *  @date   2003-11-04
   */
  //-----------------------------------------------------------------------------

  class MirrorSegFinder : public Rich::ToolBase,
                          virtual public IMirrorSegFinder
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    MirrorSegFinder( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

    virtual ~MirrorSegFinder( ); ///< Destructor

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

  private: // methods

    /// performs a full search for the spherical mirror
    const DeRichSphMirror* fullSphSearch( const Rich::DetectorType rich,
                                          const Rich::Side side,
                                          const Gaudi::XYZPoint& reflPoint ) const;

    /// performs a full search for the secondary mirror
    const DeRichSphMirror* fullSecSearch( const Rich::DetectorType rich,
                                          const Rich::Side side,
                                          const Gaudi::XYZPoint& reflPoint ) const;

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
    
  private: // classes

    /// Functor to sort RichSmartIDs by Rich then panel numbers
    class SortByDistFromBeam : std::binary_function< const DeRichSphMirror*, 
                                                     const DeRichSphMirror*, 
                                                     bool >
    {
    public:
      /** Sort operator for the Spherical mirrors
       *
       *  Sorts into order of distance from beam line (x,y) = (0,0)
       *
       *  @param m1 First mirror
       *  @param m2 Second mirror
       *
       *  @return bool indicating if m1 should be listed before m2
       */
      inline bool operator() ( const DeRichSphMirror* m1, const DeRichSphMirror* m2 ) const
        {
          return ( std::pow(m1->mirrorCentre().x(),2) + std::pow(m1->mirrorCentre().y(),2) <
                   std::pow(m2->mirrorCentre().x(),2) + std::pow(m2->mirrorCentre().y(),2) );
        }
    };

  private: // data

    /// Enumeration for flat and spherical mirror types
    enum mirrorType { sph = 0, sec = 1 };

    /// Pointers to the spherical mirror detector elements
    std::vector<const DeRichSphMirror*> m_sphMirrors[Rich::NRiches][Rich::NPDPanelsPerRICH];

    /// Pointers to the secondary (spherical) mirror detector elements
    std::vector<const DeRichSphMirror*> m_secMirrors[Rich::NRiches][Rich::NPDPanelsPerRICH];

    /// The last found mirror number for each RICH and HPD panel
    mutable const DeRichSphMirror* m_lastFoundMirror[Rich::NRiches][Rich::NPDPanelsPerRICH][2];

    /// Max distance to accept mirror
    mutable double m_maxDist[Rich::NRiches][2];

    /// job option to turn on testing of the mirror finding. Use to tune the performance parameters.
    bool m_testFinding;

    /// Tuning scale factor
    double m_tuneScale;

    /// Flag for the first update
    bool m_firstUpdate;

  };

}

#endif // RICHTOOLS_RICHMIRRORSEGFINDER_H
