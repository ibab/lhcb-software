// $Id: RelationMACROs.h,v 1.1 2002-04-03 15:35:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATIONMACROS_H 
#define RELATIONS_RELATIONMACROS_H 1
// Include files
#include "GaudiKernel/ObjectFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// Local 
#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"
#include "Relations/RelationWeighted1D.h"
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationTool.h"
#include "Relations/RelationWeightedTool.h"

/** @file RelationMACROs.h Relations/RelationMACROs.h
 *  
 *  definition of useful MACROs for fast 
 *  implementation and instantiation of
 *  object factories for relation objects(tables) 
 *  and tool factories for instantiation of relation tools
 *
 *  @warning as for any C++ MACROS, one need to take some care!
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   03/04/2002
 */


/** @def __ImplementRelation1D( T1 , T2 )
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
#define __ImplementRelation1D( T1 , T2 )                             \
 typedef Relation1D<##T1,##T2>                                       \
                                Rel1DFrom##T1To##T2 ;                \
 static const DataObjectFactory<Rel1DFrom##T1To##T2>                 \
                              s_Rel1DFrom##T1To##T2 ;                \
 const                 IFactory&Rel1DFrom##T1To##T2Factory  =        \
                              s_Rel1DFrom##T1To##T2 ;

/** @def __ImplementRelation2D( T1 , T2 )
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
#define __ImplementRelation2D( T1 , T2 )                             \
 typedef Relation2D<##T1,##T2>                                       \
                                Rel2DFrom##T1To##T2 ;                \
 static const DataObjectFactory<Rel2DFrom##T1To##T2>                 \
                              s_Rel2DFrom##T1To##T2 ;                \
 const                 IFactory&Rel2DFrom##T1To##T2Factory  =        \
                              s_Rel2DFrom##T1To##T2 ;

/** @def __ImplementRelationW1D( T1 , T2 , W )
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
#define __ImplementRelationW1D( T1 , T2 , W )                        \
 typedef RelationWeighted1D<##T1,##T2,##W>                           \
                                RelW1DFrom##T1To##T2With##W ;        \
 static const DataObjectFactory<RelW1DFrom##T1To##T2With##W>         \
                              s_RelW1DFrom##T1To##T2With##W ;        \
 const                 IFactory&RelW1DFrom##T1To##T2With##WFactory = \
                              s_RelW1DFrom##T1To##T2With##W ;

/** @def __ImplementRelationW2D( FROM , TO , WEIGHT )
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
#define __ImplementRelationW2D( T1 , T2 , W )                        \
 typedef RelationWeighted2D<##T1,##T2,##W>                           \
                                RelW2DFrom##T1To##T2With##W ;        \
 static const DataObjectFactory<RelW2DFrom##T1To##T2With##W>         \
                              s_RelW2DFrom##T1To##T2With##W ;        \
 const                 IFactory&RelW2DFrom##T1To##T2With##WFactory = \
                              s_RelW2DFrom##T1To##T2With##W ;

/** @def __ImplementRelationTool( T1 , T2 ) 
 *  
 *  mandatory macro to make ordinary uni- and bi-directional  relations 
 *  from type 'T1' to type 'T2'
 *  
 *  @see RelationTool
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define __ImplementRelationTool( T1 , T2 )                 \
 typedef RelationTool<##T1,##T2>                           \
                          RelToolFrom##T1To##T2 ;          \
 static const ToolFactory<RelToolFrom##T1To##T2>           \
                        s_RelToolFrom##T1To##T2 ;          \
 const       IToolFactory&RelToolFrom##T1To##T2Factory =   \
                        s_RelToolFrom##T1To##T2           ;

/** @def __ImplementRelationWTool( T1 , T2 , W ) 
 *  
 *  mandatory macro to make weighted uni- and bi-directional  relations 
 *  from type 'T1' to type 'T2' with weight 'W'
 *  
 *  @see RelationWeightedTool
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define __ImplementRelationWeightedTool( T1 , T2 , W )             \
 typedef RelationWeightedTool<##T1,##T2,##W>                       \
                          RelWToolFrom##T1To##T2With##W ;          \
 static const ToolFactory<RelWToolFrom##T1To##T2With##W>           \
                        s_RelWToolFrom##T1To##T2With##W ;          \
 const       IToolFactory&RelWToolFrom##T1To##T2With##WFactory =   \
                        s_RelWToolFrom##T1To##T2With##W ;

/** @def __ImplementNamedRelationTool( Name , T1 , T2 ) 
 *
 *  Implement the relation tool with given name! 
 *  
 *  @see RelationTool
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define __ImplementNamedRelationTool( Name , T1 , T2 )            \
 typedef RelationTool<##T1,##T2>                                  \
                          RelToolFrom##T1To##T2 ;                 \
   class Name : public    RelToolFrom##T1To##T2                   \
   {                                                              \
    friend ToolFactory<##Name>;                                   \
    protected:                                                    \
    Name                                                          \
     ( const std::string& type   ,                                \
       const std::string& name   ,                                \
       const IInterface*  parent )                                \
    :RelToolFrom##T1To##T2 ( type , name , parent ) {};           \
    virtual  ~##Name() {};                                        \
    private:                                                      \
    Name();                                                       \
    Name( const    Name& );                                       \
    Name& operator= ( const Name& );                              \
   };                                                             \
 static const ToolFactory<##Name>         s_##Name##Factory ;     \
 const       IToolFactory&##Name##Factory = s_##Name##Factory ;

/** @def __ImplementNamedRelationWTool( Name , T1 , T2 , W ) 
 *  
 *  Implement the relation tool with given name! 
 *  
 *  @see RelationWeightedTool
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define __ImplementNamedRelationWeightedTool( Name , T1 , T2 , W ) \
 typedef RelationWeightedTool<##T1,##T2,##W>                       \
                          RelToolFrom##T1To##T2With##W ;           \
 class Name : public      RelToolFrom##T1To##T2With##W             \
   {                                                               \
    friend ToolFactory<##Name>;                                    \
    protected:                                                     \
    Name  ( const std::string& type   ,                            \
            const std::string& name   ,                            \
            const IInterface*  parent )                            \
    : RelToolFrom##T1To##T2With##W ( type , name , parent ) {};    \
    virtual ~##Name() {};                                          \
    private:                                                       \
    Name();                                                        \
    Name( const   Name& );                                         \
    Name& operator= ( const Name& );                               \
   };                                                              \
 static const ToolFactory<##Name>           s_##Name##Factory ;    \
 const       IToolFactory&##Name##Factory = s_##Name##Factory ;

/** @def __DeclareRelation1D( T1 , T2 )
 *  
 *  mandatory macro to declare ordinary unidirectional 
 *  relations from type 'T1' to type 'T2'.
 *
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define __DeclareRelation1D( T1 , T2 ) \
          DECLARE_OBJECT( Rel1DFrom##T1To##T2 );

/** @def __DeclareRelation2D( T1 , T2 )
 *  
 *  mandatory macro to declare ordinary bidirectional
 *  relations from type 'T2' to type 'T1'.
 *
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 *  mandatory macro to declare relation
 */
#define __DeclareRelation2D( T1 , T2 ) \
          DECLARE_OBJECT( Rel2DFrom##T1To##T2 );

/** @def __DeclareRelationW1D( T1 , T2 , W )
 *  
 *  mandatory macro to declare weighted unidirectional
 *  relations from type 'T1' to type 'T2' with weight 'W'
 *
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define __DeclareRelationW1D( T1 , T2 , W ) \
          DECLARE_OBJECT( RelW1DFrom##T1To##T2With##W );

/** @def __DeclareRelationW2D( T1 , T2 , W )
 *  
 *  mandatory macro to declare weighted bidirectional
 *  relations from type 'FROM' to type 'TO' with weight 'WEIGHT'
 *
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define __DeclareRelationW2D( T1 , T2 , W )  \
          DECLARE_OBJECT( RelW2DFrom##T1To##T2With##W );

// ============================================================================
// The End  
// ============================================================================
#endif // RELATIONS_RELATIONMACROS_H
// ============================================================================
