#ifndef CALORECO_CELLULARAUTOMATON_H
#define CALORECO_CELLULARAUTOMATON_H 1
// ============================================================================
#include <string>
#include <iostream>
#include "GaudiAlg/GaudiAlgorithm.h"
#include "CaloKernel/CaloVector.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloUtils/CellSelector.h"
#include "CelAutoTaggedCell.h"

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
  ( const LHCb::CaloDigit*            digit    ,
    const DeCalorimeter*              detector , 
    const LHCb::CaloDigit::Container& hits     ) ;
  
  inline bool isLocMax 
  ( const CelAutoTaggedCell*          taggedCell ,
    const DeCalorimeter*              detector , 
    const LHCb::CaloDigit::Container& hits     ) 
  {
    if ( 0 == taggedCell ) { return false ; }
    return isLocMax ( taggedCell->digit() , detector , hits ) ;
  }
  
  inline void appliRulesTagger( CelAutoTaggedCell*   taggedCell,
                                DirVector&           taggedCellsDirect,
                                const DeCalorimeter* detector );
  
  inline void setEXYCluster( LHCb::CaloCluster*         cluster,
                             const DeCalorimeter* detector );

private :
  std::string m_inputData;
  std::string m_outputData;
  std::string m_detData;
  const DeCalorimeter* m_detector;
  
  bool m_sort     ;
  bool m_sortByET ;
  bool m_release;
  unsigned long m_passMin , m_passMax;
  double m_pass , m_clus ,m_event;
  std::string m_used;
  CellSelector m_cellSelector;
};
// ============================================================================
#endif  // CALORECO_CELLULARAUTOMATON_H
// ============================================================================










