// $Id: IRichRefractiveIndex.h,v 1.3 2004-07-15 15:36:53 jonrob Exp $
#ifndef RICHKERNEL_IRICHREFRACTIVEINDEX_H
#define RICHKERNEL_IRICHREFRACTIVEINDEX_H 1

// Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichRadiatorType.h"

/** @class IRichRefractiveIndex IRichRefractiveIndex.h
 *
 *  Interface for tool to calculate effective refractive indices for
 *  each radiator
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichRefractiveIndex( "IRichRefractiveIndex", 1, 0 );

class IRichRefractiveIndex : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRefractiveIndex; }

  /** Calculates the refractive index for a given radiator type at a
   *  given photon energy.
   *
   *  @param rad    The radiator type
   *  @param energy The photon energy at which the refractive index is to be calculated
   *
   *  @return The refractive index at the requested energy
   */
  virtual double refractiveIndex ( const Rich::RadiatorType rad,
                                   const double energy ) = 0;

  /** Calculates the average refractive index for a given radiator type
   *  for a given range of photon energies.
   *
   *  @param rad       The radiator type
   *  @param energyBot The lower bound of photon energies
   *  @param energyTop The upper bound of photon energies
   *
   *  @return The average refractive index between the two energy bounds
   */
  virtual double refractiveIndex ( const Rich::RadiatorType rad, 
                                   const double energyBot,     
                                   const double energyTop ) = 0;

  /** Calculates the average refractive index for a given radiator type
   *  for a all visable photon energies.
   *
   *  @param rad       The radiator type
   *
   *  @return The overall average refractive index
   */
  virtual double refractiveIndex ( const Rich::RadiatorType rad ) = 0;

};

#endif // RICHKERNEL_IRICHREFRACTIVEINDEX_H
