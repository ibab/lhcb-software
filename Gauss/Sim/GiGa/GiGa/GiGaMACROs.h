// $Id: GiGaMACROs.h,v 1.1 2002-04-25 13:02:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_GIGAMACROS_H 
#define GIGA_GIGAMACROS_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/DeclareFactoryEntries.h"
// GiGa 
#include "GiGa/IGiGaPhysListFactory.h"
#include "GiGa/IGiGaSensDetFactory.h"
#include "GiGa/IGiGaMagFieldFactory.h"
#include "GiGa/IGiGaStackActionFactory.h"
#include "GiGa/IGiGaTrackActionFactory.h"
#include "GiGa/IGiGaStepActionFactory.h"
#include "GiGa/IGiGaEventActionFactory.h"
#include "GiGa/IGiGaRunActionFactory.h"

/** @file GiGaMACROs.h 
 *  
 *  Useful MACROs for instantiation and declaration of all GiGa-factories 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */

/** @def DECLARE_GiGaPhysList 
 *  useful macro to declare the factory for GiGa Physics List 
 *  @see IGiGaPhysList
 *  @see IGiGaPhysListFactory
 *  @see  GiGaPhysListFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaPhysList( x ) \
 extern const IGiGaPhysListFactory&    x##Factory  ; x##Factory.addRef () ;

/** @def DECLARE_GiGaSensDet
 *  useful macro to declare the factory for GiGa Sensitive Detector 
 *  @see IGiGaSensDet 
 *  @see IGiGaSensDetFactory 
 *  @see  GiGaSensDetFactory 
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaSensDet( x ) \
 extern const IGiGaSensDetFactory&     x##Factory  ; x##Factory.addRef () ;

/** @def DECLARE_GiGaMagField
 *  useful macro to declare the factory for GiGa Magnetic Field
 *  @see IGiGaMagField 
 *  @see IGiGaMagFieldFactory 
 *  @see  GiGaMagFieldFactory 
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaMagField( x ) \
 extern const IGiGaMagFieldFactory&    x##Factory  ; x##Factory.addRef () ;

/** @def DECLARE_GiGaStackAction
 *  useful macro to declare the factory for GiGa Stack Action
 *  @see IGiGaStackAction
 *  @see IGiGaStackActionFactory
 *  @see  GiGaStackActionFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaStackAction( x ) \
 extern const IGiGaStackActionFactory& x##Factory  ; x##Factory.addRef () ;

/** @def DECLARE_GiGaTrackAction
 *  useful macro to declare the factory for GiGa Track Action
 *  @see IGiGaTrackAction
 *  @see IGiGaTrackActionFactory
 *  @see  GiGaTrackActionFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaTrackAction( x ) \
 extern const IGiGaTrackActionFactory& x##Factory  ; x##Factory.addRef () ;

/** @def DECLARE_GiGaStepAction
 *  useful macro to declare the factory for GiGa StepAction
 *  @see IGiGaStepAction
 *  @see IGiGaStepActionFactory
 *  @see  GiGaStepActionFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaStepAction( x ) \
 extern const IGiGaStepActionFactory&  x##Factory  ; x##Factory.addRef () ;

/** @def DECLARE_GiGaEventAction
 *  useful macro to declare the factory for GiGa Event Action 
 *  @see IGiGaEventAction
 *  @see IGiGaEventActionFactory
 *  @see  GiGaEventActionFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaEventAction( x ) \
 extern const IGiGaEventActionFactory& x##Factory  ; x##Factory.addRef () ;

/** @def DECLARE_GiGaRunAction
 *  useful macro to declare the factory for GiGa Run Action 
 *  @see IGiGaRunAction
 *  @see IGiGaRunActionFactory
 *  @see  GiGaRunActionFactory
 *  @see IFactory
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define DECLARE_GiGaRunAction( x ) \
 extern const IGiGaRunActionFactory&   x##Factory  ; x##Factory.addRef () ;


// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAMACROS_H
// ============================================================================
