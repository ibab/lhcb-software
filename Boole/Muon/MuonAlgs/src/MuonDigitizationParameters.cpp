#ifndef MUONALGS_MUONDIGITIZATIONPARAMETERS_CPP 
#define MUONALGS_MUONDIGITIZATIONPARAMETERS_CPP 1

#include "MuonAlgs/MuonDigitizationParameters.h"
int MuonDigitizationParameters::Parameters::readoutNumber[partition];
unsigned int MuonDigitizationParameters::Parameters::
chamberPerRegion[partition];
unsigned int MuonDigitizationParameters::Parameters::
gapPerRegion[partition];
unsigned int MuonDigitizationParameters::Parameters::
LogMapPerRegion[partition];
float MuonDigitizationParameters::Parameters::
startGapZ[numberGap][partition];
float MuonDigitizationParameters::Parameters::
stopGapZ[numberGap][partition];
unsigned int MuonDigitizationParameters::Parameters::
readoutType[maxReadoutType][partition];
unsigned int MuonDigitizationParameters::Parameters::
LogMapRType[maxReadoutType][partition];
unsigned int MuonDigitizationParameters::Parameters::
LogMapMergex[maxReadoutType][partition];
unsigned int MuonDigitizationParameters::Parameters::
LogMapMergey[maxReadoutType][partition];
unsigned int MuonDigitizationParameters::Parameters::
phChannelNX[maxReadoutType][partition];
unsigned int MuonDigitizationParameters::Parameters::
phChannelNY[maxReadoutType][partition];
float MuonDigitizationParameters::Parameters::
phChannelSizeX[maxReadoutType][partition];
float MuonDigitizationParameters::Parameters::
phChannelSizeY[maxReadoutType][partition];
float MuonDigitizationParameters::Parameters::
startPositionChamberX[numberChamber];
float MuonDigitizationParameters::Parameters::
startPositionChamberY[numberChamber];
float MuonDigitizationParameters::Parameters::
startPositionFirstGapZ[numberChamber];


unsigned int MuonDigitizationParameters::Parameters::
gridXChamber[numberChamber];
unsigned int MuonDigitizationParameters::Parameters::
gridYChamber[numberChamber];
unsigned int MuonDigitizationParameters::Parameters::
quarterChamber[numberChamber];
int MuonDigitizationParameters::Parameters::gapPerFE[partition];
int MuonDigitizationParameters::Parameters::layoutX[nLogMap][partition];
int MuonDigitizationParameters::Parameters::layoutY[nLogMap][partition];

MuonDigitizationParameters::Parameters::
Parameters(IToolSvc* toolSvc, IDataProviderSvc* detSvc, IMessageSvc * msgSvc)
{
  IMuonTileXYZTool* m_pMuonTileXYZ ; 
  static std::string numreg[] = {"1","2","3","4"};
  static std::string numsta[] = {"1","2","3","4","5"};
  static int chGridX[] ={1, 1, 1, 2};
  static int chGridY[] ={1, 2, 4, 8};  
  static std::string geoBase="/dd/Structure/LHCb/Muon/";
  MsgStream log(msgSvc, "MuonDigitizationParameters"); 
  StatusCode sc=toolSvc->retrieveTool("MuonTileIDXYZ",m_pMuonTileXYZ);
	if(sc.isFailure()){
	  log<<MSG::ERROR<<"could not retrieve the tool"<<endreq ;
  }  
  SmartDataPtr<DetectorElement> muonSystem (detSvc,
                                            "/dd/Structure/LHCb/Muon"); 

  MuonBasicGeometry basegeometry(detSvc,msgSvc); 
  m_stationNumber=basegeometry.getStations();
  m_regionNumber=basegeometry.getRegions();
  int i=0;  
  while(i<m_stationNumber){
    numsta[i]=basegeometry.getStationName(i);    
    log<<MSG::INFO<<" station "<<i<<" "<<numsta[i]<<endreq;
    i++;    
  }
  m_partition=basegeometry.getPartitions();    

  // initialize the MuonDigitzationMisc namesapce, 
  //a collection of geometrical parameters
  m_partition=m_stationNumber*m_regionNumber;  
  std::string regionName[20];
  for(i=0;i<m_stationNumber;i++){
    for (int k=0;k<4;k++){
      regionName[i*4+k]=geoBase+numsta[i]+"/R"+numreg[k];
    }
  }
  
  int chamberCounter=0;
	bool firstChamber=true;
  for (int indexRegion=0;indexRegion<m_partition;indexRegion++){
	  firstChamber=true;    
    // fetch logical maps for that region    
    SmartDataPtr<DeMuonRegion>  detRegionPointer
      (detSvc,regionName[indexRegion]);
    if(detRegionPointer){      
      setLogMapPerRegion(indexRegion,detRegionPointer-> numberLogicalMap());
      //
      // --------- logical map ------------------------
      //
      MuonParameters::ReadoutType  rType[maxReadoutType];
      int xMap[maxReadoutType],yMap[maxReadoutType];
      StatusCode sc = 
        detRegionPointer->logicalMap(0,rType[0], xMap[0], yMap[0]);
      if(sc.isFailure()){
      }else{
      }      
      if(getLogMapPerRegion(indexRegion)==2){
        StatusCode sc = 
          detRegionPointer->logicalMap(1,rType[1], xMap[1], yMap[1]);
        if(sc.isFailure()){
        }else{
        }
      }   
      for(int i=0;i<2;i++){   
        setLogMapRType(i,indexRegion,rType[i]);
        setLogMapMergex(i,indexRegion,xMap[i]);
        setLogMapMergey(i,indexRegion,yMap[i]);
      }
      setChamberPerRegion(indexRegion,detRegionPointer->chamberNum());		 
      setReadoutNumber(indexRegion,-1) ;
      setGapPerFE(indexRegion,detRegionPointer->gapsPerFE());
      IDetectorElement::IDEContainer::iterator 
        itChamber=detRegionPointer->childBegin();
      SmartDataPtr<DeMuonChamber> deChamber(detSvc,(*itChamber)->name());
      float chamberXHalfLength;
      float chamberYHalfLength;
      float chamberZHalfLength;
      float chamberXlength;
      float chamberYlength;
      //    float chamberZlength;				
      for(itChamber=detRegionPointer->childBegin();
          itChamber<detRegionPointer->childEnd(); itChamber++){
        SmartDataPtr<DeMuonChamber> deChamber(detSvc,(*itChamber)->name());
        IDetectorElement::IDEContainer::iterator 
          itGap=(deChamber)->childBegin();
        SmartDataPtr<DeMuonGasGap> muGap(detSvc, 
                                         (*(deChamber)->childBegin())->name());
        
        if(muGap!=0){
          IGeometryInfo*  geoGap=muGap->geometry();
          MuonTileID chamberTileID ;			
          m_pMuonTileXYZ->locateChamberTile(geoGap->
                                            toGlobal(HepPoint3D(0,0,0)).x(), 
                                            geoGap->toGlobal
                                            (HepPoint3D(0,0,0)).y(), 
                                            geoGap->
                                            toGlobal(
                                                     HepPoint3D(0,0,0)).z(),
                                            chamberTileID );
          const ISolid* gapSolid=geoGap->lvolume()->solid();
          const SolidBox* gapBox = dynamic_cast<const SolidBox *>(gapSolid);
          chamberXHalfLength= gapBox->xHalfLength() ;
          chamberYHalfLength= gapBox->yHalfLength() ;
          chamberZHalfLength= gapBox->zHalfLength() ;
          chamberXlength= gapBox->xHalfLength()*2.0 ;
          chamberYlength= gapBox->yHalfLength()*2.0 ;
          //        chamberZlength= gapBox->zHalfLength()*2.0 ;				
          setStartChamberPositionX(chamberCounter,geoGap->
                                   toGlobal(HepPoint3D(0,0,0)).
                                   x()-chamberXHalfLength) ; 
          setStartChamberPositionY(chamberCounter,geoGap->
                                   toGlobal(HepPoint3D(0,0,0)).y()
                                   -chamberYHalfLength ); 					
          unsigned int quarter=chamberTileID.quarter();
          setQuadrantChamber(chamberCounter,quarter);	
          unsigned int  gridInX=chamberTileID.nX();
          unsigned int  gridInY=chamberTileID.nY();
          setGridX(chamberCounter,gridInX);
          setGridY(chamberCounter,gridInY);
          float gapZlength= gapBox->zHalfLength() ;				
          float zposgap=geoGap->toGlobal(HepPoint3D(0,0,0)).z();
          float zstart=zposgap-gapZlength;
          setStartPositionFirstGapZ(chamberCounter,zstart);
          
        }
        
        int countGap=0 ;
        for(itGap=(deChamber)->childBegin();
            itGap<(deChamber)->childEnd();itGap++){
          SmartDataPtr<DeMuonGasGap> loopMuGap(detSvc, (*itGap)->name());
          IGeometryInfo*  geoGap=loopMuGap->geometry();
          const ISolid* gapSolid=geoGap->lvolume()->solid();
          const SolidBox* gapBox = dynamic_cast<const SolidBox *>(gapSolid);
			
          float gapZlength= gapBox->zHalfLength() ;				
          float zposgap=geoGap->toGlobal(HepPoint3D(0,0,0)).z();
          float zstart=zposgap-gapZlength;
          float zdiff=zstart-getStartPositionFirstGapZ(chamberCounter);
          if(firstChamber){setStartGapZ(countGap, indexRegion, zdiff);
          setStopGapZ(countGap, indexRegion, zdiff+2*gapZlength);}
          countGap=countGap+1;
        }
        if(firstChamber){setGapPerRegion(indexRegion,countGap);}
        firstChamber=false;
        
        log<<MSG::DEBUG<<"chamber parameter "<<
          regionName[indexRegion]<<" "<<chamberCounter<<" "<<
          getStartChamberPositionX(chamberCounter)
           <<" "<<getStartChamberPositionY(chamberCounter)<<
          " " <<getGridX(chamberCounter)<<" "<<getGridY(chamberCounter)<<
          endreq;
        
        chamberCounter++;				
      }
      if((detRegionPointer->FEAnodeX()!=0)||(detRegionPointer->FEAnodeY()!=0))
        { 
          setReadoutNumber(indexRegion,getReadoutNumber(indexRegion)+1); 	
          setPhChannelNX(getReadoutNumber(indexRegion),indexRegion,
                         detRegionPointer->FEAnodeX());
          setPhChannelNY(getReadoutNumber(indexRegion),indexRegion,
                         detRegionPointer->FEAnodeY());
          
          setPhChannelSizeX(getReadoutNumber(indexRegion),indexRegion,
                            chamberXlength/
                            getPhChannelNX(getReadoutNumber(indexRegion),
                                           indexRegion));
          
          setPhChannelSizeY(getReadoutNumber(indexRegion),indexRegion,
                            chamberYlength/
                            getPhChannelNY(getReadoutNumber(indexRegion),
                                           indexRegion));
          setReadoutType(getReadoutNumber(indexRegion),indexRegion ,
                         MuonParameters::Anode);				
          
          log<<MSG::DEBUG<<"anode ph ch parameters "<< 
            getPhChannelNX(getReadoutNumber(indexRegion),indexRegion)
             <<" "   <<getPhChannelNY(getReadoutNumber(indexRegion),
                                      indexRegion) 
             <<endreq;      
        }
      if((detRegionPointer->FECathodeX()!=0)||(detRegionPointer->
                                               FECathodeY()!=0)){ 
        setReadoutNumber(indexRegion,getReadoutNumber(indexRegion)+1); 	
        setPhChannelNX(getReadoutNumber(indexRegion),indexRegion,
                       detRegionPointer->FECathodeX());
        setPhChannelNY(getReadoutNumber(indexRegion),indexRegion,
                       detRegionPointer->FECathodeY());
        
        setPhChannelSizeX(getReadoutNumber(indexRegion),indexRegion,
                          chamberXlength/
                          getPhChannelNX(getReadoutNumber(indexRegion),
                                         indexRegion));
        
        setPhChannelSizeY(getReadoutNumber(indexRegion),indexRegion,
                          chamberYlength/
                          getPhChannelNY(getReadoutNumber(indexRegion),
                                         indexRegion));
        setReadoutType(getReadoutNumber(indexRegion),indexRegion ,
                       MuonParameters::Cathode);					
        log<<MSG::DEBUG<<"cathode ph ch parameters "<< 
          getPhChannelNX(getReadoutNumber(indexRegion),indexRegion)
           <<" "   <<getPhChannelNY(getReadoutNumber(indexRegion),
                                    indexRegion) <<endreq;
      }
      //			log<<MSG::INFO<<" sono qui"<<endreq;
			for(int ireadout=0; ireadout<=getReadoutNumber(indexRegion);ireadout++){
        int readoutType=getReadoutType(ireadout,indexRegion);
        for (int mreadout=0;mreadout<(int)getLogMapPerRegion(indexRegion);
             mreadout++){
          int readoutMapping=getLogMapRType(mreadout,indexRegion);
          log<<MSG::DEBUG<<" testing layout "<<ireadout<< " "<<
            readoutType<<" "<<readoutMapping<<endreq;
          if(readoutType==readoutMapping){
            
            int valueX=(chGridX[indexRegion%4]*
                        getPhChannelNX(ireadout,indexRegion))/
              getLogMapMergex(mreadout,indexRegion);
            int valueY=(chGridY[indexRegion%4]*getPhChannelNY
                        (ireadout,indexRegion))/getLogMapMergey
              (mreadout,indexRegion);
            setLayoutX(mreadout,indexRegion,valueX);
            setLayoutY(mreadout,indexRegion,valueY);
            log<<MSG::DEBUG<<" map region x "<<indexRegion<<" "
               <<ireadout<< " "<<mreadout<<" "<<
              getLayoutX(mreadout,indexRegion)<<endreq;
            log<<MSG::DEBUG<<" map region y "<<indexRegion<<" "<<
              ireadout<< " "<<mreadout<<" "<<
              getLayoutY(mreadout,indexRegion)<<endreq;
          }
        }			  
			}
      
    }
  }
}
#endif		



