// $Id: CaloExtraDigits.h,v 1.1 2002-04-07 18:15:00 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOTOOLS_CALOEXTRADIGITS_H 
#define CALOTOOLS_CALOEXTRADIGITS_H 1
// Include files
// from STL
#include <string>
// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
// from CaloInterfaces
#include "CaloInterfaces/ICaloHypoTool.h"
// from CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloEvent/Event
#include "Event/CaloDigit.h"
// from CaloUtils
#include "CaloUtils/ClusterFunctors.h"
// forward declaratios 
class IIncidentSvc ;   // from GaudiKernel

/** @class CaloExtraDigits CaloExtraDigits.h
 *  
 *
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   31/03/2002
 */
class CaloExtraDigits : 
  public virtual IIncidentListener ,
  public virtual     ICaloHypoTool ,
  public                  CaloTool 
{
  /// friend factory for instantiation 
  friend ToolFactory<CaloExtraDigits>;

public:
  
  /** standard initialization of the tool
   *  @see IAlgTool
   *  @see  AlgTool
   *  @see CaloTool
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** standard finalization of the tool
   *  @see IAlgTool
   *  @see  AlgTool
   *  @see CaloTool
   *  @return status code 
   */
  virtual StatusCode finalize () ;
  
  /** The main processing method 
   *  @see ICaloHypoTool 
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool 
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( CaloHypo* hypo  ) const ;
  
  /** handle the incident 
   *  @see Incident 
   *  @see IIncidentListener 
   *  @see IIncidentSvc 
   *  @param incident incident to be handled 
   */
  virtual void handle( const Incident& incident ) ;
  
protected:

  /** Standard constructor
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool parent 
   */
  CaloExtraDigits( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);
  
  /// Destructor (virtual and protected)
  virtual ~CaloExtraDigits();

private:
  
  /// default constructor is private 
  CaloExtraDigits();
  /// copy constructor is private
  CaloExtraDigits( const CaloExtraDigits& );
  /// assignement operator is private
  CaloExtraDigits& operator=( const CaloExtraDigits& );
  
private:
  
  IIncidentSvc*                    m_incSvc    ;
  IDataProviderSvc*                m_evtSvc    ;
  
  std::string                      m_inputData ;
  mutable CaloDigits*              m_digits    ;
  
  double                           m_z         ;
  
  double                           m_xTol      ;
  double                           m_yTol      ;
  HepPoint3D                       m_vertex    ;
  
  bool                             m_addSeed   ;
  
  // 'Ecal' cluster selector 
  ClusterFunctors::ClusterFromCalo m_selector  ;

  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOEXTRADIGITS_H
// ============================================================================
