// $Id: CaloPIDsMonitor.h,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/17 12:00:58  ibelyaev
//  add new algorithm and update for new base
// 
// ============================================================================
#ifndef CALOMONITOR_CALOPIDSMONITOR_H 
#define CALOMONITOR_CALOPIDSMONITOR_H 1
// ============================================================================
// Include files
// ============================================================================
// LHCbKernel/Relations
// ============================================================================
#include "Relations/IAssociator.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/TrStoredTrack.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloHistoAlg.h"
// ============================================================================

/** @class CaloPIDsMonitor CaloPIDsMonitor.h
 *
 *  Class for monitoring the CaloPIDs quantities  
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-15
 */
class CaloPIDsMonitor : public CaloHistoAlg
{
  /// friend factory for instantiation 
  friend class AlgFactory<CaloPIDsMonitor>;
public:
  
  /// the actual type for CaloPIDs associator 
  typedef IAssociator<TrStoredTrack,float> ITr2Calo   ;
  /// the actual type for CaloPIDs relation table 
  typedef ITr2Calo::DirectType             ICaloTable ;
  
  /// the actual type for TrStoredTrack -> MCParticle  associator 
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> ITr2MC   ;
  /// the actual type for TrStoredTrack -> MCParticle  relation table
  typedef ITr2MC::DirectType                                   IMCTable ;

public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorihtm 
   *  @see     Algorithm
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** standard algorithm execution
   *  @see     Algorithm
   *  @see    IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute    () ;
  
protected:
  
  /** Standard constructor
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see     AlgFactory
   *  @see    IAlgFactory
   *  @param name nam eof algorithm instance 
   *  @param isvc pointer to  Service Locator 
   */
  CaloPIDsMonitor ( const std::string& name , 
                    ISvcLocator*       isvc ) ;
  
  /// virtual destructor 
  virtual ~CaloPIDsMonitor() ;
  
private:
  
  // default constructor is disabled  
  CaloPIDsMonitor() ; 
  // copy constructor is disabled 
  CaloPIDsMonitor           ( const CaloPIDsMonitor& ) ; 
  // assignement operator is disabled 
  CaloPIDsMonitor& operator=( const CaloPIDsMonitor& ) ;

protected:

  /// transformation function for momentum 
  inline double pFunc( const double value ) const 
  { return tanh( value / m_pNorm ); };
  
  /** extract the momentum from TrStoredTrack
   *  @param  track pointer to the track
   *  @return the momentum of the track 
   */
  double momentum( const TrStoredTrack* track ) const ;
  
private:
  
  // type/name for Tr -> CaloPIDs associator 
  std::string m_tr2caloName  ;
  // Tr -> CaloPIDs associator itself 
  ITr2Calo*    m_tr2calo      ;
  
  // type/name for Tr -> MC associator 
  std::string m_tr2mcName     ;
  // Tr -> MC  associator itself 
  ITr2MC*     m_tr2mc         ;
  
  // particle to be identified
  int         m_pid          ;

  // cut value 
  double      m_cut          ;

  // momentum normalization 
  double      m_pNorm        ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOMONITOR_CALOPIDSMONITOR_H
// ============================================================================
