// $Id: IRichRingRecResult.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHRINGREC_IRICHRINGRECRESULT_H 
#define RICHRINGREC_IRICHRINGRECRESULT_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "RichRingRec/RichDataArrayDefinitions.h"

static const InterfaceID IID_IRichRingRecResult ( "IRichRingRecResult", 1, 0 );

/** @class IRichRingRecResult IRichRingRecResult.h RichRingRec/IRichRingRecResult.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-17
 */
class IRichRingRecResult : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRichRingRecResult; }

  virtual double TrackFoundRadiusValue (int i, int irad)=0;
  virtual double TrackFoundMeanRadiusValue (int i, int irad)=0;
  virtual double TrackFoundMRNumHitsValue (int i, int irad)=0;

  virtual void setTrackFoundRadiusValue (int i, int irad, double aValue)=0;
  virtual void setTrackFoundMeanRadiusValue (int i, int irad, double aValue)=0;
  virtual void setTrackFoundMRNumHits (int i, int irad, double aValue)=0;
  virtual  void clearReconSegmentArrays()=0;
  virtual bool HitSelectedInRecRingsValue (int ihit, int irad )=0;
  virtual void setHitSelectedInRecRingsValue(int ihit, int irad, bool aBoolVal)=0;
  virtual void  setTrackSegReconMass(int itk, int irad, double amass )=0;
  virtual double  TrackSegReconMassValue(int itk, int irad)=0;
  virtual double  TrackSegReconBGValue(int itk, int irad)=0;
  virtual void setTrackSegReconBG(int itk, int irad,double aBG)=0;
    

  virtual void clearResultTrackArrays()=0;
  virtual void resetResultTrackArrays()=0;
  virtual void  setnumberOfRichRecTrack(int aNum)=0;
  virtual void StoreBestReconMassInfo(double aMass, int aRadiatorIndex, int aTrackSegIndex, int aTrackIndex,
              int amassindex )=0;
  virtual void  StoreSelectedTrackIndex( VI TkSegIndex, int aTrackIndex )=0;
  virtual double bestReconMassValue(int aTrack, int amassindex)=0;
  virtual int bestRadiatorValue (int aTrack , int amassindex)=0;
  virtual int bestTrackSegValue (int aTrack , int amassindex) =0;
  virtual int SelectedSegmentIndexInRadiatorValue (int aTrack, int arad ) =0;
  //  virtual void setTrackBestReconPidValue(int apid, int aTrackIndex)=0;
  // virtual int  TrackBestReconPidValue(int aTrack) =0;
  

  
protected:

private:

};
#endif // RICHRINGREC_IRICHRINGRECRESULT_H
