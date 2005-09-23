
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.h
 *
 *  Header file for tool : RichMirrorSegFinder
 *
 *  CVS Log :-
 *  $Id: RichMirrorSegFinder.h,v 1.6 2005-09-23 15:48:32 papanest Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHMIRRORSEGFINDER_H
#define RICHTOOLS_RICHMIRRORSEGFINDER_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

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
#include "RichDet/DeRichFlatMirror.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
/** @class RichMirrorSegFinder RichMirrorSegFinder.h
 *
 *  Tool to find the appropriate mirror segment for a given reflection point
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
//-----------------------------------------------------------------------------

class RichMirrorSegFinder : public RichToolBase,
                            virtual public IRichMirrorSegFinder {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichMirrorSegFinder( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~RichMirrorSegFinder( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from public interface

  // Locates the spherical mirror Segment given a reflection point,
  // RICH identifier and panel
  const DeRichSphMirror* findSphMirror( const Rich::DetectorType rich,
                                        const Rich::Side side,
                                        const HepPoint3D& reflPoint ) const;

  // Locates the flat mirror Segment given a reflection point,
  // RICH identifier and panel
  const DeRichFlatMirror* findFlatMirror( const Rich::DetectorType rich,
                                          const Rich::Side side,
                                          const HepPoint3D& reflPoint ) const;

  // Locates the secondary mirror Segment given a reflection point,
  // RICH identifier and panel
  const DeRichSphMirror* findSecMirror( const Rich::DetectorType rich,
                                        const Rich::Side side,
                                        const HepPoint3D& reflPoint ) const;


private:

  /// Enumeration for falt and spherical mirror types
  enum mirrorType { sph = 0, flat = 1, sec = 1 };

  /// Pointers to the spherical mirror detector elements
  const DeRichSphMirror* m_sphMirrors[Rich::NRiches][Rich::NHPDPanelsPerRICH][30];

  /// Pointers to the flat mirror detector elements
  const DeRichFlatMirror* m_flatMirrors[Rich::NRiches][Rich::NHPDPanelsPerRICH][30];

  /// Pointers to the secondary (spherical) mirror detector elements
  const DeRichSphMirror* m_secMirrors[Rich::NRiches][Rich::NHPDPanelsPerRICH][30];

  /// The max mirror number for each RICH and HPD panel
  unsigned int m_maxMirror[Rich::NRiches][Rich::NHPDPanelsPerRICH][2];

  /// The last found mirror number for each RICH and HPD panel
  mutable unsigned int m_lastFoundMirror[Rich::NRiches][Rich::NHPDPanelsPerRICH][2];

  /// Max distance to accept mirror
  double m_maxDist[Rich::NRiches][2];

};

#endif // RICHTOOLS_RICHMIRRORSEGFINDER_H
