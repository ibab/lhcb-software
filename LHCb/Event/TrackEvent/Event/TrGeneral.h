
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef TrEvent_TrGeneral_H
#define TrEvent_TrGeneral_H 1

// Include files
#include "Kernel/CLHEPStreams.h"
#include "CLHEP/Units/PhysicalConstants.h"


/** @namespace TrGeneral 
 *
 * This namespace is defines some useful items 
 *
 * @author unknown
 * created Wed Feb  9 15:43:46 2005
 * 
 */


namespace TrGeneral
{
  /// enum
  enum ETrStatusCode{ PROBLEMS=3 };

  static const double hiTolerance     = 1e-40;          ///< High tolerance
  static const double lowTolerance    = 1e-20;          ///< Low tolerance
  static const double looseTolerance  = 1e-5;           ///< Loose tolerance
  static const double moliereFactor   = 13.6*MeV;       ///< Moliere factor

};

// -----------------------------------------------------------------------------
//   end of namespace
// -----------------------------------------------------------------------------

// Including forward declarations


#endif ///TrEvent_TrGeneral_H
