// $Id: AssociatorWeighted.h,v 1.2 2004-11-19 15:01:22 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_AssociatorWeighted_H 
#define RELATIONS_AssociatorWeighted_H 1
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

  /// export type from interface 
  typedef typename IBase::DirectType  DirectType  ;
  /// export type from interface 
  typedef typename IBase::InverseType InverseType ;
  /// export type from interface 
  typedef typename IBase::From        From        ;
  /// export type from interface 
  typedef typename IBase::To          To          ;
  /// export type from interface 
  typedef typename IBase::Weight      Weight      ;
  /// export type from interface 
  typedef typename IBase::FromRange   FromRange   ;
  /// export type from interface 
  typedef typename IBase::ToRange     ToRange     ;
  
protected:
  
  /// shortcut for local representation of direct  table 
  typedef  RelationWeighted1D<FROM,TO,WEIGHT>  LocalDirectType  ;
  /// shortcut for local representation of inverse table 
  typedef  RelationWeighted1D<TO,FROM,WEIGHT>  LocalInverseType ;
  /// shortcut for possible 2D-representation
  typedef  IRelationWeighted2D<FROM,TO,WEIGHT> Direct2DType     ;
  
public:
  
  /// friend factory for instantiation
  friend class ToolFactory<AssociatorWeighted<FROM,TO,WEIGHT> >;
  
public:
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object) (non const version)
   *  @see IAssociatorWeighted
   *  @see IRelationWeighted
   *  @return pointer to "direct" relation table 
   */
  virtual       DirectType* direct ()
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
  virtual const DirectType* direct () const  
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
  virtual       InverseType* inverse ()
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
  virtual const InverseType* inverse () const  
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

  /** Method to retrieve a range associated to a given FROM element
   *  
   *  @param from        object one want to retrieve 
   *                     associated range
   *  @param range       A range of associated objects. 
   *                     It is empty if no table was found
   *  @return Failure it no table was found
   */
  virtual StatusCode rangeFrom
  ( const From& from  , 
    ToRange&    range ) const 
  {
    const DirectType* table = direct();
    if( 0 == table ) 
    {
      range = ToRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( from );
    return StatusCode::SUCCESS;
  };

  /** Method to retrieve a range associated to a given TO element
   *
   *  @param to    object one want to retrieve associated range
   *  @param range A range of associated objects. 
   *                 It is empty if no table was found
   *  @return Failure it no table was found
   */
  virtual StatusCode rangeTo
  ( const To&  to    ,
    FromRange& range ) const
  {
    const InverseType* table = inverse();
    if( 0 == table ) 
    {
      range = FromRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( to );
    return StatusCode::SUCCESS;
  };
  
  /** Method to retrieve a range associated to a given FROM element
   *
   *  @param  from object one want to retrieve associated range
   *  @return A range of associated objects. It is empty if no table was found
   */
  virtual ToRange rangeFrom
  ( const From& from) const 
  {
    const DirectType* table = direct();
    if ( 0 == table ) { return ToRange() ; }
    return table->relations( from );
  };

  /** Method to retrieve a range associated to a given TO element
   *
   *  @param  to Poobject one want to retrieve associated range
   *  @return A range of associated objects. 
   *               It is empty if no table was found
   */
  virtual FromRange rangeTo
  ( const To& to ) const
  {
    const InverseType* table = inverse();
    if ( 0 == table ) { return FromRange() ; }
    return table->relations( to );
  };
  
  
  /** Method to retrieve a range associated to a given FROM element which 
   *  have a weight larger than a given threshold
   *
   *  @param from      object one want to retrieve associated range
   *  @param threshold Threshold on which to cut
   *  @param range     A range of associated objects. 
   *                   It is empty if no table was found
   *  @return Failure it no table was found
   */
  virtual StatusCode rangeWithLowCutFrom
  ( const From&   from      ,
    const Weight& threshold ,
    ToRange&      range     ) const
  {
    const DirectType* table = direct();
    if( 0 == table ) 
    {
      range = ToRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( from , threshold , true );
    return StatusCode::SUCCESS;
  };

  /** Method to retrieve a range associated to a given TO element which
   *  have a weight larger than a given threshold
   *
   *  @param to        object one want to retrieve associated range
   *  @param threshold Weight threshold
   *  @param range     A range of associated objects. 
   *                   It is empty if no table was found
   *  @return Failure it no table was found
   */
  virtual StatusCode rangeWithLowCutTo
  ( const To&     to        ,
    const Weight& threshold ,
    FromRange&    range     ) const
  {
    const InverseType* table = inverse();
    if( 0 == table ) 
    {
      range = FromRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( to, threshold, true );
    return StatusCode::SUCCESS;
  };
  
  /** Method to retrieve a range associated to a given FROM element which 
   *  have a weight smaller than a given threshold
   *
   *  @param from   object one want to retrieve associated range
   *  @param range  A range of associated objects. 
   *                It is empty if no table was found
   *  @param threshold Threshold on which to cut
   *  @return Failure it no table was found
   */
  virtual StatusCode rangeWithHighCutFrom
  ( const From&   from      , 
    const Weight& threshold ,
    ToRange&      range     ) const
  {
    const DirectType* table = direct();
    if( 0 == table ) 
    {
      range = ToRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( from , threshold , false );
    return StatusCode::SUCCESS;
  };
  
  /** Method to retrieve a range associated to a given TO element which
   *  have a weight smaller than a given threshold
   *
   *  @param to       object one want to retrieve associated range
   *  @param threshold   Weight threshold
   *  @param range       A range of associated objects. 
   *                     It is empty if no table was found
   *  @return Failure it no table was found
   */
  virtual StatusCode rangeWithHighCutTo
  ( const To&     to        , 
    const Weight& threshold , 
    FromRange&    range     ) const
  {
    const InverseType* table = inverse();
    if( 0 == table ) 
    {
      range = FromRange() ;
      return StatusCode::FAILURE;
    }
    range = table->relations( to, threshold, false );
    return StatusCode::SUCCESS;
  };
  
  // Methods with cuts and no StatusCode
  
  /** Method to retrieve a range associated to a given FROM element which 
   *  have a weight larger than a given threshold
   *
   *  @param from     object one want to retrieve associated range
   *  @param threshold Threshold on which to cut
   *  @return  A range of associated objects. It is empty if no table was found
   */
  virtual ToRange rangeWithLowCutFrom
  ( const From&   from      , 
    const Weight& threshold ) const
  {
    const DirectType* table = direct();
    if( 0 == table ) { return ToRange() ; }
    return table->relations( from, threshold, true );
  };
  
  /** Method to retrieve a range associated to a given TO element which
   *  have a weight larger than a given threshold
   *
   *  @param to        object one want to retrieve associated range
   *  @param threshold Weight threshold
   *  @return A range of associated objects. It is empty if no table was found
   */
  virtual FromRange rangeWithLowCutTo
  ( const To&     to        ,
    const Weight& threshold ) const
  {
    const InverseType* table = inverse();
    if( 0 == table ) { return FromRange() ; }
    return table->relations( to, threshold, true );
  };
  
  /** Method to retrieve a range associated to a given FROM element which 
   *  have a weight smaller than a given threshold
   *
   *  @param from    object one want to retrieve associated range
   *  @param threshold Threshold on which to cut
   *  @return  A range of associated objects. It is empty if no table was found
   */
  virtual ToRange rangeWithHighCutFrom
  ( const From&   from      ,
    const Weight& threshold ) const
  {
    const DirectType* table = direct();
    if( 0 == table ) { return ToRange() ; }
    return table->relations( from, threshold, false );
  };
  
  /** Method to retrieve a range associated to a given TO element which
   *  have a weight smaller than a given threshold
   *
   *  @param to        object one want to retrieve associated range
   *  @param threshold Weight threshold
   *  @return A range of associated objects. It is empty if no table was found
   */
  virtual FromRange rangeWithHighCutTo
  ( const To& to,
    const Weight& threshold ) const
  {
    const InverseType* table = inverse();
    if( 0 == table ) { return FromRange() ; }
    return table->relations( to, threshold, false );
  };
  
  /** Method to retrieve a single element associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated (first) element
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual To associatedFrom
  ( const From& from ) const
  {
    const DirectType* table = direct();
    if ( 0 == table    ) { return To () ; }
    ToRange range = table->relations( from );
    if ( range.empty() ) { return To () ; }
    if ( !m_decreasing ) return range.begin()->to();
    return range.rbegin()->to();
  };

  /** Method to retrieve a single element associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated (first) element
   *  @param weight The weight it is associated with
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual To associatedFrom
  ( const From& from, 
    Weight&     weight ) const
  {
    const DirectType* table = direct();
    if ( 0 != table) 
    {
      ToRange range = table->relations( from );
      if( !range.empty() ) 
      {
        if( !m_decreasing ) 
        {
          weight = range.begin()->weight();
          return range.begin()->to();
        } 
        else 
        {
          weight = range.rbegin()->weight();
          return range.rbegin()->to();
        }
      }
    }
    return To();
  };

  /** Method to retrieve a single element associated to a given TO element
   *
   *  @param to  'TO' object one want to retrieve associated (first) element
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual From associatedTo
  ( const To& to) const
  {
    const InverseType* table = inverse();
    if ( 0 == table    ) { return From () ; } 
    FromRange range = table->relations( to );
    if ( range.empty() ) { return From () ; }
    if( !m_decreasing ) return range.begin()->to();
    return range.rbegin()->to();
  };
  
  /** Method to retrieve a single element associated to a given TO element
   *
   *  @param to  'TO' object one want to retrieve associated (first) element
   *  @param weight The weight it is associated with
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual From associatedTo
  ( const To&  to,
    Weight&    weight ) const
  {
    const InverseType* table = inverse();
    if ( 0 != table) 
    {
      FromRange range = table->relations( to );
      if( !range.empty() ) 
      {
        if( !m_decreasing ) 
        {
          weight = range.begin()->weight();
          return range.begin()->to();
        } 
        else 
        {
          weight = range.rbegin()->weight();
          return range.rbegin()->to();
        }
      }
    }
    return From();
  };

  /* Method to test if the table does exist or not
   * @return true if direct relation table is available 
   */
  virtual bool tableExists() const
  {
    return 0 == direct() ? false : true;
  };
  
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
  
protected:
  
  /** locate/build direct relation interface 
   *  locate the object through RelaitonToolBase::object() method
   *  @see RelationToolBase
   *  @return status code  
   */
  virtual StatusCode i_direct() const 
  {
    // exists?
    if( 0 != m_direct ) { return StatusCode::SUCCESS ; }
    m_direct = get<DirectType>( location() ) ;
    addRef ( m_direct ) ;
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
        // Warning("The 'inverse' relation table is created as temporary!");
        m_inverse = new LocalInverseType( *m_direct , 1 );
      }
    ///
    addRef ( m_inverse ) ;
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
    , m_decreasing( false )
  {
    /// add the interface 
    declareInterface<IBase>             ( this );
    declareProperty( "DecreasingWeight", m_decreasing);
  };
  
  /// destructor (virtual and protected)
  virtual ~AssociatorWeighted(){};
  
  void setDecreasing( bool val ) 
  { m_decreasing = val ; }
  
private:
  
  // relations 
  mutable DirectType*   m_direct;
  mutable InverseType*  m_inverse;
  bool m_decreasing;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_AssociatorWeighted_H
// ============================================================================
