// $Id: AssociatorMACROs.h,v 1.2 2002-04-25 08:44:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/08 14:26:01  ibelyaev
//  new version of 'Relations'-subpackage
//
// ============================================================================
#ifndef RELATIONS_ASSOCIATORMACROS_H 
#define RELATIONS_ASSOCIATORMACROS_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
// local 
#include "Relations/Associator.h"
#include "Relations/AssociatorWeighted.h"
// local 
#include "Relations/RelationMACROs.h"

/** @file AssociatorMACROs.h 
 *
 *  definition of useful MACROs for fast 
 *  implementation and instantiation of
 *  tool factories for instantiation of relation tools
 *
 *  @warning as for any C++ MACROS, one need to take some care!
 *  
 *  @author Vany Belyaev Ivan.Belyaev@itep.ru
 *  @date   08/04/2002
 */

/** @def IMPLEMENT_Associator( T1 , T2 ) 
 *  
 *  mandatory macro to make ordinary uni- and bi-directional  relations 
 *  from type 'T1' to type 'T2'
 *  
 *  @see  Associator
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define IMPLEMENT_Associator( T1 , T2 )                    \
 typedef  Associator<##T1##,##T2##>                        \
                          AsctFrom##T1##To##T2 ;           \
 static const ToolFactory<AsctFrom##T1##To##T2##>          \
                        s_AsctFrom##T1##To##T2 ;           \
 const       IToolFactory&AsctFrom##T1##To##T2##Factory =  \
                        s_AsctFrom##T1##To##T2             ;

/** @def DECLARE_Associator( T1 , T2 )
 *  
 *  mandatory macro to declare ordinary associator 
 *  from type 'T1' to type 'T2'
 * 
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define DECLARE_Associator( T1 , T2 )  \
          DECLARE_TOOL( AsctFrom##T1##To##T2 );

/** @def IMPLEMENT_WAssociator( T1 , T2 , W ) 
 *  
 *  mandatory macro to make weighted uni- and bi-directional  relations 
 *  from type 'T1' to type 'T2' with weight 'W'
 *  
 *  @see AssociatorWeighted
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define  IMPLEMENT_WAssociator( T1 , T2 , W )                       \
 typedef  AssociatorWeighted<##T1##,##T2##,##W##>                   \
                          AsctwFrom##T1##To##T2##With##W ;          \
 static const ToolFactory<AsctwFrom##T1##To##T2##With##W>           \
                        s_AsctwFrom##T1##To##T2##With##W ;          \
 const       IToolFactory&AsctwFrom##T1##To##T2##With##W##Factory = \
                        s_AsctwFrom##T1##To##T2##With##W            ;

/** @def DECLARE_WAssociator( T1 , T2 , W )
 *  
 *  mandatory macro to declare weighted associator 
 *  from type 'T1' to type 'T2' with weight 'W'
 * 
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define DECLARE_WAssociator( T1 , T2 , W )  \
          DECLARE_TOOL( AsctwFrom##T1##To##T2##With##W );

/** @def IMPLEMENT_NamedAssociator( Name , T1 , T2 ) 
 *
 *  Implement the relation tool with given name! 
 *  
 *  @see Associator
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define IMPLEMENT_NamedAssociator( Name , T1 , T2 )               \
 typedef Associator<##T1##,##T2##>                                \
                          Name##AsctFrom##T1##To##T2 ;            \
   class Name : public    Name##AsctFrom##T1##To##T2              \
   {                                                              \
    friend ToolFactory<##Name##>;                                 \
    protected:                                                    \
    Name                                                          \
     ( const std::string& type   ,                                \
       const std::string& name   ,                                \
       const IInterface*  parent )                                \
    : Name##AsctFrom##T1##To##T2 ( type , name , parent ) {};     \
    virtual  ~##Name () {};                                       \
    private:                                                      \
    Name ();                                                      \
    Name ( const    Name & );                                     \
    Name & operator= ( const Name & );                            \
   };                                                             \
 static const ToolFactory<##Name##>         s_##Name##Factory  ;  \
 const       IToolFactory&##Name##Factory = s_##Name##Factory  ;

/** @def DECLARE_NamedAssociator( Name )
 *  
 *  mandatory macro to declare anmed  associator 
 * 
 *  @attention The macro is to beused in PACKAGE_load.cpp file  
 */
#define DECLARE_NamedAssociator( Name  )  \
          DECLARE_TOOL( Name  );

/** @def IMPLEMENT_NamedWAssociator( Name , T1 , T2 , W ) 
 *  
 *  Implement the relation tool with given name! 
 *  
 *  @see AssociatorWeighted
 *  @see  ToolFactory
 *  @see IToolFactory
 *  
 *  The macro need to be used in your *.cpp file 
 */
#define IMPLEMENT_NamedWAssociator( Name , T1 , T2 , W )                \
 typedef AssociatorWeighted<##T1##,##T2##,##W##>                        \
                          Name##AsctwFrom##T1##To##T2##With##W ;        \
 class Name : public      Name##AsctwFrom##T1##To##T2##With##W          \
   {                                                                    \
    friend ToolFactory<##Name##>;                                       \
    protected:                                                          \
    Name  ( const std::string& type   ,                                 \
            const std::string& name   ,                                 \
            const IInterface*  parent )                                 \
    : Name##AsctwFrom##T1##To##T2##With##W ( type , name , parent ) {}; \
    virtual ~##Name () {};                                              \
    private:                                                            \
    Name ();                                                            \
    Name ( const   Name & );                                            \
    Name & operator= ( const Name & );                                  \
   };                                                                   \
 static const ToolFactory<##Name##>         s_##Name##Factory ;         \
 const       IToolFactory&##Name##Factory = s_##Name##Factory ;


// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_ASSOCIATORMACROS_H
// ============================================================================
