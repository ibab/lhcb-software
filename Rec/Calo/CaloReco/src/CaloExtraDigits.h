// $Id: CaloExtraDigits.h,v 1.6 2007-03-02 15:31:46 cattanem Exp $
// ============================================================================
#ifndef CALORECO_CALOEXTRADIGITS_H 
#define CALORECO_CALOEXTRADIGITS_H 1
// Include files
// from STL
#include <string>
// Kernel 
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/SystemOfUnits.h"
// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
// from CaloInterfaces
#include "CaloInterfaces/ICaloHypoTool.h"
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
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
  public                  GaudiTool 
{
  /// friend factory for instantiation 
  friend class ToolFactory<CaloExtraDigits>;

public:
  
  /** standard initialization of the tool
   *  @see IAlgTool
   *  @see  AlgTool
   *  @see GaudiTool
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** The main processing method 
   *  @see ICaloHypoTool 
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  
  /** The main processing method (functor interface)
   *  @see ICaloHypoTool 
   *  @param  hypo  pointer to CaloHypo object to be processed
   *  @return status code 
   */  
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
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
  
  std::string                      m_inputData         ;
  mutable LHCb::CaloDigits*        m_digits            ;
  
  double                           m_z                 ;
  
  double                           m_xTol              ;
  double                           m_yTol              ;
  Gaudi::XYZPoint                  m_vertex            ;
  
  bool                             m_addSeed           ;

  bool                             m_addSeedNeighbours ;

  // 'Ecal' cluster selector 
  LHCb::ClusterFunctors::ClusterFromCalo m_selector          ;

  std::string m_detData;
  const DeCalorimeter* m_det;
};
// ============================================================================
#endif // CALOEXTRADIGITS_H
