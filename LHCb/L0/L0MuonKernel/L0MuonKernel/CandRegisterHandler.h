// $Id: CandRegisterHandler.h,v 1.4 2006-03-06 09:56:00 jucogan Exp $

#ifndef L0MUONKERNEL_CANDREGISTERHANDLER_H
#define L0MUONKERNEL_CANDREGISTERHANDLER_H     1

/** @class CandRegisterHandler CandRegisterHandler.h L0MuonKernel/CandRegisterHandler.h

Class defining an internal representation
of a Muon trigger candidate

*/

#include <iostream>
#include "ProcessorKernel/Register.h"
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/MuonCandidate.h"

namespace L0Muon {

  typedef unsigned int   ContentType ; 

  static const ContentType BitsCandAddM3  =  7 ;
  static const ContentType BitsCandOffM2  =  4 ;
  static const ContentType BitsCandOffM1  =  4 ;
  static const ContentType BitsCandPT     =  7 ;
  static const ContentType BitsCandCharge =  1 ;
  static const ContentType BitsCandPU     =  2 ;
  static const ContentType BitsCandBoard  =  4 ;

  static const ContentType BitsCandTot    = BitsCandAddM3+BitsCandOffM2+BitsCandOffM1+\
                                            BitsCandPT+BitsCandCharge+BitsCandPU+BitsCandBoard ;

  static const ContentType BitsCandStatus =  4 ;

  static const ContentType BitsCandRegTot = 2*BitsCandTot + BitsCandStatus;
                                                                

  static const ContentType ShiftCandAddM3 = 0;
  static const ContentType ShiftCandOffM2 = ShiftCandAddM3  + BitsCandAddM3 ;
  static const ContentType ShiftCandOffM1 = ShiftCandOffM2  + BitsCandOffM2 ;
  static const ContentType ShiftCandPT    = ShiftCandOffM1  + BitsCandOffM1 ;
  static const ContentType ShiftCandCharge= ShiftCandPT     + BitsCandPT    ;
  static const ContentType ShiftCandPU    = ShiftCandCharge + BitsCandCharge;
  static const ContentType ShiftCandBoard = ShiftCandPU     + BitsCandPU    ;

  static const ContentType ShiftCandStatus= 2*BitsCandTot;

  static const ContentType BitsCandNumber =  2 ;
  static const ContentType ShiftCandNumber= ShiftCandStatus;

  class CandRegisterHandler  {
  
  public:
    

    /// Default Constructor 
    CandRegisterHandler();
      
    /// Constructor with a pattern
    CandRegisterHandler(Register * candRegister);    

    /// Clear the candidate register
    void clear(){
      m_candRegister->reset();
    }
                                                                                                    
    /// Set the candidate's properties in the register
    void setCandAddM3 (int candAddM3 , int icand){
      m_candRegister->set(candAddM3  ,BitsCandAddM3  ,ShiftCandAddM3 +icand*BitsCandTot);
    }  
    void setCandOffM2 (int candOffM2 , int icand){
      m_candRegister->set(candOffM2  ,BitsCandOffM2  ,ShiftCandOffM2 +icand*BitsCandTot);
    }  
    void setCandOffM1 (int candOffM1 , int icand){
      m_candRegister->set(candOffM1  ,BitsCandOffM1  ,ShiftCandOffM1 +icand*BitsCandTot);
    }  
    void setCandPT    (int candPT    , int icand){
      m_candRegister->set(candPT     ,BitsCandPT     ,ShiftCandPT    +icand*BitsCandTot);
    }  
    void setCandCharge(int candCharge, int icand){
      m_candRegister->set(candCharge ,BitsCandCharge ,ShiftCandCharge+icand*BitsCandTot);
    }  
    void setCandPU    (int candPU    , int icand){
      m_candRegister->set(candPU     ,BitsCandPU     ,ShiftCandPU    +icand*BitsCandTot);
    }  
    void setCandBoard (int candBoard , int icand){
      m_candRegister->set(candBoard  ,BitsCandBoard  ,ShiftCandBoard +icand*BitsCandTot);
    }  
    void setCandStatus(int candStatus           ){
      m_candRegister->set(candStatus ,BitsCandStatus ,ShiftCandStatus);
    }  

    void setMuonCandidate(PMuonCandidate cand, int icand);

    /// Get the candidate's properties from the register
    int getCandAddM3 (int icand){
      return m_candRegister->getulong(BitsCandAddM3  ,ShiftCandAddM3 +icand*BitsCandTot);
    }  
    int getCandOffM2 (int icand){
      return m_candRegister->getulong(BitsCandOffM2  ,ShiftCandOffM2 +icand*BitsCandTot);
    }  
    int getCandOffM1 (int icand){
      return m_candRegister->getulong(BitsCandOffM1  ,ShiftCandOffM1 +icand*BitsCandTot);
    }  
    int getCandPT    (int icand){
      return m_candRegister->getulong(BitsCandPT     ,ShiftCandPT    +icand*BitsCandTot);
    }  
    int getCandCharge(int icand){
      return m_candRegister->getulong(BitsCandCharge ,ShiftCandCharge+icand*BitsCandTot);
    }  
    int getCandPU    (int icand){
      return m_candRegister->getulong(BitsCandPU     ,ShiftCandPU    +icand*BitsCandTot);
    }  
    int getCandBoard (int icand){
      return m_candRegister->getulong(BitsCandBoard  ,ShiftCandBoard +icand*BitsCandTot);
    }
    int getCandStatus(         ){
      return m_candRegister->getulong(BitsCandStatus ,ShiftCandStatus  );
    }  

    PMuonCandidate getMuonCandidate(int icand);

    int numberOfCandidates(){
      int ncand=0;
      for (int icand=0; icand<2; icand++){
        if (!isEmpty(icand)) ncand++;
      }
      return ncand;
    }  

    // Return true if the cand is empty (no bit set)
    bool isEmpty(int icand)
    {
      boost::dynamic_bitset<> cand = m_candRegister->getBitset(BitsCandTot,icand*BitsCandTot);
      return cand.none();
    }
    
    void dump(int icand=-1,std::string tab="")
    {      
      if (icand==0 || icand==1) {        
        std::cout <<tab<<"icand="<<icand;
        if ( isEmpty(icand) ){
          std::cout << " X ";
        } else {
          std::cout << " Pu= "<<getCandPU(icand);
          std::cout << " Board= "<<getCandBoard(icand);
          std::cout << " AddM3= "<<getCandAddM3(icand);
          std::cout << " OffM2= "<<getCandOffM2(icand);
          std::cout << " OffM1= "<<getCandOffM1(icand);
          std::cout << " Pt= "<<getCandPT(icand);
          if (getCandCharge(icand)>0)
            std::cout << " + ";
          else
            std::cout << " - ";
        }
        std::cout <<"\n";
//         std::cout <<tab<<"        bitset: "<<m_candRegister->getBitset(BitsCandTot,icand*BitsCandTot)<<"\n"; 
      } else {
        dump(0,tab);
        dump(1,tab);
        std::cout <<tab<< "status= "<<getCandStatus()<<"\n";
      }
      
    }
    
    
 
  private:
    Register * m_candRegister;
    
  };



}; // namespace L0Muon

#endif      // L0MUONKERNEL_CANDREGISTERHANDLER_H
