// $Id: MuBgTest.cpp,v 1.2 2003-04-16 08:33:55 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "MuonDet/MuonBasicGeometry.h"
#include "Event/MCMuonHit.h"
#include "MuonUtils/MuonGeometryStore.h" 
// local
#include "MuBgTest.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuBgTest
//
// 2003-03-10 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuBgTest>          s_factory ;
const        IAlgFactory& MuBgTestFactory = s_factory ; 


std::string MuBgTest::spill[5] = 
{"","/Prev","/PrevPrev","/Next","/NextNext"};
std::string MuBgTest ::numreg[4] = 
{"1","2","3","4"};
std::string  MuBgTest::numsta[5]= 
{"1","2","3","4","5"};
 std::string  MuBgTest::TESPathOfHitsContainer[4]=
{"Hits","ChamberNoiseHits","FlatSpilloverHits","BackgroundHits"};



//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuBgTest::MuBgTest( const std::string& name,
                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Destructor
//=============================================================================
MuBgTest::~MuBgTest() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuBgTest::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::INFO << "==> Initialise" << endreq;
  MuonBasicGeometry basegeometry(detSvc(),msgSvc()); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    msg<<MSG::INFO<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
  m_partition=basegeometry.getPartitions();
  std::string spillPath[5]={"","PREV/","PrevPrev/","Next/","NextNext/"};
  for(int ll=0;ll<4;ll++){    
    for (int y=0;y<5;y++){
      for (int kk=0;kk<4;kk++){
        std::string spillString=spillPath[ll];        
        int hh;
        hh=y*4+kk;
        char label[10];
        if(hh<9)sprintf(label,"%1i",hh+1);
        
        if(hh>=9)sprintf(label,"%2i",hh+1);
        hh=hh+20*ll;
        
        //msg<<MSG::INFO<<label<<endreq;
        std::string ap="RADIAL/";
        
        std::string label2=spillString+ap+label;
        msg<<MSG::INFO<<label2<<" "<<hh<<endreq;
        m_pointer1Dradial[hh]=
          histoSvc()->book( label2, "Radial multiplicity",200, 0., 6000. );
        
        std::string sicb="SICB/RADIAL/";
        std::string sicb2=spillString+sicb+label;
                m_sicbpointer1Dradial[hh]=
         histoSvc()->book( sicb2, "Radial multiplicity",200, 0., 6000. );

        std::string ap3="RADIALVSPHI/";
        std::string label3=spillString+ap3+label;
        m_pointer2Dphivsradial[hh] = 
          histoSvc()->book( label3, "phi vs Radial multiplicity",
                            20, 0., 6000., 20, -pi, pi );
        std::string sicb3="SICB/RADIALVSPHI/";      
        std::string sicblabel3=spillString+sicb3+label;
        m_sicbpointer2Dphivsradial[hh] = 
          histoSvc()->book( sicblabel3, "phi vs Radial multiplicity",
                            20, 0., 6000., 20, -pi, pi );
        
        std::string ap4="RADIALVSPHILOC/";
        std::string label4=spillString+ap4+label;
        m_pointer2Dphilocvsradial[hh] = 
        histoSvc()->book( label4, "phi loc vs Radial multiplicity",
                          20, 0., 6000., 20, -pi, pi );
        
        std::string ap5="RADIALVSTHETALOC/";
        std::string label5=spillString+ap5+label;
        m_pointer2Dthetalocvsradial[hh] = 
        histoSvc()->book( label5, "theta loc vs Radial multiplicity",
                          20, 0., 6000., 20, 0., pi );
        
        std::string ap6="RADIALVSTIMELOG/";
        std::string label6=spillString+ap6+label;
        m_pointer2Dtimelogvsradial[hh] = 
        histoSvc()->book( label6, "log time vs Radial multiplicity",
                          20, 0., 6000., 50, -1, 9 );
        
        std::string ap7="RADIALVSTIME/";
        std::string label7=spillString+ap7+label;
        m_pointer2Dtimevsradial[hh] = 
          histoSvc()->book( label7, "time vs Radial multiplicity pp",
                            20, 0., 6000., 50, 0., 100. );        
        std::string ap8="SICB/RADIALVSTIMELOG/";
        std::string label8=spillString+ap8+label;
        m_sicbpointer2Dtimelogvsradial[hh] = 
        histoSvc()->book( label8, "log time vs Radial multiplicity",
                          20, 0., 6000., 50, -1, 9 );
        
        std::string ap9="SICB/RADIALVSTIME/";
        std::string label9=spillString+ap9+label;
        m_sicbpointer2Dtimevsradial[hh] = 
          histoSvc()->book( label9, "time vs Radial multiplicity pp",
                            20, 0., 6000., 50, 0., 100. );        
      }
    }
  }
  




  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuBgTest::execute() {


  MsgStream  msg( msgSvc(), name() );
  msg << MSG::DEBUG << "==> Execute" << endreq;
  //loop on the bkg containers
  //int ispill=0;
  //float distToll=100;
  //float timeToll=3;
  //bool stop=false;
  //float r, phi,time,philoc,thetaloc;
  
  for(int ispill=0;ispill<4;ispill++){
    
    for(int partition=0;partition<m_partition;partition++){
      int station=partition/4;
      int region=partition%4;
      //    msg<<MSG::INFO<<" station region "<<station<<" "<<region<<endreq;
    
      std::string path="/Event"+spill[ispill]+"/MC/Muon/"+numsta[station]+
        "/R"+numreg[region]+"/"+"BackgroundHits";  
      SmartDataPtr<MCMuonHits> hitPointer(eventSvc(),path);
      int track[4];    
      track[0]=0;
      track[1]=0;
      track[2]=0;
      track[3]=0;
      fillHisto(station,region,ispill,hitPointer,track,0);    
      msg<<MSG::DEBUG<<" partition: "<<partition<<" multiplicity tracks "
         <<track[0]<< " "<<
        track[1]<< " "<<
        track[2]<< " "<<
        track[3]<< endreq;   
      track[0]=0;
      track[1]=0;
      track[2]=0;
      track[3]=0;
      std::string path1="/Event"+spill[ispill]+"/MC/Muon/"+numsta[station]+
        "/R"+numreg[region]+"/"+TESPathOfHitsContainer[0];  
      SmartDataPtr<MCMuonHits> hitPointerData(eventSvc(),path1);
      
      fillHisto(station,region,ispill,hitPointerData,track,1);    
      
    }  
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuBgTest::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
 
StatusCode MuBgTest::fillHisto(int station,int region, int ispill,
                               SmartDataPtr<MCMuonHits> hitPointer,int* track,
                               int type)
{
  
  MsgStream msg(msgSvc(), name());
  float distToll=100;
  float timeToll=3;
  bool stop=false;
  
  

  if(hitPointer!=0){
    MCMuonHits::const_iterator i,m;
    for (i=(hitPointer)->begin();i<(hitPointer)->end();i++){      
      // collect information
      int chamber=(*i)->chamberID();
      int gap=(*i)->gapID();
      float xpos=((*i)->entry().x()+(*i)->exit().x())/2.0;
      float ypos=((*i)->entry().y()+(*i)->exit().y())/2.0;
      float zpos=((*i)->entry().z()+(*i)->exit().z())/2.0;
      float time=(*i)->timeOfFlight();
      float tof=time-sqrt(xpos*xpos+ypos*ypos+zpos*zpos)/300.0;      
      if(tof<0.1)tof=0.1;
      
      msg<<MSG::DEBUG<<" hit "<<region<<" "<<
        chamber<<" "<<gap<<" "<<time<<" "<<
        xpos<<" "<<ypos<<endreq;
      int multi=0;
      stop=false;
      float r=sqrt(xpos*xpos+ypos*ypos);
      float phi;
      
      if(xpos!=0){phi=atan(ypos/xpos);
      }
      else{
        phi=pi/2;
        
      }        
      if(xpos<0&&ypos>0)phi=phi+pi;
      if(xpos<0&&ypos<0)phi=phi-pi;        
      for (m=i+1;m<(hitPointer)->end()&&!stop;m++){
        int chamberloop=(*m)->chamberID();
        int gaploop=(*m)->gapID();
        float xposloop=((*m)->entry().x()+(*m)->exit().x())/2.0;
        float yposloop=((*m)->entry().y()+(*m)->exit().y())/2.0;
        float timeloop=(*m)->timeOfFlight();
        msg<<MSG::DEBUG<<" hit in loop "<<chamberloop<<" "
           <<gaploop<<" "<<timeloop<<" "<<
          xposloop<<" "<<yposloop<<endreq;
        
        if(chamber==chamberloop){
        //msg<<MSG::INFO<<"ciao "<<endreq;
        // msg<<MSG::INFO<<"ciao "<<fabs(time-timeloop)<<" "<<timeToll<<
        //                            endreq;
          if(fabs(time-timeloop)<timeToll){  
          //  msg<<MSG::INFO<<"ciao "<<fabs(time-timeloop)<<" "<<timeToll<<
          //endreq;
            if(fabs(xposloop-xpos)<distToll&&fabs(yposloop-ypos)<distToll){
              // msg<<MSG::INFO<<"ciao "<<fabs(xposloop-xpos)<<" "<<
              // fabs(yposloop-ypos)<<" "<<distToll<<endreq;
              r=r+sqrt(xposloop*xposloop+yposloop*yposloop);              
              multi++; 
              //msg<<MSG::INFO<<" correspondingto the hit ...."
            //<<multi<<endreq;
              
            }else stop=true;            
          }else stop=true;
        }else stop=true;        
      }
      
      if(multi>0){
      //  msg<<MSG::INFO<<"multi"<<endreq;
        i=i+multi;          
      }
      //msg<<MSG::INFO<<" multi "<<multi<<endreq;
      r=r/(multi+1);      
      if(multi>3)multi=3;
      if(multi==3){
        msg<<MSG::DEBUG<<" xpos ypos "<<xpos<<" "<<ypos<<endreq;
        
      }      
      int hh=station*4+multi+ispill*20;        
      if(type==0){
        
      m_pointer1Dradial[hh]->fill(r,1.0);
      m_pointer2Dphivsradial[hh]->fill(r,phi,1.0);
      m_pointer2Dtimevsradial[hh]->fill(r,tof,1.0);
      m_pointer2Dtimelogvsradial[hh]->fill(r,log10(tof),1.0);
      m_pointer2Dtimevsradial[hh]->fill(r,tof,1.0);
      }else if(type==1){
      m_sicbpointer1Dradial[hh]->fill(r,1.0);
      m_sicbpointer2Dphivsradial[hh]->fill(r,phi,1.0);      
      m_sicbpointer2Dtimelogvsradial[hh]->fill(r,log10(tof),1.0);
      m_sicbpointer2Dtimevsradial[hh]->fill(r,tof,1.0);
          
        }
      
      
      track[multi]= track[multi]+1;      
    }      
    
  }
  return StatusCode::SUCCESS;
}




