// $Id: RelationWeighted1D.h,v 1.4 2002-04-24 21:16:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/09 07:58:54  ibelyaev
// *** empty log message ***
//
// Revision 1.2  2002/04/03 15:35:18  ibelyaev
// essential update and redesing of all 'Relations' stuff
// 
// ============================================================================
#ifndef RELATIONS_RELATIONWeighted1D_H 
#define RELATIONS_RELATIONWeighted1D_H 1
// Include files
// STD & STL 
#include <algorithm>
// from Gaudi
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// From Relation
#include "Relations/RelationUtils.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeighted.h"

/** @class RelationWeighted1D RelationWeighted1D.h 
 * 
 *  The simplest implementation of unidirectional weighted relations.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   28/01/2002
 */
template<class FROM, class TO, class WEIGHT>
class RelationWeighted1D :
  public         DataObject                                  ,    
  public virtual IRelationWeighted<FROM,TO,WEIGHT>           ,
  public virtual  Relations::RelationWeighted<FROM,TO,WEIGHT> 
{
  
public:
  
  /// shortcut for own type 
  typedef RelationWeighted1D<FROM,TO,WEIGHT>              OwnType ;
  /// shortcut for inverse type 
  typedef RelationWeighted1D<TO,FROM,WEIGHT>              InvType ;
  /// short cut for interface
  typedef IRelationWeighted<FROM,TO,WEIGHT>               IBase   ;
  /// short cut for teh actual implementation type 
  typedef Relations::RelationWeighted<FROM,TO,WEIGHT>     Base    ;
  
public:
  
  /// the standard/default constructor
  RelationWeighted1D ( const size_t reserve = 0 ) 
    : DataObject     () , Base ( reserve ) {};
  
  /** constructor from inverse object 
   *  @param inv relation object to be inverted 
   *  @param flag artificial argument to distinguish from copy constructor 
   */
  RelationWeighted1D( const InvType& inv , int flag ) 
    : DataObject( inv ) , Base( inv , flag ) {};

  /** constructor from inverse interface
   *  @param inv relation object to be inverted 
   *  @param flag artificial argument to distinguish from copy constructor 
   */
  RelationWeighted1D( const IBase::InverseType & inv , int flag ) 
    : DataObject() , Base( inv , flag ) {};

  /// destructor (virtual)
  virtual ~RelationWeighted1D (){ clear() ; };
  
  /** object identifier (static method)
   *  @see DataObject 
   *  @see ObjectTypeTraits
   *  @see RelationWeightedTypeTraits
   *  @return the unique class identifier 
   */
  static  const CLID& classID() 
  {
    static const CLID s_clid = 
      Relations::clid( "RelationWeighted1D"    , 
                       FromTypeTraits::   id() , 
                       ToTypeTraits::     id() , 
                       WeightTypeTraits:: id() ,
                       TypeTraits::    version , 0 ) ;
    return s_clid ;
  };
  
  /** object identification (virtual method)
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  virtual const CLID& clID()     const { return classID() ; }
  
  /** object serialization for writing
   *  @see DataObject 
   *  @param  s reference to the output stream
   *  @return   reference to the output stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s ) const 
  {
    typedef typename FromTypeTraits::Serializer   FromS   ;
    typedef typename FromTypeTraits::Apply        FromA   ;
    typedef typename WeightTypeTraits::Serializer WeightS ;
    typedef typename WeightTypeTraits::Apply      WeightA ;
    typedef typename ToTypeTraits::Serializer     ToS     ;
    typedef typename ToTypeTraits::Apply          ToA     ;
    // serialize the base class
    DataObject::serialize( s );
    // get all relations 
    Range range = relations() ;
    // serialize the number of relations 
    unsigned long _size = range.end() - range.begin() ;
    s << _size ;
    // serialise all relations
    for( iterator entry = range.begin() ; range.end() != entry ; ++entry ) 
      {    
        FromS::serialize   
          ( s , FromA::apply   ( (*entry).first.first  , this ) ) ;
        WeightS::serialize 
          ( s , WeightA::apply ( (*entry).first.second , this ) ) ;
        ToS::serialize     
          ( s , ToA::apply     ( (*entry).second       , this ) ) ;
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
    typedef typename FromTypeTraits::Serializer   FromS   ;
    typedef typename FromTypeTraits::Apply        FromA   ;
    typedef typename WeightTypeTraits::Serializer WeightS ;
    typedef typename WeightTypeTraits::Apply      WeightA ;
    typedef typename ToTypeTraits::Serializer     ToS     ;
    typedef typename ToTypeTraits::Apply          ToA     ;
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
        FromS::serialize   ( s , FromA::apply   ( from   , this ) ) ;
        WeightS::serialize ( s , WeightA::apply ( weight , this ) ) ;
        ToS::serialize     ( s , ToA::apply     ( to     , this ) ) ;
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
#endif // RELATIONS_RELATIONWeighted1D_H
// ============================================================================
