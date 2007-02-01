
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.h
 *
 *  Header file for tool : Rich::MirrorSegFinder
 *
 *  CVS Log :-
 *  $Id: RichMirrorSegFinder.h,v 1.14 2007-02-01 17:51:10 jonrob Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHMIRRORSEGFINDER_H
#define RICHTOOLS_RICHMIRRORSEGFINDER_H 1

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
    unsigned int fullSphSearch( const Rich::DetectorType rich,
                                const Rich::Side side,
                                const Gaudi::XYZPoint& reflPoint ) const;

    /// performs a full search for the secondary mirror
    unsigned int fullSecSearch( const Rich::DetectorType rich,
                                const Rich::Side side,
                                const Gaudi::XYZPoint& reflPoint ) const;

  private: // data

    /// Enumeration for falt and spherical mirror types
    enum mirrorType { sph = 0, sec = 1 };

    /// Pointers to the spherical mirror detector elements
    const DeRichSphMirror* m_sphMirrors[Rich::NRiches][Rich::NHPDPanelsPerRICH][30];

    /// Pointers to the secondary (spherical) mirror detector elements
    const DeRichSphMirror* m_secMirrors[Rich::NRiches][Rich::NHPDPanelsPerRICH][30];

    /// The max mirror number for each RICH and HPD panel
    unsigned int m_maxMirror[Rich::NRiches][Rich::NHPDPanelsPerRICH][2];

    /// The last found mirror number for each RICH and HPD panel
    mutable unsigned int m_lastFoundMirror[Rich::NRiches][Rich::NHPDPanelsPerRICH][2];

    /// Max distance to accept mirror
    mutable double m_maxDist[Rich::NRiches][2];

    /// job option to turn on testing of the mirror finding. Use to tune the performance parameters.
    bool m_testFinding;

    /// Tuning scale factor
    double m_tuneScale;

  };

}

#endif // RICHTOOLS_RICHMIRRORSEGFINDER_H
