// $Id: RichTabulatedGasQuartzWindowAbs.h,v 1.1 2004-04-19 23:06:16 jonesc Exp $
#ifndef RICHRECTOOLS_RICHGASQUARTZWINDOWABS_H
#define RICHRECTOOLS_RICHGASQUARTZWINDOWABS_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// RichUtils
#include "RichUtils/Rich1DTabProperty.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

// interfaces
#include "RichRecBase/IRichGasQuartzWindowAbs.h"

/** @class RichTabulatedGasQuartzWindowAbs RichTabulatedGasQuartzWindowAbs.h
 *
 *  Tool to calculate transmission properties for the gas quartz window
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class RichTabulatedGasQuartzWindowAbs : public RichRecToolBase,
                                        virtual public IRichGasQuartzWindowAbs {

public:

  /// Standard constructor
  RichTabulatedGasQuartzWindowAbs( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedGasQuartzWindowAbs() {};

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Rayleigh scattering probability for given pathlength (segment) and photyon energy
  double photonTransProb( const RichRecSegment * segment,
                          const double energy ) const;

private:  // Private data

  /// Tabulated Rayleigh scattering length
  Rich1DTabProperty * m_gasQWin;

  /// Location of tabulated  rayleigh scattering length in XML
  std::string m_gasQWinLoc;

  /// Thickness of windows
  double m_qWinZSize[Rich::NRiches];

};

#endif // RICHRECTOOLS_RICHGASQUARTZWINDOWABS_H
