// $Id: GiGaMACROs.h,v 1.3 2003-07-07 16:48:09 ranjard Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/05/07 12:21:29  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// Revision 1.1  2002/04/25 13:02:04  ibelyaev
//  small update
//
// ============================================================================
#ifndef GIGA_GIGAMACROS_H
#define GIGA_GIGAMACROS_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"
// GiGa 
#include "GiGa/GiGaFactory.h"

/** @file GiGaMACROs.h
 *
 *  Useful MACROs for implementations and declaration of all GiGa-factories
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */

/** @def IMPLEMENT_GiGaFactory
 *  useful macro to declare the factory for GiGa Objects
 *  All GiGa objects are tools 
 *  see  IAlgTool
 *  @see IToolFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define IMPLEMENT_GiGaFactory( x ) \
 static const  GiGaFactory<x> s_##x##Factory ;\
 const            IFactory& x##Factory = s_##x##Factory ;

/** @def DECLARE_GiGaFactory
 *  useful macro to declare the factory for GiGa Objects
 *  see  IAlgTool
 *  @see IToolFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaFactory( x ) \
 extern const     IFactory& x##Factory  ; x##Factory.addRef () ;

// ============================================================================
// The END
// ============================================================================
#endif // GIGA_GIGAMACROS_H
// ============================================================================
