// $Id: Relation1D.h,v 1.11 2003-06-25 14:59:01 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2003/06/16 13:27:54  sponce
// fixes for gcc 3.2 and 3.3
//
// Revision 1.9  2003/01/17 14:07:01  sponce
// support for gcc 3.2
//
// Revision 1.8  2002/07/25 15:32:14  ibelyaev
//  bug fix in destructors of relation objects
//
// Revision 1.7  2002/04/25 15:30:18  ibelyaev
//  one more attempt to make Bill Gates happy
//
// Revision 1.6  2002/04/25 14:10:13  ibelyaev
//  one more fix for Win2K
//
// =============================================================================
#ifndef RELATIONS_Relation1D_H
#define RELATIONS_Relation1D_H 1
// Include files
#include "Relations/PragmaWarnings.h"
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
  typedef typename Relations::Relation<FROM,TO>     Base           ;
  
public:
  
  /// the default constructor
  Relation1D ( const size_t reserve = 0 ) 
    : IBase(), Base(reserve), DataObject() 
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /** constructor from "inverted object"
   *  @param inv object to be inverted
   *  @param flag artificial argument to distinguisch from 
   *  copy constructor
   */
  Relation1D ( const InvType& inv , int flag ) 
    : IBase(), Base( inv  , flag ), DataObject ( inv ) 
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };

  /** constructor from "inverted interface"
   *  @param inv object to be inverted
   *  @param flag artificial argument to distinguisch from 
   *  copy constructor
   */
  Relation1D ( const typename IBase::InverseType& inv , int flag ) 
    : IBase(), Base( inv  , flag ), DataObject ()
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };

  /// destructor (virtual)
  virtual ~Relation1D() 
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().decrement( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /// the type name 
  const std::string& type() const 
  {
    static const std::string s_type( System::typeinfoName( typeid(OwnType) ) ) ;
    return s_type ;
  };
  
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
    typedef typename FromTypeTraits::Apply      ApplyF     ;
    typedef typename ToTypeTraits::Apply        ApplyT     ;
    typedef typename FromTypeTraits::Serializer SerializeF ;
    typedef typename ToTypeTraits::Serializer   SerializeT ;
    // serialize the base class
    DataObject::serialize( s );
    // get all relations 
    typename IRelation<FROM, TO>::Range range = relations() ;
    // serialize the number of relations 
    unsigned long _size = range.end() - range.begin() ;
    s << _size ;
    // serialise all relations
    for( typename Relation1D<FROM, TO>::iterator entry = range.begin() ;
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
    typename IBase::From from ;
    typename IBase::To   to ;
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
  virtual unsigned long addRef  () { return  DataObject::addRef  () ; }
  
  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references
   */
  virtual unsigned long release () { return  DataObject::release () ; }
  
};

// =============================================================================
// The End
// =============================================================================
#endif // RELATIONS_Relation1D_H
// =============================================================================
