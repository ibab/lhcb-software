// $Id: GiGaBase.h,v 1.22 2004-08-02 13:13:46 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.21  2004/02/20 18:13:34  ibelyaev
//  major update in GiGaBase and GiGaTrajectory
//
// ============================================================================
#ifndef     GIGA_GIGABASE_H
#define     GIGA_GIGABASE_H 1 
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <exception>
#include <map>
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartDataPtr.h"
// ============================================================================
// GaudiTools 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// GiGa
// ============================================================================
#include "GiGa/IGiGaInterface.h"
#include "GiGa/GiGaFactory.h"
// forward declarations 
class IMessageSvc          ;
class ISvcLocator          ;
class IChronoStatSvc       ;
class IDataProviderSvc     ;
class IIncidentSvc         ;
class IObjManager          ;
class IGiGaSvc             ; 
class IGiGaSetUpSvc        ; 
//
class PropertyMgr          ;
class MsgStream            ;
class StreamBuffer         ;
class GaudiException       ;
//

/** @class GiGaBase GiGaBase.h GiGa/GiGaBase.h
 *    
 *  Helper class for implementation of some GiGa classes. 
 *  It allows an easy configuration of properties and services location  
 *  Implement almost all virtual "technical functions".
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class  GiGaBase:
  public virtual IGiGaInterface    , 
  public virtual IIncidentListener , 
  public                 GaudiTool 
{
protected:
  
  /** standard constructor 
   *  @see GaudiTool 
   *  @see   AlgTool 
   *  @param type tool   type (?)  
   *  @param name object name 
   *  @param parent pointer to parent object  
   */
  GiGaBase( const std::string& type   ,
            const std::string& name   , 
            const IInterface*  parent );
  /// virtual destructor 
  virtual ~GiGaBase();

public:

  /** initialize the object
   *  @see GaudiTool
   *  @see   AlgTool
   *  @see  IAlgTool
   *  @return status code 
   */
  virtual StatusCode initialize() ;

  /** finalize the object 
   *  @see GaudiTool
   *  @see   AlgTool
   *  @see  IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize  () ;
  
  /** handle the incident
   *  @param i reference to the incident
   */ 
  virtual void handle( const Incident& i ) ;
  /// 
public:
  
  /** accessor to GiGa Service 
   *  @return pointer to GiGa Service 
   */
  inline IGiGaSvc*         gigaSvc   () const { return m_gigaSvc   ; }; 
  
  /** accessor to GiGa SetUp Service 
   *  @return pointer to GiGa SetUp Service 
   */
  inline IGiGaSetUpSvc*    setupSvc  () const { return m_setupSvc  ; };

private: 
  
  // default constructor    is disabled 
  GiGaBase();                              ///< no default 
  // copy    constructor    is disabled 
  GiGaBase           ( const GiGaBase& );  ///< no copy 
  // assigenement  operator is disabled 
  GiGaBase& operator=( const GiGaBase& );  ///< no assignment
  
private:
  // name of GiGa Service   
  std::string           m_gigaName   ; 
  // name of GiGa SetUp Service   
  std::string           m_setupName  ; 
  // pointer to GiGa Service 
  IGiGaSvc*             m_gigaSvc    ;
  // pointer to GiGa SetUp Service 
  IGiGaSetUpSvc*        m_setupSvc   ;
private:
};
// ============================================================================

// ============================================================================
#endif ///<   GIGA_GIGABASE_H
// ============================================================================












