// $Id: LoKiUser_load.cpp,v 1.1 2005-07-22 08:09:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

/** @file
 * 
 *  The mandatory file for implementation of component libraries 
 *  Each algorithm *MUST* be declared inside DECLARE_ALGORITHM macro:
 * 
 *  @code 
 *
 *  DECLARE_FACTORY_ENTRIES(LoKiTutor) 
 *   {
 * 
 *      DECLARE_ALGORITHM ( MyFirstAlgorithm  ) ;
 *      DECLARE_ALGORITHM ( MySecondAlgorithm ) ;
 *      DECLARE_ALGORITHM ( MyThirdAlgorithm  ) ;
 *
 *   } ;
 *
 *  @endcode 
 *
 *  @author Vanya BELYAEV Ivan.Belayev@lapp.in2p3.fr
 *  @date 2005-07-21 
 */

DECLARE_FACTORY_ENTRIES(LoKiUser) 
{
  
  DECLARE_ALGORITHM( LoKiEmpty );
  
} ;

// ============================================================================
// The END 
// ============================================================================
