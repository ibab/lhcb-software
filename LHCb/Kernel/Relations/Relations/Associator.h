// $Id: Associator.h,v 1.1.1.1 2004-07-21 07:57:25 cattanem Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.11  2003/06/25 14:59:01  ibelyaev
//  Modifications in Relations-subpackage
//
// Revision 1.10  2003/01/22 11:29:16  sponce
// makes gcc 3.2 modifications compile under windows
//
// Revision 1.9  2003/01/17 14:07:01  sponce
// support for gcc 3.2
//
// Revision 1.8  2002/05/24 18:36:31  ibelyaev
//  see $LHCBKERNELROOT/doc/release.notes
//
// Revision 1.7  2002/05/15 14:43:28  phicharp
// Make name of associators' methods consistent
//
// Revision 1.6  2002/05/13 15:54:19  ibelyaev
//  bug fix
//
// Revision 1.5  2002/05/13 09:48:25  phicharp
// Add methods associatedFrom() and To() for single objects
//
// Revision 1.4  2002/05/12 09:58:01  ibelyaev
//  see $LHCBKERNELROOT/doc/releae.notes 12 May 2002
//
// ============================================================================
#ifndef RELATIONS_ASSOCIATOR_H 
#define RELATIONS_ASSOCIATOR_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// from STL
#include <string>
// GaudiKernel
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgorithm.h"
// Relations
#include "Relations/AssociatorBase.h"
#include "Relations/Relation1D.h"
#include "Relations/IRelation2D.h"
#include "Relations/IAssociator.h"

/** @class Associator Associator.h Relations/Associator.h
 *  
 *  Implementation of generic relation tool 
 *  @see IAssociator 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */

template <class FROM,class TO>
class Associator : 
  public virtual   IAssociator<FROM,TO> , 
  public Relations::AssociatorBase       
{
  
public:
  
  /// shortcut for own type 
  typedef  Associator<FROM,TO>   OwnType          ;
  /// shortcut for interface type 
  typedef IAssociator<FROM,TO>   IBase            ;

  /// export type from interface 
  typedef typename IBase::DirectType  DirectType  ;
  /// export type from interface 
  typedef typename IBase::InverseType InverseType ;
  /// export type from interface 
  typedef typename IBase::From        From        ;
  /// export type from interface 
  typedef typename IBase::To          To          ;
  /// export type from interface 
  typedef typename IBase::FromRange   FromRange   ;
  /// export type from interface 
  typedef typename IBase::ToRange     ToRange     ;
  
protected:
  
  /// shortcut for local representation of direct  table 
  typedef  Relation1D<FROM,TO>       LocalDirectType  ;
  /// shortcut for local representation of inverse table 
  typedef  Relation1D<TO,FROM>       LocalInverseType ;
  /// shortcut for possible 2D-representation
  typedef  IRelation2D<FROM,TO>      Direct2DType     ;
  
public:
  
  /// friend factory for instantiation
  friend class ToolFactory<Associator<FROM,TO> >;
  
public:
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)  (non const version)
   *  @see IAssociator
   *  @see IRelation
   *  @return pointer to "direct" relation table 
   */
   virtual       DirectType*   direct  ()   
  {
    if( 0 == m_direct  ) { i_direct  () ; }
    return m_direct;
  };
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object) (const version)
   *  @see IAssociator
   *  @see IRelation
   *  @return pointer to "direct" relation table 
   */
  virtual const DirectType*   direct  () const  
  {
    if( 0 == m_direct  ) { i_direct  () ; }
    return m_direct;
  };

  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (non const version)
   *  @see IAssociator
   *  @see IRelation
   *  @return pointer to "inverse" relation table 
   */
  virtual       InverseType*  inverse ()   
  {
    if( 0 == m_inverse ) { i_inverse () ; }
    return m_inverse ;
  };
  
  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (const version)
   *  @see IAssociator
   *  @see IRelation
   *  @return pointer to "inverse" relation table 
   */
  virtual const InverseType* inverse  () const  
  {
    if( 0 == m_inverse ) { i_inverse () ; }
    return m_inverse ;
  };
  
  /** accessor to relation builder algorithm
   *  @see IRelation
   *  @see IAssociator
   *  @see  AssociatorBase
   *  @see IAlgorithm 
   *  @return pointer to builder algorithm
   */
  virtual IAlgorithm*         algorithm () const 
  { return AssociatorBase::algorithm () ; }
  
  /** handle the incident
   *  @see  AssociatorBase
   *  @see IIncidentListener 
   *  @see Incident 
   *  @see incident incident to be handled 
   */
  virtual void handle( const Incident& incident ) 
  {
    // release reference tables 
    release ( m_direct  ) ; m_direct  = 0 ;
    // release reference tables 
    release ( m_inverse ) ; m_inverse = 0 ;
    // handle incident with base class 
    AssociatorBase::handle( incident ) ;
  };
  
  /** Method to retrieve a range associated to a given FROM element
   *  @see IAssociator 
   *  @param from  'FROM' object one want to retrieve associated 
   *              range
   *  @param range range of associated objects. 
   *  It is empty if no table was found
   *  @return StatusCode Failure it no table was found
   */
  virtual StatusCode rangeFrom
  ( const  From& from  , 
    ToRange&     range ) const 
  {
    const DirectType* table = direct();
    if( 0 == table ) {
      range = ToRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( from );
    return StatusCode::SUCCESS;
  };
  
  /** Method to retrieve a range associated to a given TO element
   *  @see IAssociator 
   *
   *  @param to     'TO' object one want to retrieve associated range
   *  @param range  range of associated objects. 
   *  It is empty if no table was found
   *  @return StatusCode Failure it no table was found
   */
  virtual StatusCode rangeTo
  ( const To&  to    , 
    FromRange& range ) const 
  {
    const InverseType* table = inverse();
    if( 0 == table ) {
      range = InvTable::Range() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( to );
    return StatusCode::SUCCESS;
  };
  
  /** Method to retrieve a range associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated range
   *  @return range A range of associated objects. 
   *  It is empty if no table was found
   *  
   */
  virtual ToRange rangeFrom
  ( const From& from  ) const 
  {
    const DirectType* table = direct();
    if ( 0 == table ) { return ToRange () ; }
    return table->relations( from );
  };
  
  /** Method to retrieve a range associated to a given TO element
   *  @see IAssociator   
   *  @param  to     'TO' object one want to retrieve associated range
   *  @return range  range of associated objects. 
   *  It is empty if no table was found
   */
  virtual FromRange rangeTo
  ( const To& to ) const 
  {
    const InverseType* table = inverse();
    if( 0 == table ) { return FromRange() ; }
    return table->relations( to );
  };
  
  /** Method to retrieve a single element associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated (first) element
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual To    associatedFrom
  ( const From& from  ) const 
  {
    const DirectType* table = direct();
    if ( 0 == table    ) { return To () ; }
    ToRange range = table->relations( from );
    if ( range.empty() ) { return To () ; }
    return range.begin()->to() ;
  };
  
  /** Method to retrieve a single element associated to a given TO element
   *
   *  @param to  'TO' object one want to retrieve associated (first) element
   *  @return from The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual From associatedTo
  ( const To& to  ) const 
  {
    const InverseType* table = inverse();
    if ( 0 == table    ) { return From () ; }
    FromRange range = table->relations( to );
    if ( range.empty() ) { return From () ; }
    return range.begin()->to();
  };
  
  /* Method to test if the table does exist or not
   * @see IAssociator 
   * @return true if the associator contains valid relation table 
   */
  virtual bool tableExists() const 
  {
    return 0 == direct() ? false : true ;
  };
  
protected:
  
  /** locate/build direct relation interface 
   *  locate the object through RelaitonToolBase::object() method
   *  @see  AssociatorBase
   *  @return status code  
   */
  virtual StatusCode i_direct() const 
  {
    // exists?
    if( 0 != m_direct ) { return StatusCode::SUCCESS                     ; }
    IInterface* obj = object () ;
    if( 0 == obj      ) { return Error("'Object' is not located/built!") ; }
    SmartIF<DirectType> aux ( DirectType::interfaceID() , obj );
    m_direct = aux ;
    if( 0 == m_direct ) { return Error("'DirectType' points to NULL!"  ) ; }
    addRef ( m_direct ) ;
    return StatusCode::SUCCESS ;
  };
  
  /** locate/build inverse relation interface 
   *  @see AssociatorBase
   *  @return status code  
   */
  virtual StatusCode i_inverse() const 
  {
    // exists? 
    if( 0 != m_inverse ) { return StatusCode::SUCCESS ; }
    // locate direct
    StatusCode sc = i_direct() ;
    if( sc.isFailure() ) { return Error("'DirectType' is invalid!" , sc ) ; }
    if( 0 == m_direct  ) { return Error("'DirectType' points to  NULL!" ) ; }
    // check the availability of 2D-direct  interface
    SmartIF<Direct2DType> aux( Direct2DType::interfaceID() , m_direct );
    Direct2DType* tmp = aux ;
    // use 2D relations or build inverse!
    if( 0 != tmp ) { m_inverse = tmp -> inverse () ; }
    else 
      {    
        // Warning("The 'inverse' relation table is created as temporary!");
        m_inverse = new LocalInverseType( *m_direct , 1 );
      }
    ///
    addRef( m_inverse ) ;
    ///
    return StatusCode::SUCCESS ;
  };
  
  /** Locate or build the relation table 
   *  @attention the 'default' behaviour of the method
   *  could be redefined for specific relations
   *  @see AssociatorBase::locateOrBuild
   *  @see AssociatorBase
   *  @return StatusCode
   */
  virtual StatusCode locateOrBuild   () const
  { return AssociatorBase::locateOrBuild () ; }
  
protected:
  
  /** Standard constructor
   *  @see  AssociatorBase
   *  @see IAssociator 
   *  @see  AlgTool
   *  @see IAlgTool
   *  @param type   type   of the tool (?)
   *  @param name   name   of the tool 
   *  @param parent parent of the tool
   */
  Associator
  ( const std::string&  type   , 
    const std::string&  name   ,
    const IInterface*   parent )
    : AssociatorBase   ( type , name , parent )
    , m_direct  ( 0 ) 
    , m_inverse ( 0 ) 
  {
    /// add the interfaces 
    declareInterface<IBase>             ( this );
  };
  
  /// destructor (virtual and protected)
  virtual ~Associator(){};
  
private:
  
  ///  default  constructor  is  private 
  Associator() ;
  ///  copy     constructor  is  private 
  Associator
  ( const Associator& );
  ///  assignement operator  is  private 
  Associator& operator=
  ( const Associator& );  
  
private:
  
  // relations 
  mutable DirectType*   m_direct   ;
  mutable InverseType*  m_inverse  ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_ASSOCIATOR_H
// ============================================================================
