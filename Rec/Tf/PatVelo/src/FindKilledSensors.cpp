#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IUpdateManagerSvc.h"
#include "FindKilledSensors.h"
#include <iostream>
#include <vector>

//-----------------------------------------------------------------------------
//// FindKilledSensors: Function for running PatVeloGeneralTracking over CCE scan data 
////
//// 2016-02-23 : Jon Harrison, Dermot Moran
////-----------------------------------------------------------------------------

void FindKilledSensors(int cce, std::vector<unsigned int> &m_killSensorList)
{
  // Pattern definitions:
  unsigned int pattern1_sens[10] = {0,1,10,11,20,21,30,31,40,41};
  unsigned int pattern2_sens[8]  = {2,3,12,13,22,23,32,33};
  unsigned int pattern3_sens[8]  = {4,5,14,15,24,25,34,35};
  unsigned int pattern4_sens[8]  = {6,7,16,17,26,27,36,37};
  unsigned int pattern5_sens[8]  = {8,9,18,19,28,29,38,39};

  unsigned int pattern1_cceStep[13] = { 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13};
  unsigned int pattern2_cceStep[13] = {14,15,16,17,18,19,20,21,22,23,24,25,26};
  unsigned int pattern3_cceStep[13] = {27,28,29,30,31,32,33,34,35,36,37,38,39};
  unsigned int pattern4_cceStep[13] = {40,41,42,43,44,45,46,47,48,49,50,51,52};
  unsigned int pattern5_cceStep[13] = {53,54,55,56,57,58,59,60,61,62,63,64,65};

  bool isPat1 = 0;
  bool isPat2 = 0;
  bool isPat3 = 0;
  bool isPat4 = 0;
  bool isPat5 = 0;
  
  unsigned int cceStep = cce;

  for(int i=0;i<13;++i){
    if (cceStep==pattern1_cceStep[i]){
      isPat1=1;
    }
    if (cceStep==pattern2_cceStep[i]){
      isPat2=1;
    }
    if (cceStep==pattern3_cceStep[i]){
      isPat3=1;
    }
    if (cceStep==pattern4_cceStep[i]){
      isPat4=1;
    }
    if (cceStep==pattern5_cceStep[i]){
      isPat5=1;
    }
  }
  // clear vector:
  if (m_killSensorList.size()!=0) m_killSensorList.clear();

  if (isPat1){
    for (int i=0; i<10;++i){
      m_killSensorList.push_back(pattern1_sens[i]);
    }
  }
  if (isPat2){
    for (int i=0; i<8;++i){
      m_killSensorList.push_back(pattern2_sens[i]);
    }
  }
  if (isPat3){
    for (int i=0; i<8;++i){
      m_killSensorList.push_back(pattern3_sens[i]);
    }
  }
  if (isPat4){
    for (int i=0; i<8;++i){
      m_killSensorList.push_back(pattern4_sens[i]);
    }
  }
  if (isPat5){
    for (int i=0; i<8;++i){
      m_killSensorList.push_back(pattern5_sens[i]);
    }
  }
}
