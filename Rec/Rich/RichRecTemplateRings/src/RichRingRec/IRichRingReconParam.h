// $Id: IRichRingReconParam.h,v 1.2 2009-03-23 17:26:21 seaso Exp $
#ifndef RICHRINGREC_IRICHRINGRECONPARAM_H 
#define RICHRINGREC_IRICHRINGRECONPARAM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IRichRingReconParam ( "IRichRingReconParam", 1, 0 );

/** @class IRichRingReconParam IRichRingReconParam.h RichRingRec/IRichRingReconParam.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-12
 */
class IRichRingReconParam : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichRingReconParam; }

  virtual double YAgelShift()=0;
  virtual int MinRadiator()=0;
  virtual int MaxRadiator()=0;
  virtual bool  ActivateSingleEvHisto()=0;
  virtual int MinTrackNumForDebug()=0;
  virtual int MaxTrackNumForDebug()=0;
  virtual bool ActivateSingleTrackDebug()=0;
  virtual StatusCode  initialize()=0;
  virtual std::string RingRecHistoPath()=0;
  virtual bool ActivateRandomPhiLocalScaling()=0;
  virtual bool   ActivateMCCompareMassPrint()=0;
  virtual bool ActivateMaxNumberOfTrackSegmentsInRadiator()=0;
  virtual int MaxNumTrackSegmentsInAerogel() =0;
  virtual int MaxNumTrackSegmentsInR1gas() =0;
  virtual int MaxNumTrackSegmentsInR2gas() =0;
  

  //  virtual bool WriteOutAuxiliaryDebugHisto()=0;
  // virtual std::string RichDebugAuxHistoFileName()=0;
  
  
  

protected:

private:

};
#endif // RICHRINGREC_IRICHRINGRECONPARAM_H
