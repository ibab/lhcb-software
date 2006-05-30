// $Id: CaloReco_dll.cpp,v 1.3 2006-05-30 09:42:03 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.1.1.1  2002/11/13 20:46:39  ibelyaev
// new package 
//
// ============================================================================
// Include files
#include "GaudiKernel/LoadFactoryEntries.h"

/** @file
 *  mandatory file for dynamic loading of CaloReco shared library
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-10-31 
 */

LOAD_FACTORY_ENTRIES(CaloReco);

