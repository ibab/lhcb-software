// $Id: RelationWeighted2D.h,v 1.4 2002-04-09 07:58:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/08 16:17:41  ibelyaev
//  minor fix
//
// Revision 1.2  2002/04/03 15:35:18  ibelyaev
// essential update and redesing of all 'Relations' stuff
// 
// ============================================================================
#ifndef RELATIONS_BIRELATIONWITHWEIGHTOBJECT_H 
#define RELATIONS_BIRELATIONWITHWEIGHTOBJECT_H 1
// Include files
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
  virtual ~RelationWeighted2D() { clear() ; } ;
  
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
    typedef typename FromTypeTraits::SERIALIZER   FS;
    typedef typename FromTypeTraits::APPLY        FA;
    typedef typename WeightTypeTraits::SERIALIZER WS;
    typedef typename WeightTypeTraits::APPLY      WA;
    typedef typename ToTypeTraits::SERIALIZER     TS;
    typedef typename ToTypeTraits::APPLY          TA;
    // serialize the base class 
    DataObject::serialize( s );
    // get all relations 
    const Range range = relations() ;
    // serialize the number of relations 
    unsigned long _size = ( range.end() - range.begin() ) ;
    s << _size ;
    // serialise all relations
    for( iterator entry = range.begin() ; range.end() != entry ; ++entry ) 
      {    
        FS::serialize( s , FA::apply( (*entry).first.first  , this ) ) ;
        WS::serialize( s , WA::apply( (*entry).first.second , this ) ) ;
        TS::serialize( s , TA::apply( (*entry).second       , this ) ) ;
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
    typedef typename FromTypeTraits::SERIALIZER   FS;
    typedef typename FromTypeTraits::APPLY        FA;
    typedef typename WeightTypeTraits::SERIALIZER WS;
    typedef typename WeightTypeTraits::APPLY      WA;
    typedef typename ToTypeTraits::SERIALIZER     TS;
    typedef typename ToTypeTraits::APPLY          TA;
    // clear all existing relations 
    clear();
    // serialize the base class
    DataObject::serialize( s );
    unsigned long _size ;
    s >> _size ;
    while( _size-- > 0 )
      {
        //
        From from ; Weight weight  ; To to ;
        //        
        FS::serialize( s , FA::apply( from   , this ) ) ;
        WS::serialize( s , WA::apply( weight , this ) ) ;
        TS::serialize( s , TA::apply( to     , this ) ) ;
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
  virtual unsigned long addRef  () { return  DataObject::addRef  () ; }
  
  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references 
   */
  virtual unsigned long release () { return  DataObject::release () ; }

};

// ============================================================================
// The End  
// ============================================================================
#endif // RELATIONS_RelationWeighted2D_H
// ============================================================================
