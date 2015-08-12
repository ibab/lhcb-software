// Include files 

// local
#include "RichRingRecResult.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingRecResult
//
// 2007-06-17 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRingRecResult )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingRecResult::RichRingRecResult( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  :RichRingRecToolBase  ( type, name , parent ),
   m_TrackFoundRadius(VVD(0)),
   m_TrackFoundMeanRadius(VVD(0)),
   m_TrackFoundMRNumHits(VVD(0)),
   m_HitSelectedInRecRings(VVB(0)),
   m_TrackSegReconMass(VVD(0)),
   m_TrackSegReconBG(VVD(0)),
   m_bestReconMass(VVD(0)),
   m_bestTrackSegInRadiator(VVI(0)),
   m_bestTrackRadiator(VVI(0)),
   m_allSelectedSegmentsIndex(VVI(0))
   //  m_TrackBestReconPid(VI(0))
{
  declareInterface<IRichRingRecResult>(this);

}
void RichRingRecResult::clearReconSegmentArrays(){

  m_TrackFoundRadius.clear();
  m_TrackFoundMeanRadius.clear();
  m_TrackFoundMRNumHits.clear();
  m_HitSelectedInRecRings.clear();
  m_TrackSegReconMass.clear();
  m_TrackSegReconBG.clear();
  
  
  int nrad = RConst()->maxNumRadiator();
  
  m_TrackFoundRadius.resize(nrad);
  m_TrackFoundMeanRadius.resize(nrad);
  m_TrackFoundMRNumHits.resize(nrad);
  m_HitSelectedInRecRings.resize(nrad);
  m_TrackSegReconMass.resize(nrad);
  m_TrackSegReconBG.resize(nrad);

  
  

   for(int irad=0; irad< nrad; ++irad){
    int ntkSeg = tgD()->  NumChRdTrackValue(irad);
    m_TrackFoundRadius [irad].assign(ntkSeg,-1.0); //default radius of -1.
    m_TrackFoundMeanRadius [irad].assign(ntkSeg,-1.0); //default radius of -1.
    m_TrackFoundMRNumHits[irad].assign(ntkSeg, 0.0); //default numhits 0.
    m_TrackSegReconMass[irad].assign(ntkSeg, -1000.0); 
                                                // default mass of -1000.
    m_TrackSegReconBG[irad].assign(ntkSeg, -100.0); // default BG of -100
     
    int iRich = Tfm()->RichDetNumFromRadiator(irad);
    int  aNumHitsTarget = tgD()->NumHitsTarget(iRich);
    m_HitSelectedInRecRings [irad].assign(aNumHitsTarget,false);

  }
   
}
void RichRingRecResult::clearResultTrackArrays(){
   // now for the track level vectors
   m_bestReconMass.clear();       
   m_bestTrackSegInRadiator.clear();
   m_allSelectedSegmentsIndex.clear();
   // m_TrackBestReconPid.clear();
   
}

void RichRingRecResult::resetResultTrackArrays(){
  clearResultTrackArrays();
  int numPartType=2;  // this 2 is for light/heavy particles
  VD aMassVect(numPartType, -100.0);  
  m_bestReconMass.assign( m_numberOfRichRecTrack,aMassVect );

  VI aRadSegDefault ( numPartType, -10);
  VI aRadDefault ( numPartType, -10);
  m_bestTrackSegInRadiator.assign(m_numberOfRichRecTrack, aRadSegDefault);
  m_bestTrackRadiator.assign(m_numberOfRichRecTrack, aRadDefault);

  VI aRadIndexDefault(3, -10);
  m_allSelectedSegmentsIndex.assign(m_numberOfRichRecTrack,aRadIndexDefault);

  // m_TrackBestReconPid.assign(m_numberOfRichRecTrack,-10);
  
}

void RichRingRecResult::StoreBestReconMassInfo (double aMass, int aRadiatorIndex, int aTrackSegIndex ,int aTrackIndex,
   int amassindex){
  if( (aTrackIndex >= 0)  &&  (aTrackIndex < m_numberOfRichRecTrack) ) {
    
    m_bestReconMass[aTrackIndex] [amassindex] = aMass;
    m_bestTrackSegInRadiator[aTrackIndex] [amassindex] = aTrackSegIndex;
    m_bestTrackRadiator[aTrackIndex] [amassindex] =aRadiatorIndex;
    
  } 
  
}


void RichRingRecResult::StoreSelectedTrackIndex(VI TkSegIndex, int aTrackIndex) 
{ m_allSelectedSegmentsIndex [aTrackIndex] =  TkSegIndex;}


//void  RichRingRecResult::setTrackBestReconPidValue(int apid, int aTrackIndex) 
//{
//  if( (aTrackIndex >= 0)  &&  (aTrackIndex < m_numberOfRichRecTrack) ) {
//    m_TrackBestReconPid[aTrackIndex] =apid;
//  }
//}


//=============================================================================
// Destructor
//=============================================================================
RichRingRecResult::~RichRingRecResult() {} 

//=============================================================================
