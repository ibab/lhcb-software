// $Id: GiGaBase.h,v 1.26 2008-05-30 13:24:42 gcorti Exp $
#ifndef GIGA_GIGABASE_H
#define GIGA_GIGABASE_H 1 

// Include files
// from STL
#include <string>
#include <exception>
#include <map>

// from Gaudi 
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiAlg/GaudiTool.h"

// from GiGa
#include "GiGa/IGiGaInterface.h"
#include "GiGa/IGiGaSvc.h"
#include "GiGa/IGiGaSetUpSvc.h"

// Forward declarations 
class IMessageSvc          ;
class ISvcLocator          ;
class IChronoStatSvc       ;
class IDataProviderSvc     ;
class IIncidentSvc         ;
class IObjManager          ;
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

class  GiGaBase: public virtual IGiGaInterface   , 
                 public virtual IIncidentListener, 
                 public         GaudiTool 
{
public:
  
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
  virtual StatusCode initialize();

  /** finalize the object 
   *  @see GaudiTool
   *  @see   AlgTool
   *  @see  IAlgTool
   *  @return status code 
   */
  virtual StatusCode finalize();
  
  /** handle the incident
   *  @param i reference to the incident
   */ 
  virtual void handle( const Incident& i );

public:
  
  /** accessor to GiGa Service 
   *  @return pointer to GiGa Service 
   */
  inline IGiGaSvc* gigaSvc() const { 
    if( m_gigaSvc == 0 ) {
      m_gigaSvc = svc<IGiGaSvc>( m_gigaName , true );
    } 
    return m_gigaSvc;
  };

  /** accessor to GiGa SetUp Service 
   *  @return pointer to GiGa SetUp Service 
   */
  inline IGiGaSetUpSvc* setupSvc() const { 
    if( m_setupSvc == 0 ) {
      if( m_setupName == m_gigaName ) {
        // cast to correct type
        m_setupSvc = dynamic_cast<IGiGaSetUpSvc*>( gigaSvc() ); 
      }
      else {
        m_setupSvc = svc<IGiGaSetUpSvc> ( m_setupName , true );
      }
    }
    return m_setupSvc; 
  };

private: 
  
  GiGaBase();                              ///< no default constructor
  GiGaBase           ( const GiGaBase& );  ///< no copy constructor 
  GiGaBase& operator=( const GiGaBase& );  ///< no assignment operator
  
private:

  std::string    m_gigaName;        ///< Name of GiGa service
  std::string    m_setupName;       ///< Name of GiGa SetUp Service 
  mutable IGiGaSvc*      m_gigaSvc;         ///< Pointer to GiGa Service
  mutable IGiGaSetUpSvc* m_setupSvc;        ///< Pointer to GiGa SetUp Service

};


#endif ///<   GIGA_GIGABASE_H

