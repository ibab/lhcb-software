// $Id: Associator.h,v 1.2 2002-04-08 21:03:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/08 14:26:00  ibelyaev
//  new version of 'Relations'-subpackage
//
// ============================================================================
#ifndef RELATIONS_ASSOCIATOR_H 
#define RELATIONS_ASSOCIATOR_H 1
// Include files
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
  typedef  Associator<FROM,TO>   OwnType         ;
  /// shortcut for interface type 
  typedef IAssociator<FROM,TO>   IBase           ;
  
protected:
  
  /// shortcut for local representation of direct  table 
  typedef  Relation1D<FROM,TO>       LocalDirectType  ;
  /// shortcut for local representation of inverse table 
  typedef  Relation1D<TO,FROM>       LocalInverseType ;
  /// shortcut for possible 2D-representation
  typedef  IRelation2D<FROM,TO>      Direct2DType     ;
  
public:
  
  /// friend factory for instantiation
  friend ToolFactory<OwnType>;
  
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
  virtual const InverseType*  inverse  () const  
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
    // handle incident with base class 
    AssociatorBase::handle( incident ) ;
    // release reference tables 
    if( 0 != m_direct  ) { m_direct  -> release() ; m_direct  = 0 ; }
    if( 0 != m_inverse ) { m_inverse -> release() ; m_inverse = 0 ; }
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
    m_direct ->addRef () ;
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
        Warning("The 'inverse' relation table is created as temporary!");
        m_inverse = new LocalInverseType( *m_direct , 1 );
      }
    ///
    m_inverse -> addRef() ;
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
