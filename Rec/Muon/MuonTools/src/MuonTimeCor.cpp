// Include files 

// local
#include "MuonTimeCor.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTimeCor
//
// 2009-12-22 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonTimeCor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTimeCor::MuonTimeCor( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMuonTimeCor>(this);
  declareProperty( "TimeCorrectionFiles" , m_correctionFiles);
  declareProperty( "SignCorrection" , m_correctionSign);
  declareProperty( "OutCorrectionFile" , m_correctionFileOut);
}
//=============================================================================
// Destructor
//=============================================================================
MuonTimeCor::~MuonTimeCor() {} 

//=============================================================================




StatusCode MuonTimeCor::initialize()
{

  if(m_correctionFiles.size()!=m_correctionSign.size()){
    error()<<" size of correction signs and file names do not match exit"<<endmsg;
    return StatusCode::FAILURE;
    
  }


  m_muonDetector=getDet<DeMuonDetector>
    (DeMuonLocation::Default);


  m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");

  m_muonHWTool=tool<IMuonFastHWTool>("MuonFastHWTool");
  
  
  if(!m_muonHWTool) info()<<"error retrieving the muon HW fast tool "
                          <<endmsg;


  m_correction[0][0].resize(24*8*12);
  m_correction[1][0].resize(24*8*12);
  m_correction[2][0].resize(24*8*12);
  m_correction[3][0].resize(24*8*12);
  m_correction[4][0].resize(48*1*12);
  m_correction[5][0].resize(48*2*12);
  m_correction[6][0].resize(48*8*12);
  m_correction[7][0].resize(48*8*12);
  m_correction[4][1].resize(8*8*12);
  m_correction[5][1].resize(8*8*12);
  m_correction[8][0].resize(48*1*12);
  m_correction[9][0].resize(48*2*12);
  m_correction[10][0].resize(48*8*12);
  m_correction[11][0].resize(48*8*12);
  m_correction[8][1].resize(8*8*12);
  m_correction[9][1].resize(8*8*12);
  m_correction[12][0].resize(12*8*12);
  m_correction[13][0].resize(12*8*12);
  m_correction[14][0].resize(12*8*12);
  m_correction[15][0].resize(12*8*12);
  m_correction[16][0].resize(12*8*12);
  m_correction[17][0].resize(12*8*12);
  m_correction[18][0].resize(12*8*12);
  m_correction[19][0].resize(12*8*12);
  m_Outcorrection[0][0].resize(24*8*12);
  m_Outcorrection[1][0].resize(24*8*12);
  m_Outcorrection[2][0].resize(24*8*12);
  m_Outcorrection[3][0].resize(24*8*12);
  m_Outcorrection[4][0].resize(48*1*12);
  m_Outcorrection[5][0].resize(48*2*12);
  m_Outcorrection[6][0].resize(48*8*12);
  m_Outcorrection[7][0].resize(48*8*12);
  m_Outcorrection[4][1].resize(8*8*12);
  m_Outcorrection[5][1].resize(8*8*12);
  m_Outcorrection[8][0].resize(48*1*12);
  m_Outcorrection[9][0].resize(48*2*12);
  m_Outcorrection[10][0].resize(48*8*12);
  m_Outcorrection[11][0].resize(48*8*12);
  m_Outcorrection[8][1].resize(8*8*12);
  m_Outcorrection[9][1].resize(8*8*12);
  m_Outcorrection[12][0].resize(12*8*12);
  m_Outcorrection[13][0].resize(12*8*12);
  m_Outcorrection[14][0].resize(12*8*12);
  m_Outcorrection[15][0].resize(12*8*12);
  m_Outcorrection[16][0].resize(12*8*12);
  m_Outcorrection[17][0].resize(12*8*12);
  m_Outcorrection[18][0].resize(12*8*12);
  m_Outcorrection[19][0].resize(12*8*12);

//return StatusCode::SUCCESS;



  std::vector<string>::iterator iFile;
  std::vector<int>::iterator iCorrection=m_correctionSign.begin();
  
  for(iFile=m_correctionFiles.begin();iFile!=m_correctionFiles.end();iFile++){
    int Sign=*iCorrection;
    
    char inbuf[100];
    std::ifstream corList;
    int sn1,sn2,sc1,sc2,cor;
    //  std::string ode;
    char ode[8]="";
    //char testona[10]="";
    //char testina[9]="";
    corList.open(iFile->c_str());
    if(!corList) {
      error() << "Cannot open input file"<<endmsg;
      
      return StatusCode::FAILURE;;
    }    
    if (corList.good() ) {
    
      corList.getline(inbuf,sizeof(inbuf));
      do {
        
        sscanf(inbuf,"%7c  %*4c %d  %*s %*4c %d %*s  %*4c %d %*s %*4c %d %*s %*4c %d",ode,&sn1,&sc1,&sn2,&sc2,&cor);
 
        verbose()<<" test "<<ode<<" "<<sn1<<" "<<sc1<<" "<<sn2<<" "<<sc2<<" "<<cor<<endmsg;
        
        
        std::string pippo=ode;
        std::string puno=pippo.substr(0,6);
        std::string pdue=pippo.substr(6,1);
        std::string mia=puno+"_"+pdue;
        
        
        
        int cc1=0;

        int cc2=0;
        if(sn1>=0){
          cc1=sn1*8+sc1;
          cc2=sn2*8+sc2;
        }
        
  
        
        if(sn1<0){
          cc2=sn1*8+sc1;
          cc1=sn2*8+sc2;
        }
        
        LHCb::MuonTileID recoTile= m_muonDetector->getDAQInfo()->
          getPadTileInODE(mia, cc1, cc2);
 
        setCorrection(recoTile,cor*Sign);

        corList.getline(inbuf,sizeof(inbuf));
      }   while (! corList.eof());
  
 
        
        
 
    }
    iCorrection++;
    
  }
  

return StatusCode::SUCCESS;
}


int MuonTimeCor::getIndex(MuonTileID tile)
{
  //int region=tile.region();  
  //int station=tile.station();
  //int index=station*4+region;
  
  int quarter=tile.quarter();
  
  int result=-1;
  MuonLayout lay=tile.layout();
  int offset=lay.xGrid()*lay.yGrid();
  

  
  if(tile.nY()<lay.yGrid())result=lay.xGrid()*tile.nY()+tile.nX()-lay.xGrid();
  
if(tile.nY()>=lay.yGrid())result=2*lay.xGrid()*(tile.nY()-lay.yGrid())+tile.nX()+lay.xGrid()*lay.yGrid();
  
  result=result+quarter*offset*3;;
  return result; 
}

int MuonTimeCor::getLayer(MuonTileID tile)
{
  int region=tile.region();  
  int station=tile.station();
  if(station==0)return 0;
  if(station>2)return 0;
  if(region>1)return 0;
  if(region==0&&(tile.layout().xGrid()==48&&tile.layout().yGrid()==1))return 0;
  if(region==1&&(tile.layout().xGrid()==48&&tile.layout().yGrid()==2))return 0;
  if(tile.layout().xGrid()==8&&tile.layout().yGrid()==8)return 1;
  if(tile.layout().xGrid()==4&&tile.layout().yGrid()==8)return 1;
  return -1;  
}

StatusCode MuonTimeCor::setCorrection(MuonTileID tile,int corr)
{

  int lay= getLayer(tile);
  int index=getIndex(tile);
  int station=tile.station();
  int region=tile.region();


  m_correction[station*4+region][lay][index]=m_correction[station*4+region][lay][index]+corr;

 return StatusCode::SUCCESS;
 
}
StatusCode MuonTimeCor::getCorrection(MuonTileID tile,int &corr)
{

  int lay= getLayer(tile);
  int index=getIndex(tile);
  int station=tile.station();
  int region=tile.region();
  corr=m_correction[station*4+region][lay][index];
  return StatusCode::SUCCESS;
}
StatusCode MuonTimeCor::setOutCorrection(MuonTileID tile,int corr)
{

  int lay= getLayer(tile);
  int index=getIndex(tile);
  int station=tile.station();
  int region=tile.region();
  m_Outcorrection[station*4+region][lay][index]=corr;
   return StatusCode::SUCCESS;
}
StatusCode MuonTimeCor::getOutCorrection(MuonTileID tile,int &corr)
{

  int lay= getLayer(tile);
  int index=getIndex(tile);
  int station=tile.station();
  int region=tile.region();
  corr=m_Outcorrection[station*4+region][lay][index];
  return StatusCode::SUCCESS;
  
}


StatusCode MuonTimeCor::writeOutCorrection()
{
  
  ofstream outfile(m_correctionFileOut.c_str()); 
  if(!outfile) {
    error() << "Cannot open output file"<<endmsg;
    
    return StatusCode::FAILURE;;
  }  

  for (int station=0;station<5;station++){
    verbose()<<"station "<<station<<endmsg;
    
    for (int region=0;region<4;region++){
      verbose()<<"region "<<region<<endmsg;
      
      for(int qua=0;qua<4;qua++){
        verbose()<<"quarter "<<qua<<endmsg;
        
//        int index=station*4+region;
        for (int chX=0;chX<m_muonHWTool->getChamberGridX(region)*2;chX++){
          verbose()<<" chX "<<chX<<endmsg;
          
          for (int chY=0;chY<m_muonHWTool->getChamberGridY(region)*2;chY++){
            verbose()<<"chY "<<chY<<endmsg;

            if(chX<m_muonHWTool->getChamberGridX(region)&&chY<
               m_muonHWTool->getChamberGridY(region))continue;
            for(int lay=0;lay<m_muonHWTool->getRealLayoutNumber(station,region);lay++){
              verbose()<<" layout "<<lay<<endmsg;

              //      for (int d=0;d<daisy[index][lay];d++){
              for (int d=0;d<m_muonHWTool->getI2CNumber(station,region,lay);d++){
                verbose()<<" I2C "<<d<<endmsg;

                for(int cc=0;cc<m_muonHWTool->getFEBInI2CNumber(station,region,lay,d);cc++){
                  verbose()<<"FEB "<<cc<<endmsg;

                   std::vector<MuonTileID>::iterator iTile;
                   
                   std::vector<MuonTileID>& list=m_muonHWTool->getListOfTile(station,region,lay,d,cc);
                   for(iTile=list.begin();iTile!=list.end();iTile++){
                     MuonTileID Intile=*iTile;
                     MuonTileID Outtile=m_muonHWTool->transformTile(qua,chX,chY,Intile);
                     
                     verbose()<<Intile<<" "<<Outtile<<endmsg;
                     int mycor=-99;
                     
                     getOutCorrection(Outtile,mycor);
                     verbose()<<" the time is "<<mycor<<endmsg;
                     //now get the HW names and numbers
                     
                     int MyODE;
                     int MySynchOne;
                     int MySynchTwo;
        
                     m_muonHWTool-> GetHWName(Outtile,MyODE,MySynchOne,MySynchTwo); 
                     if(MySynchOne<0&&MySynchTwo<0){
                       info()<<"failed "<<Outtile<<endmsg;
                       
                       return StatusCode::FAILURE;
                       
                     }
                     int SNOne=-99;
                     int SCOne=-99;
                     int SNTwo=-99;
                     int SCTwo=-99;
                     
                     if(MySynchOne>=0){
                       SNOne=MySynchOne/8;
                       SCOne=MySynchOne%8;
                     }
                     if(MySynchTwo>=0){
                       SNTwo=MySynchTwo/8;
                       SCTwo=MySynchTwo%8;
                     }
                     std::string ECSName=m_muonDetector->
                       getDAQInfo()->getODEECSName(MyODE);
                     std::string pUno=ECSName.substr(0,6);
                     std::string pDue=ECSName.substr(7,1);
                     std::string ECSODE=pUno+pDue;
                     
                     outfile<<ECSODE<<" SN1["<<SNOne<<"] SC1["<<SCOne<<"] SN2["
                            <<SNTwo<<"] SC2["<<SCTwo<<"] COR["<<mycor<<"] "<<endl;

                     

                   }
                   
                   
                }
              }
            }
          }
          
        }
        
      }
      
    }
    
  }
  outfile.close();
  
  return StatusCode::SUCCESS;
}



StatusCode MuonTimeCor::writeCorrection()
{
  
  ofstream outfile(m_correctionFileOut.c_str()); 
  if(!outfile) {
    error() << "Cannot open output file"<<endmsg;
    
    return StatusCode::FAILURE;;
  }  

  for (int station=0;station<5;station++){
    verbose()<<"station "<<station<<endmsg;
    
    for (int region=0;region<4;region++){
      verbose()<<"region "<<region<<endmsg;
      
      for(int qua=0;qua<4;qua++){
        verbose()<<"quarter "<<qua<<endmsg;
        
//        int index=station*4+region;
        for (int chX=0;chX<m_muonHWTool->getChamberGridX(region)*2;chX++){
          verbose()<<" chX "<<chX<<endmsg;
          
          for (int chY=0;chY<m_muonHWTool->getChamberGridY(region)*2;chY++){
            verbose()<<"chY "<<chY<<endmsg;

            if(chX<m_muonHWTool->getChamberGridX(region)&&chY<
               m_muonHWTool->getChamberGridY(region))continue;
            for(int lay=0;lay<m_muonHWTool->getRealLayoutNumber(station,region);lay++){
              verbose()<<" layout "<<lay<<endmsg;

              //      for (int d=0;d<daisy[index][lay];d++){
              for (int d=0;d<m_muonHWTool->getI2CNumber(station,region,lay);d++){
                verbose()<<" I2C "<<d<<endmsg;

                for(int cc=0;cc<m_muonHWTool->getFEBInI2CNumber(station,region,lay,d);cc++){
                  verbose()<<"FEB "<<cc<<endmsg;

                   std::vector<MuonTileID>::iterator iTile;
                   
                   std::vector<MuonTileID>& list=m_muonHWTool->getListOfTile(station,region,lay,d,cc);
                   for(iTile=list.begin();iTile!=list.end();iTile++){
                     MuonTileID Intile=*iTile;
                     MuonTileID Outtile=m_muonHWTool->transformTile(qua,chX,chY,Intile);
                     
                     verbose()<<Intile<<" "<<Outtile<<endmsg;
                     int mycor=-99;
                     
                     getCorrection(Outtile,mycor);
                     verbose()<<" the time is "<<mycor<<endmsg;
                     //now get the HW names and numbers
                     
                     int MyODE;
                     int MySynchOne;
                     int MySynchTwo;
        
                     m_muonHWTool-> GetHWName(Outtile,MyODE,MySynchOne,MySynchTwo); 

                     if(MySynchOne<0&&MySynchTwo<0){
                       info()<<"failed "<<Outtile<<endmsg;
                       
                       return StatusCode::FAILURE;
                       
                     }
                     int SNOne=-99;
                     int SCOne=-99;
                     int SNTwo=-99;
                     int SCTwo=-99;
                     if(MySynchOne>=0){
                       SNOne=MySynchOne/8;
                       SCOne=MySynchOne%8;
                     }
                     if(MySynchTwo>=0){
                       SNTwo=MySynchTwo/8;
                       SCTwo=MySynchTwo%8;
                     }
                     std::string ECSName=m_muonDetector->
                       getDAQInfo()->getODEECSName(MyODE);
                     std::string pUno=ECSName.substr(0,6);
                     std::string pDue=ECSName.substr(7,1);
                     std::string ECSODE=pUno+pDue;
                     
                     outfile<<ECSODE<<" SN1["<<SNOne<<"] SC1["<<SCOne<<"] SN2["
                            <<SNTwo<<"] SC2["<<SCTwo<<"] COR["<<mycor<<"] "<<endl;

                     

                   }
                   
                   
                }
              }
            }
          }
          
        }
        
      }
      
    }
    
  }
  outfile.close();
  
  return StatusCode::SUCCESS;
}



