/** @file LHCbConstants.h
 *
 *  Constants of LHCb general interest 
 *
 *  CVS Log :-
 *  $Id: LHCbConstants.h,v 1.6 2007-02-20 16:42:02 cattanem Exp $
 *
 *  @author Marco Cattaneo
 *  @date   2004-03-26
 */

#ifndef KERNEL_LHCBCONSTANTS_H 
#define KERNEL_LHCBCONSTANTS_H 1

// Include files
#include "GaudiKernel/PhysicalConstants.h"

/**  Namespace for constants of LHCb general interest 
 *
 *  @author Marco Cattaneo
 *  @date   2004-03-26
 */
namespace LHCbConstants 
{

  // Silicon parameters, used by ST silicon simulation (M.Needham)
  static const double SiExcitationEnergy = 174*Gaudi::Units::eV; ///< SiExcitationEnergy
  static const double SiEnergyPerIonPair = 3.6*Gaudi::Units::eV; ///< SiEnergyPerIonPair

  // Description of Beetle chip (M.Needham)
  static const unsigned int nStripsInPort = 32;     ///< nStripsInPort
  static const unsigned int nStripsInBeetle = 128;  ///< nStripsInBeetle

};

#endif // KERNEL_LHCBCONSTANTS_H
