// $Id: GiGaExternalPhysList.h,v 1.1 2002-04-25 13:02:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_GIGAEXTERNALPHYSLIST_H 
#define GIGA_GIGAEXTERNALPHYSLIST_H 1
// Include files
// GiGa 
#include "GiGa/GiGaPhisListBase.h"
#include "GiGa/GiGaPhisListFactory.h"

/** @class GiGaExternalPhysList GiGaExternalPhysList.h 
 *  
 *  Helper class to import external physics list into GiGa.
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */

template<class ImportedPhysList>
class GiGaExternalPhysList : 
  virtual public GiGaPhisListBase ,
  virtual public ImportedPhysList
{ 
  /// own type 
  typedef GiGaExternalPhysList<ImportedPhysList>   MyType ;
  /// friend factory for instantiation 
  friend class GiGaPhysListFactory<MyType>                ;
  ///
protected:
  
  /** standard constructor 
   *  @see GiGaPhysListBase 
   *  @see GiGaBase 
   *  @param name of the instance 
   *  @param svc  service locator 
   */
  GiGaExternalPhysList
  ( const std::string& name , 
    ISvcLocator*       svc  )
    : GiGaPhysListBase( name , svc )
    , ImportedPhysList() 
  {};
  
  /// destructor (virtual and protected)
  virtual ~GiGaExternalPhysList(){};
  
private:
  
  /// default constructor is private 
  GiGaExternalPhysList() ; ///< no default constructor 
  /// copy constructor id private 
  GiGaExternalPhysList ( const MyType& ) ; 
  /// assignement operator is private 
  MyType& operator=    ( const MyType& ) ; 
  ///
};

/** @def IMPLEMENT_ExternalPhysList
 *  useful macro to implement a static factory for
 *  instantiation of external("imported") physics list
 *  @see IMPLEMENT_GiGaPhysList
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#define IMPLEMENT_ExternalPhysList( PL )                            \
 static const                                                       \
  GiGaPhysListFactory<GiGaExternalPhysList<PL> >  s_##PL##Factory ; \
 const                                                              \
 IGiGaPhysListFactory&##PL##Factory =             s_##PL##Factory;

// ============================================================================
// The END 
// ============================================================================
#endif // GIGA_GIGAEXTERNALPHYSLIST_H
// ============================================================================
