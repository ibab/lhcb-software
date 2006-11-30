// $Id: HidValley.h,v 1.2 2006-11-30 12:37:01 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2006/10/28 14:06:41  ibelyaev
// New package: "Hidden Valley" production 
// 
// ============================================================================
#ifndef HIDVAL_HIDVAL_H 
#define HIDVAL_HIDVAL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ===========================================================================
#include <iostream>
#include <string>
// ============================================================================
/** @file 
 *  declaration of C++ interface for "main" fortran program HidVal
 *
 *  Matt Strassler <strasslr@noether.phys.washington.edu>:
 *
 *  "This program simulates q qbar -> Z' -> v-quarks, followed by
 *  v-showering and v-hadronization to v-pions.  The current program
 *  makes various approximations and is not suitable for precision 
 *  studies, but should be good at the factor-of-two level.
 *  In particular, the v-hadronization code uses Pythias QCD
 *  hadronization code, scaled up in energy, and this is not a perfect 
 *  simulation of the real model. Also, the Z' charges are not precisely 
 *  those discussed in ref [1]; but in any case the model of [1] was 
 *  just an example, and many other models exist.  Updates to this 
 *  program will gradually correct these issues."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
// ============================================================================
namespace HidValley 
{
  /** C++ interface for "main" fortran program HidVal
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  int    HidVal ( int seed  , int events , const std::string& file ) ;
  /** set parameter in common block MYDATA   
   *  @param name parameter name 
   *  @param value parmeter value 
   *  @return true of parameter is set correctly 
   */
  bool   setPar ( std::string name , double value ) ;
  /** get the parameter fro commmon block MYDATA 
   *  @param name parameter name 
   *  @return parameter value 
   */
  double getPar ( std::string name ) ;
  /** generate the event 
   *  @return true if no problems have been detected 
   */
  bool   hvEvent ( int dummy = 0 ) ;
} // end of namespace HidValley 

// ============================================================================
// The END 
// ============================================================================
#endif // HIDVAL_HIDVAL_H
// ============================================================================
