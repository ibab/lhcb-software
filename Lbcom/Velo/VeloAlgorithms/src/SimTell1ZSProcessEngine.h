// $Id: SimTell1ZSProcessEngine.h,v 1.2 2009-09-01 11:35:38 krinnert Exp $
#ifndef SIMTELL1ZSPROCESSENGINE_H 
#define SIMTELL1ZSPROCESSENGINE_H 1

// Include files
#include "SimTell1Engine.h"

/** @class SimTell1ZSProcessEngine SimTell1ZSProcessEngine.h
 *  
 *
 *  @author Tomasz Szumlak
 *  @date   2009-08-02
 */

class SimTell1ZSProcessEngine: public SimTell1Engine{
  public:
    typedef VeloTELL1::VELO_CLUSTER_UNIT VELO_CLUSTER_UNIT;
    typedef VeloTELL1::VELO_ADC_UNIT VELO_ADC_UNIT;
    /// Standard constructor
    SimTell1ZSProcessEngine( );
    virtual ~SimTell1ZSProcessEngine( ); ///< Destructor
    void runZeroSuppression();
    int velo_zs_process(int zs_enable, VeloTELL1::Thresholds hit_threshold,
        VeloTELL1::Thresholds low_threshold_temp,
        VeloTELL1::SumThresholds sum_temp,
        const VeloTELL1::u_int32_t* strip_start_temp,
        VeloTELL1::Data in_data, VeloTELL1::u_int32_t pp_max_clusters,
        VeloTELL1::TELL1Cluster cluster, int *clus_numb,
        VeloTELL1::TELL1ADC adc_list, int *adc_numb);
    int velo_clusterization_process(int *zs_line,int *zs_h_line,
        int *zs_l_line, VeloTELL1::u_int16_t *cluster,
        VeloTELL1::u_int8_t *adc_list,
        unsigned int *pp_clus_cnt, int *pp_adc_cnt,
        VeloTELL1::u_int32_t pp_max_clusters,
        VeloTELL1::u_int8_t sum, VeloTELL1::u_int16_t strip_start);
      // NOTE:  parameter 'boundary_strip' is not needed, see comment
      // on 'loop_cnt' in .cpp implementation file.
      //
      //  int boundary_strip);
    void setHitThresholds(const VeloTELL1::ThresholdsVec& inVec);
    void setLowThresholds(const VeloTELL1::ThresholdsVec& inVec);
    void setSumThresholds(const VeloTELL1::SumThresholdsVec& inVec);
    void setBoundaryStrips(const VeloTELL1::BoundaryStripVec& inVec);
    VeloTELL1::u_int32_t producedClusters() const;
    int includedADCs() const;
    void setOutClusters(VeloTELL1::TELL1ClusterVec& clusters){ 
      m_rawClusters=&clusters;
    }
    void setOutADCS(VeloTELL1::TELL1ADCVec& adcs){
      m_rawADCS=&adcs;
    }
    virtual void initTableMembers();

  protected:

    void flushMemory();

  private:

    VeloTELL1::Thresholds  m_hitThresholds;
    VeloTELL1::Thresholds  m_lowThresholds;
    VeloTELL1::SumThresholds  m_sumThresholds;
    const VeloTELL1::u_int32_t* m_boundaryStrips;
    int m_producedClusters;
    VeloTELL1::TELL1Cluster m_clusters;
    VeloTELL1::TELL1ADC m_adcs;
    int m_includedADCS;
    VeloTELL1::TELL1ClusterVec* m_rawClusters;
    VeloTELL1::TELL1ADCVec* m_rawADCS;
    VeloTELL1::Data m_cModuleData;

};
#endif // SIMTELL1ZSPROCESSENGINE_H
