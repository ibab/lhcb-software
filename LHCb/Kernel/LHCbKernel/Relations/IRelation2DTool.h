// $Id: IRelation2DTool.h,v 1.1 2002-04-03 15:35:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_IRELATION2DTOOL_H 
#define RELATIONS_IRELATION2DTOOL_H 1

// Include files

/** @class IRelation2DTool IRelation2DTool.h Relations/IRelation2DTool.h
 *  
 *  Generic interface (tool) to access the 2D-relations from Transient Store   
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */

template<class FROM,class TO>
class IRelation2DTool:
  public virtual IRelationTool<FROM,TO>
{
public:
  
  /// shorcut for "own"  type 
  typedef IRelation2DTool<FROM,TO>  OwnType       ;
  /// shortcut for "direct relations
  typedef IRelation<FROM,TO>        DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelation<TO,FROM>        InverseType   ;
  /// shortcut for "direct relations
  typedef IRelation2D<FROM,TO>      Direct2DType  ;
  /// shortcut for "inverse" relations
  typedef IRelation2D<TO,FROM>      Inverse2DType ;
  
public:
  
  /** interface identification (static)
   *  @attention the unique interface identifier is constructed "on-fly"
   *  using hash-technique from the generic interface name and 
   *  unique identifiers of related objects 
   *  @see IInterface
   *  @return the uniqie interface identifier
   */
  static const InterfaceID& interfaceID()
  {
    static const InterfaceID s_iid  =
      Relations::interfaceID( "IRelation2DTool"                 ,
                              DirectType::FromTypeTraits::id () ,
                              DirectType::ToTypeTraits::id   () , 
                              0                                 ,
                              DirectType::TypeTraits::  version , 0 );
    return s_iid ;
  };
  
  /** accessor to  "2D-direct" relation table (non-const version)
   *  @see IRelation
   *  @see IRelatio2Dn
   *  @return pointer to "direct" relation table 
   */
  virtual       Direct2DType*   direct2D  ()       = 0 ;
  
  /** accessor to  "2D-direct" relation table (const version)
   *  @see IRelation
   *  @see IRelatio2Dn
   *  @return pointer to "direct" relation table 
   */
  virtual const Direct2DType*   direct2D  () const = 0 ;

protected:

  /// destructor (virtual and protected)
  virtual ~IRelation2DTool(){}; 

};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IRELATION2DTOOL_H
// ============================================================================
