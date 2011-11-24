// Include files 

#include <iostream> 

// Event model
#include "Event/MCParticle.h"

// local
#include "ParticleInfo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleInfo
//
// 2003-02-19 : Alessia Satta
//-----------------------------------------------------------------------------
int ParticleInfo::m_stationNumber=0;
int ParticleInfo::m_gapsNumber=0;
int ParticleInfo::maxDimension=20;  

#define MAXGAPS 4  

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

//=============================================================================
ParticleInfo::ParticleInfo(const LHCb::MCParticle* /* particle */, int station, 
int gaps, int collision ){
  m_stationNumber=station;
  m_gapsNumber=gaps;
  m_pileupEventNumber=collision;
  maxDimension=station*gaps;  
  m_storagePointer = new std::vector<std::vector<int> >(maxDimension) ;
  m_resultPointer = new std::vector<int>(maxDimension) ;
  //cout<<"max dimansion "<<maxDimension<<endl;
  
}

ParticleInfo::ParticleInfo(const LHCb::MCParticle* /* particle */, int collision)
{
//  m_pileupEventNumber=particle->collision()->key();
//temp it is  a bug not a feature...  waiting for Gloria 
 m_pileupEventNumber=collision;
  
  m_storagePointer = new std::vector<std::vector<int> >(maxDimension) ;
  m_resultPointer = new std::vector<int> (maxDimension) ;
//	cout<<"pippo "<<endl;
}

int ParticleInfo::getCollision()
{
  return  m_pileupEventNumber;  
}


void ParticleInfo::setHitIn(int station, int gap, int chamber)
{
  int position=station*m_gapsNumber+gap;
  //	cout<<"position "<<position<<endl;
  (*m_storagePointer)[position].push_back(chamber);
  //cout<<"position "<<position<<endl;       
}
std::vector<int> ParticleInfo::multiplicityResults()
{
  int station,gap,position,secondGap,secondPosition,chamber;
  std::vector<int>::reverse_iterator itHit;  
  std::vector<int>::iterator foundHit,deleteHit;  
  int multiplicity;
  
  //  int trackFound[MAXGAPS];   // never used
  std::vector<int> startOfStorage;  
  for(station=0;station<m_stationNumber;station++){
    // trackFound[0]=0;
    // trackFound[1]=0;
    // trackFound[2]=0;
    // trackFound[3]=0;
    for(gap=0;gap<m_gapsNumber;gap++){
      //loop on all the 
      position=station*m_gapsNumber+gap;
      startOfStorage=(*m_storagePointer)[position];
      for(itHit=startOfStorage.rbegin();itHit<startOfStorage.rend();
          itHit++)
        {        
          chamber=*itHit;
          multiplicity=0;
          //cout<<"station "<<station<<" 
          //chamber "<<chamber<<" gap "<<gap<<endl;
          
          for(secondGap=gap+1;secondGap<m_gapsNumber;secondGap++){
            secondPosition=station*m_gapsNumber+secondGap;
            foundHit=std::find(((*m_storagePointer)[secondPosition]).begin(),
                 ((*m_storagePointer)[secondPosition]).end(),chamber);
            if(foundHit!=((*m_storagePointer)[secondPosition]).end()){
              multiplicity++;
              ((*m_storagePointer)[secondPosition]).erase(foundHit);
              deleteHit=std::find(((*m_storagePointer)[secondPosition]).begin(),
                             ((*m_storagePointer)[secondPosition]).end(),
                             chamber);
              while(deleteHit!=((*m_storagePointer)[secondPosition]).end()){
                ((*m_storagePointer)[secondPosition]).erase(deleteHit);
                deleteHit=std::find(
                               ((*m_storagePointer)[secondPosition]).begin(),
                               ((*m_storagePointer)[secondPosition]).end(),
                               chamber);
              }
              
            }
          }
          //          cout<<"multiplicity "<<multiplicity<<endl;
          
          (*m_resultPointer)[station*m_gapsNumber+multiplicity]++;          
          startOfStorage.pop_back();  

        }
      
    }    
  }
  return *m_resultPointer;
  
}


ParticleInfo::~ParticleInfo()
{
    std::vector<std::vector<int> >::iterator it;
    for(it=m_storagePointer->begin();it<m_storagePointer->end();it++){    
      m_storagePointer->pop_back();      
    }  
    delete m_storagePointer;    
    delete m_resultPointer; 
}

