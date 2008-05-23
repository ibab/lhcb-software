// $Id: LhaPdf.h,v 1.3 2008-05-23 11:55:05 robbep Exp $
#ifndef GENERATORS_LHAPDF_H 
#define GENERATORS_LHAPDF_H 1

// Include files
#include "Generators/LHAPDFCommonBlocks.h"

/** @class LhaPdf LhaPdf.h "Generators/LhaPdf.h"
 * 
 *  Utility class to access LHAPDF Fortran Functions. 
 *
 *  @author Patrick Robbe
 *  @date   2005-12-06
 */
class LhaPdf {
 public:
  /// Access to LHACONTROL common block
  static Lhacontrol & lhacontrol() { return s_lhacontrol ; }
  
 private:
  static Lhacontrol s_lhacontrol ; ///< LHACONTROL Common Block data
};
#endif // GENERATORS_LHAPDF_H
