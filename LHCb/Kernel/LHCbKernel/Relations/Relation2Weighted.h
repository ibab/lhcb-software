// $Id: Relation2Weighted.h,v 1.1 2002-04-03 15:35:18 ibelyaev Exp $
// =============================================================================
// CV Stag $Name: not supported by cvs2svn $
// =============================================================================
// $Log: not supported by cvs2svn $
// =============================================================================
#ifndef RELATIONS_Relation2Weighted_H 
#define RELATIONS_Relation2Weighted_H 1
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
#include "Relations/IRelationWeighted2D.h"
#include "Relations/RelationWeighted.h"

namespace Relations
{
  
  /** @class Relation2Weighted Relation2Weighted.h Relations/Relation2Weighted.h
   *  
   *  @brief Implementation of weighted bidirectional relations 
   * 
   *  Implementation of weighed bidirectional relations 
   *  from objects of type "FROM" to objects of type "TO" withweight "WEIGHT"
   *  
   *  Data types for "FROM", "TO" and "WEIGHT"
   *
   *   - either inherits from ContainedObject/DataObject 
   *   - or have the templated specializations through 
   *     ObjectTypeTraits structure with defined streamer operator 
   *     to/from StreamBuffer class 
   *  
   *  @see ObjectTypeTraits 
   *  @see StreamBuffer 
   *  @see IRelationWeihted
   *  @see IRelationWeihted2D
   *  @warning for the current implementation the actual type of 
   *           FROM should differ from the actual type of TO
   *  
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
   *  @date   23/03/2002
   */
  
  template<class FROM,class TO,class WEIGHT>
  class Relation2Weighted:
    public virtual IRelationWeighted2D<FROM,TO,WEIGHT> , 
    public          RelationWeighted  <FROM,TO,WEIGHT>
  {  
  public:
    /// short cut for own     type
    typedef Relation2Weighted<FROM,TO,FROM>      OwnType        ;
    /// short cut for inverse type
    typedef Relation2Weighted<TO,FROM,FROM>      InvType        ;
    /// short cut for interface 
    typedef IRelationWeighted2D<FROM,TO,WEIGHT>  IBase          ;
    /// actual implementations of direct  type 
    typedef RelationWeighted<FROM,TO,WEIGHT>     Base           ;
    /// actual implementations of inverse type 
    typedef RelationWeighted<TO,FROM,WEIGHT>     InvBase        ;
    /// shortcut for direct base type 
    typedef Base                                 Direct         ;
    /// shortcut for inverse base type 
    typedef InvBase                              Inverse        ;
    /// shortcut for direct subinterface 
    typedef IBase::DirectType                    DirectType     ;
    /// shortcut for inverse subinterface 
    typedef IBase::InverseType                   InverseType     ;

    /** get the "direct" interface 
     *  @see IRelationWeighted2D
     *  @return pointer to the 'direct' interface 
     */
    virtual       DirectType*  direct ()        { return this       ; }

    /** get the "direct" interface  (const-version)
     *  @see IRelationWeighted2D
     *  @return pointer to the 'direct' interface 
     */
    virtual const DirectType*  direct () const  { return this       ; }

    /** get the "inverse" interface 
     *  @see IRelationWeighted2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual       InverseType* inverse ()       { return &m_inverse ; }
    
    /** get the "inverse" interface  (const version)
     *  @see IRelationWeighted2D
     *  @return pointer to the 'inverse' interface 
     */
    virtual const InverseType* inverse () const { return &m_inverse ; }
    
    /// the default constructor
    Relation2Weighted( const size_t reserve = 0  )
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
    Relation2Weighted ( const InvType& inv   , int flag ) 
      : Direct    ( inv.m_inverse )
      , m_inverse ( inv           ) 
    {
      /// set cross-links 
      Direct::setInverseBase( m_inverse.directBase () ) ;
      m_inverse.setInverseBase( Direct::directBase () );
    };
    
    /// destructor (virtual)
    virtual ~Relation2Weighted() { clear() ; }
    
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
    Relation2Weighted ( const OwnType& own     ) 
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
    Relation2Weighted& operator=( const OwnType& copy );
    
  private:
    
    Inverse   m_inverse ;  ///< the "inverse table"
    
  };
  
};



// =============================================================================
// The End 
// =============================================================================
#endif // RELATIONS_RELation2Weighted_H
// =============================================================================
