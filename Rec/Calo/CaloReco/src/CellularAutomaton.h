// $Id:
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2006/05/30 09:42:06  odescham
// first release of the CaloReco migration
//
// Revision 1.6  2005/11/07 12:12:43  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.5  2004/10/22 16:33:53  ibelyaev
//  update CellularAutomaton and remove obsolete algs
//
// ============================================================================
#ifndef CALORECO_CELLULARAUTOMATON_H
#define CALORECO_CELLULARAUTOMATON_H 1
// ============================================================================
// STD and STL 
// ============================================================================
#include <string>
#include <iostream>
// ============================================================================
/// GaudiKernel
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
/// CaloKernel 
// ============================================================================
#include "CaloKernel/CaloVector.h"
// ============================================================================
/// CaloDet 
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
/// local
// ============================================================================
#include "CelAutoTaggedCell.h"
// ============================================================================

/** @class CellularAutomaton CellularAutomaton.h 
 *
 *  Algorithm of clustering 
 *
 *  @author  Nicole Brun 
 *  @date    27/02/2001
 */
class CellularAutomaton : public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CellularAutomaton>;
public:
  
  /// container to tagged  cells with sequential access 
  typedef std::vector<CelAutoTaggedCell*> SeqVector;
  /// container to tagged  cells with direct (by CaloCellID key)  access 
  typedef CaloVector<CelAutoTaggedCell*>  DirVector ;
  
public:  
  
  virtual StatusCode initialize () ;
  virtual StatusCode execute    () ;
  virtual StatusCode finalize  ();  
protected:
  
  /** standard constructor
   *  @see Algorithm 
   *  @param name   name of the algorithm
   *  @param svcloc pointer to service locator 
   */
  CellularAutomaton
  ( const std::string& name   , 
    ISvcLocator*       svcloc ) ;

  /// destructor (virtual and protected)
  virtual ~CellularAutomaton()  ;

private:
  
  ///  default  constructor  is private 
  CellularAutomaton();
  ///  copy     constructor  is private 
  CellularAutomaton
  ( const CellularAutomaton& );
  ///  assignement operator  is private 
  CellularAutomaton& operator=
  ( const CellularAutomaton& );

protected:
  
  
  inline bool isLocMax
  ( const LHCb::CaloDigit*     digit ,
    const DirVector&     hits  ,
    const DeCalorimeter* det   ) ;
  
  inline bool isLocMax
  ( const CelAutoTaggedCell* taggedCell ,
    const DirVector&         hitsDirect ,
    const DeCalorimeter*     detector   )
  {
    if ( 0 == taggedCell ) { return false ; }
    return isLocMax ( taggedCell->digit() , hitsDirect , detector ) ;
  }
  
  inline void appliRulesTagger
  ( CelAutoTaggedCell*   taggedCell,
    DirVector&           taggedCellsDirect,
    const DeCalorimeter* detector );
  
  inline void setEXYCluster
  ( LHCb::CaloCluster*         cluster,
    const DeCalorimeter* detector );

private :
  std::string m_inputData;
  std::string m_outputData;
  std::string m_detData;
  const DeCalorimeter* m_detector;
  
  bool m_sort     ;
  bool m_sortByET ;
  
};
// ============================================================================
#endif  // CALORECO_CELLULARAUTOMATON_H
// ============================================================================










