// $Id: IAssociatorWeighted.h,v 1.8 2002-05-15 14:43:30 phicharp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/13 09:48:26  phicharp
// Add methods associatedFrom() and To() for single objects
//
// Revision 1.6  2002/05/12 09:58:02  ibelyaev
//  see $LHCBKERNELROOT/doc/releae.notes 12 May 2002
//
// Revision 1.5  2002/05/10 14:49:40  phicharp
// Add retrieve methods to IAssociatorxx
// ============================================================================
#ifndef RELATIONS_IAssociatorWeighted_H 
#define RELATIONS_IAssociatorWeighted_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Relations 
#include "Relations/IRelationWeighted.h"
// forward decalrations 
class    IAlgorithm ;   // from GaudiKernel 

/** @class IAssociatorWeighted IAssociatorWeighted.h
 *  
 *  Generic interface (tool) to access the weighted 
 *  relations from Transient Store 
 *  @see IRelationWeighted
 *  @see IRelationWeighted2D
 *  @see IAssociator
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep
 *  @date   24/03/2002
 */

template<class FROM,class TO,class WEIGHT>
class IAssociatorWeighted:
  public virtual IAlgTool
{
public:
  
  /// shorcut for "own"  type 
  typedef IAssociatorWeighted<FROM,TO,WEIGHT>   OwnType       ;
  /// shortcut for "direct relations
  typedef IRelationWeighted<FROM,TO,WEIGHT>     DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelationWeighted<TO,FROM,WEIGHT>     InverseType   ;
  /// definition of "From"   type ("FROM"   and "From"   could be different)
  typedef DirectType::From        From          ;
  /// definition of "To"     type ("TO"     and "To"     could be different)
  typedef DirectType::To          To            ;
  /// definition of "Weight" type ("WEIGHT" and "Weight" could be different)
  typedef DirectType::Weight      Weight        ;
  
  /// another set of shortcuts may be easier to remember (PhC)
  typedef OwnType                             IAsct;
  typedef DirectType                          Table;
  typedef Table::Range                        ToRange;
  typedef Table::iterator                     ToIterator;
  typedef OwnType::InverseType                InvTable;
  typedef InvTable::Range                     FromRange;
  typedef InvTable::iterator                  FromIterator;

public:
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)   (non const version)
   *  @see IRelationWeighted
   *  @return pointer to "direct" relation table 
   */
  virtual       DirectType*   direct  ()       = 0 ;
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)   (const version)
   *  @see IRelationWeighted
   *  @return pointer to "direct" relation table 
   */
  virtual const DirectType*   direct  () const = 0 ;

  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object)   (non const version)
   *  @see IRelationWeighted
   *  @return pointer to "inverse" relation table 
   */
  virtual       InverseType*  inverse ()       = 0 ;
  
  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object)   (const version)
   *  @see IRelationWeighted
   *  @return pointer to "inverse" relation table 
   */
  virtual const InverseType*  inverse () const = 0 ;
  
  /** accessor to relation builder algorithm
   *  @see IAlgorithm
   *  @return pointer to builder algorithm
   */
  virtual IAlgorithm*         algorithm () const = 0 ;

  /** Method to retrieve a range associated to a given FROM element
   *
   *  @param from        object one want to retrieve associated range
   *  @param range       A range of associated objects. 
   *                     It is empty if no table was found
   *  @return Failure it no table was found
   *
   */
  virtual StatusCode rangeFrom
  ( const From&      from  , 
    ToRange&         range ) const = 0 ;
  
  /** Method to retrieve a range associated to a given TO element
   *
   *  @param to object one want to retrieve associated range
   *  @param range A range of associated objects. 
   *                        It is empty if no table was found
   *  @return Failure it no table was found
   */
  virtual StatusCode rangeTo
  ( const To&        to    , 
    FromRange&       range ) const = 0 ;
  
  /** Method to retrieve a range associated to a given FROM element
   *
   *  @param  from object one want to retrieve associated range
   *  @return A range of associated objects. It is empty if no table was found
   */
  virtual ToRange    rangeFrom
  ( const From&      from  ) const = 0 ;
  
  /** Method to retrieve a range associated to a given TO element
   *
   *  @param to object one want to retrieve associated range
   *  @return A range of associated objects. It is empty if no table was found
   */
  virtual FromRange  rangeTo
  ( const To&        to    ) const = 0 ;
  
  // Methods with thresholds....
  
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
  ( const From&      from      , 
    const Weight&    threshold ,
    ToRange&         range     ) const = 0 ;
  
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
  ( const To&        to        , 
    const Weight&    threshold , 
    FromRange&       range     ) const = 0 ;
  
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
  ( const From&      from      , 
    const Weight&    threshold ,
    ToRange&         range     ) const = 0 ;
  
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
  ( const To&        to        , 
    const Weight&    threshold , 
    FromRange&       range     ) const = 0 ;

  // Methods with cuts and no StatusCode
  
  /** Method to retrieve a range associated to a given FROM element which 
   *  have a weight larger than a given threshold
   *
   *  @param from     object one want to retrieve associated range
   *  @param threshold Threshold on which to cut
   *  @return  A range of associated objects. It is empty if no table was found
   */
  virtual ToRange    rangeWithLowCutFrom 
  ( const From&      from      , 
    const Weight&    threshold ) const = 0 ;

  /** Method to retrieve a range associated to a given TO element which
   *  have a weight larger than a given threshold
   *
   *  @param to        object one want to retrieve associated range
   *  @param threshold Weight threshold
   *  @return A range of associated objects. It is empty if no table was found
   */
  virtual FromRange  rangeWithLowCutTo
  ( const To&        to        , 
    const Weight&    threshold ) const = 0 ;
  
  /** Method to retrieve a range associated to a given FROM element which 
   *  have a weight smaller than a given threshold
   *
   *  @param from    object one want to retrieve associated range
   *  @param threshold Threshold on which to cut
   *  @return  A range of associated objects. It is empty if no table was found
   */
  virtual ToRange    rangeWithHighCutFrom
  ( const From&      from      , 
    const Weight&    threshold ) const = 0 ;

  /** Method to retrieve a range associated to a given TO element which
   *  have a weight smaller than a given threshold
   *
   *  @param to        object one want to retrieve associated range
   *  @param threshold Weight threshold
   *  @return A range of associated objects. It is empty if no table was found
   */
  virtual FromRange  rangeWithHighCutTo
  ( const To&        to        , 
    const Weight&    threshold ) const = 0 ;
  
  /** Method to retrieve a single element associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated (first) element
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual To         associatedFrom 
  ( const From&      from  ) const = 0 ;

  /** Method to retrieve a single element associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated (first) element
   *  @param weight The weight it is associated with
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual To         associatedFrom 
  ( const From&      from , 
    Weight&          weight  ) const = 0 ;

  /** Method to retrieve a single element associated to a given TO element
   *
   *  @param to  'TO' object one want to retrieve associated (first) element
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual From       associatedTo
  ( const To&        from  ) const = 0 ;

  /** Method to retrieve a single element associated to a given TO element
   *
   *  @param to  'TO' object one want to retrieve associated (first) element
   *  @param weight The weight it is associated with
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual From       associatedTo
  ( const To&        to , 
    Weight&          weight  ) const = 0 ;

  /* Method to test if the table does exist or not
   * @return true if direct relation table is available 
   */
  virtual bool tableExists() const = 0 ;
  
  /** interface identification (static)
   *  @attention the unique interface identifier is constructed "on-fly"
   *  using hash-technique from the generic interface name and 
   *  unique identifiers of related objects 
   *  @see IInterface
   *  @return the uniqie interface identifier
   */
  static const InterfaceID& interfaceID()
  {
    static const InterfaceID s_iid  =
      Relations::interfaceID( "IAssociatorWeighted"                ,
                              DirectType::FromTypeTraits::   id () ,
                              DirectType::ToTypeTraits::     id () , 
                              DirectType::WeightTypeTraits:: id () , 
                              DirectType::TypeTraits::     version , 0 );
    return s_iid ;
  };
  
protected:

  /// destructor (virtual and protected) 
  virtual ~IAssociatorWeighted(){}; 

};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IAssociatorWeighted_H
// ============================================================================
