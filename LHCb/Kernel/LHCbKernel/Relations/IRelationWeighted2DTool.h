// $Id: IRelationWeighted2DTool.h,v 1.1 2002-04-03 15:35:17 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_IRELATIONWeighted2DTOOL_H 
#define RELATIONS_IRELATIONWeighted2DTOOL_H 1

// Include files

/** @class IRelationWeighted2DTool IRelationWeighed2DTool.h 
 *  
 *  Generic interface (tool) to access the weighted 
 *  2D-relations from Transient Store   
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */

template<class FROM,class TO, class WEIGHT>
class IRelationWeighted2DTool:
  public virtual IRelationWeightedTool<FROM,TO,WEIGHT>
{
public:
  
  /// shorcut for "own"  type 
  typedef IRelationWeighted2DTool<FROM,TO,WEIGHT>  OwnType       ;
  /// shortcut for "direct relations
  typedef IRelationWeighted<FROM,TO,WEIGHT>        DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelationWeighted<TO,FROM,WEIGHT>        InverseType   ;
  /// shortcut for "direct relations
  typedef IRelationWeighted2D<FROM,TO,WEIGHT>      Direct2DType  ;
  /// shortcut for "inverse" relations
  typedef IRelationWeighted2D<TO,FROM,WEIGHT>      Inverse2DType ;
  
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
      Relations::interfaceID( "IRelationWeighted2DTool"            ,
                              DirectType::FromTypeTraits::   id () ,
                              DirectType::ToTypeTraits::     id () , 
                              DirectType::WeightTypeTraits:: id () , 
                              DirectType::TypeTraits::     version , 0 );
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
