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
                                                 MuonTileID phChTileID[2]){  
  MuonDigitizationParameters::Parameters usefull ;
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
  
  numberTileOutput=0;

  //    cout<<"calculate "<<station<<" "<<region<<" "<<partition<<" "
  //<<chamber<<" "<<quadrant<<" "<<idX<<" "<<idY<<endl;
  
  unsigned int chamberAbs=chamber;
  //
  // get absolute chamber number to use parameters::usefull
  //

  for(int partit=0; partit<partition; partit++){
    chamberAbs=chamberAbs+usefull.getChamberPerRegion(partit);
  }
  
  //
  // loop over FE channel readout
  //
  for (int readoutNumber=0;readoutNumber<(int)usefull.
         getLogMapPerRegion(partition);readoutNumber++){
    //
    // check if current readout coincides with one of the LogMap readouts
    //
    //    cout<<"logmap type, readout "<<usefull.getLogMapRType
    //(readoutNumber,partition)<<" "<<readout<<" "<<readoutNumber<<endl;
    
    if(usefull.getLogMapRType(readoutNumber,partition)==readout)
      {
        // define order of FE channel according to the MuonTileID 
        //conventions: from 0,0 left,bottom to radial coordinates
        //
        
        for(int countReadout=0; countReadout<=usefull.
              getReadoutNumber(partition);countReadout++)
          {
            if(  usefull.getLogMapRType(readoutNumber,partition)==
                 usefull.getReadoutType(countReadout,partition)){
              numberOfPCX=usefull.getPhChannelNX(countReadout,partition);
              numberOfPCY=usefull.getPhChannelNY(countReadout,partition);
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
        
        idXGlobal=newidX+usefull.getGridX(chamberAbs)*numberOfPCX;
        idYGlobal=newidY+usefull.getGridY(chamberAbs)*numberOfPCY;
        

        //
        //  compute Logical Channel address now
        //
        idLogX=idXGlobal/usefull.getLogMapMergex(readoutNumber,partition);    
        idLogY=idYGlobal/usefull.getLogMapMergey(readoutNumber,partition);   
        //
        // create the tile of the phys chan
        //  
        ++numberTileOutput;

        MuonLayout layout(usefull.getLayoutX(readoutNumber,partition),
                          usefull.getLayoutY(readoutNumber,partition));   
        phChTileID[numberOfTileCreated].setLayout(layout);
        phChTileID[numberOfTileCreated].setStation(station);
        phChTileID[numberOfTileCreated].setReadout(usefull.
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




