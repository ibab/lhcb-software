// $Id: F77Utils.h,v 1.2 2007-06-29 13:21:06 ibelyaev Exp $
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
 *  Collection of primitive utilities to open/close FORTRAN units 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-10-03
 */
namespace F77Utils 
{
  // ==========================================================================
  /** Close the opened F77 file 
   *  @param lun logical file number 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode close       
  ( int lun , const bool verbose = false ) ;
  // ==========================================================================
  /** Open Fortran file with status="NEW" 
   *  @param lun logical file number 
   *  @param file file name 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openNew    
  ( int   lun                          , 
    const std::string& file            , 
    const bool         verbose = false ) ;
  // ==========================================================================
  /** Open Fortran file with status="OLD" 
   *  @param lun logical file number 
   *  @param file file name 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openOld     
  ( int   lun                          , 
    const std::string& file            ,
    const bool         verbose = false ) ;
  // ==========================================================================
  /** Open Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file file name 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openUnknown 
  ( int                lun             , 
    const std::string& file            ,
    const bool         verbose = false ) ;
  // ==========================================================================
  /** Open Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file file name 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode open 
  ( int                lun             , 
    const std::string& file            ,
    const bool         verbose = false ) ;
  // ==========================================================================
  /** @fn openNewFormatted 
   *  Open "FORMATTED" Fortran file with status="NEW" 
   *  @param lun logical file number 
   *  @param verbose verbosity flag
   *  @param file file name 
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openNewFormatted
  ( int                lun             , 
    const std::string& file            ,
    const bool         verbose = false ) ;
  // ==========================================================================
  /** Open "FORMATTED" Fortran file with status="OLD" 
   *  @param lun logical file number 
   *  @param file file name 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openOldFormatted 
  ( int                lun             , 
    const std::string& file            ,
    const bool         verbose = false ) ;
  // ==========================================================================
  /** Open "FORMATTED" Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file file name 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openUnknownFormatted 
  ( int                lun             , 
    const std::string& file            ,
    const bool         verbose = false ) ;
  // ==========================================================================
  /** Open "FORMATTED" Fortran file with status="UNKNOWN" 
   *  @param lun logical file number 
   *  @param file file name 
   *  @param verbose verbosity flag
   *  @return status code 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  StatusCode openFormatted
  ( int                lun             , 
    const std::string& file            ,
    const bool         verbose = false ) ;
  // ==========================================================================
  /** find free fortran units
   *  Program scans fortran units from 99 to 10 to find "free" units, 
   *  @param verbose verbosity flag
   *  @return unit 
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-10-03
   */
  int getUnit( const bool verbose = false ) ;
  // ==========================================================================
} // end of namespace F77Utils 
// ============================================================================
// The END 
// ============================================================================
#endif // GENERATORS_F77UTILS_H
// ============================================================================
