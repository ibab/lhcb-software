// $Id: F77Utils.h,v 1.1 2006-10-04 13:36:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GENERATORS_F77UTILS_H 
#define GENERATORS_F77UTILS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
/** @namespace  F77Utils F77Utils.h Generators/F77Utils.h
 *
 *  Collection of primitive utilities to open/close FORTRAN units 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
namespace F77Utils 
{
  // ==========================================================================
  /** @fn close 
   *  close the opened F77 file 
   *  @param lun logical file number 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode close       ( int lun ) ;
  // ==========================================================================
  /** @fn openNew 
   *  Open Fortran file with status="NEW" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openNew     ( int lun , const std::string& file ) ;
  // ==========================================================================
  /** @fn openOld
   *  Open Fortran file with status="OLD" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openOld     ( int lun , const std::string& file ) ;
  // ==========================================================================
  /** @fn openUnknown
   *  Open Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openUnknown ( int lun , const std::string& file ) ;
  // ==========================================================================
  /** @fn open
   *  Open Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode open ( int lun , const std::string& file )  ;
  // ==========================================================================
  /** @fn openNewFormatted 
   *  Open "FORMATTED" Fortran file with status="NEW" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openNewFormatted ( int lun , const std::string& file ) ;
  // ==========================================================================
  /** @fn openOldFormatted 
   *  Open "FORMATTED" Fortran file with status="OLD" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openOldFormatted ( int lun , const std::string& file ) ;
  // ==========================================================================
  /** @fn openUnknownFormatted
   *  Open "FORMATTED" Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openUnknownFormatted ( int lun , const std::string& file ) ;
  // ==========================================================================
  /** @fn openFormatted
   *  Open "FORMATTED" Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file fiel name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openFormatted ( int lun , const std::string& file )  ;
  // ==========================================================================

} // end of namespace F77Utils 
// ============================================================================
// The END 
// ============================================================================
#endif // GENERATORS_F77UTILS_H
// ============================================================================
