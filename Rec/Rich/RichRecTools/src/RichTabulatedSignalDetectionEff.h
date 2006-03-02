
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.h
 *
 *  Header file for tool : RichTabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.h,v 1.10 2006-03-02 15:29:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H
#define RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"

// Kernel
#include "Kernel/RichDetectorType.h"

// Event model
#include "Event/RichRecSegment.h"

// constants
#include "Kernel/PhysicalConstants.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

//-----------------------------------------------------------------------------
/** @class RichTabulatedSignalDetectionEff RichTabulatedSignalDetectionEff.h
 *
 *  Tool providing calculations for the photon detection efficiencies
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 *
 *  @todo Add support for different QE curves for each HPD
 *  @todo Use RICH2 nominal HPD when available
 */
//-----------------------------------------------------------------------------

class RichTabulatedSignalDetectionEff : public RichRecToolBase,
                                        virtual public IRichSignalDetectionEff
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichTabulatedSignalDetectionEff( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

  /// Destructor
  virtual ~RichTabulatedSignalDetectionEff() {};

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns the detection efficiency for a cherenkov photon of a given energy
  double photonDetEfficiency( LHCb::RichRecSegment * segment,
                              const double energy ) const;

private:  // Private data

  /// Pointers to RICHes
  std::vector<const DeRich*> m_riches;

  /// quartz window losses efficiency
  double m_quartzWinEff;

  /// Digitisation pedestal loss
  double m_pedLoss;

};

#endif // RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H
