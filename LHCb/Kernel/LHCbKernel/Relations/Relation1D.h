// $Id: Relation1D.h,v 1.3 2002-04-09 07:58:53 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/04/03 15:35:17  ibelyaev
// essential update and redesing of all 'Relations' stuff
//
// =============================================================================
#ifndef RELATIONS_RELATIONOBJECT_H
#define RELATIONS_RELATIONOBJECT_H 1
// Include files
// STD & STL
#include <algorithm>
// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// From Relations
#include "Relations/RelationUtils.h"
#include "Relations/IRelation.h"
#include "Relations/RelationBase.h"
#include "Relations/Relation.h"

/** @class Relation1D Relation1D.h Relations/Relation1D.h
 *
 *  @brief Implementation of ordinary unidirectional relations 
 * 
 *  Implementation of ordinary unidirectional relations 
 *  from objects of type "FROM" to objects of type "TO".
 *  
 *  Data types for "FROM" and "TO":
 *
 *   - either inherits form ContainedObject/DataObject 
 *   - or have the templated specializations through 
 *     ObjectTypeTraits structure with defined streamer operator 
 *     to/from StreamBuffer class 
 *  
 *  @see DataObject 
 *  @see ContainedObject 
 *  @see ObjectTypeTraits 
 *  @see StreamBuffer 
 *  @warning for the current implementation the actual type of 
 *           FROM should differ from the actual type of TO
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/01/2002
 */

template<class FROM,class TO>
class Relation1D :
  public         DataObject                   ,
  public virtual IRelation <FROM,TO>          ,
  public virtual  Relations::Relation<FROM,TO>   
{
  
public:
  
  /// short cut for own     type
  typedef Relation1D<FROM,TO>              OwnType        ;
  /// short cut for inverse type
  typedef Relation1D<TO,FROM>              InvType        ;
  /// short cut for interface 
  typedef IRelation<FROM,TO>               IBase          ;
  /// short cut for the actual implementation type 
  typedef Relations::Relation<FROM,TO>     Base           ;
  
public:
  
  /// the default constructor
  Relation1D ( const size_t reserve = 0 ) 
    : DataObject () , Base( reserve    ) {};
  
  /** constructor from "inverted object"
   *  @param inv object to be inverted
   *  @param flag artificial argument to distinguisch from 
   *  copy constructor
   */
  Relation1D ( const InvType& inv , int flag ) 
    : DataObject ( inv ) , Base( inv  , flag ) {};

  /** constructor from "inverted interface"
   *  @param inv object to be inverted
   *  @param flag artificial argument to distinguisch from 
   *  copy constructor
   */
  Relation1D ( const IBase::InverseType& inv , int flag ) 
    : DataObject () , Base( inv  , flag ) {};
  
  /// destructor (virtual)
  virtual ~Relation1D(){ clear() ; };
  
  /** object identification (static method)
   *  @see DataObject
   *  @see ObjectTypeTraits
   *  @see RelationWeightedTypeTraits
   *  @return the unique class identifier
   */
  static  const CLID& classID()
  {
    static const CLID s_clid =
      Relations::clid( "Relation1D"             ,
                       FromTypeTraits:: id()    ,
                       ToTypeTraits::   id()    ,
                       0                        ,
                       TypeTraits::     version , 0 );
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
    typedef typename FromTypeTraits::SERIALIZER FS;
    typedef typename FromTypeTraits::APPLY      FA;
    typedef typename ToTypeTraits::SERIALIZER   TS;
    typedef typename ToTypeTraits::APPLY        TA;
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
        FS::serialize( s , FA::apply( (*entry).first  , this ) ) ;
        TS::serialize( s , TA::apply( (*entry).second , this ) ) ;
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
    typedef typename FromTypeTraits::SERIALIZER FS;
    typedef typename FromTypeTraits::APPLY      FA;
    typedef typename ToTypeTraits::SERIALIZER   TS;
    typedef typename ToTypeTraits::APPLY        TA;
    // clear all existing relations 
    clear();
    // serialize the base class
    DataObject::serialize( s );
    unsigned long _size ;
    s >> _size ;
    while( _size-- > 0 )
      {
        //
        From from ; To to ;
        FS::serialize( s , FA::apply( from   , this ) ) ;
        TS::serialize( s , TA::apply( to     , this ) ) ;
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
  virtual unsigned long addRef  ()  { return  DataObject::addRef  () ; }
  
  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references
   */
  virtual unsigned long release ()  { return  DataObject::release () ; }
  
};

// =============================================================================
// The End
// =============================================================================
#endif // RELATIONS_Relation1D_H
// =============================================================================
