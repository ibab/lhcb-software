// $Id: DeRichHPDPanel.cpp,v 1.5 2003-06-20 14:34:33 papanest Exp $
#define DERICHHPDPANEL_CPP

// Include files
#include "RichDet/DeRichHPDPanel.h"

// CLHEP files
#include "CLHEP/Units/SystemOfUnits.h"


//----------------------------------------------------------------------------
//
// Implementation of class :  DeRichHPDPanel
//
//----------------------------------------------------------------------------


// Standard Constructor
DeRichHPDPanel::DeRichHPDPanel()
{}
  
// Standard Destructor
DeRichHPDPanel::~DeRichHPDPanel() {}


StatusCode DeRichHPDPanel::readoutChannelList(std::vector<RichSmartID>&
                                             readoutChannels) {

  double pixelSize = 0.5*mm;
  int pixColMax = 32;
  int pixRowMax= 32;
  double activeRadius = 7.5*mm;
  double radius = 8.7*mm;
  unsigned int my_PDMax;
  my_PDMax = static_cast<unsigned int>(m_PDMax);  

  for (unsigned int PD = 0; PD < my_PDMax; ++PD) {
    for (int pixRow = 0; pixRow < pixRowMax; pixRow++) {        
      for (int pixCol = 0; pixCol < pixColMax; pixCol++) {

        double xpix = (pixRow + 0.5)*pixelSize - radius;
        double ypix = (pixCol + 0.5)*pixelSize - radius;
        double xcorn = 9999.;
        double ycorn = 9999.;
        
        if( (xpix < 0.0) && (ypix < 0.0) ) {
          xcorn = xpix - 0.5*pixelSize ;
          ycorn = ypix - 0.5*pixelSize ;
        }
        
        if( (xpix < 0.0) && (ypix >= 0.0) ) {
          xcorn = xpix - 0.5*pixelSize ;
          ycorn = ypix + 0.5*pixelSize ;
        }
        
        if( (xpix >= 0.0) && (ypix >= 0.0) ) {
          xcorn = xpix + 0.5*pixelSize ;
          ycorn = ypix + 0.5*pixelSize ;
        }
        
        if( (xpix >= 0.0) && (ypix < 0.0) ) {
          xcorn = xpix + 0.5*pixelSize ;
          ycorn = ypix - 0.5*pixelSize ;
        }
        
        double radcorn = sqrt(xcorn*xcorn + ycorn*ycorn) ;
        if(radcorn <= (activeRadius) ) {          
          RichSmartID id(0,0,PDRow(PD),PDCol(PD),pixRow,pixCol);
          readoutChannels.push_back(id);
        }
      }   

    }
        
  }

  return StatusCode::SUCCESS;
}

