// $Id: VeloDecodeConf.h,v 1.2 2007-12-09 22:03:00 szumlat Exp $
#ifndef VELODECODECONF_H 
#define VELODECODECONF_H 1

// Include files

/** @namespace VeloDecodeConf VeloDecodeConf.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2006-03-23
 */

namespace VeloTELL1
{
  // general parameters describing layout of the data section
  enum sections{
    SectionBShift=1,
    SectionCShift,
    SectionsPerBlock,
    EventInfoLength=8,
    NumberOfALinks=64,
    ALL_ALINKS=72,
    PPFPGAPedBlock=192,
    PPFPGADataSection=216
  };
  // parameters related to organization of the sections within data stream
  enum blocks{
    NumberOfPPFPGA=4,
    PPFPGADataBlock=224
  };
  // banks types  
  enum BankTypes{
    VeloFull=18,
    VeloError=29,
    VeloPedestal=30,
    VeloHeader=40,   // this is not a real bank! 
    VeloEvtInfo=50   // added for the sake of simplicity during decoding
  };
  // details related to organization of the words within the data section
  enum Decoding{
    ALinkPerDataStream=2,
    DataStreamPerSection=3,
    ADCHeaders=4,
    ADCShift=10,
    CHANNELS=32,
    ALinkShift=36,
    ADCMask=0x000003ff
  };
  // entry dedicated to cope with asymmetric section C
  enum emptySection{
    emptyField=2
  };
  // error bank
  enum errorBank{
    NextMarker=1,
    FirstMarkerLocation=4,
    EmptyBankSize=28,
    FullBankSize=52
  };
  // errorBankJumps -  distance in 32 bit words to next PPFPGA section
  // in current Error Bank
  enum errorBankJumps{
    NextBankIfEmptyJump=2,
    BankJump=8,
    PPFPGASectionJump=13
  };
  //
  enum pedSubConsts{
    BIT_PERFECT_SUBTRACTOR=0,
    FAST_SUBTRACTOR=1,
    CONVERGENCE_LIMIT=6500,    // number of events needed for ped subtractor to
    FAST_CONVERGENCE_LIMIT=200  // get stable value for pedestal
  };
  // numbers of bits in ProcessInfo field in Event Info block
  enum processing{ 
    PED_SUBTRACTION=2,
    COMMON_MODE_SUBTRACTION=3,
    BIT_LIMIT=4,
    FIR_FILTER=6,
    CHANNEL_REORDERING=7,
    CLUSTER_MAKER=8
  };
  //
  enum dataTypes{
    SIM_DATA=0,
    REAL_DATA=1,
    DEV_DATA=2
  };
  //
  // const from reordering (facilitate using of the convertChannel tool)
  enum modes{
    NO_REORDERING=0,
    PHI_SENSOR=1,
    R_SENSOR=2,
    PILE_UP=3
  };
  //
  // these parameters govern of the reordering algorithm behaviour
  // first one allows to calculate channel position before reordering
  // using it's new position, second one has analogous meaning  
  enum directions{
    NEW_TO_OLD_POSITION=0,
    OLD_TO_NEW_POSITION=1
  };
  // number of distinc stages for digitalization, 10 bit width band
  enum digiDyn{
    MAX_ADC_8_BIT=127,
    MIN_ADC_8_BIT=128,    // use it as -MIN_ADC_8_BIT!!
    MAX_ADC_9_BIT=255,
    MIN_ADC_9_BIT=256,
    MAX_ADC_10_BIT=511,
    MIN_ADC_10_BIT=512,
    DIGIT_MAX_COUNT=1023
  };
  //  
  enum clusterization{
    DUMMY_STRIPS=64,
    STRIPS_IN_PROC_CHANNEL=64,  /// number of strips per PPFPGA proc. channel
    STRIPS_IN_PPFPGA=512,
    SENSOR_CHANNELS=2048
  };
  // test run for VeloTELL1Algorithms
  enum testing{
    TEST_RUN=0,
    PROCESSING_RUN=1
  };
  // saturation modes
  enum saturation{
    CUT_MSB=0,
    CUT_MIDDLE=1,
    CUT_LSB=2
  };
}
#endif // VELODECODECONF_H
//
