// $Id: CaloPIDsData.h,v 1.1 2004-03-17 16:11:40 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOPIDS_CALOPIDSDATA_H 
#define CALOPIDS_CALOPIDSDATA_H 1
// ============================================================================
// Include files
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloTupleAlg.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/TrStoredTrack.h"
#include "Event/MCParticle.h"
// ============================================================================

/** @class CaloPIDsData CaloPIDsData.h
 *
 *  Simple algorithm to fill NTuple with information, 
 *  needed to obtain the reference histograms 
 *  for CaloPIDs package 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-03-17
 */
class CaloPIDsData : public CaloTupleAlg 
{
  friend class AlgFactory<CaloPIDsData> ;
public:
  
  /// the actual type of Track -> Eval associator  
  typedef IAssociator<TrStoredTrack,float> IAsct ;
  /// the actual type of Track -> Eval relation table 
  typedef IAsct::DirectType                Table ;
  /// the actual type of Track -> Eval range  
  typedef Table::Range                     Range ;
  
  /// the actual type for TrStoredTrack -> MCParticle  associator 
  typedef IAssociatorWeighted<TrStoredTrack,MCParticle,double> ITrAsct  ;
  /// the actual type for TrStoredTrack -> MCParticle  relation table
  typedef ITrAsct::DirectType                                  TrTable  ;
  typedef TrTable::Range                                       TrRange  ;
  
public: 
  
  /** initialzation of the algorithm
   *  @see  CaloTupleAlg
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm
   *  @see      Algorithm
   *  @see     IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
  /** execution     of the algorithm
   *  @see     IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute    () ;
  
protected:
  
  /** Standard constructor
   *  @see  CaloTupleAlg
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm 
   *  @see      Algorithm 
   *  @see      AlgFactory 
   *  @see     IAlgFactory
   *  @param name algorithm instance name 
   *  @param iscv pointer to Service Locator 
   */
  CaloPIDsData ( const std::string& name  , 
                 ISvcLocator*       isvc  ) ;
  
  /// virtual destructor 
  virtual ~CaloPIDsData( ); 
  
protected:
  
  /// get the momentum of the track
  double momentum ( const TrStoredTrack* track ) const ;
  
private:
  
  // default constructor is disabled 
  CaloPIDsData();
  // copy constructor is disabled 
  CaloPIDsData( const CaloPIDsData& ) ;
  // assignement operator is disabled 
  CaloPIDsData& operator= ( const CaloPIDsData& );
  
private:

  std::string m_name_Tr2MC     ;
  
  std::string m_name_PrsE      ;
  std::string m_name_EcalChi2  ;
  std::string m_name_BremChi2  ;
  std::string m_name_EcalE     ;
  std::string m_name_HcalE     ;
  
  ITrAsct*    m_Tr2MC     ;
  
  IAsct*      m_PrsE      ; // e+/e-
  IAsct*      m_EcalChi2  ; // e+/e- 
  IAsct*      m_BremChi2  ; // e+/e-
  IAsct*      m_EcalE     ; // mu+/mu- 
  IAsct*      m_HcalE     ; // e+/e-/mu+/mu-
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOPIDSDATA_H
// ============================================================================
