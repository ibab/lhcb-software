// $Id: CaloEFlowAlg.h,v 1.2 2009-10-16 15:39:40 odescham Exp $
#ifndef CALOENERGYFLOWMONITOR_H 
#define CALOENERGYFLOWMONITOR_H 1

// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "Kernel/CaloCellID.h"
// ============================================================================
// Event
// ============================================================================
#include  "Event/CaloDigit.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include  "CaloMoniAlg.h"
// ============================================================================

/** @class CaloEFlowAlg CaloEFlowAlg.h
 *  
 *
 *  The algorithm for dedicated "EnergyFlow" monitoring of "CaloDigits" containers.
 *  The algorithm produces the following histograms:
 *   1. CaloDigit multiplicity
 *   2. CaloDigit ocupancy 2D plot per area
 *   3. CaloDigit energy and transverse energy 2D plot per area
 *  The same set of histograms, but with cut on Et (or E), is produced if specified
 *
 *  Histograms reside in the directory @p /stat/"Name" , where
 *  @p "Name" is the name of the algorithm
 *
 *  @see   CaloMoniAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see      Algorithm
 *  @see     IAlgorithm
 *
 *  @author Aurelien Martens
 *  @date   2009-04-08
 */
class CaloEFlowAlg : public CaloMoniAlg{
public:
  
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  CaloEFlowAlg( const std::string &name, ISvcLocator *pSvcLocator );
  /// destructor (virtual and protected)
  virtual ~CaloEFlowAlg();
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected: 

private:
  
  /// default  construstor  is  private
  CaloEFlowAlg();
  /// copy     construstor  is  private
  CaloEFlowAlg( const CaloEFlowAlg& );
  /// assignement operator  is  private
  CaloEFlowAlg &operator=( const CaloEFlowAlg& );

  DeCalorimeter *m_calo;

  
  double m_eFilterMin;
  double m_etFilterMin;
  double m_eFilterMax;
  double m_etFilterMax;  

};
#endif // CALOENERGYFLOWMONITOR_H

