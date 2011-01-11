// $Id: L0DUBase.h,v 1.10 2010-02-12 23:41:48 odescham Exp $
//
#ifndef      L0MUON_L0DUBASE_H
#define      L0MUON_L0DUBASE_H 1
#include <string>

namespace L0DUBase{
  

  /*
    L0DU firmware limitations
  */
  namespace NumberOf{
    static const unsigned int Compounds    = 8;
    static const unsigned int Data        = 27;
    static const unsigned int Channels    = 32;
    static const unsigned int Conditions  =128;
    static const unsigned int ConditionsInBank  =32;
  }  

  namespace CompoundData{
    enum Type {None=0,CaloEt,CaloAdd,PuCont,PuPos,MuonPt,MuonAdd,MuonSgn};
    static const std::string Name[NumberOf::Compounds] = {"None",
                                                          "CompoundCaloEt",
                                                          "CompoundCaloAdd", 
                                                          "CompoundPuCont",
                                                          "CompoundPuPos",
                                                          "CompoundMuonPt",
                                                          "CompoundMuonAdd",
                                                          "CompoundMuonSgn"};
    

    static const unsigned int MaxNumber[NumberOf::Compounds] = { 0, 4, 10, 4, 2, 4, 6, 6 };  
    static const unsigned int ConditionOrder[NumberOf::Compounds]  = { 0, 18,19,20,21,22,23,24 };  
    static const unsigned int OperatorType[NumberOf::Compounds]= { 0, 1,2,1,1,1,2,1 };  
  }
  
  namespace PredefinedData{
    enum Type {ElectronEt=0,PhotonEt, GlobalPi0Et,LocalPi0Et,HadronEt,
               SumEt,SpdMult,
               PuPeak1,PuPeak2,
               PuPeak1Pos,PuPeak2Pos,
               PuHits,
               Muon1Pt,Muon2Pt,Muon3Pt,DiMuonPt,
               ElectronAdd,PhotonAdd,GlobalPi0Add,LocalPi0Add,HadronAdd,
               Muon1Add,Muon2Add,Muon3Add,
               Muon1Sgn,Muon2Sgn,Muon3Sgn};

    static const std::string Name[NumberOf::Data]= {
      "Electron(Et)", "Photon(Et)", "GlobalPi0(Et)", "LocalPi0(Et)","Hadron(Et)","Sum(Et)","Spd(Mult)",
      "PUPeak1(Cont)","PUPeak2(Cont)","PUPeak1(Pos)","PUPeak2(Pos)","PUHits(Mult)",
      "Muon1(Pt)","Muon2(Pt)","Muon3(Pt)","DiMuon(Pt)",
      "Electron(Add)", "Photon(Add)", "GlobalPi0(Add)", "LocalPi0(Add)","Hadron(Add)",
      "Muon1(Add)","Muon2(Add)","Muon3(Add)",
      "Muon1(Sgn)","Muon2(Sgn)","Muon3(Sgn)",
    };
    static const unsigned int CompoundType[NumberOf::Data]={
      CompoundData::CaloEt,CompoundData::CaloEt,CompoundData::CaloEt,CompoundData::CaloEt,CompoundData::CaloEt,
      CompoundData::None,CompoundData::None,
      CompoundData::PuCont,CompoundData::PuCont,
      CompoundData::PuPos,CompoundData::PuPos,
      CompoundData::None,
      CompoundData::MuonPt,CompoundData::MuonPt,CompoundData::MuonPt,CompoundData::MuonPt,
      CompoundData::CaloAdd,CompoundData::CaloAdd,CompoundData::CaloAdd,CompoundData::CaloAdd,CompoundData::CaloAdd,
      CompoundData::MuonAdd,CompoundData::MuonAdd,CompoundData::MuonAdd,
      CompoundData::MuonSgn,CompoundData::MuonSgn,CompoundData::MuonSgn
    };
    static const unsigned int MaxNumber[NumberOf::Data]={8,6,5,5,8,
                                                         6,6,
                                                         4,4,
                                                         2,2,
                                                         6,
                                                         8,6,4,8,
                                                         0,0,0,0,0,
                                                         0,0,0,
                                                         0,0,0};
    
    static const unsigned int ConditionOrder[NumberOf::Data]={1,2,3,4,5,
                                                              6,7,
                                                              8,9,10,
                                                     11,12,
                                                              13,14,15,16,
                                                              // BCID + compoound Data inserted here
                                                              25,26,27,28,29,
                                                              30,31,32,
                                                     33,34,35};  
    }
  
  namespace RAMBCID{
    static const unsigned int Max = 255;
    static const unsigned int ConditionOrder = 17;
    static const unsigned int MaxNumber = 4;
    static const std::string  Name = "RamBCID";
  }
  

  /*
    Empty L0Processor data  
   */
  static const unsigned int EmptyData = 1 << 16;

  /*
    Calo candidate types as defined in EDMS-845277
  */
  namespace CaloType {
    enum Type {Electron=0, Photon, Hadron,Pi0Local,Pi0Global,SumEt,SpdMult,
               HadronSlave1Out,HadronSlave2Out,HadronSlave1In,HadronSlave2In,
               SumEtSlave1Out,SumEtSlave2Out,SumEtSlave1In,SumEtSlave2In};
  }
  
  /*
    L0DU Fiber numbering : FOLLOW THE HARDWARE ORDERING (needed to decode the Status bit from raw)
  */
  namespace Fiber {
    enum Type{ Pu1=0,Pu2,CaloSumEt,CaloSpdMult,CaloHadron,CaloPi0Global,CaloPi0Local,CaloPhoton,CaloElectron,Spare1,Spare2,Spare3
               , MuonCU0,MuonSU0,MuonCU1,MuonSU1,MuonCU2,MuonSU2,MuonCU3,MuonSU3,Spare4, Spare5, Spare6, Spare7
               , Empty }; // MUST BE ENDED WITH 'EMPTY' fibre
  }

  /*
     Data scale :
  */
  namespace Type {
    enum Scale{ Digit = 0 ,  MuonPt , CaloEt };
  }
  
  
  /*
    Shifts & Masks needed for the definition of the L0Processor Data Pattern
  */
  
  namespace Calo {
    namespace Et      { static const unsigned int Mask  = 0x1FE      , Shift = 1;  } // e,p,h,pi0G,pi0L
    namespace Address { static const unsigned int Mask  = 0x7FFE0000 , Shift = 17; }
    namespace BCID    { static const unsigned int Mask  = 0xFE00     , Shift = 9; }
    namespace Status  { static const unsigned int Mask  = 0x80000000 , Shift = 31; }
    namespace Sum     { static const unsigned int Mask  = 0x7FFE0000 , Shift = 17; } // SpdMult & SumEt 
  }
  namespace Muon {
    // from CU
    namespace Pt2      { static const unsigned int Mask  = 0xFE       , Shift = 1;  }
    namespace Address2 { static const unsigned int Mask  = 0x7F00     , Shift = 8;  }
    namespace BCID0    { static const unsigned int Mask  = 0x8000     , Shift = 15;  }
    namespace Pt1      { static const unsigned int Mask  = 0xFE0000   , Shift = 17; }
    namespace Address1 { static const unsigned int Mask  = 0x7F000000 , Shift = 24; }
    namespace BCID1    { static const unsigned int Mask  = 0x80000000 , Shift = 31;  }
    // from SU
    namespace Pu2      { static const unsigned int Mask  = 0x6       , Shift = 1; } // PU address
    namespace Pb2      { static const unsigned int Mask  = 0x78      , Shift = 3; } // PB address
    namespace Sign2    { static const unsigned int Mask  = 0x80      , Shift = 7; }
    namespace BCID     { static const unsigned int Mask  = 0xFE00    , Shift = 9; }
    namespace Pu1      { static const unsigned int Mask  = 0x60000   , Shift = 17;} // PU address
    namespace Pb1      { static const unsigned int Mask  = 0x780000  , Shift = 19;} // PB address
    namespace Sign1    { static const unsigned int Mask  = 0x800000  , Shift = 23; }
    namespace Status   { static const unsigned int Mask  = 0xF0000000, Shift = 28; }
    static const unsigned int AddressSize = 7;
    static const unsigned int BCID0Size = 7;
  }
  // L0Pu
  namespace L0Pu    {
    //  Same Pattern for both content and address of Peak1 and  Peak2 
    namespace Peak    { static const unsigned int Mask  = 0x1FE      , Shift = 1;  }
    namespace Address { static const unsigned int Mask  = 0x1FE0000  , Shift = 17; }
    namespace Status  { static const unsigned int Mask  = 0x80000000 , Shift = 31; }
    // specific to Pu1 
    namespace BCID1   { static const unsigned int Mask  = 0xFE00     , Shift = 9;  }
    namespace MoreInfo{ static const unsigned int Mask  = 0xF000000  , Shift = 24; }
    // specific to Pu2 
    namespace BCID2   { static const unsigned int Mask  = 0xF800     , Shift = 11;  }
    namespace HitsLSB { static const unsigned int Mask  = 0x600      , Shift = 9;  }
    namespace HitsMSB { static const unsigned int Mask  = 0x7E000000 , Shift = 25;  }
    static const unsigned int HitsLSBSize = 2 ;
  }
  
  /*
    Hard coded cross-references
  */
  
  namespace Index{
    enum Position {Fiber=0, Scale , Mask, Shift, Fiber2, Mask2, Shift2, Offset }; 
    static const int Size = 8;
  }

  namespace Electron{
    static const unsigned int  
    Et[Index::Size] = { Fiber::CaloElectron , Type::CaloEt, Calo::Et::Mask , Calo::Et::Shift , Fiber::Empty,0,0,0,},
      Address[Index::Size] = { Fiber::CaloElectron, Type::Digit ,Calo::Address::Mask, Calo::Address::Shift, Fiber::Empty,0,0,0},
        BCID[Index::Size]  = { Fiber::CaloElectron, Type::Digit ,Calo::BCID::Mask,Calo::BCID::Shift,Fiber::Empty,0,0,0},
          Status[Index::Size]  = { Fiber::CaloElectron, Type::Digit ,Calo::Status::Mask,Calo::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Photon{
    static const unsigned int  
    Et[Index::Size] = { Fiber::CaloPhoton , Type::CaloEt, Calo::Et::Mask , Calo::Et::Shift , Fiber::Empty,0,0,0},
      Address[Index::Size] = { Fiber::CaloPhoton, Type::Digit ,Calo::Address::Mask, Calo::Address::Shift, Fiber::Empty,0,0,0},
        BCID[Index::Size]  = { Fiber::CaloPhoton, Type::Digit ,Calo::BCID::Mask,Calo::BCID::Shift,Fiber::Empty,0,0,0},
          Status[Index::Size]  = { Fiber::CaloPhoton, Type::Digit ,Calo::Status::Mask,Calo::Status::Shift,Fiber::Empty,0,0,0};    
  }
  namespace Hadron{
    static const unsigned int  
    Et[Index::Size] = { Fiber::CaloHadron , Type::CaloEt, Calo::Et::Mask , Calo::Et::Shift , Fiber::Empty,0,0,0},
      Address[Index::Size] = { Fiber::CaloHadron, Type::Digit ,Calo::Address::Mask, Calo::Address::Shift, Fiber::Empty,0,0,0},
        BCID[Index::Size]  = { Fiber::CaloHadron, Type::Digit ,Calo::BCID::Mask,Calo::BCID::Shift,Fiber::Empty,0,0,0},
          Status[Index::Size]  = { Fiber::CaloHadron, Type::Digit ,Calo::Status::Mask,Calo::Status::Shift,Fiber::Empty,0,0,0};    
  }
  namespace Pi0Global{
    static const unsigned int  
    Et[Index::Size] = { Fiber::CaloPi0Global , Type::CaloEt, Calo::Et::Mask , Calo::Et::Shift , Fiber::Empty,0,0,0},
      Address[Index::Size] = { Fiber::CaloPi0Global, Type::Digit ,Calo::Address::Mask, Calo::Address::Shift, Fiber::Empty,0,0,0},
        BCID[Index::Size]  = { Fiber::CaloPi0Global, Type::Digit ,Calo::BCID::Mask,Calo::BCID::Shift,Fiber::Empty,0,0,0},
          Status[Index::Size]  = { Fiber::CaloPi0Global, Type::Digit ,Calo::Status::Mask,Calo::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Pi0Local{
    static const unsigned int  
    Et[Index::Size] = { Fiber::CaloPi0Local , Type::CaloEt, Calo::Et::Mask , Calo::Et::Shift , Fiber::Empty,0,0,0},
      Address[Index::Size] = { Fiber::CaloPi0Local, Type::Digit ,Calo::Address::Mask, Calo::Address::Shift, Fiber::Empty,0,0,0},
        BCID[Index::Size]  = { Fiber::CaloPi0Local, Type::Digit ,Calo::BCID::Mask,Calo::BCID::Shift,Fiber::Empty,0,0,0},
          Status[Index::Size]  = { Fiber::CaloPi0Local, Type::Digit ,Calo::Status::Mask,Calo::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Sum{
    static const unsigned int  
    Et[Index::Size] = { Fiber::CaloSumEt , Type::CaloEt, Calo::Sum::Mask , Calo::Sum::Shift , Fiber::Empty,0,0,0},
      BCID[Index::Size]  = { Fiber::CaloSumEt, Type::Digit ,Calo::BCID::Mask,Calo::BCID::Shift,Fiber::Empty,0,0,0},
        Status[Index::Size]  = { Fiber::CaloSumEt, Type::Digit ,Calo::Status::Mask,Calo::Status::Shift,Fiber::Empty,0,0,0};    
  }
  namespace Spd{
    static const unsigned int  
    Mult[Index::Size] = { Fiber::CaloSpdMult, Type::Digit, Calo::Sum::Mask , Calo::Sum::Shift , Fiber::Empty,0,0,0},
      BCID[Index::Size]  = { Fiber::CaloSpdMult, Type::Digit ,Calo::BCID::Mask,Calo::BCID::Shift,Fiber::Empty,0,0,0},
        Status[Index::Size]  = { Fiber::CaloSpdMult, Type::Digit ,Calo::Status::Mask,Calo::Status::Shift,Fiber::Empty,0,0,0};    
  }

  namespace Muon1{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU0, Type::MuonPt,Muon::Pt1::Mask, Muon::Pt1::Shift, Fiber::Empty, 0 , 0 , 0 },
      Sign[Index::Size]= { Fiber::MuonSU0, Type::Digit ,Muon::Sign1::Mask, Muon::Sign1::Shift, Fiber::Empty, 0 , 0 , 0 },
        Address[Index::Size] = { Fiber::MuonCU0, Type::Digit ,Muon::Address1::Mask , Muon::Address1::Shift , 
                                 Fiber::MuonSU0, Muon::Pu1::Mask|Muon::Pb1::Mask , Muon::Pu1::Shift,
                                 Muon::AddressSize},
          BCID1[Index::Size]    = { Fiber::MuonCU0, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                    Fiber::Empty                , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
            BCID2[Index::Size]    = { Fiber::MuonSU0, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
              Status[Index::Size]  = { Fiber::MuonSU0, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
    
  }
  namespace Muon2{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU0, Type::MuonPt,Muon::Pt2::Mask, Muon::Pt2::Shift, Fiber::Empty, 0 , 0 , 0 },
      Sign[Index::Size]= { Fiber::MuonSU0, Type::Digit ,Muon::Sign2::Mask, Muon::Sign2::Shift, Fiber::Empty, 0 , 0 , 0 },
        Address[Index::Size] = { Fiber::MuonCU0, Type::Digit ,Muon::Address2::Mask , Muon::Address2::Shift , 
                                 Fiber::MuonSU0, Muon::Pu2::Mask|Muon::Pb2::Mask ,
                                 Muon::Pu2::Shift,Muon::AddressSize},
          BCID1[Index::Size]    = { Fiber::MuonCU0, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                    Fiber::MuonCU0              , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
            BCID2[Index::Size]    = { Fiber::MuonSU0, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
              Status[Index::Size]  = { Fiber::MuonSU0, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Muon3{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU1, Type::MuonPt,Muon::Pt1::Mask, Muon::Pt1::Shift, Fiber::Empty, 0 , 0 , 0 },
      Sign[Index::Size]= { Fiber::MuonSU1, Type::Digit ,Muon::Sign1::Mask, Muon::Sign1::Shift, Fiber::Empty, 0 , 0 , 0 },
        Address[Index::Size] = { Fiber::MuonCU1, Type::Digit ,Muon::Address1::Mask , Muon::Address1::Shift , 
                                 Fiber::MuonSU1, Muon::Pu1::Mask|Muon::Pb1::Mask ,
                                 Muon::Pu1::Shift,Muon::AddressSize},
          BCID1[Index::Size]    = { Fiber::MuonCU1, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                    Fiber::MuonCU1              , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
            BCID2[Index::Size]    = { Fiber::MuonSU1, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
              Status[Index::Size]  = { Fiber::MuonSU1, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Muon4{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU1, Type::MuonPt,Muon::Pt2::Mask, Muon::Pt2::Shift, Fiber::Empty, 0 , 0 , 0 },
      Sign[Index::Size]= { Fiber::MuonSU1, Type::Digit ,Muon::Sign2::Mask, Muon::Sign2::Shift, Fiber::Empty, 0 , 0 , 0 },
        Address[Index::Size] = { Fiber::MuonCU1, Type::Digit ,Muon::Address2::Mask , Muon::Address2::Shift , 
                                 Fiber::MuonSU1, Muon::Pu2::Mask|Muon::Pb2::Mask , Muon::Pu2::Shift,
                                 Muon::AddressSize},
          BCID1[Index::Size]    = { Fiber::MuonCU1, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                    Fiber::MuonCU1              , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
            BCID2[Index::Size]    = { Fiber::MuonSU1, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
              Status[Index::Size]  = { Fiber::MuonSU1, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
  }

  namespace Muon5{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU2, Type::MuonPt,Muon::Pt1::Mask, Muon::Pt1::Shift, Fiber::Empty, 0 , 0 , 0 },
      Sign[Index::Size]= { Fiber::MuonSU2, Type::Digit ,Muon::Sign1::Mask, Muon::Sign1::Shift, Fiber::Empty, 0 , 0 , 0 },
        Address[Index::Size] = { Fiber::MuonCU2, Type::Digit ,Muon::Address1::Mask , Muon::Address1::Shift , 
                                 Fiber::MuonSU2, Muon::Pu1::Mask|Muon::Pb1::Mask , Muon::Pu1::Shift, 
                                 Muon::AddressSize},
          BCID1[Index::Size]    = { Fiber::MuonCU2, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                    Fiber::MuonCU2              , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
            BCID2[Index::Size]    = { Fiber::MuonSU2, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
              Status[Index::Size]  = { Fiber::MuonSU2, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Muon6{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU2, Type::MuonPt,Muon::Pt2::Mask, Muon::Pt2::Shift, Fiber::Empty, 0 , 0 , 0 },
      Sign[Index::Size]= { Fiber::MuonSU2, Type::Digit ,Muon::Sign2::Mask, Muon::Sign2::Shift, Fiber::Empty, 0 , 0 , 0 },
        Address[Index::Size] = { Fiber::MuonCU2, Type::Digit ,Muon::Address2::Mask , Muon::Address2::Shift , 
                                 Fiber::MuonSU2, Muon::Pu2::Mask|Muon::Pb2::Mask , Muon::Pu2::Shift,
                                 Muon::AddressSize},
          BCID1[Index::Size]    = { Fiber::MuonCU2, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                    Fiber::MuonCU2              , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
            BCID2[Index::Size]    = { Fiber::MuonSU2, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
              Status[Index::Size]  = { Fiber::MuonSU2, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Muon7{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU3, Type::MuonPt,Muon::Pt1::Mask, Muon::Pt1::Shift, Fiber::Empty, 0 , 0 , 0 },
    Sign[Index::Size]= { Fiber::MuonSU3, Type::Digit ,Muon::Sign1::Mask, Muon::Sign1::Shift, Fiber::Empty, 0 , 0 , 0 },
    Address[Index::Size] = { Fiber::MuonCU3, Type::Digit ,Muon::Address1::Mask , Muon::Address1::Shift , 
                             Fiber::MuonSU3, Muon::Pu1::Mask|Muon::Pb1::Mask , Muon::Pu1::Shift,
                             Muon::AddressSize},
      BCID1[Index::Size]    = { Fiber::MuonCU3, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                Fiber::MuonCU3              , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
        BCID2[Index::Size]    = { Fiber::MuonSU3, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
          Status[Index::Size]  = { Fiber::MuonSU3, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
  }
  namespace Muon8{
    static const unsigned int  
    Pt[Index::Size]  = { Fiber::MuonCU3, Type::MuonPt,Muon::Pt2::Mask, Muon::Pt2::Shift, Fiber::Empty, 0 , 0 , 0 },
    Sign[Index::Size]= { Fiber::MuonSU3, Type::Digit ,Muon::Sign2::Mask, Muon::Sign2::Shift, Fiber::Empty, 0 , 0 , 0 },
    Address[Index::Size] = { Fiber::MuonCU3, Type::Digit ,Muon::Address2::Mask , Muon::Address2::Shift , 
                             Fiber::MuonSU3, Muon::Pu2::Mask|Muon::Pb2::Mask , Muon::Pu2::Shift,
                             Muon::AddressSize},
      BCID1[Index::Size]    = { Fiber::MuonCU3, Type::Digit , Muon::BCID0::Mask  , Muon::BCID0::Shift  ,
                                Fiber::MuonCU3              , Muon::BCID1::Mask  , Muon::BCID1::Shift  , Muon::BCID0Size},
        BCID2[Index::Size]    = { Fiber::MuonSU3, Type::Digit , Muon::BCID::Mask  , Muon::BCID::Shift  ,Fiber::Empty,0,0,0},
          Status[Index::Size]  = { Fiber::MuonSU3, Type::Digit , Muon::Status::Mask, Muon::Status::Shift,Fiber::Empty,0,0,0};
  }





  namespace PileUp{
    static const unsigned int  
    Peak1[Index::Size] = { Fiber::Pu1, Type::Digit , L0Pu::Peak::Mask, L0Pu::Peak::Shift, Fiber::Empty, 0 , 0 , 0  }, 
    Peak2[Index::Size] = { Fiber::Pu2, Type::Digit , L0Pu::Peak::Mask, L0Pu::Peak::Shift, Fiber::Empty, 0 , 0 , 0  },
    Peak1Pos[Index::Size] = { Fiber::Pu1, Type::Digit , L0Pu::Address::Mask, L0Pu::Address::Shift, Fiber::Empty, 
                              0 , 0 , 0 },
    Peak2Pos[Index::Size] = { Fiber::Pu2, Type::Digit , L0Pu::Address::Mask, L0Pu::Address::Shift, Fiber::Empty, 
                                0 , 0 , 0 },
    Hits[Index::Size]  = {Fiber::Pu2, Type::Digit, L0Pu::HitsLSB::Mask, L0Pu::HitsLSB::Shift, 
                              Fiber::Pu2, L0Pu::HitsMSB::Mask, L0Pu::HitsMSB::Shift, L0Pu::HitsLSBSize  },   
    BCID1[Index::Size]   = { Fiber::Pu1, Type::Digit , L0Pu::BCID1::Mask  , L0Pu::BCID1::Shift  ,Fiber::Empty,0,0,0},
      BCID2[Index::Size]   = { Fiber::Pu2, Type::Digit , L0Pu::BCID2::Mask  , L0Pu::BCID2::Shift  ,Fiber::Empty,0,0,0},
        Status1[Index::Size] = { Fiber::Pu1, Type::Digit , L0Pu::Status::Mask , L0Pu::Status::Shift  ,Fiber::Empty,0,0,0},
          Status2[Index::Size] = { Fiber::Pu2, Type::Digit , L0Pu::Status::Mask , L0Pu::Status::Shift  ,Fiber::Empty,0,0,0},
            MoreInfo[Index::Size]= { Fiber::Pu1, Type::Digit , L0Pu::MoreInfo::Mask, L0Pu::MoreInfo::Shift,Fiber::Empty,0,0,0};
  }
}

#endif // L0MUON_L0DUBASE_H
