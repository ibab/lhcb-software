// $Id: RelationMACROs.h,v 1.2 2002-04-08 14:26:01 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/03 15:35:18  ibelyaev
// essential update and redesing of all 'Relations' stuff
//
// ============================================================================
#ifndef RELATIONS_RELATIONMACROS_H 
#define RELATIONS_RELATIONMACROS_H 1
// Include files
#include "GaudiKernel/ObjectFactory.h"
// Local 
#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"
#include "Relations/RelationWeighted1D.h"
#include "Relations/RelationWeighted2D.h"
// Local 
#include "Relations/AssociatorMACROs.h"

/** @file RelationMACROs.h
 *  
 *  definition of useful MACROs for fast 
 *  implementation and instantiation of
 *  object factories for relation objects(tables) 
 *
 *  @warning as for any C++ MACROS, one need to take some care!
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   03/04/2002
 */

/** @def IMPLEMENT_Relation1D( T1 , T2 )
 *  
 *  mandatory macro to make ordinary unidorectional relations 
 *  from type 'T1' to type 'T2' persistent 
 * 
 *  @see Relation1D
 *  @see DataObjectFactory
 *  @see IFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define IMPLEMENT_Relation1D( T1 , T2 )                          \
 typedef Relation1D<##T1##,##T2##>                               \
                                Rel1DFrom##T1##To##T2 ;          \
 static const DataObjectFactory<Rel1DFrom##T1##To##T2##>         \
                              s_Rel1DFrom##T1##To##T2 ;          \
 const                 IFactory&Rel1DFrom##T1##To##T2##Factory = \
                              s_Rel1DFrom##T1##To##T2 ;

/** @def DECLARE_Relation1D( T1 , T2 )
 *  
 *  mandatory macro to declare ordinary unidirectional 
 *  relations from type 'T1' to type 'T2'.
 *
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define DECLARE_Relation1D( T1 , T2 ) \
          DECLARE_OBJECT( Rel1DFrom##T1##To##T2 );


/** @def IMPLEMENT_Relation2D( T1 , T2 )
 *  
 *  mandatory macro to make ordinary bidirectional relations 
 *  from type 'T1' to type 'T2' persistent 
 *  
 *  @see Relation2D
 *  @see DataObjectFactory
 *  @see IFactory
 *
 *  The macro need to be used in your *.cpp file 
 */
#define IMPLEMENT_Relation2D( T1 , T2 )                           \
 typedef Relation2D<##T1##,##T2##>                                \
                                Rel2DFrom##T1##To##T2 ;           \
 static const DataObjectFactory<Rel2DFrom##T1##To##T2##>          \
                              s_Rel2DFrom##T1##To##T2 ;           \
 const                 IFactory&Rel2DFrom##T1##To##T2##Factory  = \
                              s_Rel2DFrom##T1##To##T2 ;

/** @def DECLARE_Relation2D( T1 , T2 )
 *  
 *  mandatory macro to declare ordinary bidirectional
 *  relations from type 'T2' to type 'T1'.
 *
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 *  mandatory macro to declare relation
 */
#define DECLARE_Relation2D( T1 , T2 ) \
          DECLARE_OBJECT( Rel2DFrom##T1##To##T2 );


/** @def IMPLEMENT_RelationW1D( T1 , T2 , W )
 *  
 *  mandatory macro to make weighted unidirectional relations 
 *  from type 'T1' to type 'T2' with weight 'W' persistent 
 *
 *  @see RelationWeighted1D
 *  @see DataObjectFactory
 *  @see IFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define IMPLEMENT_RelationW1D( T1 , T2 , W )                               \
 typedef RelationWeighted1D<##T1##,##T2##,##W##>                           \
                                RelW1DFrom##T1##To##T2##With##W ;          \
 static const DataObjectFactory<RelW1DFrom##T1##To##T2##With##W##>         \
                              s_RelW1DFrom##T1##To##T2##With##W ;          \
 const                 IFactory&RelW1DFrom##T1##To##T2##With##W##Factory = \
                              s_RelW1DFrom##T1##To##T2##With##W ;

/** @def DECLARE_RelationW1D( T1 , T2 , W )
 *  
 *  mandatory macro to declare weighted unidirectional
 *  relations from type 'T1' to type 'T2' with weight 'W'
 *
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define DECLARE_RelationW1D( T1 , T2 , W ) \
          DECLARE_OBJECT( RelW1DFrom##T1##To##T2##With##W );


/** @def IMPLEMENT_RelationW2D( FROM , TO , WEIGHT )
 *  
 *  mandatory macro to make weighted bidirectional relations 
 *  from type 'FROM' to type 'TO' with weight 'WEIGHT' persistent 
 *  
 *  @see RelationWeighted2D
 *  @see DataObjectFactory
 *  @see IFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define IMPLEMENT_RelationW2D( T1 , T2 , W )                               \
 typedef RelationWeighted2D<##T1##,##T2##,##W##>                           \
                                RelW2DFrom##T1##To##T2##With##W ;          \
 static const DataObjectFactory<RelW2DFrom##T1##To##T2##With##W##>         \
                              s_RelW2DFrom##T1##To##T2##With##W ;          \
 const                 IFactory&RelW2DFrom##T1##To##T2##With##W##Factory = \
                              s_RelW2DFrom##T1##To##T2##With##W ;


/** @def DECLARE_RelationW2D( T1 , T2 , W )
 *  
 *  mandatory macro to declare weighted bidirectional
 *  relations from type 'T1' to type 'T2' with weight 'W'
 * 
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define DECLARE_RelationW2D( T1 , T2 , W )  \
          DECLARE_OBJECT( RelW2DFrom##T1##To##T2##With##W );

// ============================================================================
// The End  
// ============================================================================
#endif // RELATIONS_RELATIONMACROS_H
// ============================================================================
