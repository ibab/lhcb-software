// $Id: RichMirrorSegFinder.h,v 1.2 2004-06-21 13:34:40 jonesc Exp $
#ifndef RICHDETTOOLS_RICHMIRRORSEGFINDER_H
#define RICHDETTOOLS_RICHMIRRORSEGFINDER_H 1

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

/** @class RichMirrorSegFinder RichMirrorSegFinder.h
 *
 *  Tool to find the appropriate mirror segment for a given reflection point
 *
 *  @author Antonis Papanestis
 *  @date   2003-11-04
 */
class RichMirrorSegFinder : public RichToolBase,
                            virtual public IRichMirrorSegFinder {
public:
  /// Standard constructor
  RichMirrorSegFinder( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  virtual ~RichMirrorSegFinder( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  DeRichSphMirror* findSphMirror(const Rich::DetectorType rich,
                                 const Rich::Side side,
                                 const HepPoint3D& reflPoint) const;

  DeRichFlatMirror* findFlatMirror(const Rich::DetectorType rich,
                                   const Rich::Side side,
                                   const HepPoint3D& reflPoint) const;

protected:

private:

  enum mirrorType {sph = 0, flat };

  // mirror collections
  DeRichSphMirror* m_sphMirrors[2][2][30];
  DeRichFlatMirror* m_flatMirrors[2][2][30];

  unsigned int m_lastMirror[2][2][2];

};
#endif // RICHDETTOOLS_RICHMIRRORSEGFINDER_H
