// $Id: CaloHashMap.h,v 1.6 2003-06-26 09:24:34 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2002/04/22 16:56:04  ibelyaev
//  move several files to Kernel/LHCbKernel package
//
// ============================================================================
#ifndef CALOKERNEL_CALOHASHMAP_H 
#define CALOKERNEL_CALOHASHMAP_H 1
// Include files
// LHCbKernel
#include "Kernel/CaloHashMap.h"

/** @file CaloKernel/CaloHashMap.h 
 *
 *  @brief trick to hide std::hash_map on Win32
 *
 *  An auxillary class to hide underlying std::hash_map 
 *  implementation for Microsoft Visual-C++ (Win32)  
 *
 *  The public interface is minimal! 
 *
 *  @warning the implementation is platform dependent!!!
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   20/03/2002
 */

// ============================================================================
// The End 
// ============================================================================
#endif // CALOKERNEL_CALOMAP_H
// ============================================================================
