// $Id: RelationWeighted2D.h,v 1.10 2003-01-17 14:07:02 sponce Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/07/25 15:32:15  ibelyaev
//  bug fix in destructors of relation objects
//
// Revision 1.8  2002/04/25 15:30:18  ibelyaev
//  one more attempt to make Bill Gates happy
//
// Revision 1.7  2002/04/25 14:10:23  ibelyaev
//  one more fix for Win2K
//
// ============================================================================
#ifndef RELATIONS_RelationWeighted2D_H 
#define RELATIONS_RelationWeighted2D_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL 
#include <algorithm>
// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
// From Relations
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeightedBase.h"
#include "Relations/Relation2Weighted.h"
// Forward declaration
class StreamBuffer;

/** @class RelationWeighted2D RelationWeighted2D.h 
 *  
 *  Templated object, which implements the bidirectional 
 *  relations with the weight
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/01/2002
 */
template<class FROM, class TO, class WEIGHT>
class RelationWeighted2D :
  public         DataObject                                     ,
  public virtual IRelationWeighted2D           <FROM,TO,WEIGHT> ,
  public virtual  Relations::Relation2Weighted <FROM,TO,WEIGHT> 
{
public:
  
  /// shortcut for own type 
  typedef RelationWeighted2D<FROM,TO,WEIGHT>              OwnType    ;
  /// shortcut for inverse type 
  typedef RelationWeighted2D<TO,FROM,WEIGHT>              InvType    ;
  /// shortcut for "direct"(="main") interface
  typedef IRelationWeighted2D<FROM,TO,WEIGHT>             IBase      ;
  /// shortcut for "direct"(="main") interface
  typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase1     ;
  /// shortcut for "inverse interface  interface
  typedef IRelationWeighted<TO,FROM,WEIGHT>               IBase2     ;
  /// shortcut for actual implementation  
  typedef Relations::Relation2Weighted<FROM,TO,WEIGHT>    Base       ;
  
public:
  
  /** Standard/default  constructor
   *  @param reserve the map-size to be preallocated
   */
  RelationWeighted2D ( const size_t reserve  = 0 ) 
    : DataObject () , Base ( reserve ) {};
  
  /** constructor from the inverse type!
   *  @attention it is the way to "invert" all relations!
   *  @param inv the inverse relation object
   *  @param flag artificial argument to invert the relations 
   */
  RelationWeighted2D ( const InvType& inv , int flag ) 
    : DataObject ( inv ) , Base( inv , flag ) {};
  
  /// destructor (virtual) 
  virtual ~RelationWeighted2D() {} ;
  
  /** object identification (static method) 
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  static  const CLID& classID() 
  {
    static const CLID s_clid = 
      Relations::clid( "RelationWeighted2D"            ,
                       IBase1::FromTypeTraits::   id() , 
                       IBase1::ToTypeTraits::     id() , 
                       IBase1::WeightTypeTraits:: id() ,
                       IBase1::TypeTraits::    version , 0 ) ;
    return s_clid ;
  };

  /** object identification (virtual method)
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  virtual const CLID& clID()     const { return classID() ; }
  
  /** object serialization for writing
   *  only direct relations are serialized 
   *  @see DataObject 
   *  @param  s reference to the output stream
   *  @return   reference to the output stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s ) const 
  {
    typedef typename FromTypeTraits::Apply        ApplyF     ;
    typedef typename WeightTypeTraits::Apply      ApplyW     ;
    typedef typename ToTypeTraits::Apply          ApplyT     ;
    typedef typename FromTypeTraits::Serializer   SerializeF ;
    typedef typename WeightTypeTraits::Serializer SerializeW ;
    typedef typename ToTypeTraits::Serializer     SerializeT ;
    // serialize the base class 
    DataObject::serialize( s );
    // get all relations 
    const typename RelationWeighted2D<FROM,TO,WEIGHT>::Range range=relations();
    // serialize the number of relations 
    unsigned long _size = ( range.end() - range.begin() ) ;
    s << _size ;
    // serialise all relations
    for (typename RelationWeighted2D<FROM,TO,WEIGHT>::iterator entry =
           range.begin(); range.end() != entry ; ++entry ) 
      {    
        SerializeF::serialize 
          ( s , ApplyF::apply ( (*entry).first.first   , this ) );
        SerializeW::serialize 
          ( s , ApplyW::apply ( (*entry).first.second  , this ) );
        SerializeT::serialize 
          ( s , ApplyT::apply ( (*entry).second        , this ) );
      };
    ///
    return s ;
  };
  
  /** object serialization for reading
   *  @see DataObject 
   *  @param  s reference to the input  stream
   *  @return   reference to the input  stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s )       
  {
    typedef typename FromTypeTraits::Apply        ApplyF ;
    typedef typename WeightTypeTraits::Apply      ApplyW ;
    typedef typename ToTypeTraits::Apply          ApplyT ;
    typedef typename FromTypeTraits::Serializer   SerializeF ;
    typedef typename WeightTypeTraits::Serializer SerializeW ;
    typedef typename ToTypeTraits::Serializer     SerializeT ;
    // clear all existing relations 
    clear();
    // serialize the base class
    DataObject::serialize( s );
    unsigned long _size ;
    s >> _size ;
    typename RelationWeighted2D<FROM,TO,WEIGHT>::From from;
    typename RelationWeighted2D<FROM,TO,WEIGHT>::Weight weight;
    typename RelationWeighted2D<FROM,TO,WEIGHT>::To to;
    while( _size-- > 0 )
      {
        //
        SerializeF::serialize ( s , ApplyF::apply ( from   , this ) ) ;
        SerializeW::serialize ( s , ApplyW::apply ( weight , this ) ) ;
        SerializeT::serialize ( s , ApplyT::apply ( to     , this ) ) ;
        //
        relate( from , to , weight ) ;
      }
    return s ;
  };
  
  /** increase the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references 
   */
  virtual unsigned long addRef  () 
  { return  DataObject::addRef  () ; }
  
  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references 
   */
  virtual unsigned long release () 
  { return  DataObject::release () ; }
  
};

// ============================================================================
// The End  
// ============================================================================
#endif // RELATIONS_RelationWeighted2D_H
// ============================================================================
