// $Id: CdfPhotonReco.h,v 1.1.1.1 2004-06-17 12:04:08 cattanem Exp $
#ifndef RICHDETTOOLS_CDFPHOTONRECO_H
#define RICHDETTOOLS_CDFPHOTONRECO_H 1

// Include files
// from STL
#include <string>

// base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichPhotonReconstruction.h"

// Event model
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"

// Kernel
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/Rich1DTabProperty.h"

// local
#include "Rich1.h"
#include "Rich2.h"


/** @class CdfPhotonReco CdfPhotonReco.h
 *  A tool to reconstruct photons.  Uses the "cdf" geometry without mirror 
 *  segmentation. 
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */
class CdfPhotonReco : public RichToolBase,
                      virtual public IRichPhotonReconstruction {

public:

  /// Standard constructor
  CdfPhotonReco( const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~CdfPhotonReco( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize  ();

  /// Reconstructs the geometrical photon candidate for a given track segment
  /// and pixel smart ID
  virtual StatusCode reconstructPhoton( const RichTrackSegment&,
                                        const RichSmartID&,
                                        RichGeomPhoton& ) const;

  /// Reconstructs the geometrical photon candidate for a given track segment
  /// and pixel global position
  virtual StatusCode reconstructPhoton( const RichTrackSegment&,
                                        const HepPoint3D&,
                                        RichGeomPhoton& ) const;
private:

  StatusCode globalPosition ( const RichSmartID&, HepPoint3D& ) const;

  RichX::PointerCollection m_rich;

  // for fast reference. All radiators (both riches) in one container
  RichRadiator::PointerCollection m_radiator;

  // Reflectors/detectors in separate container for each rich1/2
  RichReflector::PointerCollection m_reflector[2];
  RichDetector::PointerCollection  m_detector[2];

  /// Allowable tolerance on state z positions
  std::vector<double> m_zTolerance;
  /// Nominal z positions of states at RICHes
  std::vector<double> m_nomZstates;

};
#endif // RICHDETTOOLS_CDFPHOTONRECO_H
