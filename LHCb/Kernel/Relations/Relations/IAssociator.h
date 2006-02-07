// $Id: IAssociator.h,v 1.4 2006-02-07 09:22:23 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/01/27 14:48:48  cattanem
// v2r2
//
// Revision 1.2  2004/11/19 15:01:22  ibelyaev
//  substitute explict algorithm management by 'Data-On-Demand' service
//
// ============================================================================
#ifndef RELATIONS_IAssociator_H 
#define RELATIONS_IAssociator_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// Relations 
#include "Relations/IRelation.h"
// forward decalrations 
class    IAlgorithm ;   // from GaudiKernel 

/** @class IAssociator IAssociator.h Relations/IAssociator.h
 *  
 *  Generic interface (tool) to access the relations from Transient Store 
 *
 *  @see IRelation
 *  @see IRelation2D
 *  @see IAssociatorWeighted
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   24/03/2002
 */

template<class FROM,class TO>
class IAssociator:
  public virtual IAlgTool
{
public:
  
  /// shorcut for "own"  type 
  typedef IAssociator<FROM,TO>    OwnType       ;
  /// shortcut for "direct relations
  typedef IRelation<FROM,TO>      DirectType    ;
  /// shortcut for "inverse" relations
  typedef IRelation<TO,FROM>      InverseType   ;
  /// definition of "From" type ("FROM" and "From" could be different)
  typedef typename DirectType::From        From          ;
  /// definition of "To" type   ("TO" and "To" could be different)
  typedef typename DirectType::To          To            ;
  
  /// another set of shortcuts which are may be more explicit (PhC)
  typedef OwnType                             IAsct;
  typedef DirectType                          Table;
  typedef typename Table::Range               ToRange;
  typedef typename Table::iterator            ToIterator;
  typedef InverseType                         InvTable;
  typedef typename InvTable::Range            FromRange;
  typedef typename InvTable::iterator         FromIterator;

public:
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object)  (non const version)
   *  @see IRelation
   *  Usage: 
   *  @code 
   *  typedef IAssociator<FROM,TO> Asct;
   *  // retrieve associator 
   *  Asct* asct = ... ; 
   *  // get the direct relation table from the associator 
   *  Asct::DirectType* table = asct->direct() ; 
   *  @endcode 
   *  @return pointer to "direct" relation table 
   */
  virtual       DirectType*   direct    ()       = 0 ;
  
  /** accessor to  "direct" relations 
   *  (from 'FROM'-objects  to 'TO'-object) (const version)
   *  @see IRelation
   *  Usage: 
   *  @code 
   *  typedef const IAssociator<FROM,TO> Asct;
   *  // retrieve associator 
   *  Asct* asct = ... ; 
   *  // get the constant direct relation table from the associator 
   *  const Asct::DirectType* table = asct->direct() ; 
   *  @endcode 
   *  @return pointer to "direct" relation table 
   */
  virtual const DirectType*   direct    () const = 0 ;

  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (non const version)
   *  @see IRelation
   *  Usage: 
   *  @code 
   *  typedef IAssociator<FROM,TO> Asct;
   *  // retrieve associator 
   *  Asct* asct = ... ; 
   *  // get the inverse relation table from the associator 
   *  Asct::InverseType* table = asct->direct() ; 
   *  @endcode 
   *  @return pointer to "inverse" relation table 
   */
  virtual       InverseType*  inverse   ()       = 0 ;
  
  /** accessor to  "inverse" relations 
   *  (from 'TO'-objects  to 'FROM'-object) (const version)
   *  @see IRelation
   *  Usage: 
   *  @code 
   *  typedef const IAssociator<FROM,TO> Asct;
   *  // retrieve associator 
   *  Asct* asct = ... ; 
   *  // get the constant inverse relation table from the associator 
   *  const Asct::InverseType* table = asct->direct() ; 
   *  @endcode 
   *  @return pointer to "inverse" relation table 
   */
  virtual const InverseType*  inverse   () const = 0 ;
  
  /** accessor to relation builder algorithm
   *  @see IAlgorithm
   *  @return pointer to builder algorithm
   */
  virtual IAlgorithm*         algorithm () const = 0 ;
  
  /** Method to retrieve a range associated to a given FROM element
   *
   * @param from  'FROM' object one want to retrieve associated 
   *              range
   * @param range range of associated objects. 
   *              It is empty if no table was found
   * @return StatusCode Failure it no table was found
   */
  virtual StatusCode rangeFrom
  ( const From&      from  , 
    ToRange&         range ) const = 0 ;
  
  /** Method to retrieve a range associated to a given TO element
   *
   *  @param to     'TO' object one want to retrieve associated range
   *  @param range  range of associated objects. 
   *                It is empty if no table was found
   *  @return StatusCode Failure it no table was found
   */
  virtual StatusCode rangeTo 
  ( const To&        to    , 
    FromRange&       range ) const = 0 ;
  
  /** Method to retrieve a range associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated range
   *  @return range A range of associated objects. 
   *  It is empty if no table was found
   *  
   */
  virtual ToRange    rangeFrom       
  ( const From&      from  ) const = 0 ;
  
  /** Method to retrieve a range associated to a given TO element
   *  @see IAssociator   
   *  @param  to     'TO' object one want to retrieve associated range
   *  @return range  range of associated objects. 
   *  It is empty if no table was found
   */
  virtual FromRange rangeTo
  ( const To&       to     ) const = 0 ;
  
  /** Method to retrieve a single element associated to a given FROM element
   *
   *  @param from  'FROM' object one want to retrieve associated (first) element
   *  @return to The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual To         associatedFrom
  ( const From&      from  ) const = 0 ;
  
  /** Method to retrieve a single element associated to a given TO element
   *
   *  @param to  'TO' object one want to retrieve associated (first) element
   *  @return from The (first) associated object. 
   *  It is a null pointer if no table was found
   *  
   */
  virtual From       associatedTo
  ( const To&        to  ) const = 0 ;
  
  /* Method to test if the table does exist or not
   * @return true if the associator contains valid relation table 
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
	Relations::interfaceID( System::typeinfoName( typeid(OwnType) ) ) ;
      return s_iid ;
  };
  
protected:
  
  /// destructor (virtual and protected) 
  virtual ~IAssociator(){}; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IAssociator_H
// ============================================================================
