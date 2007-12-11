// $Id: VeloTell1Core.h,v 1.3 2007-12-11 08:39:10 cattanem Exp $
#ifndef VELOTELL1CORE_H 
#define VELOTELL1CORE_H 1

// Include files
#include <vector>

/** @namespace VeloTell1Core VeloTell1Core.h Kernel/VeloTell1Core.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2007-07-22
 */

namespace VeloTELL1{

  typedef unsigned int u_int32_t;
  typedef unsigned short int u_int16_t;
  typedef unsigned char u_int8_t;

  enum strips{
    HEADER_STRIPS=4
  };

  enum tableLimits{
    PP_FPGA=4,
    HEADERS_PER_ALINK=8,
    PROCESSING_CHANNELS=9,
    CHANNELS_PER_ALINK=32,
    SECTORS=36,
    ALINKS=64,
    PP_MAX_CLUSTERS=255,
    ALL_STRIPS=2304         /// real and dummy strips
  };
  /// as defined in tell1lib: velo/velo_offline.h
  /// VELO_CLUSTER_UNIT and VELO_ADC_UNIT needed by ZS module
  // TELL1 raw cluster structure (as defined in edms note EDMS-690585)
  // the implementation follows the same pattern that was adapted in
  // TELL1 library 'tell1lib'
  typedef union {
  u_int16_t All;
    struct {    //starts from bit0
      u_int16_t CLUSTER_ISP   :3;    /*!< Inner strip position */ 
      u_int16_t CLUSTER_POS   :11;   /*!< Cluster strip position */ 
      u_int16_t CLUSTER_SIZE  :1;    /*!< Cluster size: '0'-1,2; '1'-3,4 */ 
      u_int16_t CLUSTER_SO    :1;    /*!< Spill over bit */ 
    } Sepe;
  }VELO_CLUSTER_UNIT;

  typedef union {
    u_int8_t All;
    struct {    //starts from bit0
      u_int8_t  ADC_VALUE        :7;  /*!< Cluster value*/ 
      u_int8_t  ADC_EOC          :1;  /*!< End of cluster bit*/ 
    } Sepe;
  }VELO_ADC_UNIT;
  // parameters related to clusterization
  enum clusterCoding{
     CLUSTER_CNT=1,
     CLUSTER_PER_WORD=2,
     ADCS_PER_WORD=4,
     ADC_SHIFT=8,
     CLUSTER_SHIFT=16
  };

  enum remaindeADCS{
    ADC_REMAINDER_1=1,
    ADC_REMAINDER_2=2
  };
  ///------------------------------------------------------------------
  /// type defs related with interfacing wrappers and
  /// c-modules
  ///------------------------------------------------------------------
  // mask link - pedestal subtractor
  typedef u_int8_t LinkMask [ALINKS][CHANNELS_PER_ALINK];
  typedef u_int8_t* miT;
  typedef const u_int8_t* const_miT;
  typedef std::vector<u_int8_t> LinkMaskVec;
  typedef std::vector<u_int8_t>::iterator lmIT;
  typedef std::vector<u_int8_t>::const_iterator const_lmIT;
  // pedestals - pedestal subtractor
  typedef u_int16_t LinkPedestal [ALINKS][CHANNELS_PER_ALINK];
  typedef u_int16_t* piT;
  typedef const u_int16_t* const_piT;
  typedef std::vector<u_int16_t> LinkPedestalVec;
  typedef std::vector<u_int16_t>::iterator lpIT;
  typedef std::vector<u_int16_t>::const_iterator const_lpIT;
  // heder correction values
  typedef u_int32_t HeaderStrip0 [HEADER_STRIPS];
  typedef u_int32_t* hiT;
  typedef const u_int32_t* const_hiT;
  typedef std::vector<u_int32_t> HeaderStrip0Vec;
  typedef std::vector<u_int32_t>::iterator hsIT;
  typedef std::vector<u_int32_t>::const_iterator const_hsIT;
  // heders
  typedef int Header [PP_FPGA][PROCESSING_CHANNELS][HEADERS_PER_ALINK];
  typedef int Data [PP_FPGA][PROCESSING_CHANNELS][ALINKS];
  typedef int* iT;          /// iterator
  typedef const int* const_iT;   /// const_iterator
  // seeding thresholds for cluster maker
  typedef u_int8_t HitThreshold [PP_FPGA][PROCESSING_CHANNELS][ALINKS];
  typedef std::vector<u_int8_t> HitThresholdVec;
  typedef std::vector<u_int8_t>::iterator hthIT;
  typedef std::vector<u_int8_t>::const_iterator const_hthIT;
  // inclusion and sum thresholds - cluster maker
  typedef u_int32_t LowThreshold [SECTORS];
  typedef u_int32_t SumThreshold [SECTORS];
  typedef std::vector<u_int32_t> Thresholds;
  typedef std::vector<u_int32_t>::iterator thIT;
  typedef std::vector<u_int32_t>::const_iterator const_thIT;
  // boundary strips - cluster maker
  typedef u_int32_t BoundaryStrip [SECTORS];
  typedef std::vector<u_int32_t> BoundaryStripVec;
  typedef std::vector<u_int32_t>::iterator bsIT;
  typedef std::vector<u_int32_t>::const_iterator const_bsIT;
  // encoded cluster unit - cluster maker
  typedef u_int16_t TELL1Cluster [4*PP_MAX_CLUSTERS];
  typedef std::vector<u_int16_t> TELL1ClusterVec;
  typedef std::vector<u_int16_t>::iterator cluIt;
  typedef std::vector<u_int16_t>::const_iterator const_cluIt;
  typedef std::vector<TELL1ClusterVec> CLUSTER_MEMORY;
  // encoded adc value - cluster maker
  typedef u_int8_t TELL1ADC [8*PP_MAX_CLUSTERS];
  typedef std::vector<u_int8_t> TELL1ADCVec;
  typedef std::vector<u_int8_t>::iterator adcIt;
  typedef std::vector<u_int8_t>::const_iterator const_adcIt;
  typedef std::vector<TELL1ADCVec> ADC_MEMORY;
  // all other - used in all engine classes
  typedef std::vector<int> EngineVec;
  typedef EngineVec::iterator diT;
  typedef EngineVec::const_iterator const_diT;
  // these are needed after upadate of the VeloTELL1Data
  typedef std::vector<signed int>::const_iterator cIT;
  // Pair of const iterators
  typedef std::pair<cIT, cIT> ALinkPair;
};
#endif // VELOTELL1CORE_H
