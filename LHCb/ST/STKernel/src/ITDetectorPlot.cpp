#include "Kernel/ITDetectorPlot.h"
#include "Kernel/STDAQDefinitions.h"
#include <string>

using namespace ST;

ITDetectorPlot::ITDetectorPlot(const std::string& name, const std::string& title,
                               const unsigned int xBinsPerSector):
  ST::Histo2DProperties(name,title,29*xBinsPerSector, 51, -14.5, 14.5, -0.5, 50.5) 
{
  // constructor
  m_xBinsPerSector = xBinsPerSector;
  m_plotByBeetle = false;
  m_plotByPort = false;
  if(3u == m_xBinsPerSector) m_plotByBeetle = true;
  if(12u == m_xBinsPerSector) m_plotByPort = true;
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
     theBins.xBin = 4. - sector;
     if (box == 3) { 
       theBins.yBin += 4 + layer;
     }   
     else {
       theBins.yBin += 12 + layer;
     }
   } // top or bottom box
   else if (box == 1) {
     theBins.yBin += 8 + layer;
     theBins.xBin = 11. - sector;
   } // cside
   else {
     theBins.yBin += 8 + layer;
     theBins.xBin = -3. - sector;
   }  
   const unsigned int strip=chanID.strip();
   if(m_plotByBeetle) {
     unsigned int beetle=(strip-1)/(STDAQ::nstrips*STDAQ::nports);
     theBins.xBin += 0.5 - (beetle+0.5)/m_xBinsPerSector;
   } else if(m_plotByPort) {
     unsigned int port=(strip-1)/(STDAQ::nstrips);
     theBins.xBin += 0.5 - (port+0.5)/m_xBinsPerSector;
   }
   return theBins;
}




