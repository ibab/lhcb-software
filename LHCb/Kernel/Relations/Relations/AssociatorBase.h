// $Id: AssociatorBase.h,v 1.2 2004-11-19 15:01:22 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_AssociatorBase_H 
#define RELATIONS_AssociatorBase_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL
#include <map> 
// GaudiKernel
#include "GaudiKernel/IIncidentListener.h"
// GaudiTool
#include "GaudiAlg/GaudiTool.h"
// forward declarations
//
class IAlgorithm         ; // GaudiKernel 
class IToolSvc           ; // GaudiKernel
class IChronoStatSvc     ; // GaudiKernel
class IDataProviderSvc   ; // GaudiKernel
class IIncidentSvc       ; // GaudiKernel 
class GaudiException     ; // GaudiKernel 

namespace Relations
{
  
  /** @class AssociatorBase AssociatorBase.h Relations/AssociatorBase.h
   *   
   *  The base class for implementation of relation tools  
   * 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @see IIncidentListener 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   24/03/2002
   */  
  class AssociatorBase:
    public                 GaudiTool  ,
    public virtual IIncidentListener 
  {
  public:
    // the type for counter 
    typedef long counter ;
  public:
    
    /** standard initialization method
     *  @see  AlgTool 
     *  @see IAlgTool 
     *  @return status code 
     */
    virtual StatusCode initialize () ;
    
    /** standard initialization method
     *  @see  AlgTool 
     *  @see IAlgTool 
     *  @return status code 
     */
    virtual StatusCode finalize   () ;
    
    /** handle the incident 
     *  @see IIncidentListener 
     *  @see  Incident 
     *  @see IIncidentSvc  
     *  @see incident incident to be handled 
     */
    virtual void handle ( const Incident& incident ) ;
    
  protected:
    
    /** Standard constructor
     *  @param type   tool type (?) 
     *  @param name   tool name 
     *  @param parent tool parent
     */
    AssociatorBase
    ( const std::string& type   , 
      const std::string& name   ,
      const IInterface*  parent ) ;
    
    /// destructor (virtual and protected)
    virtual ~AssociatorBase () ;
  
  protected:
    
    /** accessor to data location
     *  @return data locationin Gaudi ETS 
     */
    inline const std::string& location  () const ;
    
    /** The "base" method for access the relation data 
     *
     *  @attention it is virtual method ! One coudl 
     *  redefine it for "specific" behaviour 
     *
     *  'Default' behaviour:
     *  
     *   - locate relation table in Gaudi Event Transient Store 
     *   - call for Relation builder algorithm to build and 
     *     register  the relation table in Gaudi Event Transient Store
     *   - again locate relation table in Gaudi Event Transient Store 
     *
     *  @return status code 
     */
    virtual StatusCode locateOrBuild   () const;
    
    /** accessor to relations builder,
     *  locate algorthm, if not yet done 
     *  @see IAlgorithm
     *  @return poinere to relations builder
     */
    IAlgorithm*        algorithm () const ;
    
    /** accessor to relations table via interface, 
     *  locate or build the table, if not yet done 
     *  @see IInterface 
     *  @return poinere to relations table 
     */
    IInterface*        object    () const ;

  private:
    
    /** locate the algorithm for building the relations
     *  @return status code 
     */
    StatusCode locateAlgorithm () const ;
    
  private:
    
    /// default constructor is private 
    AssociatorBase();
    /// copy    constructor is private 
    AssociatorBase           
    ( const AssociatorBase& );
    /// assignement operator is private  
    AssociatorBase& operator=
    ( const AssociatorBase& );
    
  protected: 
    
    /** add the reference to existing interafce 
     *  @param  iif interface 
     *  @return current value of reference counter 
     */
    inline counter addRef  ( IInterface* iif ) const ;
    
    /** release the used interface 
     *  @param  iif interface to be released 
     *  @return current value of reference counter 
     */
    inline counter release ( IInterface* iif ) const ;
    
  private:
    
    // data location in Gaudi Event Transient Store 
    std::string         m_location     ; ///< data location in Gaudi ETS 
    // relation builder type 
    std::string         m_builderType  ; ///< relation builder type 
    // relation builder name 
    mutable std::string m_builderName  ; ///< relation builder name 
    // reference counter 
    mutable counter     m_counter      ; ///< reference counter
    
  };
  
  /** accessor to data location
   *  @see AssociatorBase 
   *  @return data location in Gaudi ETS 
   */
  inline const std::string& AssociatorBase::location  () const 
  { return m_location ; }
  
  /** add the reference to existing interface 
   *  @param  iif interface 
   *  @return current value of reference counter 
   */
  inline AssociatorBase::counter 
  AssociatorBase::addRef  ( IInterface* iif ) const 
  {
    if ( 0 == iif ) { return m_counter ; }
    iif -> addRef () ; 
    return ++m_counter ;
  };
  
  /** release the used interface 
   *  @param  iif interface to be released 
   *  @return current value of reference counter 
   */
  inline AssociatorBase::counter 
  AssociatorBase::release ( IInterface* iif ) const 
  {
    if ( 0 == iif ) { return m_counter ; }
    iif->release () ; 
    return --m_counter ;
  };
  
}; ///< end of namespace Relations 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_AssociatorBase_H
// ============================================================================
