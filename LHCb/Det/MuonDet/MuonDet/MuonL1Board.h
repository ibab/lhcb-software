// $Id: MuonL1Board.h,v 1.2 2007-02-28 18:33:18 marcocle Exp $
#ifndef MUONDET_MUONL1BOARD_H 
#define MUONDET_MUONL1BOARD_H 1
#include <vector>
#include "GaudiKernel/DataObject.h"
#include "DetDesc/Condition.h"


// Include files
#include "MuonDet/CLID_MuonL1Board.h"
//static const CLID  CLID_MuonL1Board = 11090;

/** @class MuonL1Board MuonL1Board.h MuonDet/MuonL1Board.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-01-05
 */
class MuonL1Board:public Condition {
public: 
  /// Standard constructor
  MuonL1Board( ); 

  virtual ~MuonL1Board( ); ///< Destructor
  /// Class ID of this class
  inline static  const CLID& classID() {
    return CLID_MuonL1Board;
  }
  inline long L1Number(){return m_L1Number;};	
  void  setL1Number(long number){m_L1Number=number;};	
  void  setL1Station(long number){m_station=number;};

  inline long numberOfODE(){return m_numberOfODE;};
  inline void setNumberOfODE(long ode){m_numberOfODE=ode;};
  void addODE(long number, std::string name);
  void addLayout(long region, long X,long Y);  
  inline std::string getODEName(int i){return m_ODEName[i];};
  inline long getTSLayoutX(long i){return m_regionLayout[i][0];}  ;
  inline long getTSLayoutY(long i){return m_regionLayout[i][1];}  ;
  long getODEPosition(long number);  
  inline int getStation(){return m_station;};
protected:

private:
  long m_station;	
  long m_numberOfODE;
  long m_L1Number;
  std::vector<long> m_ODENumberList;
  std::vector<std::string> m_ODEName;
  long m_regionLayout[4][2]  ;
  
  

};
#endif // MUONDET_MUONL1BOARD_H
