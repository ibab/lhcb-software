#include "Kernel/TTDetectorPlot.h"
#include "STDet/DeTTSector.h"
#include "Kernel/STChannelID.h"

#include <string>
#include <iostream>

using namespace ST;

TTDetectorPlot::TTDetectorPlot(const std::string& name, const std::string& title):
ST::Histo2DProperties(name,title,21, 81, -10.5, 10.5, -0.5, 80.5) 
{
// constructor
}
 
TTDetectorPlot::~TTDetectorPlot(){}

TTDetectorPlot::Bins TTDetectorPlot::toBins(const DeTTSector* theSector) const {
  
   Bins theBins;
  
   const LHCb::STChannelID chan = theSector->elementID();
   const unsigned int station = chan.station();
   const unsigned int theCol = theSector->column(); 
   station == 1 ? theBins.xBin = theCol - 8: theBins.xBin =  theCol - 9 ;

   // now get the overall bin in y
   theBins.beginBinY = (40 * (station-1u)) + (20* (chan.layer() - 1u)) + 1; 

   theBins.endBinY = theBins.beginBinY;
   std::string position = theSector->position();
   unsigned int nSensors = theSector->sensors().size(); 
   if (position == "B"){
     switch(nSensors){
       case 1:
         theBins.beginBinY += 6;
         theBins.endBinY = theBins.beginBinY + 1; 
         break;
       case 2:
         theBins.beginBinY += 4;
         theBins.endBinY = theBins.beginBinY + 2; 
         break;
       case 3: 
         theBins.beginBinY += 4;
         theBins.endBinY = theBins.beginBinY + 3; 
         break;
     default:
       theBins.endBinY = theBins.beginBinY + 4;  
       break;
     }  // switch sensors
   } 
   else {
     switch(nSensors){
       case 1:
         theBins.beginBinY += 7;
         theBins.endBinY = theBins.beginBinY + 1; 
         break;
       case 2:
         theBins.beginBinY += 8;
         theBins.endBinY = theBins.beginBinY + 2; 
         break;
       case 3: 
         theBins.beginBinY += 7;
         theBins.endBinY = theBins.beginBinY + 3; 
         break;
     default:
       theBins.beginBinY += 10;    
       theBins.endBinY = theBins.beginBinY + 4;  
       break;
     }  // switch sensors
   }   

   return theBins;
}




