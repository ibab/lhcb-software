// $Id: RichTargetDataTool.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef RICHTARGETDATATOOL_H 
#define RICHTARGETDATATOOL_H 1

// Include files

// from Gaudi
#include "RichRingRec/RichRingRecToolBase.h"
#include "RichRingRec/IRichTargetDataTool.h"            // Interface

/** @class RichTargetDataTool RichTargetDataTool.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-05-14
 */
class RichTargetDataTool : public RichRingRecToolBase ,
                           virtual public IRichTargetDataTool {
public: 
  /// Standard constructor
  RichTargetDataTool( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~RichTargetDataTool( ); ///< Destructor

     int RichDetNumFromRadiator(int irad);
     int RichDetNumFromHitZ(double GlobalZHit);
     void clearHitInputs();
     void clearTrackInputs();
  
     void StoreTargetHitData(VC rh, int rs, VI rinv );
  


  

     double XHitInpValue(int i, int iRich) {return (m_HitInp [iRich] [i]).x();}
     double YHitInpValue(int i, int iRich) {return (m_HitInp [iRich] [i]).y();}
     double ZHitInpValue(int i, int iRich) {return (m_HitInp [iRich] [i]).z();}

     CD CHitInp (int i, int iRich ){return m_HitInp [iRich] [i];}
  
     int  NumHitsTarget(int iRich) {return  m_NumHitsRichInp[iRich]; }
     int  HitInvIndexValue(int i, int iRich) {return m_HitInvIndex[iRich] [i];}

     int NumHitsInp() {return m_NumHitsInp;}
     int NumChTrack() {return m_NumChTrack;}
     void setNumHitsInp(int aNum ){ m_NumHitsInp= aNum;}
     void setNumChTrack(int aNumt){m_NumChTrack=aNumt;}
  
  
    
    

     double TrackXcInpValue(int i, int rad) { return (m_TrackInp [rad] [i]).x(); }
     double TrackYcInpValue(int i, int rad) { return (m_TrackInp [rad] [i]).y(); }
     double TrackZcInpValue(int i, int rad) { return (m_TrackInp [rad] [i]).z(); }
     CD CTrackInp (int i, int rad) { return m_TrackInp [rad] [i];}
     double TrackRecMomValue(int i, int rad){return m_TrackRecMom [rad] [i];}
     int TrackInvIndexValue(int i, int rad ){  return m_TrackInvIndex [rad] [i];} 
     int FindSegmentIndexValue(int aIndex, int rad );
     int NumChRdTrackValue(int rad ) {return m_NumChRdTrack[rad] ; }
     void StoreTargetTrackData(int ns, VC th, VI ti, VD mh );
     int   FindIndexValue(int aIndex, int rad );
  
protected:

private:

      


   // Target Rich hits
   VVC m_HitInp;  // Rich hit XYZ coord on HPD plane.
   VI  m_NumHitsRichInp; // number of hits in each rich det.


   VVI m_HitInvIndex; // inverse mapping of richhit coord to the 
                      // original rich hits in the container
   int m_NumHitsInp;  // total number of Rich hits in the event.


   // Target Track hits
   VVC  m_TrackInp;  // track XYZ on the Rich HPD plane.
   int m_NumChTrack;  // number of tracks in the event
   VI  m_NumChRdTrack; // number of track segments in each radiator
   VVD m_TrackRecMom;   // reconstructed track momentum
   VVI m_TrackInvIndex; // inverse mapping of track hit to the original
                       // richrec segments
  
};
#endif // RICHTARGETDATATOOL_H
