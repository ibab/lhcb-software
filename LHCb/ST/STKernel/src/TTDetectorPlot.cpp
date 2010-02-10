#include "Kernel/TTDetectorPlot.h"
#include "STDet/DeTTSector.h"
#include "Kernel/STChannelID.h"

#include <string>

using namespace ST;

TTDetectorPlot::TTDetectorPlot(const std::string& name, 
                               const std::string& title,
                               const unsigned int xBinsPerSector) :
  ST::Histo2DProperties(name, title, 19*xBinsPerSector,134,-9.5,9.5,-0.5,133.5)
{
// constructor
}
 
TTDetectorPlot::~TTDetectorPlot(){}

TTDetectorPlot::Bins TTDetectorPlot::toBins(const DeTTSector* theSector) const {
  
   Bins theBins;
  
   const LHCb::STChannelID chan = theSector->elementID();
   const unsigned int station = chan.station();
   const unsigned int theCol = theSector->column(); 
   station == 1 ? theBins.xBin = 8 - theCol : theBins.xBin = 9 - theCol ;

   // now get the overall bin in y
   theBins.beginBinY = (34 * (2*(station-1u)+chan.layer()-1u)) + 2; 

   theBins.endBinY = theBins.beginBinY;
   std::string position = theSector->position();
   unsigned int nSensors = theSector->sensors().size(); 
   if (position == "B"){
     if( theBins.xBin == 0 ) theBins.beginBinY -= 1;
     switch(nSensors){
       case 1:
         theBins.beginBinY += 12;
         theBins.endBinY = theBins.beginBinY + 2; 
         break;
       case 2:
         theBins.beginBinY += 8;
         theBins.endBinY = theBins.beginBinY + 4; 
         break;
       case 3: 
         theBins.beginBinY += 8;
         theBins.endBinY = theBins.beginBinY + 6; 
         break;
     default:
       theBins.endBinY = theBins.beginBinY + 8;  
       break;
     }  // switch sensors
   } 
   else {
     if( theBins.xBin == 0 ) theBins.beginBinY += 1;
     switch(nSensors){
       case 1:
         theBins.beginBinY += 14;
         theBins.endBinY = theBins.beginBinY + 2; 
         break;
       case 2:
         theBins.beginBinY += 16;
         theBins.endBinY = theBins.beginBinY + 4; 
         break;
       case 3: 
         theBins.beginBinY += 14;
         theBins.endBinY = theBins.beginBinY + 6; 
         break;
     default:
       theBins.beginBinY += 20;    
       theBins.endBinY = theBins.beginBinY + 8;  
       break;
     }  // switch sensors
   }   

   return theBins;
}




