// $Id: AssociatorWeighted.h,v 1.2 2002-04-08 21:03:08 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/08 14:26:01  ibelyaev
//  new version of 'Relations'-subpackage
//
// ============================================================================
#ifndef RELATIONS_AssociatorWeighted_H 
#define RELATIONS_AssociatorWeighted_H 1
// Include files
// from STL
#include <string>
// GaudiKernel
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAlgorithm.h"
// Relations
#include "Relations/AssociatorBase.h"
#include "Relations/RelationWeighted1D.h"
#include "Relations/IRelationWeighted2D.h"
#include "Relations/IAssociatorWeighted.h"

/** @class AssociatorWeighted AssociatorWeighted.h 
 * 
 *  Generic relation tool for weighted relations 
 *  @see IAssociatorWeighted
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */

template <class FROM , class TO , class WEIGHT>
class AssociatorWeighted : 
  public virtual   IAssociatorWeighted<FROM,TO,WEIGHT> , 
  public Relations::AssociatorBase                     
{
  
public:
  
  /// shortcut for own type 
  typedef  AssociatorWeighted<FROM,TO,WEIGHT>   OwnType         ;
  /// shortcut for interface type 
  typedef IAssociatorWeighted<FROM,TO,WEIGHT>   IBase           ;
  
protected:
  
  /// shortcut for local representation of direct  table 
  typedef  RelationWeighted1D<FROM,TO,WEIGHT>  LocalDirectType  ;
  /// shortcut for local representation of inverse table 
  typedef  RelationWeighted1D<TO,FROM,WEIGHT>  LocalInverseType ;
  /// shortcut for possible 2D-representation
  typedef  IRelationWeighted2D<FROM,TO,WEIGHT> Direct2DType     ;
  
public:
  
  /// friend factory for instantiation
  friend ToolFactory<OwnType>;
  
public:
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object) (non const version)
   *  @see IAssociatorWeighted
   *  @see IRelationWeighted
   *  @return pointer to "direct" relation table 
   */
   virtual       DirectType*   direct  ()   
  {
    if( 0 == m_direct  ) { i_direct  () ; }
    return m_direct;
  };
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object) (const version)
   *  @see IAssociatorWeighted
   *  @see IRelationWeighted
   *  @return pointer to "direct" relation table 
   */
  virtual const DirectType*   direct  () const  
  {
    if( 0 == m_direct  ) { i_direct  () ; }
    return m_direct;
  };

  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (non const version)
   *  @see IAssociatorWeighted
   *  @see IRelationWeighted
   *  @return pointer to "inverse" relation table 
   */
  virtual       InverseType*  inverse ()   
  {
    if( 0 == m_inverse ) { i_inverse () ; }
    return m_inverse ;
  };
  
  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (const version)
   *  @see IAssociatorWeighted
   *  @see IRelationWeighted
   *  @return pointer to "inverse" relation table 
   */
  virtual const InverseType*  inverse () const  
  {
    if( 0 == m_inverse ) { i_inverse () ; }
    return m_inverse ;
  };
  
  /** accessor to relation builder algorithm
   *  @see IAssociatorWeighted
   *  @see  AssociatorBase
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
    AssociatorBase::handle( incident ) ;
    if( 0 != m_direct  ) { m_direct  -> release() ; m_direct  = 0 ; }
    if( 0 != m_inverse ) { m_inverse -> release() ; m_inverse = 0 ; }
  };
  
protected:
  
  /** locate/build direct relation interface 
   *  locate the object through RelaitonToolBase::object() method
   *  @see RelationToolBase
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
   *  @see RelationToolBase
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
   *  @see AssociatorBase 
   *  @see IAssociatorWeighted 
   *  @see  AlgTool
   *  @see IAlgTool
   *  @param type   type   of the tool (?)
   *  @param name   name   of the tool 
   *  @param parent parent of the tool
   */
  AssociatorWeighted
  ( const std::string&  type   , 
    const std::string&  name   ,
    const IInterface*   parent )
    : AssociatorBase   ( type , name , parent )
    , m_direct  ( 0 ) 
    , m_inverse ( 0 ) 
  {
    /// add the interface 
    declareInterface<IBase>             ( this );
  };
  
  /// destructor (virtual and protected)
  virtual ~AssociatorWeighted(){};
  
private:
  
  // relations 
  mutable DirectType*   m_direct       ;
  mutable InverseType*  m_inverse      ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_AssociatorWeighted_H
// ============================================================================
