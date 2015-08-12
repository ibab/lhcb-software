// Include files 

// local
#include "RichTargetDataTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTargetDataTool
//
// 2007-05-14 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichTargetDataTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichTargetDataTool::RichTargetDataTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : RichRingRecToolBase ( type, name , parent ),
    m_HitInp(VVC(0)),
    m_NumHitsRichInp(VI (0) ),
    m_HitInvIndex(VVI (0)),
    m_TrackInp(VVC(0)),
    m_NumChRdTrack(VI (0)),
    m_TrackRecMom(VVD (0)),
    m_TrackInvIndex(VVI (0))
{
  // the 0 above is just dummy values
  declareInterface<IRichTargetDataTool>(this);

}
//=============================================================================
//=============================================================================
// utility functions
void RichTargetDataTool::clearHitInputs() 
{
  int nri= RConst()-> maxNumRich ();
  
  m_NumHitsInp=0;  
  m_HitInp.clear(); m_HitInp.reserve(nri);  // for the two rich det.
  m_NumHitsRichInp.clear(); m_NumHitsRichInp.reserve(nri );
  m_HitInvIndex.clear(); m_HitInvIndex.reserve(nri);
   

}
void  RichTargetDataTool::clearTrackInputs() 
{
  int nrd =  RConst()-> maxNumRadiator();
  
  m_NumChTrack=0;
  m_NumChRdTrack.clear();m_NumChRdTrack.reserve( nrd);
  m_TrackInp.clear();m_TrackInp.reserve(nrd);
  m_TrackInvIndex.clear(); m_TrackInvIndex.reserve(nrd);
  m_TrackRecMom.clear(); m_TrackRecMom.reserve(nrd); 
  
}

int  RichTargetDataTool::RichDetNumFromRadiator(int irad) {
  int iRich=0;
  if(irad== 2) {iRich =1 ; }
  return iRich;
}

void RichTargetDataTool::StoreTargetHitData(VC rh, int rs, VI rinv ) 
{

  m_HitInp.push_back(rh);
  m_NumHitsRichInp.push_back(rs);
  m_HitInvIndex.push_back(rinv);  

  // test
  //  info() <<"Current rich det in the event for input hits "<< (int) m_HitInp.size() <<endmsg;  
  //  info() << "Number of hits in this Rich in the event "<<rs<<endmsg;
  //   for(int i=0; i< rs; ++i ) {
  //  info()<<" Rich hit coord XYZ "<<i<<"  "<<rh[i].x()<<"  "<<rh[i].y()<<"  "<<rh[i].z()<<endmsg;
  //  }
  //  info () <<"End of hits in a rich "<<endmsg;
  //  end of test

}

void RichTargetDataTool::StoreTargetTrackData(int ns, VC th, VI ti, VD mh ){

  m_NumChRdTrack.push_back(ns);
  m_TrackInp.push_back(th);
  m_TrackInvIndex.push_back(ti);
  m_TrackRecMom.push_back(mh);  

  // test print
  // info() <<" current radiator in the event for tracks "<<((int) m_NumChRdTrack.size()) -1 <<endmsg;
  // info() <<" Number of track segments in this radiator  "<<ns<<endmsg;
  //  for (int i=0; i<ns; ++i ) {
  //  info()<<" Rich track index coordxyz in Hpd Mom "<<i<<"  "<< th[i].x()<<"  "<<th[i].y()<<"  "
  //        <<th[i].z()<<"  "<<mh[i]<<endmsg;
  //                                                      
  //  }
  
  // end test print

}

//=============================================================================

int RichTargetDataTool::RichDetNumFromHitZ(double GlobalZHit) {
  int iRich=0;
  if(GlobalZHit >   ( RConst()->Rich1Rich2ZLimit()) ) {iRich = 1; }
  return iRich;

}
int RichTargetDataTool::FindIndexValue(int aIndex, int rad ){
  int aRadIndex=-100;
  if(aIndex >=0 ) {  
    VI_it aBeg =   m_TrackInvIndex [rad].begin();
    VI_it aEnd =   m_TrackInvIndex [rad].end();
    VI_it aRadInd = find(aBeg,aEnd, aIndex);
    if(aRadInd != aEnd) {    aRadIndex = aRadInd-aBeg;}
  }
  
  return aRadIndex;
}

//=============================================================================
// Destructor
//=============================================================================
RichTargetDataTool::~RichTargetDataTool() {} 

//=============================================================================
