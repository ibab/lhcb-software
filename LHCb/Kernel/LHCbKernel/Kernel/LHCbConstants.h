// $Id: LHCbConstants.h,v 1.3 2005-10-12 15:32:41 mneedham Exp $
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
  static const unsigned int nStripsInBeetle = 128;


};
#endif // KERNEL_LHCBCONSTANTS_H
