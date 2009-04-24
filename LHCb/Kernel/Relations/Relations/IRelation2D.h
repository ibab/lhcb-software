// $Id: IRelation2D.h,v 1.5 2009-04-24 15:26:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef RELATIONS_IRELATION2D_H 
#define RELATIONS_IRELATION2D_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"
// ============================================================================
/// local
// ============================================================================
#include "Relations/RelationTypeTraits.h"
#include "Relations/RelationUtils.h"
#include "Relations/IRelation.h"
// ============================================================================
/** @class IRelation2D IRelation2D.h Relations/IRelation2D.h
 *  
 *  An absract interface for bidirectional templated relations
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   23/03/2002
 */
template< class FROM, class TO>
class IRelation2D : public IRelation<FROM,TO>
{
public: 
  // ==========================================================================
  /// shortcut to own type
  typedef IRelation2D<FROM,TO>                   OwnType        ;
  /// shortcut to inverse type
  typedef IRelation2D<TO,FROM>                   InvType        ;
  /// shortcut to "direct" type
  typedef IRelation<FROM,TO>                     DirectType     ;
  /// shortcut to "inverse" type
  typedef IRelation<TO,FROM>                     InverseType    ;
  // ==========================================================================
public:
  // ==========================================================================
  /** get the "direct" interface 
   *  @return pointer to the 'direct' interface 
   */
  virtual       DirectType*  direct ()       = 0 ;
  /** get the "direct" interface  (const-version)
   *  @return pointer to the 'direct' interface 
   */
  virtual const DirectType*  direct () const = 0 ;
  /** get the "inverse" interface 
   *  @return pointer to the 'inverse' interface 
   */
  virtual       InverseType* inverse ()       = 0 ;
  /** get the "inverse" interface  (const version)
   *  @return pointer to the 'inverse' interface 
   */
  virtual const InverseType* inverse () const = 0 ;
  // ==========================================================================
public:
  // ==========================================================================
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
      Relations::interfaceID( System::typeinfoName(typeid(OwnType)) );
    return s_iid ;
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /// destructor (virtual and protected)
  virtual ~IRelation2D(){}
  // ==========================================================================
};
// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_IRELATION2D_H
// ============================================================================
