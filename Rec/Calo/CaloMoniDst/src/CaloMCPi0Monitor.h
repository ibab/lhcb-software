// $Id: CaloMCPi0Monitor.h,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/03/17 16:35:19  ibelyaev
//  regular update: MCCaloMonitor and CaloPIDsMonitor
//
// ============================================================================
#ifndef CALOEX_CAloMCPi0Monitor_H 
#define CALOEX_CAloMCPi0Monitor_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// from CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/MCParticle.h"
// ============================================================================
// from CaloUtils
// ============================================================================
#include "CaloUtils/ClusterFunctors.h"
// ============================================================================
// Local
// ============================================================================
#include "CaloMoniAlg.h"
// ============================================================================
// forward declarations
class IHistogram1D         ;   // from AIDA
class IParticlePropertySvc ;   // from GaudiKernel

/** @class CaloMCPi0Monitor CaloMCPi0Monitor.h
 *  
 *  Simple MC pi0 monitoring algorithm
 *  It produces 3 histograms 
 *  <ol>
 *  <li> "Raw" mass distribution of 2 photons </li>
 *  <li> Mass distribution of 2 photons after Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons aftre Pt cut for combination </li>
 *  </ol>
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloMCPi0Monitor : public CaloMoniAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloMCPi0Monitor>;

public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloMCPi0Monitor( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloMCPi0Monitor();
  
private:

  /// default  construstor  is  private 
  CaloMCPi0Monitor(); 
  /// copy     construstor  is  private 
  CaloMCPi0Monitor
  ( const CaloMCPi0Monitor& );
  /// assignement operator  is  private 
  CaloMCPi0Monitor& operator=
  ( const CaloMCPi0Monitor& );
  
private:

  // asosciator to MC truth 
  typedef IAssociatorWeighted<CaloCluster,MCParticle,float> Asct ;
  std::string                      m_associatorType              ;
  std::string                      m_associatorName              ;
  Asct*                            m_associator                  ;
  
  // photon "purity" cut 
  float                            m_cut                         ;
  
  ClusterFunctors::ClusterFromCalo m_calo                        ;
  
  IParticlePropertySvc*            m_ppSvc                       ;
  std::string                      m_pi0Name                     ;
  ParticleID                       m_pi0ID                       ;
  
  // 2 photon  mass distribution  (no cuts)
  IHistogram1D*                    m_2g                          ;
  
  // 2 photon  mass distribution  (fromo the same particle) 
  IHistogram1D*                    m_2gPi0                       ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CAloMCPi0Monitor_H
// ============================================================================
