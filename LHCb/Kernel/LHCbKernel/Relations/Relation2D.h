// $Id: Relation2D.h,v 1.1 2002-03-18 19:32:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_RELATION2D_H 
#define RELATIONS_RELATION2D_H 1
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
#include "Relations/IRelation.h"
#include "Relations/RelationBase.h"
#include "Relations/Apply.h"

/** @class Relation2D Relation2D.h Relations/Relation2D.h
 *  
 ** Templated object which implements bidirectional relations 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   28/01/2002
 */

template< class FROM, class TO>
class Relation2D :
  public virtual IRelation<FROM,TO> , 
  public virtual IRelation<TO,FROM> , 
  public         DataObject            
{
public:
  
  /// shortcut for own type 
  typedef Relation2D<FROM,TO>              OwnType    ;
  /// shortcut for inverse type 
  typedef Relation2D<TO,FROM>              InvType    ;
  /// shortcut for "direct"(="main") interface
  typedef IRelation<FROM,TO>               IBase      ;
  /// shortcut for "direct"(="main") interface
  typedef IRelation<FROM,TO>               IBase1     ;
  /// shortcut for "inverse interface  interface
  typedef IRelation<TO,FROM>               IBase2     ;
  /// shortcut for actual implementation of direct relations 
  typedef Relations::RelationBase<FROM,TO> Base1      ;
  /// shortcut for actual implementation of inverse relations 
  typedef Relations::RelationBase<TO,FROM> Base2      ;
  
public:
  
  /// Standard/default  constructor
  Relation2D ( const typename Base1::size_type reserve  = 0 ) 
    : DataObject () , m_base1 ( reserve ) , m_base2 ( reserve ) {};
  
  /** constructor from the inverse type!
   *  @attention it is the way to "invert" all relations!
   *  @param inv the inverse relation object
   */
  Relation2D ( const InvType& inv ) 
    : DataObject () , m_base1 ( inv.m_base2 ) , m_base2 ( inv.m_base1 ) {};
  
  /// destructor (virtual) 
  virtual ~Relation2D() {} ;
  
  /** object identification (static method) 
   *  @see DataObject 
   *  @return the unique class identifier 
   */ 
  static  const CLID& classID() 
  {
    static const CLID s_clid = 
      Relations::clid( "Relation2D"                  , 
                       IBase1::FromTypeTraits:: id() ,
                       IBase1::ToTypeTraits::   id() );
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
    // serialize the base class 
    DataObject::serialize( s );
    // serialize total number of relations
    s << m_base1.i_entries().size() ;
    // serialize all relations
    for( Base1::CIT it = m_base1.i_entries().begin() ; 
         m_base1.i_entries().end() != it ; ++it )
      { 
        s << Relations::apply( (*it).first  , this ) 
          << Relations::apply( (*it).second , this ) ;
      }
    return s ;
  };
  
  /** object serialization for reading
   *  @see DataObject 
   *  @param  s reference to the input  stream
   *  @return   reference to the input  stream 
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s )       
  {
    // reset existing relations
    m_base1.i_entries().clear();
    m_base2.i_entries().clear();
    // serialize the base class 
    DataObject::serialize( s );
    // get the total number of relations 
    typename Base1::Entries::size_type num;
    s >> num ; 
    while( num-- > 0 )
      {
        // serialize all relations
        IBase1::From from ; 
        IBase1::To   to   ; 
        s >> Relations::apply( from , this )  
          >> Relations::apply( to   , this )  ;
        m_base1.i_relate( from , to    );
        m_base2.i_relate( to   , from  );
      }
    ///
    return s ;
  };
  
  /** retrive all relations from the object 
   *  @attention direct interface!
   *
   *   - the CPU performance is proportional to log(N), 
   *     where N is the total number of relations
   *
   *  @see    IRelation
   *  @see    IBiRelation
   *  @see    RelationBase
   *  @param  object the object
   *  @return pair of iterators for output relations   
   */
  virtual  IBase1::Range relations
  ( const  IBase1::From& object ) const 
  { 
    Base1::IP ip = m_base1.i_relations( object );
    return IBase1::Range( ip.first , ip.second );
  };
  
  /** retrive all relations from the object 
   *  @attention inverse  interface
   *
   *   - the CPU performance is proportional to log(N), 
   *     where N is the total number of relations
   *
   *  @see    IRelation
   *  @see    IBiRelation
   *  @see    RelationBase
   *  @param  object  the object
   *  @return pair of iterators for output relations   
   */
  virtual  IBase2::Range relations
  ( const  IBase2::From& object ) const 
  { 
    Base2::IP ip = m_base2.i_relations( object );
    return IBase2::Range( ip.first , ip.second );
  };
  
  /** make the relation between 2 objects 
   *
   *   - StatusCode::FAILURE is returned if the relation
   *     between the given objects  is already set
   *
   *   - the CPU performance is proportional to log(N)
   *     for location of the object plus some overhead for 
   *     list operations, which is more or less constant for 
   *     std::deque implementation of the underlying relation 
   *     store and proportional to N for std::vector implementation, 
   *     where N is the total number of relations 
   *
   *  @attention direct interface!
   *  @see    IRelation
   *  @see    RelationBase
   *  @param  object1 the first object
   *  @param  object2 the second object 
   *  @return status  code 
   */
  virtual  StatusCode    relate 
  ( const  IBase1::From& object1 , 
    const  IBase1::To&   object2 )      
  {
    StatusCode              sc = m_base1.i_relate( object1 , object2 ) ;
    if( sc.isSuccess() ) {  sc = m_base2.i_relate( object2 , object1 ) ; }
    return sc ;
  };
  
  /** make the relation between 2 objects 
   *
   *   - StatusCode::FAILURE is returned if the relation
   *     between the given objects  is already set
   *
   *   - the CPU performance is proportional to log(N)
   *     for location of the object plus some overhead for 
   *     list operations, which is more or less constant for 
   *     std::deque implementation of the underlying relation 
   *     store and proportional to N for std::vector implementation, 
   *     where N is the total number of relations 
   *
   *  @attention inverse interface!
   *  @see    IRelation
   *  @see    RelationBase
   *  @param  object1 the first object
   *  @param  object2 the second object 
   *  @return status  code 
   */
  virtual  StatusCode    relate 
  ( const  IBase2::From& object1 , 
    const  IBase2::To&   object2 )      
  { 
    StatusCode              sc = m_base2.i_relate( object1 , object2 ) ;
    if( sc.isSuccess() ) {  sc = m_base1.i_relate( object2 , object1 ) ; }
    return sc ;
  };
  
  /** remove the concrete relation between objects
   *
   *   - StatusCode::FAILURE is returned if the relation
   *     between the given objects  is already set
   *
   *   - the CPU performance is proportional to log(N)
   *     for location of the object plus some overhead for 
   *     list operations, which is more or less constant for 
   *     std::deque implementation of the underlying relation 
   *     store and proportional to N for std::vector implementation, 
   *     where N is the total number of relations 
   *
   *  @attention direct interface !
   *  @see IRelation
   *  @see RelationBase
   *  @param  object1 the first object
   *  @param  object2 the second object 
   *  @return status  code 
   */
  virtual  StatusCode    remove 
  ( const  IBase1::From& object1 , 
    const  IBase1::To&   object2 )      
  { 
    StatusCode              sc = m_base1.i_remove( object1 , object2 ) ;
    if( sc.isSuccess() ) {  sc = m_base2.i_remove( object2 , object1 ) ; }
    return sc ;
  };
  
  /** remove the concrete relation between objects
   *
   *   - StatusCode::FAILURE is returned if the relation
   *     between the given objects  is already set
   *
   *   - the CPU performance is proportional to log(N)
   *     for location of the object plus some overhead for 
   *     list operations, which is more or less constant for 
   *     std::deque implementation of the underlying relation 
   *     store and proportional to N for std::vector implementation, 
   *     where N is the total number of relations 
   *
   *  @attention inverse interface !
   *  @see IRelation
   *  @see RelationBase
   *  @param  object1 the first object
   *  @param  object2 the second object 
   *  @return status code 
   */
  virtual  StatusCode    remove 
  ( const  IBase2::From& object1 , 
    const  IBase2::To&   object2 )      
  { 
    StatusCode             sc = m_base2.i_remove( object1 , object2 ) ;
    if( sc.isSuccess() ) { sc = m_base1.i_remove( object2 , object1 ) ; }
    return sc ;
  };
  
  /** remove all relations FROM the defined object
   *
   *   - StatusCode::FAILURE is returned if there are no relations
   *     from the given object
   *
   *   - Method is a combinaiton of "fast direct" and 
   *     "slow inverse" methods, the CPU performance is 
   *     proportional to the N, where N is the 
   *     total number of relations 
   *
   *  @attention direct interface !
   *  @see    IRelation
   *  @see    RelationBase
   *  @param  object  smart reference to the object
   *  @return status code 
   */
  virtual  StatusCode    remove 
  ( const  IBase1::From& object ) 
  { 
    StatusCode              sc = m_base1.i_remove( object ) ;
    if( sc.isSuccess() ) {  sc = m_base2.i_remove( object ) ; }
    return sc ;
  };
  
  /** remove all relations TO the defined object
   *
   *   - StatusCode::FAILURE is returned if there are no relations
   *     from the given object
   *
   *   - Method is a combinaiton of "slow direct" and 
   *     "fast inverse" methods, the CPU performance is 
   *     proportional to the N, where N is the 
   *     total number of relations 
   *
   *  @attention inverse interface !
   *  @see IRelation
   *  @see RelationBase
   *  @param  object  the object
   *  @return status code 
   */
  virtual  StatusCode    remove 
  ( const  IBase1::To&    object )
  {
    StatusCode              sc = m_base1.i_remove( object ) ;
    if( sc.isSuccess () ) { sc = m_base2.i_remove( object ) ; }
    return sc ;
  };
  
  /** query the interface 
   *  @see    IRelation
   *  @see    IInterface 
   *  @param  iid interface identifier 
   *  @param  ppi placeholder 
   *  @return status code
   */
  virtual StatusCode queryInterface( const InterfaceID& iID , void** ppI )
  {
    if( 0 == ppI  )          { return StatusCode::FAILURE ; }   // RETURN !!!
    if( IInterface::interfaceID() == iID ) 
      { *ppI = static_cast<IInterface*> ( this ); }
    else if( IBase1::interfaceID() == iID ) 
      { *ppI = static_cast<IBase1*>     ( this ); }
    else if( IBase2::interfaceID() == iID ) 
      { *ppI = static_cast<IBase2*>     ( this ); }
    else                     { return StatusCode::FAILURE ; }   //  RETURN !!!
    ///
    addRef() ;
    return StatusCode::SUCCESS ;
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

private:
  
  Base1 m_base1 ; ///< the actual store of direct  relations 
  Base2 m_base2 ; ///< the actual store of inverse relations 

};

// ============================================================================
// The End
// ============================================================================
#endif // RELATIONS_RELATION2D_H
// ============================================================================
