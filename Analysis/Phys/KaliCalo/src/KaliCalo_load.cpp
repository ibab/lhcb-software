// $Id: KaliCalo_load.cpp,v 1.1.1.1 2005-05-31 13:03:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================

/** @file 
 *  mandatory file for implementation of component library
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-05-31
 */

DECLARE_FACTORY_ENTRIES(KaliCalo) 
{
  DECLARE_ALGORITHM ( NeutralPPsFromCPsAlg1 ) ;
  DECLARE_ALGORITHM ( LoKi_Pi0Calibr        ) ;
};


// ============================================================================
// The END 
// ============================================================================
