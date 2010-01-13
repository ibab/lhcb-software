// $Id: STTell1Core.h,v 1.4 2010-01-13 13:09:23 akeune Exp $
#ifndef STTELL1CORE_H 
#define STTELL1CORE_H 1

// Include files
#include <sys/types.h>
#include <vector>
#include <map>

/** @namespace STTell1Core STTell1Core.h Tell1Kernel/STTell1Core.h
 *  
 *
 *  @author Tomasz Szumlak, Anne Keune
 *  @date   2007-07-22
 */

namespace STTELL1{
  
  enum totalStrips{
    ALL_HEADERS=384,
    ALL_STRIPS=3072
  };
  
  enum tableLimits{
    PP_FPGA=4,
    BEETLES_PER_PP_FPGA = 6, 
    ALINKS_PER_BEETLE = 4,
    CHANNELS_PER_ALINK=32,
    HEADERS_PER_ALINK=4,
    SECTORS=48,
    ALINKS=96,
    PP_MAX_CLUSTERS=255,
    HEADER_CORRECTIONS=8,
    STRIPS_TO_CORRECT=6,
    HEADER_THRESHOLDS=2
  };

  // as defined in tell1lib: st/st_offline.h
  // ST_CLUSTER_UNIT and ST_ADC_UNIT needed by ZS module
  // TELL1 raw cluster structure (as defined in edms note EDMS-690585)
  // the implementation follows the same pattern that was adapted in
  // TELL1 library 'tell1lib'

  typedef union {
    u_int16_t All;
    struct {    //starts from bit0
      u_int16_t CLUSTER_ISP   :2;    //!< Inner strip position  
      u_int16_t CLUSTER_POS   :12;   //!< Cluster strip position  
      u_int16_t CLUSTER_SIZE  :1;    //!< Cluster size: '0'-1,2; '1'-3,4 
      u_int16_t CLUSTER_SO    :1;    //!< Spill over bit 
    } Sepe;
  }ST_CLUSTER_UNIT;
  
  typedef union {
    u_int8_t All;
    struct {    //starts from bit0
      u_int8_t  ADC_VALUE        :7;  //!< Cluster value 
      u_int8_t  ADC_EOC          :1;  //!< End of cluster bit 
    } Sepe;
  }ST_ADC_UNIT;
    
  // parameters related to clusterization - all used in ClusterMaker
  enum clusterCoding{ 
    CLUSTER_PER_WORD=2,
    ADCS_PER_WORD=4,
    ADC_SHIFT=8,
    CLUSTER_SHIFT=16
  };

  ///------------------------------------------------------------------
  /// type defs related with interfacing wrappers and
  /// c-modules
  ///------------------------------------------------------------------
  // mask link/pedestals - pedestal subtractor
  typedef u_int8_t LinkMask [ALINKS][CHANNELS_PER_ALINK];
  typedef u_int8_t LinkPedestal [ALINKS][CHANNELS_PER_ALINK];
  typedef std::vector<u_int8_t> LinkVec;
  typedef std::vector<u_int8_t>::iterator linkIT;
  typedef std::map<unsigned int,LinkVec> LinkMap;
  
  // header correction values
  typedef u_int32_t HeaderThreshold [HEADER_THRESHOLDS];
  typedef std::vector<u_int32_t> HeaderThresholdVec;
  typedef std::map<unsigned int,HeaderThresholdVec> HeaderThresholdMap;
  typedef std::vector<u_int32_t>::iterator HeaderThresholdIT;
  typedef int HeaderCorrection [ALINKS][STRIPS_TO_CORRECT][HEADER_CORRECTIONS];
  typedef std::vector<int> HeaderCorrectionVec;
  typedef std::map<unsigned int,HeaderCorrectionVec> HeaderCorrectionMap;
  
  // headers/data
  typedef int Header [PP_FPGA][BEETLES_PER_PP_FPGA][ALINKS_PER_BEETLE][HEADERS_PER_ALINK];
  typedef int Data [PP_FPGA][BEETLES_PER_PP_FPGA][ALINKS_PER_BEETLE][CHANNELS_PER_ALINK];
  typedef std::vector<int> EngineVec;
    
  //CMS thresholds
  typedef u_int8_t CMSThreshold [ALINKS][CHANNELS_PER_ALINK];
  typedef std::vector<u_int8_t> CMSThresholdVec;
  typedef std::map<unsigned int,CMSThresholdVec> CMSThresholdMap;

  // thresholds for cluster maker
  typedef u_int32_t HitThreshold [ALINKS][CHANNELS_PER_ALINK];
  typedef std::vector<u_int32_t> HitThresholdVec;
  typedef std::map<unsigned int,HitThresholdVec> HitThresholdMap;

  typedef int ConfirmationThreshold [SECTORS];
  typedef int SpillOverThreshold [SECTORS];
  typedef std::vector<int> ConfirmationThresholdVec;
  typedef std::map<unsigned int,ConfirmationThresholdVec> ConfirmationThresholdMap;
  typedef std::vector<int> SpillOverThresholdVec;
  typedef std::map<unsigned int,SpillOverThresholdVec> SpillOverThresholdMap;

  // boundary strips - cluster maker
  typedef u_int32_t BoundaryStrip [SECTORS];
  typedef std::vector<u_int32_t> BoundaryStripVec;

  // encoded cluster unit - cluster maker
  typedef u_int16_t TELL1Cluster [4*PP_MAX_CLUSTERS];
  typedef std::vector<u_int16_t> TELL1ClusterVec;
  typedef std::map<int,TELL1ClusterVec> CLUSTER_MEMORY;

  // encoded adc value - cluster maker
  typedef u_int8_t TELL1ADC [20*PP_MAX_CLUSTERS];
  typedef std::vector<u_int8_t> TELL1ADCVec;
  typedef std::map<int,TELL1ADCVec> ADC_MEMORY;

};
#endif // STTELL1CORE_H
