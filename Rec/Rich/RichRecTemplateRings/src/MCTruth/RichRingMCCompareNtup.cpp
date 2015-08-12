// $Id: RichRingMCCompareNtup.cpp,v 1.4 2009-07-30 11:22:55 jonrob Exp $
// Include files

// local
#include "RichRingMCCompareNtup.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingMCCompareNtup
//
// 2007-06-29 : Sajan EASO
//-----------------------------------------------------------------------------

using namespace Rich::Rec::TemplateRings;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichRingMCCompareNtup )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingMCCompareNtup::RichRingMCCompareNtup( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  :RichRingRecTupleAlgBase  ( name , pSvcLocator ),
   m_trSelector        ( NULL )
{

}
//=============================================================================
// Destructor
//=============================================================================
RichRingMCCompareNtup::~RichRingMCCompareNtup() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichRingMCCompareNtup::initialize() {
  StatusCode sc = RichRingRecTupleAlgBase::initialize(); // must be executed first


  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  acquireTool( "TrackSelector",       m_trSelector, this );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichRingMCCompareNtup::execute() {

  info() << "==> Execute RichRingMCCompareNtup " << endmsg;


  RingReconNtup();
  RingGlobalReconNtup();
  // RingGlobalReconComparePrint();

  return StatusCode::SUCCESS;
}
void RichRingMCCompareNtup:: RingReconNtup(){

  Tuple rgTuple = nTuple(20, "Rich RingRecon Ntuple");

  IRichRingRecToolBase* art = rt();

  for(int irad = art->RParam()->MinRadiator(); irad <= art->RParam()->MaxRadiator();    ++irad){
    VI tkMM = art->Tfm()->getTrackIndexLimits( irad);
    // info()<<" track num limits "<<tkMM<<endmsg;

    for(int itk=tkMM[0]; itk< (tkMM [1]) ; ++itk){
      double aFoundRadius =  art->RRslt()->TrackFoundRadiusValue (itk, irad) ;
      double aFoundMeanRadius =  art->RRslt()->TrackFoundMeanRadiusValue (itk, irad) ;
      double aTrueRadius  =  art->MCT()->MCTrueRadiusValue(itk, irad) ;
      double aNumHitsRecon = art->RRslt()->TrackFoundMRNumHitsValue(itk, irad) ;
      double aTrueBeta = art->MCT()->MCTrueBetaValue(itk,irad);
      double aTrueGammma = art->MCT()->MCTrueGammaValue(itk,irad);
      double aTruePid = art->MCT()->MCTruePidValue(itk,irad);
      double aExpRadius  = art->Tfm()->TrackExpectedRadiusFromTruePidCode(itk,irad, (int) aTruePid );
      double aTrueNumHits = art->MCT()-> MCTrueNumHitValue(itk, irad);
      //double aTrackSegReconMass = art->RRslt()->TrackSegReconMassValue(itk, irad);
      double aTrackRecMom = art->tgD()->TrackRecMomValue(itk, irad);



      // info()<<"Ntup filling true radius "<<irad<<" "<<itk<<"  "<<aTrueRadius<<endmsg;


      rgTuple->column("Radiator", irad);
      rgTuple->column("TkSegNum", itk);
      rgTuple->column("RecRawRad", aFoundRadius);
      rgTuple->column("RecMeanRad", aFoundMeanRadius);
      rgTuple->column("MCTrueRad",  aTrueRadius);
      rgTuple->column("RecNHit", aNumHitsRecon );
      rgTuple->column("MCTrueBeta",aTrueBeta);
      rgTuple->column("MCTrueGamma", aTrueGammma);
      rgTuple->column("MCTruePid", aTruePid);
      rgTuple->column("MCTrueNumhits", aTrueNumHits );
      // rgTuple->column("RecSegMass",  aTrackSegReconMass);
      rgTuple->column("RecMom",  aTrackRecMom);
      rgTuple->column("MCExpRad",aExpRadius );



      rgTuple->write();
    }
  }
}
void RichRingMCCompareNtup::RingGlobalReconNtup(){
  Tuple sgTuple = nTuple(30, "Rich GlobalRingRecon Ntuple");

  IRichRingRecToolBase* art = rt();

  // loop through the RichRec Tracks
  for ( LHCb::RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track ){

    int cIndex = track- richTracks()->begin();

    // apply track selection
    if ( !m_trSelector->trackSelected(*track) ) continue;
    // get the reco track
    const LHCb::Track * trtrack = dynamic_cast<const LHCb::Track *>((*track)->parentTrack());
    if ( !trtrack ){
      Warning( "RingGlobalReconNtup: SelectMassFromRadiators:Input track type is not Track -> RichRecTrack skipped" );
      continue;
    }
    double abestRecMassLight = art->RRslt()->bestReconMassValue(cIndex, 0);
    double abestRecMassHeavy = art->RRslt()->bestReconMassValue(cIndex, 1);
    //int aReconPid = art->RRslt()->TrackBestReconPidValue(cIndex);
    int aReconPid =0;

    int irad =  art->RRslt()-> bestRadiatorValue(cIndex,0);
    int itk =   art->RRslt()->bestTrackSegValue(cIndex,0);
    // int nrd =  art->RConst()-> maxNumRadiator();
    if( ( irad >=  art-> RParam()->MinRadiator() )  &&  ( irad <=  art->RParam()->MaxRadiator() ) ) {
      int ntkMax =  art->MCT()->numTrueTkInfoValue(irad);
      if( (itk>=0) && (itk < ntkMax) ) {

        double aFoundMeanRadius =  art->RRslt()->TrackFoundMeanRadiusValue (itk, irad) ;
        double aTrueRadius  =  art->MCT()->MCTrueRadiusValue(itk, irad) ;
        double aNumHitsRecon = art->RRslt()->TrackFoundMRNumHitsValue(itk, irad) ;
        double aTrueBeta = art->MCT()->MCTrueBetaValue(itk,irad);
        double aTrueGammma = art->MCT()->MCTrueGammaValue(itk,irad);
        double aTruePid = art->MCT()->MCTruePidValue(itk,irad);
        double aExpRadius  = art->Tfm()->TrackExpectedRadiusFromTruePidCode(itk,irad, (int) aTruePid );
        double aTrueNumHits = art->MCT()-> MCTrueNumHitValue(itk, irad);
        double aTrackRecMom = art->tgD()->TrackRecMomValue(itk, irad);

        // test print
        //  if(fabs(aTruePid) == 321 ) {
        //
        //  info()<<" MCPIDitkIradcIndexRecmomTruepidRecpidRecmasslRecmassh recradius expradius trueradius"
        //        << itk <<"  "<<irad<<"  "<<cIndex<<"  "<<aTrackRecMom <<"   "<<aTruePid <<"   "
        //      <<aReconPid<<"   "<<abestRecMassLight<<"    "<<abestRecMassHeavy<<"  "
        //        << aFoundMeanRadius<<"  "<< aExpRadius<<"   "<<aTrueRadius<<endmsg;
        //  }


        //end test print

        sgTuple->column("bestMassL", abestRecMassLight);
        sgTuple->column("bestMassH", abestRecMassHeavy);
        sgTuple->column("Radiator", irad);
        sgTuple->column("TkSegNum", itk);
        sgTuple->column("RecMeanRad", aFoundMeanRadius);
        sgTuple->column("MCTrueRad",  aTrueRadius);
        sgTuple->column("RecNHit", aNumHitsRecon );
        sgTuple->column("MCTrueBeta",aTrueBeta);
        sgTuple->column("MCTrueGamma", aTrueGammma);
        sgTuple->column("MCTruePid", aTruePid);
        sgTuple->column("MCTrueNumhits", aTrueNumHits );
        sgTuple->column("RecMom",  aTrackRecMom);
        sgTuple->column("MCExpRad",aExpRadius );
        sgTuple->column("RecPid", aReconPid*1.0);

        sgTuple->write();

      }else {

        warning()<<"RichRingRec: MCGlobalNtupRecon: Unknown track segment  number for RichRecTrack in radiator "
                 <<itk<<"  "<<irad<<endmsg;

      }


    }else {
      warning()<<"RichRingRec: MCGlobalNtupRecon: Unknown best radiator number for RichRecTrack  "<<irad<<endmsg;

    }



  }


}
void RichRingMCCompareNtup::RingGlobalReconComparePrint(){
  IRichRingRecToolBase* art = rt();

  int nRa = art->RConst()->maxNumRadiator();

  // loop through the RichRec Tracks
  for ( LHCb::RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track ){

    int cIndex = track- richTracks()->begin();

    // apply track selection
    if ( !m_trSelector->trackSelected(*track) ) continue;
    // get the reco track
    const LHCb::Track * trtrack = dynamic_cast<const LHCb::Track *>((*track)->parentTrack());
    if ( !trtrack ){
      Warning( "RingGlobalReconNtup: SelectMassFromRadiators:Input track type is not Track -> RichRecTrack skipped" );
      continue;
    }

    double abestRecMassLight = art->RRslt()->bestReconMassValue(cIndex, 0);
    double abestRecMassHeavy = art->RRslt()->bestReconMassValue(cIndex, 1);
    //          int aReconPid = art->RRslt()->TrackBestReconPidValue(cIndex);
    int aReconPid  =0;


    VI SegmentIndex ( (rt()->RConst()->maxNumRadiator()) , -10);
    LHCb::RichRecSegment* segment0 = (*track)-> segmentInRad(Rich::Aerogel);
    if( segment0) {
      SegmentIndex[0] =  find(richSegments()->begin(),richSegments()->end(),
                              segment0)- richSegments()->begin();
    }
    LHCb::RichRecSegment* segment1 = (*track)-> segmentInRad(Rich::Rich1Gas);
    if( segment1) {
      SegmentIndex[1] =  find(richSegments()->begin(),richSegments()->end(),
                              segment1)- richSegments()->begin();
    }
    LHCb::RichRecSegment* segment2 = (*track)-> segmentInRad(Rich::Rich2Gas);
    if( segment2) {
      SegmentIndex[2] =  find(richSegments()->begin(),richSegments()->end(),
                              segment2)- richSegments()->begin();
    }
    for(int irad = 0; irad < nRa;    ++irad){
      int tkIndexInRad=  art->tgD()->FindIndexValue(SegmentIndex[irad], irad);
      if(tkIndexInRad >0 ) {
        double aFoundMeanRadius =  art->RRslt()->TrackFoundMeanRadiusValue (tkIndexInRad , irad) ;
        double aTruePid = art->MCT()->MCTruePidValue(tkIndexInRad ,irad);
        double aExpRadius  = art->Tfm()->TrackExpectedRadiusFromTruePidCode(tkIndexInRad,irad, (int) aTruePid );
        double aNumHitsRecon = art->RRslt()->TrackFoundMRNumHitsValue(tkIndexInRad, irad) ;
        double aTrueBeta = art->MCT()->MCTrueBetaValue(tkIndexInRad,irad);
        double aTrueGammma = art->MCT()->MCTrueGammaValue(tkIndexInRad,irad);
        double aTrackRecMom = art->tgD()->TrackRecMomValue(tkIndexInRad, irad);
        info()<<" Pid ComapreMC tkindex indexinrad irad TruePid RecPid RecRadius ExpRadius RecNumHits RecMom TrueBG massLH "
              << cIndex <<"  "<< tkIndexInRad<<"  "<<irad<<"  "<<aTruePid<<"   "<<  aReconPid <<"   "
              <<  aFoundMeanRadius<<"  "<<aExpRadius <<"   "<< aNumHitsRecon <<"   "<< "   "
              << aTrackRecMom<<"   "<<aTrueBeta* aTrueGammma
              <<"   "<<abestRecMassLight <<"  "<<abestRecMassHeavy<< endmsg;
      }

    }



  }





}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRingMCCompareNtup::finalize() {

  debug() << "==> Finalize" << endmsg;

  return RichRingRecTupleAlgBase::finalize();
  // must be called after all other actions


}

//=============================================================================
