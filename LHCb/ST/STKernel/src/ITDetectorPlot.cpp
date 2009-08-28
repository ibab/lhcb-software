#include "Kernel/ITDetectorPlot.h"
#include <string>

using namespace ST;


ITDetectorPlot::ITDetectorPlot(const std::string& name, const std::string& title):
ST::Histo2DProperties(name,title,29, 51, -14.5, 14.5, -0.5, 50.5) 
{
// constructor
}
 
ITDetectorPlot::~ITDetectorPlot(){}

ITDetectorPlot::Bins ITDetectorPlot::toBins(const LHCb::STChannelID& chanID) const {
  
   // method to return the bins in x and y 
    
   const unsigned int station( chanID.station() );
   const unsigned int box( chanID.detRegion() );
   const unsigned int layer( chanID.layer() ); 
   const unsigned int sector( chanID.sector() );

   Bins theBins;
 
   theBins.yBin = 16 * (station - 1);
   theBins.xBin = 0;

   if (box == 3 || box == 4) {
     theBins.xBin = 4 - sector;
     if (box == 3) { 
       theBins.yBin += 4 + layer;
     }   
     else {
       theBins.yBin += 12 + layer;
     }
   } // top or bottom box
   else if (box == 1) {
     theBins.yBin += 8 + layer;
     theBins.xBin = 11 - sector;
   } // cside
   else {
     theBins.yBin += 8 + layer;
     theBins.xBin = -3 - sector;
   }  
   return theBins;
}




