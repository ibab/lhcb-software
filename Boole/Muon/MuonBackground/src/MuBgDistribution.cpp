// Include files 
#include <iostream>  


// local
#include "MuBgDistribution.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuBgDistribution
//
// 2003-03-03 : Alessia Satta
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuBgDistribution::MuBgDistribution(std::vector<Rndm::Numbers*>  
                                   pointerToDistributions,  
                                   std::vector<bool> pointerToFlags,
                                   float xmin, float xmax  ) {
  m_xmin=xmin;
  m_xmax=xmax;
  m_pointerToDistributions=pointerToDistributions;
  m_pointerToFlags=pointerToFlags;  
  m_dimension=1;
  
}
MuBgDistribution::MuBgDistribution(std::vector<Rndm::Numbers*>  
                                   pointerToDistributions,  
                                   std::vector<bool> pointerToFlags,
                                   float xmin, float xmax, int nbinx, 
                                   float ymin,float ymax  ) {

  m_xmin=xmin;
  m_xmax=xmax;
  m_ymin=ymin;
  m_ymax=ymax;
  m_nbinx=nbinx;  
  m_pointerToDistributions=pointerToDistributions;
  m_pointerToFlags=pointerToFlags;
  m_dimension=2;

}
//=============================================================================
float MuBgDistribution::giveRND()
{

  if(m_pointerToFlags[0]){
    double nd = (*(m_pointerToDistributions[0]))();
    float  nc = float(nd*(m_xmax-m_xmin)+m_xmin);
    return nc;    
  }
  else return 0;
  
}
float MuBgDistribution::giveRND(float x)
{
  float lenght = (m_xmax-m_xmin)/m_nbinx;
  unsigned int index =(int) ((x-m_xmin)/lenght);
  if(index<m_pointerToFlags.size()){
    
    if(m_pointerToFlags[index]){    
      double nd = (*(m_pointerToDistributions[index]))(); 
      float  nc = float(nd*(m_ymax-m_ymin)+m_ymin);
      return nc;
    }
    
    else return 0;
  }  else return 0;
  
}
MuBgDistribution::~MuBgDistribution()
{
  //   
  /* std::vector<Rndm::Numbers*>::iterator it;
  for (it=m_pointerToDistributions.begin();it<m_pointerToDistributions.end();
       it++){
    cout<<" size "<< m_pointerToDistributions.size()<<endl;
    
    Rndm::Numbers* pippo=*it;
       delete pippo; 
    cout<<" size dopo "<< m_pointerToDistributions.size()<<endl;
 
  }
  cout<<" size dopo alessia  "<<endl;
  m_pointerToDistributions.resize(0);
  
  cout<<" size dopo alessia  mammm  "<<endl;*/
    while(m_pointerToDistributions.size()>0)
  {
    //  cout<<" size "<< m_pointerToDistributions.size()<<endl;
    
    Rndm::Numbers* pippo=m_pointerToDistributions.back();
    //cout<<" rnd "<< (*pippo)()<<endl;
    StatusCode sc=pippo->finalize();
	if(sc.isFailure()){}
    else {delete pippo; }  
    //cout<<" size dopo "<< m_pointerToDistributions.size()<<endl;
 
        m_pointerToDistributions.pop_back();
    }
        
}
