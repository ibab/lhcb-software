// $Id: CaloTrack2IDAlg.h,v 1.3 2009-08-21 16:49:45 odescham Exp $
// ============================================================================
#ifndef CALOPIDS_CALOTRACK2IDALG_H 
#define CALOPIDS_CALOTRACK2IDALG_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackIdEval.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/Calo2Track.h"
#include "CaloTrackAlg.h"
#include "CaloUtils/CaloAlgUtils.h"
// ============================================================================
// Local
// ============================================================================
#include "ToVector.h"
// ============================================================================
/** @class CaloTrack2IDAlg CaloTrack2IDAlg.h
 *  
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-15
 */
class CaloTrack2IDAlg : public CaloTrackAlg
{
  friend class AlgFactory<CaloTrack2IDAlg> ;
public:
  /// standard algorithm initilization 
  virtual StatusCode initialize () ;
  /// standard algorithm execution
  virtual StatusCode execute    () ;
protected:  
  /// Standard protected constructor
  CaloTrack2IDAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) ;
  /// virtual protected destructor 
  virtual ~CaloTrack2IDAlg(){} ;  
private:
  CaloTrack2IDAlg() ;
  CaloTrack2IDAlg           ( const CaloTrack2IDAlg& ) ;
  CaloTrack2IDAlg& operator=( const CaloTrack2IDAlg& ) ;
protected:
  typedef std::vector<std::string>  Inputs ;
  // input tracks 
  Inputs                  m_inputs   ; ///< input tracks
  // output data 
  std::string             m_output   ; ///< output data
  // filter  
  std::string             m_filter   ; ///< filter 
  // tool to be used 
  std::string             m_toolName ; ///< tool to be used
private:
  // tool to be used for evaluation 
  const ICaloTrackIdEval* m_tool     ; ///< tool to be used for evaluation 
} ;
// ============================================================================
// The END 
// ============================================================================
#endif // CALOTRACK2IDALG_H
// ============================================================================
