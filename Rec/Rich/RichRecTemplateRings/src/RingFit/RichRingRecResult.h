// $Id: RichRingRecResult.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RINGFIT_RICHRINGRECRESULT_H 
#define RINGFIT_RICHRINGRECRESULT_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichRingRecResult.h"            // Interface


/** @class RichRingRecResult RichRingRecResult.h RingFit/RichRingRecResult.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-17
 */
class RichRingRecResult : public RichRingRecToolBase, 
                          virtual public IRichRingRecResult {
public: 
  /// Standard constructor
  RichRingRecResult( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~RichRingRecResult( ); ///< Destructor

  double TrackFoundRadiusValue (int i, int irad)
  {    return m_TrackFoundRadius [irad] [i];}
  void setTrackFoundRadiusValue (int i, int irad, double aValue){m_TrackFoundRadius [irad] [i]=aValue;}


  double TrackFoundMeanRadiusValue (int i, int irad){    return m_TrackFoundMeanRadius [irad] [i];}

  void setTrackFoundMeanRadiusValue (int i, int irad, double aValue)  {m_TrackFoundMeanRadius [irad] [i]=aValue;  }


  double TrackFoundMRNumHitsValue (int i, int irad){    return m_TrackFoundMRNumHits [irad] [i];}
  void setTrackFoundMRNumHits (int i, int irad, double aValue){m_TrackFoundMRNumHits [irad] [i]=aValue; }
  bool HitSelectedInRecRingsValue (int ihit, int irad ){return  m_HitSelectedInRecRings [irad] [ihit];}
  void  setHitSelectedInRecRingsValue(int ihit, int irad, bool aBoolVal)
                                      { m_HitSelectedInRecRings [irad] [ihit] = aBoolVal;}
  

  void  setTrackSegReconMass(int itk, int irad, double amass )
    {    m_TrackSegReconMass[irad] [itk] = amass;}
  void  setTrackSegReconBG(int itk, int irad,double aBG )
    {m_TrackSegReconBG [irad] [itk]= aBG;}
  
  double TrackSegReconMassValue(int itk, int irad)  
  {    return m_TrackSegReconMass [irad] [itk];}

  double TrackSegReconBGValue(int itk, int irad)
  {  return m_TrackSegReconBG [irad] [itk];}


  double bestReconMassValue(int aTrack, int massindex ){  return m_bestReconMass [aTrack] [massindex] ;}

  VI bestTrackSegInRadiator(int aTrack ) 
  {  return  m_bestTrackSegInRadiator [aTrack];}

  int bestRadiatorValue (int aTrack , int massindex) 
  {  return m_bestTrackRadiator [aTrack] [massindex];}
  int bestTrackSegValue (int aTrack, int massindex ) 
  {  return m_bestTrackSegInRadiator [aTrack] [massindex];}
  
  VI  allSelectedSegmentsIndex (int aTrack )  {  return m_allSelectedSegmentsIndex [aTrack];}
  int SelectedSegmentIndexInRadiatorValue (int aTrack, int arad ) 
  {  return  m_allSelectedSegmentsIndex [aTrack] [arad];}
 

 
  void StoreBestReconMassInfo (double aMass, int aRadiatorIndex, int aTrackSegIndex, int aTrackIndex, int massindex );
  void StoreSelectedTrackIndex( VI TkSegIndex, int aTrackIndex);
  

  
  int numberOfRichRecTrack()  {  return  m_numberOfRichRecTrack;}
  void setnumberOfRichRecTrack(int aNum)  {m_numberOfRichRecTrack=aNum;}
  

  //   int TrackBestReconPidValue(int aTrack) {return m_TrackBestReconPid [aTrack];}         // the best pid reconstructed.
  // void setTrackBestReconPidValue(int apid, int aTrackIndex);
  
  
  

  void clearReconSegmentArrays();
  void clearResultTrackArrays();
  void resetResultTrackArrays();
  
  
protected:

private:

  // tracksegment level vectors
  VVD m_TrackFoundRadius;
  VVD m_TrackFoundMeanRadius;
  VVD m_TrackFoundMRNumHits;
  VVB m_HitSelectedInRecRings;
  VVD m_TrackSegReconMass;
  VVD m_TrackSegReconBG;
  

  // track level vectors
  int m_numberOfRichRecTrack;
  
  VVD  m_bestReconMass;           // each element is a vector of size 2 with best mass [0] for light [1] for heavy.
  VVI  m_bestTrackSegInRadiator; // each element is a vector of size 2 with //[0]=  TrackIndexInBestRadiatorForLight,
                                                                            //[1] = TrackIndexInBestRadiatorForHeavy
  VVI  m_bestTrackRadiator;  // each element is a vector of size 2 with //[0]=  BestRadiatorForLight,
                                                                        //[1]= BestRadiatorForHeavy
   
  VVI  m_allSelectedSegmentsIndex;  // each element is a vector of size 3 which are the SelectedTrackIndex in radiator 0,1,2.
  //  VI m_TrackBestReconPid;          // the best pid reconstructed.

  
};
#endif // RINGFIT_RICHRINGRECRESULT_H
