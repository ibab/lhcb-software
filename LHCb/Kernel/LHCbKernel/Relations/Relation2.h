// $Id: Relation2.h,v 1.1 2002-04-03 15:35:17 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// =============================================================================
#ifndef RELATIONS_Relation2_H 
#define RELATIONS_Relation2_H 1
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
#include "Relations/IRelation2D.h"
#include "Relations/Relation.h"

namespace Relations
{
  /** @class Relation2 Relation2.h Relations/Relation2.h
   *  
   *  @brief Implementation of ordinary bidirectional relations 
   * 
   *  Implementation of ordinary bidirectional relations 
   *  from objects of type "FROM" to objects of type "TO".
   *  
   *  Data types for "FROM" and "TO":
   *
   *   - either inherits from ContainedObject/DataObject 
   *   - or have the templated specializations through 
   *     ObjectTypeTraits structure with defined streamer operator 
   *     to/from StreamBuffer class 
   *  
   *  @see ObjectTypeTraits 
   *  @see StreamBuffer 
   *  @see IRelation
   *  @warning for the current implementation the actual type of 
   *           FROM should differ from the actual type of TO
   *  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   23/03/2002
   */
  
  template<class FROM,class TO>
  class Relation2 :
    public virtual IRelation2D<FROM,TO> , 
    public          Relation  <FROM,TO>
  {  
  public:
    /// short cut for own     type
    typedef Relation2<FROM,TO>               OwnType        ;
    /// short cut for inverse type
    typedef Relation2<TO,FROM>               InvType        ;
    /// short cut for interface 
    typedef IRelation2D<FROM,TO>             IBase          ;
    /// actual implementations of direct  type 
    typedef Relation<FROM,TO>                Base           ;
    /// actual implementations of inverse type 
    typedef Relation<TO,FROM>                InvBase        ;
    /// shortcut for direct base type 
    typedef Base                             Direct         ;
    /// shortcut for inverse base type 
    typedef InvBase                          Inverse        ;
    /// shortcut for direct subinterface 
    typedef IBase::DirectType                DirectType     ;
    /// shortcut for inverse subinterface 
    typedef IBase::InverseType               InverseType     ;
    
  public:
    
    /** get the "direct" interface 
     *  @see IRelation2D
     *  @return pointer to the 'direct' interface 
     */
    virtual       DirectType*  direct ()        { return this       ; }

    /** get the "direct" interface  (const-version)
     *  @see IRelation2D
     *  @return pointer to the 'direct' interface 
     */
    virtual const DirectType*  direct () const  { return this       ; }

    /** get the "inverse" interface 
     *  @see IRelation2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual       InverseType* inverse ()       { return &m_inverse ; }
    
    /** get the "inverse" interface  (const version)
     *  @see IRelation2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual const InverseType* inverse () const { return &m_inverse ; }

    /// the default constructor
    Relation2( const size_t reserve = 0  )
      :  Direct( reserve ) , m_inverse( reserve ) 
    {
      /// set cross-links 
      Direct::setInverseBase( m_inverse.directBase () ) ;
      m_inverse.setInverseBase( Direct::directBase () );
    };
    
    /** constructor from "inverted object"
     *  @param copy object to be inverted
     *  @param int artificial argument to distinguish from copy constructor
     */
    Relation2 ( const InvType& inv   , int flag ) 
      : Direct    ( inv.m_inverse )
      , m_inverse ( inv           ) 
    {
      Direct::setInverseBase( m_inverse.directBase () ) ;
      m_inverse.setInverseBase( Direct::directBase () );
    };
    
    /// destructor (virtual)
    virtual ~Relation2() { clear() ; }
    
    /** query the interface
     *  @see    IRelation
     *  @see    IRelation2D
     *  @see    IInterface
     *  @param  id  interface identifier
     *  @param  ret placeholder for returned interface 
     *  @return status code
     */
    virtual StatusCode queryInterface
    ( const InterfaceID& id , void** ret )
    {
      if( 0 == ret  )          { return StatusCode::FAILURE ; } // RETURN !!!
      if( IInterface::interfaceID()             == id )
        { *ret = static_cast<IInterface*>        ( this ); }
      else if( IBase::interfaceID()             == id )
        { *ret = static_cast<IBase*>             ( this ); }
      else if( IBase::DirectType::interfaceID() == id )
        { *ret = static_cast<IBase::DirectType*> ( this ); }
      else { return Direct::queryInterface( id , ret ) ; } //  RETURN !!!
      ///
      addRef() ;
      return StatusCode::SUCCESS ;
    };

  private:
    
    /** copy constructor is private! 
     *  @param copy object to be copied 
     */
    Relation2 ( const OwnType& own     ) 
      : Direct    ( copy           )
      , m_inverse ( copy.m_inverse ) 
    {
      /// set cross-links 
      Direct::setInverseBase( m_inverse.directBase () ) ;
      m_inverse.setInverseBase( Direct::directBase () );
    };
    
    /** assignement operator is private!
     *  @param copy object to be copied 
     */
    Relation2& operator=( const OwnType& copy );

  private:
    
    Inverse   m_inverse ;  ///< the "inverse table"
    
  };
  
  
}; // end of namespace Relations

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_Relation2_H
// ============================================================================
