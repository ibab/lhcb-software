// $Id: CandRegisterHandler.h,v 1.8 2008-07-15 12:48:04 jucogan Exp $

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


  class CandRegisterHandler  {
  
  private:
    static const ContentType BitsCandOffM1   = 4 ;
    static const ContentType BitsCandOffM2   = 4 ;
    static const ContentType BitsCandColM3   = 5 ;
    static const ContentType BitsCandRowM3   = 2 ;
    static const ContentType BitsCandPadding = 1 ;
    static const ContentType BitsCandPT      = 7 ;
    static const ContentType BitsCandCharge  = 1 ;
    static const ContentType BitsCandBoard   = 4 ;
    static const ContentType BitsCandPU      = 2 ;
    static const ContentType BitsCandQuarter = 2 ;
                                             
    static const ContentType BitsStatus      = 4 ;
    
    static const ContentType ShiftCandOffM1  = 0;                                                  //  0
    static const ContentType ShiftCandOffM2  = ShiftCandOffM1  + BitsCandOffM1  ;                  //  4
    static const ContentType ShiftCandColM3  = ShiftCandOffM2  + BitsCandOffM2  ;                  //  8
    static const ContentType ShiftCandRowM3  = ShiftCandColM3  + BitsCandColM3  ;                  // 13
    static const ContentType ShiftCandPT     = ShiftCandRowM3  + BitsCandRowM3  + BitsCandPadding; // 16
    static const ContentType ShiftCandCharge = ShiftCandPT     + BitsCandPT     ;                  // 23
    static const ContentType ShiftCandBoard  = ShiftCandCharge + BitsCandCharge ;                  // 24
    static const ContentType ShiftCandPU     = ShiftCandBoard  + BitsCandBoard  ;                  // 28   
    static const ContentType ShiftCandQuarter= ShiftCandPU     + BitsCandPU     ;                  // 30

    static const ContentType BitsCandTot     = ShiftCandQuarter+ BitsCandQuarter;                  // 32
    static const ContentType BitsCandRegTot  = 2*BitsCandTot + BitsStatus;
    
    static const ContentType ShiftStatus     = 2*BitsCandTot;

    static const ContentType BitsCandAddM3   = BitsCandColM3 + BitsCandRowM3; 
    static const ContentType ShiftCandAddM3  = ShiftCandColM3;

  public:
    

    /// Default Constructor 
    CandRegisterHandler();
      
    /// Constructor with a pattern
    CandRegisterHandler(Register * candRegister);    

    /// Clear the candidate register
    void clear(){
      m_candRegister->reset();
    }

    bool isValid()
    {
      return (m_candRegister!=0);
    }
    

    /// Candidate register size in bits
    static const int size() {return BitsCandRegTot;}   

    /// Set the candidate's properties in the register
    void setCandColM3 (int candColM3 , int icand){
      m_candRegister->set(candColM3  ,BitsCandColM3  ,ShiftCandColM3 +icand*BitsCandTot);
    }  
    void setCandRowM3 (int candRowM3 , int icand){
      m_candRegister->set(candRowM3  ,BitsCandRowM3  ,ShiftCandRowM3 +icand*BitsCandTot);
    }  
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
    void setCandQuarter (int candQuarter , int icand){
      m_candRegister->set(candQuarter  ,BitsCandQuarter  ,ShiftCandQuarter +icand*BitsCandTot);
    }  
    void setStatus(int status ){
      m_candRegister->set(status ,BitsStatus ,ShiftStatus);
    }  

    void setMuonCandidate(PMuonCandidate cand, int icand);

    /// Get the candidate's properties from the register
    int getCandColM3(int icand){
      return m_candRegister->getulong(BitsCandColM3  ,ShiftCandColM3 +icand*BitsCandTot);
    }  
    int getCandRowM3(int icand){
      return m_candRegister->getulong(BitsCandRowM3  ,ShiftCandRowM3 +icand*BitsCandTot);
    }  
    int getCandAddM3(int icand){
      return m_candRegister->getulong(BitsCandAddM3  ,ShiftCandAddM3 +icand*BitsCandTot);
    }  
    int getCandOffM2(int icand){
      return m_candRegister->getulong(BitsCandOffM2  ,ShiftCandOffM2 +icand*BitsCandTot);
    }  
    int getCandOffM1(int icand){
      return m_candRegister->getulong(BitsCandOffM1  ,ShiftCandOffM1 +icand*BitsCandTot);
    }  
    int getCandPT(int icand){
      return m_candRegister->getulong(BitsCandPT     ,ShiftCandPT    +icand*BitsCandTot);
    }  
    int getCandPTCharged(int icand){
      return m_candRegister->getulong(BitsCandPT+BitsCandCharge ,ShiftCandPT    +icand*BitsCandTot);
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
    int getCandQuarter (int icand){
      return m_candRegister->getulong(BitsCandQuarter  ,ShiftCandQuarter +icand*BitsCandTot);
    }
    int getStatus(){
      return m_candRegister->getulong(BitsStatus ,ShiftStatus  );
    }  

    PMuonCandidate getMuonCandidate(int icand);

    int numberOfCandidates(bool useStatus=true) ;
    
    // Return true if the cand is empty (no bit set)
    bool isEmpty(int icand)
    {
      boost::dynamic_bitset<> cand = m_candRegister->getBitset(BitsCandTot,icand*BitsCandTot);
      return cand.none();
    }
    
    void dump(int icand=-1,std::string tab="") ;
    
 
  private:
    Register * m_candRegister;
    
  };



}; // namespace L0Muon

#endif      // L0MUONKERNEL_CANDREGISTERHANDLER_H
