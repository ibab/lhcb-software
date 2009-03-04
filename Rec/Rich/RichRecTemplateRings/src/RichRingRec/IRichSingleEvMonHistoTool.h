// $Id: IRichSingleEvMonHistoTool.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGREC_IRICHSINGLEEVMONHISTOTOOL_H 
#define RICHRINGREC_IRICHSINGLEEVMONHISTOTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichSingleEvMonHistoTool ( "IRichSingleEvMonHistoTool", 1, 0 );

/** @class IRichSingleEvMonHistoTool IRichSingleEvMonHistoTool.h RichRingRec/IRichSingleEvMonHistoTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-18
 */
class IRichSingleEvMonHistoTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichSingleEvMonHistoTool; }

  virtual void PlotHitsAndTracksInEv(int irad) =0;
  virtual void PlotTargetAndTemplate()=0;
  virtual void  PlotInvCorr( VVD aRPInv)=0;

  


  //  virtual void InitDebugHistoSetup()=0;  
  // virtual void WriteOutDebugHistogramsToAuxiliaryFile() =0;

  
  


protected:

private:

};
#endif // RICHRINGREC_IRICHSINGLEEVMONHISTOTOOL_H
