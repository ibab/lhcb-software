#ifndef MuonAlgs_MuonPhysicalChannelOutput_CPP
#define MuonAlgs_MuonPhysicalChannelOutput_CPP  1
#include "MuonAlgs/MuonPhysicalChannelOutput.h"

#include <iostream>

// constructor from MuonPhysicalChannel
MuonPhysicalChannelOutput::MuonPhysicalChannelOutput
(MuonPhysicalChannel& orig){
  std::vector<MuonHitTraceBack>::iterator iterOrigin;
  
  for(iterOrigin=orig.hitsTraceBack().begin();
      iterOrigin<orig.hitsTraceBack().end();iterOrigin++){
	  m_Hits.push_back((*iterOrigin));
	}
  //  m_Hits = orig.hitsTraceBack();
  setPhChID(*(orig.phChID()));
  m_FiringTime = 0;
}


void MuonPhysicalChannelOutput::calculateTileID( int& numberTileOutput, 
                                                 MuonTileID phChTileID[2],
IMuonGetInfoTool *usefullPointer){  
  
  unsigned int station=phChID()->getStation();
  unsigned int region=phChID()->getRegion();
  unsigned int chamber=phChID()->getChamber();
  unsigned int readout=phChID()->getReadout();
  unsigned int quadrant=phChID()->getQuadrant();
  unsigned int idX=phChID()->getPhChIDX();
  unsigned int idY=phChID()->getPhChIDY();
  unsigned int newidX,newidY;
  unsigned int idXGlobal,idYGlobal;
  unsigned int idLogX,idLogY;
  int numberOfTileCreated=0;
  unsigned int numberOfPCX, numberOfPCY;
  int partition=(station)*4+region;
  //  
  // reset number of distinct phys chan
  //
  newidX=0;
  newidY=0;
  numberOfPCX=0;
  numberOfPCY=0;
  numberTileOutput=0;

  //    cout<<"calculate "<<station<<" "<<region<<" "<<partition<<" "
  //<<chamber<<" "<<quadrant<<" "<<idX<<" "<<idY<<endl;
  
  unsigned int chamberAbs=chamber;
  //
  // get absolute chamber number to use parameters::usefull
  //

  for(int partit=0; partit<partition; partit++){
    chamberAbs=chamberAbs+usefullPointer->getChamberPerRegion(partit);
  }
  
  //
  // loop over FE channel readout
  //
  for (int readoutNumber=0;readoutNumber<(int)usefullPointer->
         getLogMapPerRegion(partition);readoutNumber++){
    //
    // check if current readout coincides with one of the LogMap readouts
    //
    //    cout<<"logmap type, readout "<<usefull.getLogMapRType
    //(readoutNumber,partition)<<" "<<readout<<" "<<readoutNumber<<endl;
    
    if(usefullPointer->getLogMapRType(readoutNumber,partition)==readout)
      {
        // define order of FE channel according to the MuonTileID 
        //conventions: from 0,0 left,bottom to radial coordinates
        //
        
        for(int countReadout=0; countReadout<=usefullPointer->
              getReadoutNumber(partition);countReadout++)
          {
            if(  usefullPointer->getLogMapRType(readoutNumber,partition)==
                 usefullPointer->getReadoutType(countReadout,partition)){
              numberOfPCX=usefullPointer->
                getPhChannelNX(countReadout,partition);
              numberOfPCY=usefullPointer->
                getPhChannelNY(countReadout,partition);
            }
          }
        // 
        // FE ch address relative to the chamber transformed 
        //in quadrant reference system
        //
        if(quadrant==0){
          newidX=idX; 
          newidY=idY;
        }else if(quadrant==3){
          newidX=numberOfPCX-idX-1; 
          newidY=idY;                          
        }else if(quadrant==2){
          newidX=numberOfPCX-idX-1; 
          newidY=numberOfPCY-idY-1;
        }else if(quadrant==1){
          newidX=idX;
          newidY=numberOfPCY-idY-1;
        }
        // 
        // FE ch address in the whole quadrant
        //
        
        idXGlobal=newidX+usefullPointer->getGridX(chamberAbs)*numberOfPCX;
        idYGlobal=newidY+usefullPointer->getGridY(chamberAbs)*numberOfPCY;
        

        //
        //  compute Logical Channel address now
        //
        idLogX=idXGlobal/usefullPointer->
          getLogMapMergex(readoutNumber,partition);    
        idLogY=idYGlobal/usefullPointer->
          getLogMapMergey(readoutNumber,partition);   
        //
        // create the tile of the phys chan
        //  
        ++numberTileOutput;

        MuonLayout layout(usefullPointer->getLayoutX(readoutNumber,partition),
                          usefullPointer->
                          getLayoutY(readoutNumber,partition));   
        phChTileID[numberOfTileCreated].setLayout(layout);
        phChTileID[numberOfTileCreated].setStation(station);
        phChTileID[numberOfTileCreated].setReadout(usefullPointer->
                                                   getLogMapRType
                                                   (readoutNumber,partition));
        phChTileID[numberOfTileCreated].setRegion(region);
        phChTileID[numberOfTileCreated].setQuarter(quadrant);
        phChTileID[numberOfTileCreated].setX(idLogX);
        phChTileID[numberOfTileCreated].setY(idLogY);        
        ++numberOfTileCreated;
      }else{
      }
  }
}
#endif




