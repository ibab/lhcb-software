// $Id: LHCbConstants.h,v 1.2 2004-03-26 08:42:39 cattanem Exp $
#ifndef KERNEL_LHCBCONSTANTS_H 
#define KERNEL_LHCBCONSTANTS_H 1

// Include files
#include "CLHEP/Units/PhysicalConstants.h"

/** @file LHCbConstants.h Kernel/LHCbConstants.h
 *  Namespace for constants of LHCb general interest 
 *
 *  @author Marco Cattaneo
 *  @date   2004-03-26
 */
namespace LHCbConstants {

  // Silicon parameters, used by ST silicon simulation (M.Needham)
  static const double SiExcitationEnergy = 174*eV;
  static const double SiEnergyPerIonPair = 3.6*eV;

  // Description of Beetle chip (M.Needham)
  static const unsigned int nStripsInPort = 32;

};
#endif // KERNEL_LHCBCONSTANTS_H
