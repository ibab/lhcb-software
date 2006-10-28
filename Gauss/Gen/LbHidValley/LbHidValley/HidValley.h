// $Id: HidValley.h,v 1.1.1.1 2006-10-28 14:06:41 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
