// $Id: GiGaVisManager.h,v 1.2 2007-01-12 15:48:39 ranjard Exp $
#ifndef GIGA_GIGAVISMANAGER_H 
#define GIGA_GIGAVISMANAGER_H 1

// Include files
#include "GiGa/IGiGaVisManager.h"
#include "GiGa/GiGaBase.h"
// forward declarations
//template <class TYPE> class GiGaFactory;

/** @class GiGaVisManager GiGaVisManager.h
 *
 *  The simplest imeplemenattion of IGiGaVisManager interface 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-04
 */
class GiGaVisManager :
  public virtual IGiGaVisManager , 
  public          GiGaBase       
{
  /// friend factory for instantiation
  //  friend class GiGaFactory<GiGaVisManager>;
public:
  
  /** initialization method 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         initialize  ()        ;

  /** finalization method 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode         finalize    ()        ;
  
  /** get the pointer to G4 visual manager
   *  @see IGiGaVisManager
   *  @return pointer to G4 visual manager 
   */
  virtual G4VVisManager* visMgr () const ;
  
  //protected:
  
  /** standard constructor 
   *  @param type type 
   *  @param name instance name 
   *  @param parent pointer to parent 
   */
  GiGaVisManager
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GiGaVisManager();
 
protected:
  
private:
  
  /// default constructor  is private 
  GiGaVisManager() ;
  /// copy    constructor  is private 
  GiGaVisManager( const GiGaVisManager& ) ;  
  /// assignement operator is private 
  GiGaVisManager& operator=( const GiGaVisManager& ) ;
  
private:
  
  G4VVisManager*  m_visMgr ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAVISMANAGER_H
// ============================================================================
