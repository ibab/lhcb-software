// $Id: CandRegisterHandler.h,v 1.10 2008-12-09 18:47:28 cattanem Exp $

#ifndef L0MUONKERNEL_CANDREGISTERHANDLER_H
#define L0MUONKERNEL_CANDREGISTERHANDLER_H     1


#include <iostream>
#include "ProcessorKernel/Register.h"
#include <boost/dynamic_bitset.hpp>
#include "L0MuonKernel/MuonCandidate.h"

namespace L0Muon {

  typedef unsigned int   ContentType ;


  /** @class CandRegisterHandler CandRegisterHandler.h L0MuonKernel/CandRegisterHandler.h

      Class to handle register containing information on pair of candidates of a FPGA.

      It allows to store or retrieve in a Register any information concerning the candidates.
      It also deals with the status information.

  */
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

    /// Return true if the Register is not empty
    bool isValid()
    {
      return (m_candRegister!=0);
    }

    /// Candidate register size in bits
    static int size() {return BitsCandRegTot;}

    //
    // Set the candidate's properties from the register
    //

    /** Set in the register the candidate's adress in M3 (column)

        @param candColM3 : column index of the candidate in M3
        @param icand : index of the candidate (0,1)
    */
    void setCandColM3 (int candColM3 , int icand){
      m_candRegister->set(candColM3  ,BitsCandColM3  ,ShiftCandColM3 +icand*BitsCandTot);
    }

    /** Set in the register the candidate's adress in M3 (row)

        @param candRowM3 : row index of the candidate in M3
        @param icand : index of the candidate (0,1)
    */
    void setCandRowM3 (int candRowM3 , int icand){
      m_candRegister->set(candRowM3  ,BitsCandRowM3  ,ShiftCandRowM3 +icand*BitsCandTot);
    }

    /** Set in the register the candidate's adress in M3

        @param candAddM3 : adress of the candidate in M3 (row and column)
        @param icand : index of the candidate (0,1)
    */
    void setCandAddM3 (int candAddM3 , int icand){
      m_candRegister->set(candAddM3  ,BitsCandAddM3  ,ShiftCandAddM3 +icand*BitsCandTot);
    }

    /** Set in the register the candidate's offset in M2

        @param candOffM2 : offset of the candidate in M2
        @param icand : index of the candidate (0,1)
    */
    void setCandOffM2 (int candOffM2 , int icand){
      m_candRegister->set(candOffM2  ,BitsCandOffM2  ,ShiftCandOffM2 +icand*BitsCandTot);
    }

    /** Set in the register the candidate's offset in M1

        @param candOffM1 : offset of the candidate in M1
        @param icand : index of the candidate (0,1)
    */
    void setCandOffM1 (int candOffM1 , int icand){
      m_candRegister->set(candOffM1  ,BitsCandOffM1  ,ShiftCandOffM1 +icand*BitsCandTot);
    }

    /** Set in the register the candidate's pt (absolute value)

        @param candPT : absolute value of the candidate pt
        @param icand : index of the candidate (0,1)
    */
    void setCandPT    (int candPT    , int icand){
      m_candRegister->set(candPT     ,BitsCandPT     ,ShiftCandPT    +icand*BitsCandTot);
    }

    /** Set in the register the candidate's charge (0 for negative, 1 for positive)

        @param candCharge : candidate's charge (0 for negative, 1 for positive)
        @param icand : index of the candidate (0,1)
    */
    void setCandCharge(int candCharge, int icand){
      m_candRegister->set(candCharge ,BitsCandCharge ,ShiftCandCharge+icand*BitsCandTot);
    }

    /** Set in the register the candidate's PU index (0 to 3)

        @param candPU : candidate's PU index (0 to 3)
        @param icand : index of the candidate (0,1)
    */
    void setCandPU    (int candPU    , int icand){
      m_candRegister->set(candPU     ,BitsCandPU     ,ShiftCandPU    +icand*BitsCandTot);
    }

    /** Set in the register the candidate's board index (0 to 11)

        @param candBoard : candidate's board index (0 to 11)
        @param icand : index of the candidate (0,1)
    */
    void setCandBoard (int candBoard , int icand){
      m_candRegister->set(candBoard  ,BitsCandBoard  ,ShiftCandBoard +icand*BitsCandTot);
    }

    /** Set in the register the candidate's quarter (0 to 3)

        @param candQuarter : candidate's quarter (0 to 3)
        @param icand : index of the candidate (0,1)
    */
    void setCandQuarter (int candQuarter , int icand){
      m_candRegister->set(candQuarter  ,BitsCandQuarter  ,ShiftCandQuarter +icand*BitsCandTot);
    }

    /** Set in the register the status bits

        @param status : status
        @param icand : index of the candidate (0,1)
    */
    void setStatus(int status ){
      m_candRegister->set(status ,BitsStatus ,ShiftStatus);
    }

    /** Set in the register the full candidate information

        @param cand : PMuonCandidate representing the candidate
        @param icand : index of the candidate (0,1)
    */
    void setMuonCandidate(PMuonCandidate cand, int icand);

    //
    // Get the candidate's properties from the register
    //

    /** Get from the register the candidate's adress in M3 (column)

        @param icand : index of the candidate (0,1)
    */
    int getCandColM3(int icand){
      return m_candRegister->getulong(BitsCandColM3  ,ShiftCandColM3 +icand*BitsCandTot);
    }

    /** Get from the register the candidate's adress in M3 (row)

        @param icand : index of the candidate (0,1)
    */
    int getCandRowM3(int icand){
      return m_candRegister->getulong(BitsCandRowM3  ,ShiftCandRowM3 +icand*BitsCandTot);
    }

    /** Get from the register the candidate's adress in M3

        @param icand : index of the candidate (0,1)
    */
    int getCandAddM3(int icand){
      return m_candRegister->getulong(BitsCandAddM3  ,ShiftCandAddM3 +icand*BitsCandTot);
    }

    /** Get from the register the candidate's offset in M2

        @param icand : index of the candidate (0,1)
    */
    int getCandOffM2(int icand){
      return m_candRegister->getulong(BitsCandOffM2  ,ShiftCandOffM2 +icand*BitsCandTot);
    }

    /** Get from the register the candidate's offset in M1

        @param icand : index of the candidate (0,1)
    */
    int getCandOffM1(int icand){
      return m_candRegister->getulong(BitsCandOffM1  ,ShiftCandOffM1 +icand*BitsCandTot);
    }

    /** Get from the register the candidate's pt (absolute value)

        @param icand : index of the candidate (0,1)
    */
    int getCandPT(int icand){
      return m_candRegister->getulong(BitsCandPT     ,ShiftCandPT    +icand*BitsCandTot);
    }

    /** Get from the register the candidate's pt (signed value)

        @param icand : index of the candidate (0,1)
    */
    int getCandPTCharged(int icand){
      return m_candRegister->getulong(BitsCandPT+BitsCandCharge ,ShiftCandPT    +icand*BitsCandTot);
    }

    /** Get from the register the candidate's charge (0 for negative, 1 for positive)

        @param icand : index of the candidate (0,1)
    */
    int getCandCharge(int icand){
      return m_candRegister->getulong(BitsCandCharge ,ShiftCandCharge+icand*BitsCandTot);
    }

    /** Get from the register the candidate's PU index (0 to 3)

        @param icand : index of the candidate (0,1)
    */
    int getCandPU    (int icand){
      return m_candRegister->getulong(BitsCandPU     ,ShiftCandPU    +icand*BitsCandTot);
    }

    /** Get from the register the candidate's board index (0 to 11)

        @param icand : index of the candidate (0,1)
    */
    int getCandBoard (int icand){
      return m_candRegister->getulong(BitsCandBoard  ,ShiftCandBoard +icand*BitsCandTot);
    }

    /** Get from the register the candidate's quarter (0 to 3)

        @param icand : index of the candidate (0,1)
    */
    int getCandQuarter (int icand){
      return m_candRegister->getulong(BitsCandQuarter  ,ShiftCandQuarter +icand*BitsCandTot);
    }

    /** Get from the register the status bits

        @param status : status
        @param icand : index of the candidate (0,1)
    */
    int getStatus(){
      return m_candRegister->getulong(BitsStatus ,ShiftStatus  );
    }

    /** Get from the register the PMuonCandidate representing the candidate the full information

        @param icand : index of the candidate (0,1)
    */
    PMuonCandidate getMuonCandidate(int icand);

    /** Get the number of candidates in the Register

        @param useStatus : if true the number of candidates is extracted from the status bits,
        otherwise it is guessed from the content of the candidate address and PT

    */
    int numberOfCandidates(bool useStatus=true) ;

    /// Return true if the cand is empty (no bit set)
    bool isEmpty(int icand)
    {
      boost::dynamic_bitset<> cand = m_candRegister->getBitset(BitsCandTot,icand*BitsCandTot);
      return cand.none();
    }

    /// Dump the candidate to the standard output
    void dump(int icand=-1,std::string tab="") ;


  private:
    Register * m_candRegister; ///< Register where the candidates information is stored

  };



} // namespace L0Muon

#endif      // L0MUONKERNEL_CANDREGISTERHANDLER_H
