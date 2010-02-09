// $Id: MuonClusterTool.cpp,v 1.1 2010-02-09 16:02:24 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MuonClusterTool.h"
using namespace std;

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonClusterTool
//
// 2010-01-15 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonClusterTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonClusterTool::MuonClusterTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMuonClusterTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
MuonClusterTool::~MuonClusterTool() {} 

//=============================================================================

StatusCode MuonClusterTool::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;
  m_muonDetector=getDet<DeMuonDetector>
        ("/dd/Structure/LHCb/DownstreamRegion/Muon");
 return StatusCode::SUCCESS ;
}


StatusCode MuonClusterTool::doCluster(const std::string Input,const std::string Output){
  if(!exist<LHCb::MuonCoords>(Input)){
	error()<<" no coords at the locaion "<<Input<<endreq;
     return StatusCode::FAILURE;
  }
  if(exist<LHCb::MuonClusters>(Output)){
	error()<<"  cluster at the locaion "<<Output<<
                " already exist"<<endreq;
     return StatusCode::FAILURE;
  }
  MuonCoords* coords=get<LHCb::MuonCoords>(Input);
debug()<<" arrivato qui "<<coords->size()<< endreq;
  StatusCode sc;
  
  sc=initClusterVector(coords);
  if(sc.isFailure())return StatusCode::FAILURE;
  
  sc=mergeCluster();
  if(sc.isFailure())return StatusCode::FAILURE;
  SaveOutput(Output);
  ClearMemory();


 LHCb::MuonClusters* muoncl=get<LHCb::MuonClusters>
     ("Raw/Muon/TAECluster");


   if(muoncl)debug()<<" nel tool trovato cluster "<<muoncl->size()<<endreq;
 


  return StatusCode::SUCCESS;
  

};

StatusCode  MuonClusterTool::initClusterVector(LHCb::MuonCoords* coords)
{
  LHCb::MuonCoords::iterator i;
  debug()<<" list of input coords "<<endreq;
  
  for(i=coords->begin();i!=coords->end();i++){
    double xp,dx,yp,dy,zp,dz;
    MuonTileID tile((*i)->key());
    StatusCode sc =  m_muonDetector->Tile2XYZ(tile,xp,dx,yp,dy,zp,dz);
    MuonCluster*  clust=new MuonCluster((*i),xp,dx,yp,dy,zp,dz);
    std::pair<MuonCluster*,int> pippo(clust,0);
    debug()<<tile<<endreq;
    
    m_inputclust[tile.station()].push_back(pippo);
    

  }
  debug()<<" creati cluster "<<m_inputclust[0].size()+
    m_inputclust[1].size()+
    m_inputclust[2].size()+
    m_inputclust[3].size()+
    m_inputclust[4].size()
        <<endreq;
  return StatusCode::SUCCESS;
  
}



StatusCode MuonClusterTool::mergeCluster()
{
  for(int i=0;i<5;i++){

    mergeStation(i);
    
  }
  return StatusCode::SUCCESS;
  
  
}
StatusCode MuonClusterTool::mergeStation(int station)
{
 
  std::vector<std::pair<MuonCluster *,int> >::iterator itcluOne;
  std::vector<std::pair<MuonCluster *,int> >::iterator itcluTwo;
  
  bool  doIteration=false;
  int howMany=0;
  debug()<<" starting merge in station "<<endreq;
  do{
    verbose()<<" start a new turn "<<endreq;
    
    for(itcluOne=m_inputclust[station].begin();itcluOne!=m_inputclust[station].end();
        itcluOne++){
      if(itcluOne->second<0)continue;
      
      for(itcluTwo=itcluOne+1;itcluTwo!=m_inputclust[station].end();
          itcluTwo++){
        if(itcluTwo->second<0)continue;
        
        bool near=false;

        near=detectCluster(itcluOne->first,itcluTwo->first);
        verbose()<<"near "<<near<<endreq;
        if(near){
          (itcluOne->first)->mergeCluster(itcluTwo->first);
          itcluOne->second=1;        
          itcluTwo->second=2;        
        }
        
      }
    }  
    
    howMany=0;
    doIteration=false;
    int count=0;
    
    for(itcluOne=m_inputclust[station].begin();itcluOne!=m_inputclust[station].end();
        itcluOne++){
      count++;
      
      if(itcluOne->second==0){
        m_finalclust[station].push_back(itcluOne->first);
        itcluOne->second=-2;
      }
      
      if(itcluOne->second==2)itcluOne->second=-1;
      if(itcluOne->second==1){
        //check if cluster overlap completely with another one
        bool partOf=false;
        partOf=isIncluded(station,itcluOne->first);
        verbose()<<" a new cluster is merged  "<<endreq;
        const SmartRefVector<LHCb::MuonCoord> mycoord =(itcluOne->first)->coords();
        SmartRefVector<LHCb::MuonCoord>::const_iterator myit;
        for(myit=mycoord.begin();myit!=mycoord.end();myit++){
          MuonTileID mytile((*myit)->key());
          
          verbose()<<" in merge "<<mytile<<endreq;
          
        }
        
        verbose()<<" partOf "<< partOf<<endreq;
        
    


        if(partOf) {
          itcluOne->second=-2;
        }else{
          
        
          verbose()<<"modified cluster "<<count-1<<endreq;
          doIteration=true;
          howMany++;
          itcluOne->second=0;      
        }    
      }
      
    }
  }while(howMany>0);
  

  //remove cluster not in output

  
  return StatusCode::SUCCESS;
  
  
}



void MuonClusterTool::ClearMemory()
{



  

  std::vector<std::pair<MuonCluster *,int> >::iterator itcluOne;

  
  for(int station=0;station<5;station++){
    
    
    for(itcluOne=m_inputclust[station].begin();itcluOne!=m_inputclust[station].end();
        itcluOne++){
      //      if(itcluOne->second!=-2)continue;
      delete itcluOne->first;                                
    }
    m_inputclust[station].clear();
    m_finalclust[station].clear();
    
  }  


}

StatusCode MuonClusterTool::SaveOutput(std::string output)
{
  std::vector<MuonCluster *>::iterator itcluOne;
  MuonClusters *clusters = new MuonClusters;
  
  int totcoord=0;
  
  for(int station=0;station<5;station++){
    
    
    for(itcluOne=m_finalclust[station].begin();itcluOne!=m_finalclust[station].end();
        itcluOne++){

      const SmartRefVector<LHCb::MuonCoord> mycoord =(*itcluOne)->coords();
      SmartRefVector<LHCb::MuonCoord>::const_iterator myit;
      float xmin=0;
      float ymin=0;
      float zmin=0;
      float xmax=0;
      float ymax=0;
      float zmax=0;
      totcoord=0;
      for(myit=mycoord.begin();myit!=mycoord.end();myit++){
        MuonTileID mytile((*myit)->key());
        totcoord++;
        
        debug()<<mytile<<endreq;
           double dx,dy,dz;
           double x,y,z;                   
           m_muonDetector->Tile2XYZ(mytile,x,dx,y,dy,z,dz);
        if(totcoord==1){
          xmin=x-dx;
          ymin=y-dy;
          zmin=z-dz;
          xmax=x+dx;
          ymax=y+dy;
          zmax=z+dz;
          
        }else{
          if(xmin>x-dx)xmin=x-dx;
          if(ymin>y-dy)ymin=y-dy;
          if(zmin>z-dz)zmin=z-dz;
          if(xmax<x+dx)xmax=x+dx;
          if(ymax<y+dy)ymax=y+dy;
          if(zmax<z+dz)zmax=z+dz;
          
        }
        
        
      }
 
      MuonCluster*  copycluster=new MuonCluster(**itcluOne);
      copycluster->setX((xmin+xmax)/2);
      copycluster->setY((ymin+ymax)/2);
      copycluster->setZ((zmin+zmax)/2);
      copycluster->setXmin(xmin);
      copycluster->setXmax(xmax);
      copycluster->setYmin(ymin);
      copycluster->setYmax(ymax);
      copycluster->setZmin(zmin);
      copycluster->setZmax(zmax);
      clusters->insert(copycluster);
      verbose()<<" a new cluster "<<*itcluOne<<" "<<copycluster<<endreq;
      debug()<<(*itcluOne)->coords()<<" "<<copycluster->coords()<<endreq;
      
    }
    
  }
  debug()<<"cluster size "<<clusters->size()<<" "<<totcoord<<endreq;
  put(clusters,output);
  
  LHCb::MuonClusters* muoncl=get<LHCb::MuonClusters>
     ("Raw/Muon/TAECluster");


   if(muoncl)debug()<<" nel tool trovato cluster "<<muoncl->size()<<endreq;
  
  return StatusCode::SUCCESS;
  
}

bool MuonClusterTool::detectCluster(LHCb::MuonCluster* one,
                                   LHCb::MuonCluster* two)
{
  bool near=false;
  const SmartRefVector<LHCb::MuonCoord> coordsOne=one->coords();
  const SmartRefVector<LHCb::MuonCoord> coordsTwo=two->coords();
  verbose()<<coordsOne.size()<<" "<<coordsTwo.size()<<endreq;
  SmartRefVector<MuonCoord>::const_iterator iOne;
  SmartRefVector<MuonCoord>::const_iterator iTwo;
  for(iTwo=coordsTwo.begin();iTwo!=coordsTwo.end();iTwo++){
    bool same=one->checkPad(*iTwo);
    verbose()<<" same pad ? "<<same<<endreq;
    if(!same){
      MuonTileID tileTwo((*iTwo)->key());
      for(iOne=coordsOne.begin();iOne!=coordsOne.end();iOne++){
        MuonTileID tileOne((*iOne)->key());
        //same quarter
       verbose()<<" tile one "<<tileTwo<<endreq;
        verbose()<<" tile Two "<<tileOne<<endreq;
        if(tileOne.quarter()==tileTwo.quarter()){
          //same region
          if(tileOne.region()==tileTwo.region()){
            if(tileOne.nX()==tileTwo.nX()){
              long int d=abs((int)(tileOne.nY()-tileTwo.nY()));
              
              if(d==1)return true;
              
            }
            if(tileOne.nY()==tileTwo.nY()){
              long int d=abs((int)(tileOne.nX()-tileTwo.nX()));
              if(d==1)return true;
              
            }
            
          }else if(abs(int(tileOne.region()-tileTwo.region()))==1){
            if(tileOne.region()<tileTwo.region()){
              if(tileOne.nX()==(2*tileOne.layout().xGrid()-1)
                 ||tileOne.nY()==(2*tileOne.layout().yGrid()-1)){
                //near in X
                if(tileOne.nX()==(2*tileOne.layout().xGrid()-1)&&tileTwo.nX()==0){
                  //check if near in Y 
                  if((tileOne.nY()/2)==tileTwo.nY())return true;                  
                }                
                //near in Y
                if(tileOne.nY()==(2*tileOne.layout().xGrid()-1)&&tileTwo.nX()==0){
                  //check if near in Y 
                  if((tileOne.nY()/2)==tileTwo.nY())return true;                  
                }                
              }              
            }else if(tileOne.region()>tileTwo.region()){
              if(tileTwo.nX()==(2*tileOne.layout().xGrid()-1)
                 ||tileTwo.nY()==(2*tileOne.layout().yGrid()-1)){
                //near in X
                if(tileTwo.nX()==(2*tileTwo.layout().xGrid()-1)&&tileOne.nX()==0){
                  //check if near in Y 
                  if((tileTwo.nY()/2)==tileOne.nY())return true;                  
                }                
                //near in Y
                if(tileTwo.nY()==(2*tileTwo.layout().xGrid()-1)&&tileOne.nX()==0){
                  //check if near in Y 
                  if((tileTwo.nY()/2)==tileOne.nY())return true;                  
                }                
              }              
            }
            
          }
          
        }else if(tileOne.quarter()!=tileTwo.quarter()){
          if(tileOne.region()==tileTwo.region()){
            if(tileOne.quarter()==0){
              if(tileTwo.quarter()==1){
                if(tileOne.nY()==tileTwo.nY()&&tileOne.nY()==0){
                  if(tileOne.nX()==tileTwo.nX() )return true;
                  
                }
                
              }else if(tileTwo.quarter()==3){
                if(tileOne.nX()==tileTwo.nX()&&tileOne.nX()==0){
                  if(tileOne.nY()==tileTwo.nY() )return true;
                  
                }
              }
              
              
            }else if(tileOne.quarter()==1){
              if(tileTwo.quarter()==0){
                if(tileOne.nY()==tileTwo.nY()&&tileOne.nY()==0){
                  if(tileOne.nX()==tileTwo.nX() )return true;
                  
                }
                
              }else if(tileTwo.quarter()==2){
                if(tileOne.nX()==tileTwo.nX()&&tileOne.nX()==0){
                  if(tileOne.nY()==tileTwo.nY() )return true;
                  
                }
              }
              
              
            } else if(tileOne.quarter()==2){
              if(tileTwo.quarter()==3){
                if(tileOne.nY()==tileTwo.nY()&&tileOne.nY()==0){
                  if(tileOne.nX()==tileTwo.nX() )return true;
                  
                }
                
              }else if(tileTwo.quarter()==1){
                if(tileOne.nX()==tileTwo.nX()&&tileOne.nX()==0){
                  if(tileOne.nY()==tileTwo.nY() )return true;
                  
                }
              }
            } else if(tileOne.quarter()==3){
              if(tileTwo.quarter()==2){
                if(tileOne.nY()==tileTwo.nY()&&tileOne.nY()==0){
                  if(tileOne.nX()==tileTwo.nX() )return true;
                  
                }
                
              }else if(tileTwo.quarter()==0){
                if(tileOne.nX()==tileTwo.nX()&&tileOne.nX()==0){
                  if(tileOne.nY()==tileTwo.nY() )return true;
                  
                }
              }              
            }            
          }          
        }        
      }      
    }    
  }
  
  return near;
  
}


bool MuonClusterTool::isIncluded(int station,LHCb::MuonCluster* cluster)
{
  std::vector<std::pair<LHCb::MuonCluster *,int> >::const_iterator it;  
  const SmartRefVector<LHCb::MuonCoord> mycoord =(cluster)->coords();
  for(it= m_inputclust[station].begin();it!= m_inputclust[station].end();it++){
    verbose()<<    it->first<<" "<<cluster<<endreq;
    
    if(it->first==cluster)continue;
    MuonCluster* other=it->first;
    const SmartRefVector<LHCb::MuonCoord> othercoord =(other)->coords();
    SmartRefVector<LHCb::MuonCoord>::const_iterator myit;
    SmartRefVector<LHCb::MuonCoord>::const_iterator otherit;
    for(myit=mycoord.begin();myit!=mycoord.end();myit++){
      MuonTileID mytile((*myit)->key());
      bool same=false;
      verbose()<<" looking for "<<mytile<<endreq;
      
      for(otherit=othercoord.begin();otherit!=othercoord.end();otherit++){
        MuonTileID othertile((*otherit)->key());
        verbose()<<" check "<<othertile<<endreq;
        
        if(othertile==mytile){
          same=true;
          break;
        }
        
      }
      
      if(!same)break;
      
      if(same&&myit==mycoord.end()-1)return true;
      
    } //return true;
  }
  return false;
  
}
