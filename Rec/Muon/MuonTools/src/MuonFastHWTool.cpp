// Include files 

// local
#include "MuonFastHWTool.h"


using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : MuonFastHWTool
//
// 2003-04-16 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MuonFastHWTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonFastHWTool::MuonFastHWTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) 
    , m_stationNumber(0)
    , m_regionNumber(0)
    //, m_DDS()
{
  declareInterface<IMuonFastHWTool>(this);
  
  return ;
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
StatusCode MuonFastHWTool::initialize() {
  
  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;
  debug()<<" initialization "<<endmsg;
 

  m_muonDetector=getDet<DeMuonDetector>
	("/dd/Structure/LHCb/DownstreamRegion/Muon");

  /// get tile tool
  MuonBasicGeometry basegeometry( detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();



  m_layoutX[0][0]=24;
  m_layoutX[0][1]=24;
  m_layoutX[0][2]=24;
  m_layoutX[0][3]=24;
  m_layoutX[0][4]=48;
  m_layoutX[0][5]=48;
  m_layoutX[0][6]=48;
  m_layoutX[0][7]=48;
  m_layoutX[1][4]=8;
  m_layoutX[1][5]=4;
  m_layoutX[1][6]=2;
  m_layoutX[1][7]=2;
  m_layoutX[0][8]=48;
  m_layoutX[0][9]=48;
  m_layoutX[0][10]=48;
  m_layoutX[0][11]=48;
  m_layoutX[1][8]=8;
  m_layoutX[1][9]=4;
  m_layoutX[1][10]=2;
  m_layoutX[1][11]=2;
  m_layoutX[0][12]=12;
  m_layoutX[0][13]=12;
  m_layoutX[0][14]=12;
  m_layoutX[0][15]=12;
  m_layoutX[1][13]=4;
  m_layoutX[1][14]=2;
  m_layoutX[1][15]=2;
  m_layoutX[0][16]=12;
  m_layoutX[0][17]=12;
  m_layoutX[0][18]=12;
  m_layoutX[0][19]=12;
  m_layoutX[1][17]=4;
  m_layoutX[1][18]=2;
  m_layoutX[1][19]=2;

  m_layoutY[0][0]=8;
  m_layoutY[0][1]=8;
  m_layoutY[0][2]=8;
  m_layoutY[0][3]=8;
  m_layoutY[0][4]=1;
  m_layoutY[0][5]=2;
  m_layoutY[0][6]=2; 
  m_layoutY[0][7]=2; 
  m_layoutY[1][4]=8; 
  m_layoutY[1][5]=8; 
  m_layoutY[1][6]=8; 
  m_layoutY[1][7]=8; 
  m_layoutY[0][8]=1; 
  m_layoutY[0][9]=2; 
  m_layoutY[0][10]=2;
  m_layoutY[0][11]=2;
  m_layoutY[1][8]=8;
  m_layoutY[1][9]=8; 
  m_layoutY[1][10]=8;
  m_layoutY[1][11]=8; 
  m_layoutY[0][12]=8; 
  m_layoutY[0][13]=2;
  m_layoutY[0][14]=2;
  m_layoutY[0][15]=2;
  m_layoutY[1][13]=8;
  m_layoutY[1][14]=8;  
  m_layoutY[1][15]=8; 
  m_layoutY[0][16]=8; 
  m_layoutY[0][17]=2; 
  m_layoutY[0][18]=2;
  m_layoutY[0][19]=2;
  m_layoutY[1][17]=8;
  m_layoutY[1][18]=8; 
  m_layoutY[1][19]=8; 


  layout[0]=1;
  layout[1]=1;
  layout[2]=1;  
  layout[3]=1;
  layout[4]=2;
  layout[5]=2;
  layout[6]=2;
  layout[7]=2;  
  layout[8]=2;
  layout[9]=2;
  layout[10]=2;
  layout[11]=2;
  layout[12]=1;
  layout[13]=2;
  layout[14]=2;
  layout[15]=2;
  layout[16]=1;
  layout[17]=2;
  layout[18]=2;
  layout[19]=2;

  long L1Number=0;
  long link_number=0;
  long ODE_number=0;
  long ode_ch=0;
  int add=0;  
  for (int station=0;station<5;station++){
    for (int region=0;region<4;region++){
      int totone=0;
      int tottwo=0;
      totone=4*m_layoutY[0][station*4+region]*m_layoutX[0][station*4+region];
      
      if(layout[station*4+region]==2){
        tottwo=4*m_layoutY[1][station*4+region]*m_layoutX[1][station*4+region];
      }
      for (int quadrant=0;quadrant<4;quadrant++){

        
        m_ODENum[station*16+region*4+quadrant][0].resize(totone);
        m_synchNum[station*16+region*4+quadrant][0].resize(totone);
        if(layout[station*4+region]==2){
          m_ODENum[station*16+region*4+quadrant][1].resize(tottwo);
          m_synchNum[station*16+region*4+quadrant][1].resize(tottwo);
        }
      }
    }
  }




  for (int station=0;station<5;station++){
    for (int region=0;region<4;region++){
      int num_lay=layout[station*4+region];
      for(int ilay=0;ilay<num_lay;ilay++){
        MuonLayout lay(m_layoutX[ilay][station*4+region],
                       m_layoutY[ilay][station*4+region]);
        
        for(int quadrant=0;quadrant<4;quadrant++){
          
          
          for(int x=0;x<m_layoutX[ilay][station*4+region];x++){
            for(int y=0;y<m_layoutY[ilay][station*4+region];y++){
              MuonTileID muontile1(station,lay,region,
                                   quadrant,x+m_layoutX[ilay][station*4+region],
                                   y);
              MuonTileID muontile2(station,lay,region,
                                   quadrant,x,y+
                                   m_layoutY[ilay][station*4+region]);
              MuonTileID muontile3(station,lay,region,
                                   quadrant,x+m_layoutX[ilay][station*4+region],
                                   y+m_layoutY[ilay][station*4+region]);
              
              
              L1Number=0;
              link_number=0;
              ODE_number=0;
              ode_ch=0;
              
              m_muonDetector->getDAQInfo()->findHWNumber(muontile1,
                                                    L1Number,link_number,
                                                    ODE_number,ode_ch);
              
              add=y*m_layoutX[ilay][station*4+region]*2+x+m_layoutX[ilay][station*4+region];
              m_ODENum[station*16+region*4+quadrant][ilay][add]=ODE_number;
              m_synchNum[station*16+region*4+quadrant][ilay][add]=ode_ch;
              
              L1Number=0;
              link_number=0;
              ODE_number=0;
              ode_ch=0;
              
              m_muonDetector->getDAQInfo()->findHWNumber(muontile2,
                                                    L1Number,link_number,  ODE_number,ode_ch);
              add=(y+m_layoutY[ilay][station*4+region])*m_layoutX[ilay][station*4+region]*2+x;
              m_ODENum[station*16+region*4+quadrant][ilay][add]=ODE_number;
              
              m_synchNum[station*16+region*4+quadrant][ilay][add]=ode_ch;
              
              
              
              m_muonDetector->getDAQInfo()->findHWNumber(muontile3,
                                                    L1Number,link_number,
                                                    ODE_number,ode_ch);   
              
              add=(y+m_layoutY[ilay][station*4+region])*m_layoutX[ilay][station*4+region]*2
                +x+m_layoutX[ilay][station*4+region];
              m_ODENum[station*16+region*4+quadrant][ilay][add]=ODE_number;
              m_synchNum[station*16+region*4+quadrant][ilay][add]=ode_ch;  
            }
          }
        }
      }       
    }
  }
  initFEB();
 return StatusCode::SUCCESS ;
}



//=============================================================================
StatusCode MuonFastHWTool::GetHWName(LHCb::MuonTileID tile,int& ODENum,int &SynchOne,int &SynchTwo)
{
  //check wheter tile is strips or cross of two strips

  if(!tile.isValid()) return StatusCode::FAILURE ;

  int reg=tile.region();
  int sta=tile.station();
  int quadrant=tile.quarter();
  MuonLayout tilelay=tile.layout();
  ODENum=-1;
  SynchOne=-1;
  SynchTwo=-1;
  
  if(layout[sta*4+reg]==1){
     int  add=tile.nY()*m_layoutX[0][sta*4+reg]*2+tile.nX();
     ODENum=  m_ODENum[sta*16+reg*4+quadrant][0][add];
     SynchOne= m_synchNum[sta*16+reg*4+quadrant][0][add];  
        
  }else{
    MuonLayout tmplayOne(m_layoutX[0][sta*4+reg],m_layoutY[0][sta*4+reg]);
    MuonLayout tmplayTwo(m_layoutX[1][sta*4+reg],m_layoutY[1][sta*4+reg]);
    
    if(tilelay==tmplayOne||tilelay==tmplayTwo){
      int Numlay=-1;
      
      if(tilelay==tmplayOne)Numlay=1;
      if(tilelay==tmplayTwo)Numlay=2;
      int  add=tile.nY()*m_layoutX[Numlay-1][sta*4+reg]*2+tile.nX();
      ODENum=  m_ODENum[sta*16+reg*4+quadrant][Numlay-1][add];
      SynchOne= m_synchNum[sta*16+reg*4+quadrant][Numlay-1][add];
    }else{ 
      for(int i=0;i<2;i++){
        
        MuonLayout Mylay(m_layoutX[i][sta*4+reg],
                         m_layoutY[i][sta*4+reg]);
        MuonTileID tmp=tile.containerID (Mylay);
        int  add=tmp.nY()*m_layoutX[i][sta*4+reg]*2+tmp.nX();
        ODENum=  m_ODENum[sta*16+reg*4+quadrant][i][add];
       if(i==0) SynchOne= m_synchNum[sta*16+reg*4+quadrant][i][add];     
       if(i==1) SynchTwo= m_synchNum[sta*16+reg*4+quadrant][i][add];
    }
    
    

    
    }
  }
  
  
  return StatusCode::SUCCESS ;
}
  

void MuonFastHWTool::initFEB()
{

  m_realLayout[0]=1;
  m_realLayout[1]=1;
  m_realLayout[2]=1;
  m_realLayout[3]=1;
  m_realLayout[4]=2;
  m_realLayout[5]=2;
  m_realLayout[6]=1;
  m_realLayout[7]=1;
  m_realLayout[8]=2;
  m_realLayout[9]=2;
  m_realLayout[10]=1;
  m_realLayout[11]=1;
  m_realLayout[12]=1;
  m_realLayout[13]=1;
  m_realLayout[14]=1;
  m_realLayout[15]=1;
  m_realLayout[16]=1;
  m_realLayout[17]=1;
  m_realLayout[18]=1;
  m_realLayout[19]=1;
 

  
  m_chamberX[0]=1;
  m_chamberX[1]=1;
  m_chamberX[2]=1;
  m_chamberX[3]=2;
  m_chamberY[0]=1;
  m_chamberY[1]=2;
  m_chamberY[2]=4;
  m_chamberY[3]=8;
 
 
  //info()<<"      qui "<<endmsg;
  int station=-1;
  int region=-1;

  std::vector< MuonLayout> myLayout;
  for (station=0;station<5;station++){
    for (region=0;region<4;region++){

      int NLogicalMap = m_muonDetector->getLogMapInRegion(station,region);



      unsigned int x1=0;
      unsigned int x2=0;
      unsigned int y1=0;
      unsigned int y2=0;
      x1 = m_muonDetector->getLayoutX(0,station,region);
      y1 = m_muonDetector->getLayoutY(0,station,region);
      verbose()<<" station  reg lay "<<station<<" "<<region
            <<" "<<x1<<" "<<y1<<endmsg;
      
      if( NLogicalMap ==2){ 
        x2 = m_muonDetector->getLayoutX(1,station,region);
        y2 = m_muonDetector->getLayoutY(1,station,region);
        verbose()<<" station  reg lay "<<station<<" "<<region
                 <<" "<<x2<<" "<<y2<<endmsg;
      }
      
      if(region==1){
        y1=y1/2;
        y2=y2/2;
      }
      if(region==2){
        y1=y1/4;
        y2=y2/4;
      }
      if(region==3){
        y1=y1/8;
        y2=y2/8;
        x1=x1/2;
        x2=x2/2;
      }
      if(m_realLayout[station*4+region]==1&&NLogicalMap ==1){
       
        MuonLayout layoutOne(x1,y1);
        MuonLayout layoutTwo(x2,y2);
        m_padInChLayout.push_back(layoutOne);
        m_padInChLayout.push_back(layoutTwo);
        verbose()<<"case 1 Layout "<<layoutOne<<endmsg;

      }
       else if(m_realLayout[station*4+region]==1&&NLogicalMap ==2){


        MuonLayout layoutOne(x1,y2);
        MuonLayout layoutTwo(x2,y1);
        m_padInChLayout.push_back(layoutOne);
        m_padInChLayout.push_back(layoutTwo);
        verbose()<<"case 2 Layout "<<layoutOne<<endmsg;
      }else{
        MuonLayout layoutOne(x1,y1);
        MuonLayout layoutTwo(x2,y2);
        m_padInChLayout.push_back(layoutOne);
        m_padInChLayout.push_back(layoutTwo);
        verbose()<<"case 3 Layout "<<layoutOne<<" "<<layoutTwo<<endmsg;

      }
        

    }
  }




  for(int sta=0;sta<5;sta++){
    for(int reg=0;reg<4;reg++){
      int index=sta*4+reg;      
      for(int lay=0;lay<m_realLayout[index];lay++){
        int xx=m_padInChLayout[index*2+lay].xGrid();
        int yy=m_padInChLayout[index*2+lay].yGrid();
        m_I2C[index][lay].resize(xx*yy);
        m_FEB[index][lay].resize(xx*yy);
                         
                               
      }    
    }
    
  }
  
  
  
  //info()<<" step 1"<<endmsg;
  
   //how many daisy chain  per chambers per layout

  //M1
  m_daisy[0][0]=4;
  m_daisy[1][0]=4;
  m_daisy[2][0]=2;
  m_daisy[3][0]=1;
  //M2
  m_daisy[4][0]=1;
  m_daisy[5][0]=1;
  m_daisy[6][0]=2;
  m_daisy[7][0]=1;

  m_daisy[4][1]=2;
  m_daisy[5][1]=2;
  //M3
  m_daisy[8][0]=1;
  m_daisy[9][0]=1;
  m_daisy[10][0]=2;
  m_daisy[11][0]=1;

  m_daisy[8][1]=2;
  m_daisy[9][1]=2;

  //M4
       
  m_daisy[12][0]=2;
  m_daisy[13][0]=1;
  m_daisy[14][0]=1;
  m_daisy[15][0]=1;
  

  //M5
  m_daisy[16][0]=2;
  m_daisy[17][0]=1;
  m_daisy[18][0]=1;
  m_daisy[19][0]=1;

 //M1R1
  m_cardiac[0][0][0]=6;
  m_cardiac[0][0][1]=6;
  m_cardiac[0][0][2]=6;
  m_cardiac[0][0][3]=6;
  //M1R2
  m_cardiac[1][0][0]=4;
  m_cardiac[1][0][1]=8;
  m_cardiac[1][0][2]=4;
  m_cardiac[1][0][3]=8;
  //M1R3
  m_cardiac[2][0][0]=6;
  m_cardiac[2][0][1]=6;
  //M1R4
  m_cardiac[3][0][0]=3;

  //M2R1
  m_cardiac[4][0][0]=6;
  m_cardiac[4][1][0]=4;
  m_cardiac[4][1][1]=4;
  //M2R2
  m_cardiac[5][0][0]=6;
  m_cardiac[5][1][0]=4;
  m_cardiac[5][1][1]=4;
  //M2R3
  m_cardiac[6][0][0]=6;
  m_cardiac[6][0][1]=6;
  //M3R4
  m_cardiac[7][0][0]=3;


  //M3R1
  m_cardiac[8][0][0]=6;
  m_cardiac[8][1][0]=4;
  m_cardiac[8][1][1]=4;
  //M3R2
  m_cardiac[9][0][0]=6;
  m_cardiac[9][1][0]=4;
  m_cardiac[9][1][1]=4;
  //M3R3
  m_cardiac[10][0][0]=6;
  m_cardiac[10][0][1]=6;
  //M3R4
  m_cardiac[11][0][0]=3;

  //M4R1
  m_cardiac[12][0][0]=6;
  m_cardiac[12][0][1]=6;
  //M4R2
  m_cardiac[13][0][0]=6;
  //M4R3
  m_cardiac[14][0][0]=6;
  //M4R4
  m_cardiac[15][0][0]=3;


  //M5R1
  m_cardiac[16][0][0]=6;
  m_cardiac[16][0][1]=6;
  //M5R2
  m_cardiac[17][0][0]=6;
  //M5R3
  m_cardiac[18][0][0]=6;
  //M5R4
  m_cardiac[19][0][0]=3;
  //info()<<" step 2"<<endmsg;
  //fill list of pad in each I2C / FEb  per chamber type
  int index=-1;
  int lay=-1;
  int d=-1;
  int c=-1;
  int qua=0;
  station=-1;
  region=-1;
  
  MuonTileID tileInC;

  int offsetX=0;
  //M1R1
  
  
  station=0; region=0; lay=0;
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  //info()<<" M1R1 "<<offsetX<<endmsg;
  
  d=0;  c=0;
  setI2CFeb(8,12,6,8,offsetX,index,lay,d,c,tileInC);  

  d=0;  c=1;
  setI2CFeb(4,8,6,8,offsetX,index,lay,d,c,tileInC);  

  d=0;  c=2;
  setI2CFeb(0,4,6,8,offsetX,index,lay,d,c,tileInC);

  d=0;  c=3;
  setI2CFeb(4,12,5,6,offsetX,index,lay,d,c,tileInC);  

  d=0;  c=4;
  setI2CFeb(0,4,5,6,offsetX,index,lay,d,c,tileInC);  
  setI2CFeb(8,12,4,5,offsetX,index,lay,d,c,tileInC);  

  d=0;  c=5;
  setI2CFeb(0,8,4,5,offsetX,index,lay,d,c,tileInC);  


  d=1;  c=0;
  setI2CFeb(0,8,3,4,offsetX,index,lay,d,c,tileInC);  

  d=1;  c=1;
  setI2CFeb(8,12,3,4,offsetX,index,lay,d,c,tileInC);  
  setI2CFeb(0,4,2,3,offsetX,index,lay,d,c,tileInC);  

  d=1;  c=2;
  setI2CFeb(4,12,2,3,offsetX,index,lay,d,c,tileInC);  

  d=1;  c=3;
  setI2CFeb(0,4,0,2,offsetX,index,lay,d,c,tileInC);  

  d=1;  c=4;
  setI2CFeb(4,8,0,2,offsetX,index,lay,d,c,tileInC);  

  d=1;  c=5;
  setI2CFeb(8,12,0,2,offsetX,index,lay,d,c,tileInC);  
 
  d=2;  c=0;
  setI2CFeb(12,16,0,2,offsetX,index,lay,d,c,tileInC);  
    
  d=2;  c=1;
  setI2CFeb(16,20,0,2,offsetX,index,lay,d,c,tileInC);  
  d=2;  c=2;
  setI2CFeb(20,24,0,2,offsetX,index,lay,d,c,tileInC);  
      
  d=2;  c=3;
  setI2CFeb(12,20,2,3,offsetX,index,lay,d,c,tileInC);  

  d=2;  c=4;
  setI2CFeb(20,24,2,3,offsetX,index,lay,d,c,tileInC);  
  setI2CFeb(12,16,3,4,offsetX,index,lay,d,c,tileInC);  

  d=2;  c=5;
  setI2CFeb(16,24,3,4,offsetX,index,lay,d,c,tileInC);  

  d=3;  c=0;
  setI2CFeb(16,24,4,5,offsetX,index,lay,d,c,tileInC);  


  d=3;  c=1;
  setI2CFeb(12,16,4,5,offsetX,index,lay,d,c,tileInC);  
  setI2CFeb(20,24,5,6,offsetX,index,lay,d,c,tileInC);

  d=3;  c=2;
  setI2CFeb(12,20,5,6,offsetX,index,lay,d,c,tileInC);  

  d=3;  c=3;
  setI2CFeb(20,24,6,8,offsetX,index,lay,d,c,tileInC);  

  d=3;  c=4;
  setI2CFeb(16,20,6,8,offsetX,index,lay,d,c,tileInC);  

  d=3;  c=5;
  setI2CFeb(12,16,6,8,offsetX,index,lay,d,c,tileInC);  
 

  //info()<<" step 3"<<endmsg;

 //M1R2


  station=0; region=1; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  d=0;  c=0;


  d=0;  c=0;  
  setI2CFeb(0,4,3,4,offsetX,index,lay,d,c,tileInC); 
  
  d=0;  c=1; 
  setI2CFeb(0,4,2,3,offsetX,index,lay,d,c,tileInC); 

  d=0;  c=2;  
  setI2CFeb(0,4,1,2,offsetX,index,lay,d,c,tileInC); 
 
  d=0;  c=3;
  setI2CFeb(0,4,0,1,offsetX,index,lay,d,c,tileInC);

  d=1;  c=0; 
  setI2CFeb(4,6,0,2,offsetX,index,lay,d,c,tileInC); 
  
  d=1;  c=1; 
  setI2CFeb(6,8,0,2,offsetX,index,lay,d,c,tileInC);

  d=1;  c=2;
  setI2CFeb(8,10,0,2,offsetX,index,lay,d,c,tileInC); 
  
  d=1;  c=3; 
  setI2CFeb(10,12,0,2,offsetX,index,lay,d,c,tileInC); 
  
  d=1;  c=4;
  setI2CFeb(12,14,0,2,offsetX,index,lay,d,c,tileInC); 
  
  d=1;  c=5; 
  setI2CFeb(14,16,0,2,offsetX,index,lay,d,c,tileInC); 
  

  d=1;  c=6;  
  setI2CFeb(16,18,0,2,offsetX,index,lay,d,c,tileInC); 
  
  d=1;  c=7;  
  setI2CFeb(18,20,0,2,offsetX,index,lay,d,c,tileInC); 
 





  //first daisy chain cardiac

  d=2;  c=0; 
  setI2CFeb(20,24,0,1,offsetX,index,lay,d,c,tileInC); 

  d=2;  c=1; 
  setI2CFeb(20,24,1,2,offsetX,index,lay,d,c,tileInC);

  d=2;  c=2;  
  setI2CFeb(20,24,2,3,offsetX,index,lay,d,c,tileInC); 

  d=2;  c=3; 
  setI2CFeb(20,24,3,4,offsetX,index,lay,d,c,tileInC); 

  d=3;  c=0;
  setI2CFeb(18,20,2,4,offsetX,index,lay,d,c,tileInC); 

  d=3;  c=1;  
  setI2CFeb(16,18,2,4,offsetX,index,lay,d,c,tileInC);

  d=3;  c=2; 
  setI2CFeb(14,16,2,4,offsetX,index,lay,d,c,tileInC);

  d=3;  c=3;
  setI2CFeb(12,14,2,4,offsetX,index,lay,d,c,tileInC);

  d=3;  c=4;
  setI2CFeb(10,12,2,4,offsetX,index,lay,d,c,tileInC); 

  d=3;  c=5; 
  setI2CFeb(8,10,2,4,offsetX,index,lay,d,c,tileInC);

  d=3;  c=6;
  setI2CFeb(6,8,2,4,offsetX,index,lay,d,c,tileInC); 

  d=3;  c=7; 
  setI2CFeb(4,6,2,4,offsetX,index,lay,d,c,tileInC); 

  //M1R3


  station=0; region=2; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  d=0;  c=0; 
  setI2CFeb(0,4,0,1,offsetX,index,lay,d,c,tileInC); 
 
  d=0;  c=1;
  setI2CFeb(4,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=2;
  setI2CFeb(8,12,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=3;
  setI2CFeb(12,16,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=4; 
  setI2CFeb(16,20,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=5; 
  setI2CFeb(20,24,0,1,offsetX,index,lay,d,c,tileInC);

  d=1;  c=0; 
  setI2CFeb(20,24,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=1;
  setI2CFeb(16,20,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=2;
  setI2CFeb(12,16,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=3; 
  setI2CFeb(8,12,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=4; 
  setI2CFeb(4,8,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=5; setI2CFeb(0,4,1,2,offsetX,index,lay,d,c,tileInC);

  //M1R4
  station=0; region=3; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  d=0;  c=0;
  setI2CFeb(0,4,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(4,8,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=2;
  setI2CFeb(8,12,0,1,offsetX,index,lay,d,c,tileInC);
 
  // info()<<" finita M1 1"<<endmsg;

    //fill one by one the list of tiles for cardiac
  //M2R1
  station=1; region=0; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();

  d=0;  c=0;
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=1;
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);
    
  d=0;  c=3;
  setI2CFeb(24,32,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=4;
  setI2CFeb(32,40,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=5;
  setI2CFeb(40,48,0,1,offsetX,index,lay,d,c,tileInC);
  
  station=1; region=0; lay=1;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
 
  lay=1;
 
  d=0;  c=0;
  setI2CFeb(0,4,0,2,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(0,4,2,4,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=2;
  setI2CFeb(0,4,4,6,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=3; 
  setI2CFeb(0,4,6,8,offsetX,index,lay,d,c,tileInC);
  
  lay=1;

  d=1;  c=3;
  setI2CFeb(4,8,0,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=2; 
  setI2CFeb(4,8,2,4,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=1; 
  setI2CFeb(4,8,4,6,offsetX,index,lay,d,c,tileInC);
 
  d=1;  c=0; 
  setI2CFeb(4,8,6,8,offsetX,index,lay,d,c,tileInC);
  
    //fill one by one the list of tiles for cardiac
  //M2R2
  station=1; region=1; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  
  d=0;  c=0; 
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=3; 
  setI2CFeb(24,32,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=4;
  setI2CFeb(32,40,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=5; 
  setI2CFeb(40,48,0,1,offsetX,index,lay,d,c,tileInC);

  //lay  1

  station=1; region=1; lay=1;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
   
  lay=1;
  d=0;
  

  d=0;  c=0;
  setI2CFeb(0,2,0,1,offsetX,index,lay,d,c,tileInC);
  d=0;  c=1; 
  setI2CFeb(0,2,1,2,offsetX,index,lay,d,c,tileInC);
  d=0;  c=2;  
  setI2CFeb(0,2,2,3,offsetX,index,lay,d,c,tileInC); 
  d=0;  c=3; 
  setI2CFeb(0,2,3,4,offsetX,index,lay,d,c,tileInC);

  lay=1;
  d=1;
  

  d=1;  c=3; 
  setI2CFeb(2,4,0,1,offsetX,index,lay,d,c,tileInC);

  d=1;  c=2; 
  setI2CFeb(2,4,1,2,offsetX,index,lay,d,c,tileInC);

  d=1;  c=1; 
  setI2CFeb(2,4,2,3,offsetX,index,lay,d,c,tileInC);

  d=1;  c=0; 
  setI2CFeb(2,4,3,4,offsetX,index,lay,d,c,tileInC);

    //fill one by one the list of tiles for cardiac
  //M2R3
  station=1; region=2; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  
  
  d=0;  c=0; 
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=1; 
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);
   
  d=0;  c=3; 
  setI2CFeb(24,32,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=4; 
  setI2CFeb(32,40,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=5;
  setI2CFeb(40,48,0,1,offsetX,index,lay,d,c,tileInC);
   
  //second daisy chain cardiac
  d=1;  
  
  d=1;  c=5;
  setI2CFeb(0,8,1,2,offsetX,index,lay,d,c,tileInC);

  d=1;  c=4; 
  setI2CFeb(8,16,1,2,offsetX,index,lay,d,c,tileInC);
 
  d=1;  c=3;
  setI2CFeb(16,24,1,2,offsetX,index,lay,d,c,tileInC);
    
  d=1;  c=2; 
  setI2CFeb(24,32,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=1; 
  setI2CFeb(32,40,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=0; 
  setI2CFeb(40,48,1,2,offsetX,index,lay,d,c,tileInC);
  
  //M2R4
  station=1; region=3; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
    
  //first daisy chain cardiac
  d=0;  c=0; 
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=1; 
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);
  



  //M3R1
  station=2; region=0; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();

  d=0;  c=0;
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=1;
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);
    
  d=0;  c=3;
  setI2CFeb(24,32,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=4;
  setI2CFeb(32,40,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=5;
  setI2CFeb(40,48,0,1,offsetX,index,lay,d,c,tileInC);
  
  station=2; region=0; lay=1;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
 
  lay=1;
 
  d=0;  c=0;
  setI2CFeb(0,4,0,2,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(0,4,2,4,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=2;
  setI2CFeb(0,4,4,6,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=3; 
  setI2CFeb(0,4,6,8,offsetX,index,lay,d,c,tileInC);
  
  lay=1;

  d=1;  c=3;
  setI2CFeb(4,8,0,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=2; 
  setI2CFeb(4,8,2,4,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=1; 
  setI2CFeb(4,8,4,6,offsetX,index,lay,d,c,tileInC);
 
  d=1;  c=0; 
  setI2CFeb(4,8,6,8,offsetX,index,lay,d,c,tileInC);
  
    
  //M3R2
  station=2; region=1; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  
  d=0;  c=0; 
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=3; 
  setI2CFeb(24,32,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=4;
  setI2CFeb(32,40,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=5; 
  setI2CFeb(40,48,0,1,offsetX,index,lay,d,c,tileInC);

  //lay  1

  station=2; region=1; lay=1;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
   
  lay=1;
  d=0;
  

  d=0;  c=0;
  setI2CFeb(0,2,0,1,offsetX,index,lay,d,c,tileInC);
  d=0;  c=1; 
  setI2CFeb(0,2,1,2,offsetX,index,lay,d,c,tileInC);
  d=0;  c=2;  
  setI2CFeb(0,2,2,3,offsetX,index,lay,d,c,tileInC); 
  d=0;  c=3; 
  setI2CFeb(0,2,3,4,offsetX,index,lay,d,c,tileInC);

  lay=1;
  d=1;
  

  d=1;  c=3; 
  setI2CFeb(2,4,0,1,offsetX,index,lay,d,c,tileInC);

  d=1;  c=2; 
  setI2CFeb(2,4,1,2,offsetX,index,lay,d,c,tileInC);

  d=1;  c=1; 
  setI2CFeb(2,4,2,3,offsetX,index,lay,d,c,tileInC);

  d=1;  c=0; 
  setI2CFeb(2,4,3,4,offsetX,index,lay,d,c,tileInC);

    //fill one by one the list of tiles for cardiac
  //M3R3
  station=2; region=2; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  
  
  d=0;  c=0; 
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=1; 
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);
   
  d=0;  c=3; 
  setI2CFeb(24,32,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=4; 
  setI2CFeb(32,40,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=5;
  setI2CFeb(40,48,0,1,offsetX,index,lay,d,c,tileInC);
   
  //second daisy chain cardiac
  d=1;  
  
  d=1;  c=5;
  setI2CFeb(0,8,1,2,offsetX,index,lay,d,c,tileInC);

  d=1;  c=4; 
  setI2CFeb(8,16,1,2,offsetX,index,lay,d,c,tileInC);
 
  d=1;  c=3;
  setI2CFeb(16,24,1,2,offsetX,index,lay,d,c,tileInC);
    
  d=1;  c=2; 
  setI2CFeb(24,32,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=1; 
  setI2CFeb(32,40,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=1;  c=0; 
  setI2CFeb(40,48,1,2,offsetX,index,lay,d,c,tileInC);
  
  //M3R4
  station=2; region=3; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
    
  //first daisy chain cardiac
  d=0;  c=0; 
  setI2CFeb(0,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=1; 
  setI2CFeb(8,16,0,1,offsetX,index,lay,d,c,tileInC);
 
  d=0;  c=2;
  setI2CFeb(16,24,0,1,offsetX,index,lay,d,c,tileInC);
  





  //fill one by one the list of tiles for cardiac
  //M4R1
  station=3; region=0; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  d=0;  c=0;
  
  setI2CFeb(0,6,0,1,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(4,6,1,2,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(0,4,1,3,offsetX,index,lay,d,c,tileInC);

  d=0;  c=2;
  setI2CFeb(4,6,2,3,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(0,6,3,4,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=3;
  setI2CFeb(0,6,4,5,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(4,6,5,6,offsetX,index,lay,d,c,tileInC);

  d=0;  c=4;
  setI2CFeb(0,4,5,7,offsetX,index,lay,d,c,tileInC);
    
  d=0;  c=5;
  setI2CFeb(4,6,6,7,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(0,6,7,8,offsetX,index,lay,d,c,tileInC);

  d=1;  c=0;
  setI2CFeb(6,8,6,7,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,12,7,8,offsetX,index,lay,d,c,tileInC);

  d=1;  c=1;
  setI2CFeb(8,12,5,7,offsetX,index,lay,d,c,tileInC);

  d=1;  c=2;
  setI2CFeb(6,12,4,5,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,8,5,6,offsetX,index,lay,d,c,tileInC);

  d=1;  c=3;
  setI2CFeb(6,8,2,3,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,12,3,4,offsetX,index,lay,d,c,tileInC);

  d=1;  c=4;
  setI2CFeb(8,12,1,3,offsetX,index,lay,d,c,tileInC);

  d=1;c=5;
  setI2CFeb(6,12,0,1,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,8,1,2,offsetX,index,lay,d,c,tileInC);
  

  //fill one by one the list of tiles for cardiac
  //M4R2
  station=3; region=1; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
    
  //first daisy chain cardiac
  d=0;  c=0;

  setI2CFeb(0,2,0,4,offsetX,index,lay,d,c,tileInC);
  d=0;  c=1;
  setI2CFeb(2,4,0,4,offsetX,index,lay,d,c,tileInC);
  d=0;  c=2;
  setI2CFeb(4,6,0,4,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=3;
  setI2CFeb(6,8,0,4,offsetX,index,lay,d,c,tileInC);
  d=0;  c=4;
  setI2CFeb(8,10,0,4,offsetX,index,lay,d,c,tileInC);
    
  d=0;  c=5;
  setI2CFeb(10,12,0,4,offsetX,index,lay,d,c,tileInC);
  //M4R3
  station=3; region=2; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
    
  //first daisy chain cardiac
  d=0;  c=0;
  
  setI2CFeb(0,4,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(4,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=2;
  setI2CFeb(8,12,0,1,offsetX,index,lay,d,c,tileInC);
  
  
  d=0;  c=3;
  setI2CFeb(8,12,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=4;
  setI2CFeb(4,8,1,2,offsetX,index,lay,d,c,tileInC);

    
  d=0;  c=5;
  setI2CFeb(0,4,1,2,offsetX,index,lay,d,c,tileInC);

  //fill one by one the list of tiles for cardiac
  //M4R4
  station=3; region=3; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
 
  
  //first daisy chain cardiac
  d=0;  c=0;
  setI2CFeb(0,2,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(2,4,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=2;
  setI2CFeb(4,6,0,1,offsetX,index,lay,d,c,tileInC);
  
  

 //fill one by one the list of tiles for cardiac
  //M5R1
  station=4; region=0; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
  
  
  //first daisy chain cardiac
  d=0;  c=0;
  
  setI2CFeb(0,6,0,1,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(4,6,1,2,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(0,4,1,3,offsetX,index,lay,d,c,tileInC);

  d=0;  c=2;
  setI2CFeb(4,6,2,3,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(0,6,3,4,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=3;
  setI2CFeb(0,6,4,5,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(4,6,5,6,offsetX,index,lay,d,c,tileInC);

  d=0;  c=4;
  setI2CFeb(0,4,5,7,offsetX,index,lay,d,c,tileInC);
    
  d=0;  c=5;
  setI2CFeb(4,6,6,7,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(0,6,7,8,offsetX,index,lay,d,c,tileInC);

  d=1;  c=0;
  setI2CFeb(6,8,6,7,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,12,7,8,offsetX,index,lay,d,c,tileInC);

  d=1;  c=1;
  setI2CFeb(8,12,5,7,offsetX,index,lay,d,c,tileInC);

  d=1;  c=2;
  setI2CFeb(6,12,4,5,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,8,5,6,offsetX,index,lay,d,c,tileInC);

  d=1;  c=3;
  setI2CFeb(6,8,2,3,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,12,3,4,offsetX,index,lay,d,c,tileInC);

  d=1;  c=4;
  setI2CFeb(8,12,1,3,offsetX,index,lay,d,c,tileInC);

  d=1;c=5;
  setI2CFeb(6,12,0,1,offsetX,index,lay,d,c,tileInC);
  setI2CFeb(6,8,1,2,offsetX,index,lay,d,c,tileInC);
  

  //fill one by one the list of tiles for cardiac
  //M5R2
  station=4; region=1; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
    
  //first daisy chain cardiac
  d=0;  c=0;

  setI2CFeb(0,2,0,4,offsetX,index,lay,d,c,tileInC);
  d=0;  c=1;
  setI2CFeb(2,4,0,4,offsetX,index,lay,d,c,tileInC);
  d=0;  c=2;
  setI2CFeb(4,6,0,4,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=3;
  setI2CFeb(6,8,0,4,offsetX,index,lay,d,c,tileInC);
  d=0;  c=4;
  setI2CFeb(8,10,0,4,offsetX,index,lay,d,c,tileInC);
    
  d=0;  c=5;
  setI2CFeb(10,12,0,4,offsetX,index,lay,d,c,tileInC);
  //M5R3
  station=4; region=2; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
    
  //first daisy chain cardiac
  d=0;  c=0;
  
  setI2CFeb(0,4,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(4,8,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=2;
  setI2CFeb(8,12,0,1,offsetX,index,lay,d,c,tileInC);
  
  
  d=0;  c=3;
  setI2CFeb(8,12,1,2,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=4;
  setI2CFeb(4,8,1,2,offsetX,index,lay,d,c,tileInC);

    
  d=0;  c=5;
  setI2CFeb(0,4,1,2,offsetX,index,lay,d,c,tileInC);

  //fill one by one the list of tiles for cardiac
  //M5R4
  station=4; region=3; lay=0;  
  index=station*4+region;
  tileInC.setStation(station);
  tileInC.setRegion(region);
  tileInC.setQuarter(qua);
  tileInC.setLayout(m_padInChLayout[index*2+lay]);
  
  offsetX=m_padInChLayout[index*2+lay].xGrid();
 
  
  //first daisy chain cardiac
  d=0;  c=0;
  setI2CFeb(0,2,0,1,offsetX,index,lay,d,c,tileInC);

  d=0;  c=1;
  setI2CFeb(2,4,0,1,offsetX,index,lay,d,c,tileInC);
  
  d=0;  c=2;
  setI2CFeb(4,6,0,1,offsetX,index,lay,d,c,tileInC);
  
}




int MuonFastHWTool::getPadChIndex(MuonTileID tile)
{
  //tile is in chamber reference
  unsigned int localX=tile.nX();
  unsigned int localY=tile.nY();
  if(localX>=tile.layout().xGrid())localX=localX-tile.layout().xGrid();
  if(localY>=tile.layout().yGrid())localY=localY-tile.layout().yGrid();
  int index=localY*tile.layout().xGrid()+localX;
  return index;
  
}


void MuonFastHWTool::fillTileParameter(MuonTileID tile,int lay,int i2c,int feb)
{
  int index=tile.station()*4+tile.region();
    
  int padindex=getPadChIndex(tile);
  // info()<<" file tile "<<tile<<" "<<index<<" "<<padindex<<" " <<lay<<endmsg;
  
  m_I2C[index][lay][padindex]=i2c;
  m_FEB[index][lay][padindex]=feb;
  
  
}

StatusCode MuonFastHWTool::GetI2C(LHCb::MuonTileID tile,int& I2C){
  int result;
  
  int index;
  
  int lay;
  
  StatusCode sc=getGlobalIndex(tile,result,index,lay);
  if(sc.isFailure())return sc;
  I2C=-1;
  
  
  I2C=m_I2C[index][lay][result];
 return StatusCode::SUCCESS;  
}

StatusCode MuonFastHWTool::GetFEBInCh(LHCb::MuonTileID tile,int& FEB){
  int result;
  
  int index;
  
  int lay;
  
  StatusCode sc=getGlobalIndex(tile,result,index,lay);
  if(sc.isFailure())return sc;
  FEB=-1;
  
  
  FEB=m_FEB[index][lay][result];
 return StatusCode::SUCCESS;  
  
}

StatusCode MuonFastHWTool::GetFEBInI2C(LHCb::MuonTileID tile,int& FEB){
  int result;
  
  int index;
  
  int lay;
  
  StatusCode sc=getGlobalIndex(tile,result,index,lay);
  if(sc.isFailure())return sc;
  FEB=-1;
  
  
  FEB=m_FEB[index][lay][result];
 return StatusCode::SUCCESS;  
  
}

StatusCode  MuonFastHWTool::getGlobalIndex(MuonTileID tile,int &result,int& index,int& lay)
{
   int region=tile.region();
   int station=tile.station();
   index=station*4+region;
  
   MuonLayout tilelay=tile.layout();
   MuonLayout myLayout(tilelay.xGrid()/m_chamberX[region],tilelay.yGrid()/m_chamberY[region]);
   lay=-1;
  
  //which layout if more than one???
  if(m_realLayout[index]==1){
    if(myLayout== m_padInChLayout[index*2])    lay=0;
    
  }
  if(m_realLayout[index]==2){
    if(myLayout== m_padInChLayout[index*2])    lay=0;
    if(myLayout== m_padInChLayout[index*2+1])    lay=1;
    
  }
  if(lay<0)return StatusCode::FAILURE;
  
  int localX=tile.nX()%(m_padInChLayout[index*2+lay].xGrid());
  int localY=tile.nY()%(m_padInChLayout[index*2+lay].yGrid());
  result=localX+localY* m_padInChLayout[index*2+lay].xGrid();
  return StatusCode::SUCCESS;
  
}
LHCb::MuonTileID MuonFastHWTool::transformTile(int quarter,int chX,int 
chY,MuonTileID tile){
MuonTileID Outtile;
int station=tile.station();
int region=tile.region();
MuonLayout  inLay=tile.layout();
MuonLayout outLay(inLay.xGrid()*m_chamberX[region],inLay.yGrid()*m_chamberY[region]);
Outtile.setStation(station);
Outtile.setRegion(region);
Outtile.setLayout(outLay);
Outtile.setQuarter(quarter);
int Xlocal=tile.nX();
int Ylocal=tile.nY();
int XGlob=Xlocal-inLay.xGrid()+chX*inLay.xGrid();
int YGlob=Ylocal+chY*inLay.yGrid();
Outtile.setX(XGlob);
Outtile.setY(YGlob);
return Outtile;
}
 
  void MuonFastHWTool::setI2CFeb(int xmin, int xmax, 
                                 int ymin, int ymax,
                                 int offsetX,int index,
                                 int lay,int d, int c,
                                 LHCb::MuonTileID tilePart)
{
  for(int ii=xmin;ii<xmax;ii++){
    for(int j=ymin;j<ymax;j++){      
      tilePart.setX(ii+offsetX);
      tilePart.setY(j);
      m_padInCh[index][lay][d][c].push_back(tilePart); 
      fillTileParameter(tilePart,lay,d,c);  
    }
  }


}

