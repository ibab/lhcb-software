// $Id: IRichRefractiveIndex.h,v 1.2 2004-06-18 09:39:02 jonrob Exp $
#ifndef RICHKERNEL_IRICHREFRACTIVEINDEX_H
#define RICHKERNEL_IRICHREFRACTIVEINDEX_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"

/** @class IRichRefractiveIndex IRichRefractiveIndex.h
 *
 *  Interface for tool to calculate the refractive indices
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRefractiveIndex( "IRichRefractiveIndex", 1, 0 );

class IRichRefractiveIndex : public virtual IAlgTool {

public:

  /// Static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichRefractiveIndex; }

  /// Refractive index for a given energy
  virtual double refractiveIndex( const Rich::RadiatorType rad,
                                  const double energy ) = 0;

  /// Average refractive index for a given radiator between two photon energies
  virtual double refractiveIndex( const Rich::RadiatorType rad,
                                  const double energyBot,
                                  const double energyTop ) = 0;

  /// Average refractive index for given radiator
  virtual double refractiveIndex( const Rich::RadiatorType rad ) = 0;

};

#endif // RICHKERNEL_IRICHREFRACTIVEINDEX_H
