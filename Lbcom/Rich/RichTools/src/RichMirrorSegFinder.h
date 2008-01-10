
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.h
 *
 *  Header file for tool : Rich::MirrorSegFinder
 *
 *  CVS Log :-
 *  $Id: RichMirrorSegFinder.h,v 1.17 2008-01-10 17:17:34 papanest Exp $
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

    /// Get the mirrors compatible with DC06 DDDB
    StatusCode getMirrors_old( );
    
  private: // classes

    /// Functor to sort RichSmartIDs by Rich then panel numbers
    class SortByDistFromBeam : std::binary_function< const DeRichSphMirror*, const DeRichSphMirror*, bool >
    {
    public:
      /** Sort operator for the Spherical mirrors
       *
       *  Sorts into order of distance from beam line (x,y) = (0,0)
       *
       *  @param m1 First mirror
       *  @param m2 Second mirror
       *
       *  @return bool indicating if p1 should be listed before p2
       */
      inline bool operator() ( const DeRichSphMirror* m1, const DeRichSphMirror* m2 ) const
        {
          return ( pow(m1->mirrorCentre().x(),2) + pow(m1->mirrorCentre().y(),2) <
                   pow(m2->mirrorCentre().x(),2) + pow(m2->mirrorCentre().y(),2) );
        }
    };

  private: // data

    /// Enumeration for falt and spherical mirror types
    enum mirrorType { sph = 0, sec = 1 };

    /// Pointers to the spherical mirror detector elements
    std::vector<const DeRichSphMirror*> m_sphMirrors[Rich::NRiches][Rich::NHPDPanelsPerRICH];

    /// Pointers to the secondary (spherical) mirror detector elements
    std::vector<const DeRichSphMirror*> m_secMirrors[Rich::NRiches][Rich::NHPDPanelsPerRICH];

    /// The last found mirror number for each RICH and HPD panel
    mutable const DeRichSphMirror* m_lastFoundMirror[Rich::NRiches][Rich::NHPDPanelsPerRICH][2];

    /// Max distance to accept mirror
    mutable double m_maxDist[Rich::NRiches][2];

    /// job option to turn on testing of the mirror finding. Use to tune the performance parameters.
    bool m_testFinding;

    /// Tuning scale factor
    double m_tuneScale;

  };

}

#endif // RICHTOOLS_RICHMIRRORSEGFINDER_H
