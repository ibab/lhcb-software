// $Id: Relation2D.h,v 1.7 2002-04-25 15:30:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/04/25 14:10:16  ibelyaev
//  one more fix for Win2K
//
// ============================================================================
#ifndef RELATIONS_Relation2D_H 
#define RELATIONS_Relation2D_H 1
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
#include "Relations/IRelation.h"
#include "Relations/IRelation2D.h"
#include "Relations/Relation2.h"

/** @class Relation2D Relation2D.h Relations/Relation2D.h
 *  
 ** Templated object which implements bidirectional relations 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/01/2002
 */

template< class FROM, class TO>
class Relation2D :
  public          DataObject                    , 
  public virtual IRelation2D<FROM,TO>           , 
  public virtual  Relations::Relation2<FROM,TO>
{
public:
  
  /// shortcut for own type 
  typedef Relation2D<FROM,TO>              OwnType    ;
  /// shortcut for inverse type 
  typedef Relation2D<TO,FROM>              InvType    ;
  /// shortcut for "direct"(="main") interface
  typedef IRelation2D<FROM,TO>             IBase      ;
  /// shortcut for "direct"(="main") interface
  typedef IRelation<FROM,TO>               IBase1     ;
  /// shortcut for "inverse interface  interface
  typedef IRelation<TO,FROM>               IBase2     ;
  /// shortcut for actual implementation  
  typedef Relations::Relation2<FROM,TO>    Base       ;
  
public:
  
  /** Standard/default  constructor
   *  @param reserve the map-size to be preallocated
   */
  Relation2D ( const size_t reserve  = 0 ) 
    : DataObject () , Base ( reserve ) {};
  
  /** constructor from the inverse type!
   *  @attention it is the way to "invert" all relations!
   *  @param inv the inverse relation object
   *  @param flag artificial argument to invert the relations 
   */
  Relation2D ( const InvType& inv , int flag ) 
    : DataObject ( inv ) , Base( inv , flag ) {};
  
  /// destructor (virtual) 
  virtual ~Relation2D() { clear() ; } ;
  
  /** object identification (static method) 
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  static  const CLID& classID() 
  {
    static const CLID s_clid = 
      Relations::clid( "Relation2D"                  , 
                       IBase1::FromTypeTraits:: id() ,
                       IBase1::ToTypeTraits::   id() , 
                       0                             , 
                       IBase1::TypeTraits:: version  , 0 );
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
    typedef typename FromTypeTraits::Apply      ApplyF     ;
    typedef typename ToTypeTraits::Apply        ApplyT     ;
    typedef typename FromTypeTraits::Serializer SerializeF ;
    typedef typename ToTypeTraits::Serializer   SerializeT ;
    // serialize the base class
    DataObject::serialize( s );
    // get all relations 
    Range range = relations() ;
    // serialize the number of relations 
    unsigned long _size = range.end() - range.begin() ;
    s << _size ;
    // serialise all relations
    for( iterator entry = range.begin() ;
         range.end() != entry ; ++entry ) 
      {
        SerializeF::serialize 
          ( s , ApplyF::apply ( (*entry).first   , this ) );
        SerializeT::serialize 
          ( s , ApplyT::apply ( (*entry).second  , this ) );
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
    typedef typename FromTypeTraits::Apply      ApplyF     ;
    typedef typename ToTypeTraits::Apply        ApplyT     ;
    typedef typename FromTypeTraits::Serializer SerializeF ;
    typedef typename ToTypeTraits::Serializer   SerializeT ;
    // clear all existing relations 
    clear();
    // serialize the base class
    DataObject::serialize( s );
    unsigned long _size ;
    s >> _size ;
    From from ; To to ;
    while( _size-- > 0 )
      {
        //
        SerializeF::serialize ( s , ApplyF::apply ( from   , this ) ) ;
        SerializeT::serialize ( s , ApplyT::apply ( to     , this ) ) ;
        //
        relate( from , to ) ;
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
#endif // RELATIONS_Relation2D_H
// ============================================================================
