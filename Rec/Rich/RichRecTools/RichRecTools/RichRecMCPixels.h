// $Id: RichRecMCPixels.h,v 1.1.1.1 2002-07-28 10:46:22 jonesc Exp $
#ifndef RICHRECTOOLS_RICHRECMCPIXELS_H
#define RICHRECTOOLS_RICHRECMCPIXELS_H 1

// Fram Gaudi framework
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ContainedObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"

// From Rich
#include "Event/RichRecPixel.h"
#include "Event/MCHit.h"

#include "CLHEP/Units/PhysicalConstants.h"

/** @class RichRecMCPixels RichRecMCPixels.h
 *
 *  Algorithm for forming RichRecPixels from MC hits
 *
 *  @author Christopher.Rob.Jones@cern.ch
 *  created Fri Mar 15 11:10:17 2002
 *
 */

class RichRecMCPixels : public Algorithm {

public:

  /// Standard algorithm constructor
  RichRecMCPixels (const std::string &name,
                   ISvcLocator *svcLoc);

  /// Default destructor
  ~RichRecMCPixels();

  /// Gaudi framework initialize method
  virtual StatusCode initialize ();

  /// Gaudi framework execute method
  virtual StatusCode execute ();

  /// Gaudi framework finalize method
  virtual StatusCode finalize ();

private:

  /// Default constructor
  RichRecMCPixels();

  /// Default copy constructor
  RichRecMCPixels(const RichRecMCPixels &right);

  /// Default copy constructor
  RichRecMCPixels & operator=(const RichRecMCPixels &right);

  /// String containing input MC Hits location in TDS
  std::string m_mcHitsLocation;

  /// String containing output RichRecPixel location in TDS
  std::string m_richRecPixelsLocation;

};

#endif // RICHRECTOOLS_RICHRECMCPIXELS_H
