// $Id: CaloID2DLL.h,v 1.1 2006-06-18 18:35:27 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $$Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef CALOPIDS_CALOID2DLL_H 
#define CALOPIDS_CALOID2DLL_H 1
// ============================================================================
// Include files
// ============================================================================
/// STD& STL
// ============================================================================
#include <cmath>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/VectorMap.h" 
// ============================================================================
// AIDA
// ============================================================================
#include "AIDA/IHistogram2D.h"
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloTrackAlg.h"
// ============================================================================
#include "ToString.h"
#include "ToVector.h"
// ============================================================================
/** @class CaloID2DLL CaloID2DLL.h
 *  
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-18
 */
// ============================================================================
class CaloID2DLL : public CaloTrackAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloID2DLL>;
public:
  /// Algorithm initialization
  virtual StatusCode initialize () ;    
  /// Algorithm execution
  virtual StatusCode execute    () ;    
protected: 
  /// Standard protected constructor
  CaloID2DLL 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) ;
  /// virtual protected destructor 
  virtual ~CaloID2DLL(){}; 
private:
  CaloID2DLL () ;
  CaloID2DLL            ( const CaloID2DLL& ) ;
  CaloID2DLL& operator= ( const CaloID2DLL& ) ;
protected:
  //
  inline double dLL 
  ( const double             p , 
    const double             v , 
    const LHCb::Track::Types t ) const ;
protected:
  // input relation table 
  std::string             m_input    ; ///< input relation table 
  // output relation table 
  std::string             m_output   ; ///< output relation table 
  // histogram title 
  std::string             m_title    ; ///< histogram title 
private:
  double                    m_pScale ;
  double                    m_vScale ;  
  const AIDA::IHistogram2D* m_histo  ;
} ;
// ============================================================================
inline double CaloID2DLL::dLL
( const double             p , 
  const double             v , 
  const LHCb::Track::Types t ) const 
{
  const double _x = ::tanh ( p / m_pScale ) ;
  const double _y = ::tanh ( v / m_vScale ) ;
  const int    ix = m_histo->coordToIndexX ( _x ) ;
  const int    iy = m_histo->coordToIndexY ( _y ) ;
  return m_histo->binHeight(  ix , iy ) ;
} ;
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
#endif // CALOID2DLL_H
// ============================================================================

