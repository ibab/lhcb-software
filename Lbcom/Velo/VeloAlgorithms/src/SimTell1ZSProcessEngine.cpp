// $Id: SimTell1ZSProcessEngine.cpp,v 1.2 2009-09-01 11:35:38 krinnert Exp $
// Include files 
#include <cstring>

#include<vector>
#include<algorithm>

// local
#include "SimTell1ZSProcessEngine.h"

// std
#include <iostream>

//-----------------------------------------------------------------------------
// Implementation file for class : SimTell1ZSProcessEngine
//
// 2009-08-02 : Tomasz Szumlak
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
SimTell1ZSProcessEngine::SimTell1ZSProcessEngine(  ):
  SimTell1Engine(),
  m_producedClusters ( 0 ),
  m_includedADCS ( 0 ),
  m_rawClusters ( ),
  m_rawADCS ( )
{
  this->initTableMembers();
}
//=============================================================================
// Destructor
//=============================================================================
SimTell1ZSProcessEngine::~SimTell1ZSProcessEngine() {}
//=============================================================================
void SimTell1ZSProcessEngine::initTableMembers()
{
  std::memset((**m_hitThresholds),0,sizeof(VeloTELL1::Thresholds));
  std::memset((**m_lowThresholds),0,sizeof(VeloTELL1::Thresholds));
  std::memset((m_sumThresholds),0,sizeof(VeloTELL1::SumThresholds));
  std::memset((m_clusters), 0, sizeof(VeloTELL1::TELL1Cluster));
  std::memset((m_adcs), 0, sizeof(VeloTELL1::TELL1ADC));
  m_boundaryStrips = 0;  
}
//=============================================================================
int SimTell1ZSProcessEngine::velo_zs_process(int zs_enable,
    VeloTELL1::Thresholds hit_threshold,
    VeloTELL1::Thresholds low_threshold_temp,
    VeloTELL1::SumThresholds sum_temp,
    const VeloTELL1::u_int32_t* strip_start_temp,
    VeloTELL1::Data in_data, VeloTELL1::u_int32_t pp_max_clusters,
    VeloTELL1::TELL1Cluster cluster, int *clus_numb,
    VeloTELL1::TELL1ADC adc_list, int *adc_numb)
{
  int err = 0;
  int pp, j,  t;

  int zs_line[69];
  int zs_h_line[69];        //4 left +64 +1 right
  int zs_l_line[69];        //4 left +64 +1 right

  // NOTE:  parameter 'boundary_strip' is not needed, see comment
  // on 'loop_cnt' below.
  //
  //int boundary_strip=0;


  unsigned int pp_clus_cnt;
  int pp_adc_cnt;

  (*clus_numb)= 0;
  (*adc_numb) =0;
  //printf("\nhit_threshold  ="); printf("%d ",hit_threshold[0][0][0]); 
  //printf("\nlow_threshold  ="); printf("%d ",low_threshold_temp[0][0][0]); 

  // printf("\ndata ="); for(t=0;t<32;t++)  printf("%2X ",in_data[0][0][t]);

  for(pp=0; pp<4; pp++)        //4 pp
  {
    if(zs_enable==0) continue;

    pp_clus_cnt = 0;
    pp_adc_cnt = 0;

    for(j=0;j<9;j++)        //9channel
    {
      //channel 1   substract high threshold and low threshold
      for(t=0;t<4;t++)      //channel 1
      {	
        if(j==0) { zs_line[t]=0; zs_h_line[t] = -1; zs_l_line[t] = -1; }
        else     { zs_h_line[t] = in_data[pp][j-1][60+t] - hit_threshold[pp][j-1][60+t];
          zs_l_line[t] = in_data[pp][j-1][60+t] - low_threshold_temp[pp][j-1][60+t];
          zs_line[t]   = in_data[pp][j-1][60+t];
        }
      }	    
      for(t=0;t<64;t++)
      {	
        zs_h_line[t+4] = in_data[pp][j][t] - hit_threshold[pp][j][t]; 
        zs_l_line[t+4] = in_data[pp][j][t] - low_threshold_temp[pp][j][t];
        zs_line[t+4]   = in_data[pp][j][t];
      }
      if(j==8)
      { 
        zs_line[68] = 0;
        zs_h_line[68] = -1;
        zs_l_line[68] = -1;
        // NOTE:  parameter 'boundary_strip' is not needed, see comment
        // on 'loop_cnt' below.
        //
        //boundary_strip = 1;
      }
      else
      { 
        zs_line[68] = in_data[pp][j+1][0];
        zs_h_line[68] = in_data[pp][j+1][0] - hit_threshold[pp][j+1][0];
        zs_l_line[68] = in_data[pp][j+1][0] - low_threshold_temp[pp][j+1][0];
        // NOTE:  parameter 'boundary_strip' is not needed, see comment
        // on 'loop_cnt' below.
        //
        //boundary_strip = 0;
      }

      // NOTE:  parameter 'boundary_strip' is not needed, see comment
      // on 'loop_cnt' below.
      //
      //err |= velo_clusterization_process(zs_line,zs_h_line, zs_l_line, cluster, adc_list, &pp_clus_cnt, &pp_adc_cnt,pp_max_clusters, sum_temp[pp*9+j],strip_start_temp[pp*9+j],boundary_strip);
      err |= velo_clusterization_process(zs_line,zs_h_line, zs_l_line, cluster, adc_list, &pp_clus_cnt, &pp_adc_cnt,pp_max_clusters, sum_temp[pp*9+j],strip_start_temp[pp*9+j]);
      if(pp_clus_cnt == pp_max_clusters)   	break;


    }//9channel
    cluster = cluster+pp_clus_cnt;
    adc_list = adc_list+pp_adc_cnt;
    (*clus_numb) += pp_clus_cnt;
    (*adc_numb) += pp_adc_cnt;
  }//4 pp

  return(err);
}
//=============================================================================
int SimTell1ZSProcessEngine::velo_clusterization_process(int *zs_line,
    int *zs_h_line,
    int *zs_l_line,
    VeloTELL1::u_int16_t *cluster,
    VeloTELL1::u_int8_t *adc_list,
    unsigned int *pp_clus_cnt,
    int *pp_adc_cnt,
    VeloTELL1::u_int32_t pp_max_clusters,
    VeloTELL1::u_int8_t sum,
    VeloTELL1::u_int16_t strip_start)

    // NOTE:  parameter 'boundary_strip' is not needed, see comment
    // on 'loop_cnt' below.
    //
    //int boundary_strip)
{
  int err = 0;
  int i;
  int pos;
  //int loop_cnt;

  int sum_temp;
  int sum_pos;
  int pos_shift;

  int zs_h_line_not_clear[69];

  VELO_CLUSTER_UNIT cluster_unit;
  VELO_ADC_UNIT     adc_unit;

  // NOTE: the variable 'loop_cnt' set below is never used in
  // subsequent code. Since the code behaves correctly, i.e. 
  // is bit perfect wrt. the TELL1, we comment these two lines out
  // to avoid compiler warnings with gcc 4.6.
  //
  //if(boundary_strip ==1) loop_cnt = 69;
  //else loop_cnt = 68;

  for(i=0;i<69;i++) zs_h_line_not_clear[i] = zs_h_line[i];

  for(i=4; i<69; i++)
  {
    if(     ((i==4) && (zs_h_line[4]>0)) 
        || ((i>4)  && (i<68))
        || ((i==68)&& (zs_h_line[68]<=0))  
      )
    {
      if((zs_h_line[i]<=0)&&(zs_h_line[i-1]>0)&&(zs_h_line[i-2]<=0))   //010
      {//printf("->  010   %d<- \n",i);
        zs_h_line[i  ] = 0;
        zs_h_line[i-1] = 0;
        zs_h_line[i-2] = 0;
        zs_h_line[i-3] = 0;
        if((zs_l_line[i]<=0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]>0))//110
        {
          sum_temp = zs_line[i-1]+zs_line[i-2];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 0;

          pos_shift = 2;
          sum_pos = (pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2];

          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;
          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

          //printf(RED"\n010>110: zs_line[i-1]= %d zs_line[i-2]= %d pos= %d  isp = %d (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000) = %d \n "
          //                   , zs_line[i-1],zs_line[i-2],cluster_unit.Sepe.CLUSTER_POS,cluster_unit.Sepe.CLUSTER_ISP,(sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000) );
          //printf(RED"\n010>110:((int)(65536.0F/(float)sum_temp+0.5))=%d ,sum_pos*((int)(65536.0F/(float)sum_temp+0.5)) = %d,(sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000) = %d (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192 = %d \n "
          //             ,((int)(65536.0F/(float)sum_temp+0.5)), sum_pos*((int)(65536.0F/(float)sum_temp+0.5)), (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000) , (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192 );

          //getchar();

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;

        }
        if((zs_l_line[i]>0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]<=0))//011
        {
          sum_temp = zs_line[i-1]+zs_line[i];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 0;

          pos_shift = 1;
          sum_pos = (pos_shift-0)*zs_line[i]+(pos_shift-1)*zs_line[i-1];

          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;
          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

          //printf(RED"\n010>110:((int)(65536.0F/(float)sum_temp+0.5))=%d ,sum_pos*((int)(65536.0F/(float)sum_temp+0.5)) = %d,(sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000) = %d (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192 = %d \n "
          //             ,((int)(65536.0F/(float)sum_temp+0.5)), (sum_pos-(int)sum_temp)*((int)(65536.0F/(float)sum_temp+0.5)), ((sum_pos-(int)sum_temp)*((int)(65536.0F/(float)sum_temp+0.5))+0x1000) , ((sum_pos-(int)sum_temp)*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192 );

          //printf(RED"\n010>011: zs_line[i]= %d zs_line[i-1]= %d pos= %d  isp = %d\n ", zs_line[i],zs_line[i-1],cluster_unit.Sepe.CLUSTER_POS,cluster_unit.Sepe.CLUSTER_ISP );
          //getchar();

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
        }
        if((zs_l_line[i]>0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]>0))//111
        {
          sum_temp =zs_line[i] + zs_line[i-1]+zs_line[i-2];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 1;
          pos_shift = 2;
          sum_pos = (pos_shift-0)*zs_line[i]+(pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2];
          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;
          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);
          //printf(RED"\n010>110: zs_line[i]= %d zs_line[i-1]= %d zs_line[i-2]= %d pos= %d  isp = %d\n :::a_tmep=%d, b_temp= %d",zs_line[i], zs_line[i-1],zs_line[i-2],cluster_unit.Sepe.CLUSTER_POS,cluster_unit.Sepe.CLUSTER_ISP, a_temp,b_temp );
          //getchar();

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
        }
        if((zs_l_line[i]<=0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]<=0))//010
        {
          sum_temp = zs_line[i-1];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 0;
          pos_shift = 1;
          sum_pos = (pos_shift-1)*zs_line[i-1];
          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;
          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
        }
      }
      if((zs_h_line[i]<=0)&&(zs_h_line[i-1]>0)&&(zs_h_line[i-2]>0)&&(zs_h_line[i-3]<=0))   //0110
      {//printf("->  0110   %d<- \n",i);
        zs_h_line[i  ] = 0;
        zs_h_line[i-1] = 0;
        zs_h_line[i-2] = 0;
        zs_h_line[i-3] = 0;
        if((zs_l_line[i]<=0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]>0)&&(zs_l_line[i-3]>0))//1110
        {
          sum_temp = zs_line[i-1]+zs_line[i-2]+zs_line[i-3];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 1;
          pos_shift = 3;
          sum_pos = (pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2]+(pos_shift-3)*zs_line[i-3];
          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;

          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-3];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;

        }
        if((zs_l_line[i]>0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]>0)&&(zs_l_line[i-3]<=0))//0111
        {
          sum_temp =zs_line[i-2]+zs_line[i-1]+zs_line[i];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 1;
          pos_shift = 2;
          sum_pos = (pos_shift-0)*zs_line[i]+(pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2];

          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;
          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);
          //printf(RED"\n010>110: zs_line[i]= %d zs_line[i-1]= %d zs_line[i-2]= %d pos= %d  isp = %d\n :::a_tmep=%d,b_temp=%d",zs_line[i], zs_line[i-1],zs_line[i-2],cluster_unit.Sepe.CLUSTER_POS,cluster_unit.Sepe.CLUSTER_ISP, a_temp,b_temp );
          //getchar();

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
        }
        if((zs_l_line[i]>0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]>0)&&(zs_l_line[i-3]>0))//1111
        {
          sum_temp =zs_line[i] + zs_line[i-1]+zs_line[i-2]+zs_line[i-3];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 1;
          pos_shift = 3;
          sum_pos = (pos_shift-0)*zs_line[i]+(pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2]+(pos_shift-3)*zs_line[i-3];
          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;
          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

          //printf(RED"\n010>110: zs_line[i]= %X zs_line[i-1]= %X zs_line[i-2]= %X zs_line[i-3]= %X pos= %X  isp = %d \n :::sum_temp = %X, sum_pos = %X  a_tmep=%X"
          //           ,zs_line[i], zs_line[i-1],zs_line[i-2],zs_line[i-3],cluster_unit.Sepe.CLUSTER_POS,cluster_unit.Sepe.CLUSTER_ISP,(int)sum_temp,sum_pos,a_temp);
          //getchar();

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-3];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
          adc_unit.Sepe.ADC_VALUE = zs_line[i];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
        }
        if((zs_l_line[i]<=0)&&(zs_l_line[i-1]>0)&&(zs_l_line[i-2]>0)&&(zs_l_line[i-3]<=0))//0110
        {
          sum_temp = zs_line[i-1]+zs_line[i-2];
          if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
          else             cluster_unit.Sepe.CLUSTER_SO = 0;

          cluster_unit.Sepe.CLUSTER_SIZE = 0;

          pos_shift = 2;
          sum_pos = (pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2];
          pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
          cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;

          cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

          *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
          (*pp_clus_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
          adc_unit.Sepe.ADC_EOC = 0;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;

          adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
          adc_unit.Sepe.ADC_EOC = 1;
          *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
          (*pp_adc_cnt)++;
        }
      }
      if((zs_h_line[i]<=0)&&(zs_h_line[i-1]>0)&&(zs_h_line[i-2]>0)&&(zs_h_line[i-3]>0)&&(zs_h_line[i-4]<=0))   //01110
      {//printf("->  01110   %d<- \n",i);
        zs_h_line[i  ] = 0;
        zs_h_line[i-1] = 0;
        zs_h_line[i-2] = 0;
        zs_h_line[i-3] = 0;
        sum_temp = zs_line[i-1]+zs_line[i-2]+zs_line[i-3];
        if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
        else             cluster_unit.Sepe.CLUSTER_SO = 0;

        cluster_unit.Sepe.CLUSTER_SIZE = 1;

        pos_shift = 3;
        sum_pos = (pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2]+(pos_shift-3)*zs_line[i-3];
        pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
        cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;

        cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

        *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
        (*pp_clus_cnt)++;

        adc_unit.Sepe.ADC_VALUE = zs_line[i-3];
        adc_unit.Sepe.ADC_EOC = 0;
        *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
        (*pp_adc_cnt)++;
        adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
        adc_unit.Sepe.ADC_EOC = 0;
        *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
        (*pp_adc_cnt)++;
        adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
        adc_unit.Sepe.ADC_EOC = 1;
        *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
        (*pp_adc_cnt)++;

      }
      if((zs_h_line[i]>0)&&(zs_h_line[i-1]>0)&&(zs_h_line[i-2]>0)&&(zs_h_line[i-3]>0)&&(zs_h_line[i-4]<=0))   //01111
      {  //printf("->  01111   %d<- \n",i);

        zs_h_line[i  ] = 0;
        zs_h_line[i-1] = 0;
        zs_h_line[i-2] = 0;
        zs_h_line[i-3] = 0;
        sum_temp = zs_line[i]+zs_line[i-1]+zs_line[i-2]+zs_line[i-3];
        if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
        else             cluster_unit.Sepe.CLUSTER_SO = 0;

        cluster_unit.Sepe.CLUSTER_SIZE = 1;

        pos_shift = 3;
        sum_pos = (pos_shift-0)*zs_line[i]+(pos_shift-1)*zs_line[i-1]+(pos_shift-2)*zs_line[i-2]+(pos_shift-3)*zs_line[i-3];
        pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
        cluster_unit.Sepe.CLUSTER_POS = (strip_start+i-4-pos_shift+ ((pos>>3)&0x7FF))&0x7FF;
        cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

        *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
        (*pp_clus_cnt)++;

        adc_unit.Sepe.ADC_VALUE = zs_line[i-3];
        adc_unit.Sepe.ADC_EOC = 0;
        *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
        (*pp_adc_cnt)++;
        adc_unit.Sepe.ADC_VALUE = zs_line[i-2];
        adc_unit.Sepe.ADC_EOC = 0;
        *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
        (*pp_adc_cnt)++;
        adc_unit.Sepe.ADC_VALUE = zs_line[i-1];
        adc_unit.Sepe.ADC_EOC = 0;
        *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
        (*pp_adc_cnt)++;
        adc_unit.Sepe.ADC_VALUE = zs_line[i];
        adc_unit.Sepe.ADC_EOC = 1;
        *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
        (*pp_adc_cnt)++;


      }
      if((zs_h_line[i]>0)&&(zs_h_line[i-1]>0)&&(zs_h_line[i-2]>0)&&(zs_h_line[i-3]>0)&&(zs_h_line[i-4]>0))   //11111
      { 
        zs_h_line[i-1]   = 0;
        zs_h_line[i-2]   = 0;
        zs_h_line[i-3]   = 0;
        zs_h_line[i-4]   = 0;	
      }
      if((*pp_clus_cnt)==pp_max_clusters) return(err);
    }

  }	
  //end of process channel(i==68)
  if((zs_h_line_not_clear[68]>0)&&(zs_h_line_not_clear[67]>0)&&(zs_h_line_not_clear[66]>0)&&(zs_h_line_not_clear[65]>0)&&(zs_h_line_not_clear[64]>0))   //11111
  { 
    if((zs_h_line[66]<=0)&&(zs_h_line[67]>0))  //01   --1 strip left( local strip 63)
    {    //printf("->*  01  <- \n");
      sum_temp = zs_line[67];
      if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
      else             cluster_unit.Sepe.CLUSTER_SO = 0;

      cluster_unit.Sepe.CLUSTER_SIZE = 0;

      cluster_unit.Sepe.CLUSTER_POS = strip_start+63;
      cluster_unit.Sepe.CLUSTER_ISP = 0;

      *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
      (*pp_clus_cnt)++;

      adc_unit.Sepe.ADC_VALUE = zs_line[67];
      adc_unit.Sepe.ADC_EOC = 1;
      *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
      (*pp_adc_cnt)++;
    }
    if((zs_h_line[65]<=0)&&(zs_h_line[66]>0)&&(zs_h_line[67]>0))  //011  --2strips left(local strip 62,63)
    {  //printf("->*  011  <- \n");
      sum_temp = zs_line[66]+zs_line[67];
      if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
      else             cluster_unit.Sepe.CLUSTER_SO = 0;

      cluster_unit.Sepe.CLUSTER_SIZE = 0;
      sum_pos = zs_line[67];
      pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
      cluster_unit.Sepe.CLUSTER_POS = (strip_start+62+ ((pos>>3)&0x7FF))&0x7FF;
      cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

      *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
      (*pp_clus_cnt)++;

      adc_unit.Sepe.ADC_VALUE = zs_line[66];
      adc_unit.Sepe.ADC_EOC = 0;
      *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
      (*pp_adc_cnt)++;
      adc_unit.Sepe.ADC_VALUE = zs_line[67];
      adc_unit.Sepe.ADC_EOC = 1;
      *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
      (*pp_adc_cnt)++;
    }
    if((zs_h_line[64]<=0)&&(zs_h_line[65]>0)&&(zs_h_line[66]>0)&&(zs_h_line[67]>0))  //0111
    {  //printf("->*  0111  <- \n");	
      sum_temp =zs_line[65]+zs_line[66]+zs_line[67];
      if(sum_temp>sum) cluster_unit.Sepe.CLUSTER_SO = 1;
      else             cluster_unit.Sepe.CLUSTER_SO = 0;

      cluster_unit.Sepe.CLUSTER_SIZE = 1;
      sum_pos = 2*zs_line[67]+zs_line[66];
      pos = (sum_pos*((int)(65536.0F/(float)sum_temp+0.5))+0x1000)/8192;
      cluster_unit.Sepe.CLUSTER_POS = (strip_start+61+ ((pos>>3)&0x7FF))&0x7FF;
      cluster_unit.Sepe.CLUSTER_ISP = (pos&0x7);

      *(cluster+(*pp_clus_cnt)) = cluster_unit.All;
      (*pp_clus_cnt)++;

      adc_unit.Sepe.ADC_VALUE = zs_line[65];
      adc_unit.Sepe.ADC_EOC = 0;
      *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
      (*pp_adc_cnt)++;
      adc_unit.Sepe.ADC_VALUE = zs_line[66];
      adc_unit.Sepe.ADC_EOC = 0;
      *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
      (*pp_adc_cnt)++;
      adc_unit.Sepe.ADC_VALUE = zs_line[67];
      adc_unit.Sepe.ADC_EOC = 1;
      *(adc_list+(*pp_adc_cnt)) = adc_unit.All;
      (*pp_adc_cnt)++;
    }
  }
  return(err);
}
//=============================================================================
void SimTell1ZSProcessEngine::setHitThresholds(
    const VeloTELL1::ThresholdsVec& inData)
{
  std::memcpy(**m_hitThresholds,&(*inData.begin()),
      sizeof(VeloTELL1::Thresholds));
}
//=============================================================================
void SimTell1ZSProcessEngine::setLowThresholds(
    const VeloTELL1::ThresholdsVec& inData)
{
  std::memcpy(**m_lowThresholds,&(*inData.begin()),
      sizeof(VeloTELL1::Thresholds));  
}
//=============================================================================
void SimTell1ZSProcessEngine::setSumThresholds(
    const VeloTELL1::SumThresholdsVec& inData)
{
  std::memcpy(m_sumThresholds, &(*inData.begin()),
      inData.size()*sizeof(VeloTELL1::u_int32_t)); 
}
//=============================================================================
void SimTell1ZSProcessEngine::setBoundaryStrips(
    const VeloTELL1::BoundaryStripVec& inData)
{
  m_boundaryStrips = &(*inData.begin());
}
//=============================================================================
void SimTell1ZSProcessEngine::flushMemory()
{
  std::memset(m_clusters,0,sizeof(VeloTELL1::TELL1Cluster));
  std::memset(m_adcs,0,sizeof(VeloTELL1::TELL1ADC));
  m_producedClusters=0;
  m_includedADCS=0;
}
//=============================================================================
void SimTell1ZSProcessEngine::runZeroSuppression()
{
  // prepare data for the c-module
  flushMemory();
  std::memcpy(**m_cModuleData,&(*inData().begin()),sizeof(VeloTELL1::Data));

  // run zero suppresion 
  int error=velo_zs_process(processEnable(), 
      m_hitThresholds,
      m_lowThresholds,
      m_sumThresholds,
      m_boundaryStrips,
      m_cModuleData,
      VeloTELL1::PP_MAX_CLUSTERS,
      m_clusters,
      &m_producedClusters,
      m_adcs,
      &m_includedADCS);
  // return created clusters and added adcs
  if(!error){
    m_rawClusters->resize(m_producedClusters);
    std::memcpy(&(*m_rawClusters->begin()),m_clusters,
        m_producedClusters*sizeof(VeloTELL1::u_int16_t));
    m_rawADCS->resize(m_includedADCS);
    std::memcpy(&(*m_rawADCS->begin()),
        m_adcs,m_includedADCS*sizeof(VeloTELL1::u_int8_t));
  }else if(error){
    std::cout<< " --> Zero-Suppression Module FAILED! " <<std::endl;
  }
}
//--
