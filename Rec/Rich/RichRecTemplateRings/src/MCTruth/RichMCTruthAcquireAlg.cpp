// $Id: RichMCTruthAcquireAlg.cpp,v 1.4 2009-07-30 11:22:55 jonrob Exp $
// Include files

// from Gaudi
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"

// local
#include "RichMCTruthAcquireAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichMCTruthAcquireAlg
//
// 2007-06-26 : Sajan EASO
//-----------------------------------------------------------------------------
using namespace Rich::Rec::TemplateRings;


// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichMCTruthAcquireAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMCTruthAcquireAlg::RichMCTruthAcquireAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : RichRingRecAlgBase  ( name , pSvcLocator ),
    m_richRecMCTruth    ( NULL ),
    m_mcTool            ( NULL ),
    m_trSelector        ( NULL )
{
}

//=============================================================================
// Destructor
//=============================================================================
RichMCTruthAcquireAlg::~RichMCTruthAcquireAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichMCTruthAcquireAlg::initialize() {
  StatusCode sc = RichRingRecAlgBase::initialize(); // must be executed first

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  acquireTool( "RichRecMCTruthTool", m_richRecMCTruth );
  // acquireTool( "RichMCTruthTool", m_mcTool,   0, true );
  acquireTool( "TrackSelector",       m_trSelector, this );


  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichMCTruthAcquireAlg::execute() {

  StatusCode sc=  StatusCode::SUCCESS;



  sc = AcquireMCTruthInfo();

  //  sc = CompareReconRadiusWithMC();
  // if(rt()->RParam()-> ActivateMCCompareMassPrint() ) {
  //  sc =   CompareMassFromRadiators();
  // }


  return sc;

}
StatusCode RichMCTruthAcquireAlg::CompareMassFromRadiators(){

  debug()<<" Now in compare Mass from radiators "<<endmsg;

  StatusCode sc= StatusCode::SUCCESS;
  IRichRingRecToolBase* art = rt();
  for ( LHCb::RichRecTracks::iterator track = richTracks()->begin();
        track != richTracks()->end(); ++track ){

    int CurrentTrackIndex = track- richTracks()->begin();
    // apply track selection
    if ( !m_trSelector->trackSelected(*track) ) continue;

    // get the reco track
    const LHCb::Track * trtrack = dynamic_cast<const LHCb::Track *>((*track)->parentTrack());
    if ( !trtrack ){
      Warning( "RichRingMasterAlg: SelectMassFromRadiators:Input track type is not Track -> RichRecTrack skipped" );
      continue;
    }
    // final result
    double selMassLight=  art-> RRslt()->bestReconMassValue(CurrentTrackIndex,0 );
    double selectedRadLight= art-> RRslt()->bestRadiatorValue(CurrentTrackIndex,0 );
    double selMassHeavy=  art-> RRslt()->bestReconMassValue(CurrentTrackIndex,1 );
    double selectedRadHeavy= art-> RRslt()->bestRadiatorValue(CurrentTrackIndex,1 );


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


    VD recrad(3);
    VD recmass(3);
    VD recBG (3);

    VD recMom (3);
    VD TrueBG (3);
    VD Truepid (3); VD Truerad(3);


    for(int irad = art->RParam()->MaxRadiator(); irad >= art->RParam()->MinRadiator();    --irad){
      int tkIndexInRad=  art-> tgD()->FindIndexValue(SegmentIndex[irad], irad);
      if(  tkIndexInRad >= 0 ) {
        recrad[irad]=  art-> RRslt()->TrackFoundMeanRadiusValue(tkIndexInRad,irad);
        recmass[irad] = art-> RRslt()->TrackSegReconMassValue(tkIndexInRad,irad);
        recBG [irad] = art-> RRslt()->TrackSegReconBGValue(tkIndexInRad,irad);
        recMom[irad] = art->tgD()-> TrackRecMomValue(tkIndexInRad,irad);
        TrueBG[irad] =  (art->MCT()->MCTrueBetaValue(tkIndexInRad,irad))* (art->MCT()->MCTrueGammaValue(tkIndexInRad,irad));
        Truepid [irad] = art->MCT()->MCTruePidValue(tkIndexInRad,irad);
        Truerad[irad] = art->MCT()->MCTrueRadiusValue(tkIndexInRad,irad);
      }

    }
    debug()<<" Compare mass values "<< CurrentTrackIndex<< endmsg;

    for(int irad = art->RParam()->MaxRadiator(); irad >= art->RParam()->MinRadiator();    --irad){

      if(  fabs(Truepid [irad]) == 211 ) {
        //  if(  fabs(Truepid [irad]) == 321 ) {

        debug()<<" truepid recmass BG recrad recmom truerad trueBG "<< irad<<"  "<<Truepid [irad]<<"  "
               << recmass[irad]<<"   "<<recBG [irad]
               <<"   "<<recrad[irad]<<"   "<<  recMom[irad]<<"   "<< Truerad[irad]<<"   "<< TrueBG[irad]<<endmsg;

      }

    }


    debug()<<" Selected Light: radiatorAndmass  Heavy radiatorAndmass  "<<selectedRadLight<<"   "<< selMassLight<<"  "
           <<selectedRadHeavy<<"     "<< selMassHeavy<<"   "<<endmsg;
    debug()<<" end compare mass for this track "<< CurrentTrackIndex<< endmsg;




  }


  debug()<<" end comapre mass values "<<endmsg;


  return sc;

}

StatusCode RichMCTruthAcquireAlg::CompareReconRadiusWithMC(){

  IRichRingRecToolBase* art = rt();

  // loop through radiators and tracks

  for(int irad = art->RParam()->MinRadiator(); irad <= art->RParam()->MaxRadiator();    ++irad){
    VI tkMM = art->Tfm()->getTrackIndexLimits( irad);
    for(int itk=tkMM[0]; itk< (tkMM [1]) ; ++itk){

      double aFoundRadius =  art->RRslt()->TrackFoundRadiusValue (itk, irad) ;
      double aFoundMeanRadius =  art->RRslt()->TrackFoundMeanRadiusValue (itk, irad) ;
      double aTrueRadius  =  art->MCT()->MCTrueRadiusValue(itk, irad) ;
      double aNumHitsRecon = art->RRslt()->TrackFoundMRNumHitsValue(itk, irad) ;
      double aNumHitsTrue  = art->MCT()-> MCTrueNumHitValue(itk, irad);

      info() <<"Radius comparison :  Numhits rec true.   Radius: Found MeanFound True  " << irad<<"  "<<itk<<"  "
             <<aNumHitsRecon<< "    "<< aNumHitsTrue<<"  "<<aFoundRadius<<"  "
             <<  aFoundMeanRadius<< "    "<<aTrueRadius <<endmsg;
    }

  }
  return  StatusCode::SUCCESS;

}

StatusCode RichMCTruthAcquireAlg::AcquireMCTruthInfo(){
  rt()->MCT()->ResetMCTruthArrays();



  std::vector< const LHCb::MCParticle * > m_mcPhotVecAgel ;
  std::vector< const LHCb::MCParticle *> m_mcPhotVecr1g ;
  std::vector< const LHCb::MCParticle * > m_mcPhotVecr2g ;

  VVI m_HitTruthAssocAgel;
  VVI m_HitTruthAssocr1g;
  VVI m_HitTruthAssocr2g;

  typedef std::vector< const LHCb::MCParticle * >::iterator MI;

  int aNumHitsInp = (int) richPixels()->size();
  // info()<<" MC Truth Alg : richPixelsSize "<<aNumHitsInp<<endmsg;


  if( aNumHitsInp > 0 ) {
    // Loop through all the RICH hits
    for ( LHCb::RichRecPixels::const_iterator iPix = richPixels()->begin();
          iPix != richPixels()->end(); ++iPix ){
      int pixelIndex = iPix - (richPixels()->begin());

      const LHCb::RichRecPixel * pixel = *iPix;
      SmartRefVector<LHCb::MCRichHit> mcHits;
      // const LHCb::MCRichHits* mcHitsInp = get<LHCb::MCRichHits>(LHCb::MCRichHitLocation::Default);
      //const MCParticles*  mcParticlesInp = get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default);

      // MC/Rich/Hits
      //info()<<" MCTruthalg  MCRichHit Size  "<< (int) mcHits->size()<<endmsg;
      // m_richRecMCTruth->m_richRecMCTruth->mcPartToMCRichHisMap();

      //const LHCb::MCRichHits* mcHitsInp = m_mcTool->mcRichHits();
      //m_mcTool->mcPartToMCRichHitsMap();
      // m_mcTool->smartIDToMCRichHitsMap();

      m_richRecMCTruth->mcRichHits( pixel, mcHits );
      // info()<<" MCTruthalg  MCRichHit Size  "<< (int) mcHits.size()<<endmsg;

      // SmartRefVector<LHCb::MCRichHit> mcHitsA;
      // m_richRecMCTruth->mcRichHits(  pixel, mcHitsA );

      if((int) mcHits.size() > 1 ) {

        debug()<<"RichMCTruthFinder multiple mchit per pixel: current pixel mchitsize "
               <<pixelIndex<<"   "<< (int) mcHits.size()<<endmsg;
      }

      for ( SmartRefVector<LHCb::MCRichHit>::const_iterator iHit = mcHits.begin();
            iHit != mcHits.end(); ++iHit ){

        //      info()<<" McHit coord signalflag radiator MCP  smartid "<< (*iHit) ->entry()<<"  "<<(*iHit)->isSignal()
        //      <<"   "<<(*iHit)->radiator()<<"  "<<(*iHit)->mcParticle()<< (*iHit)-> sensDetID () <<endmsg;

        //begin test
        //for( LHCb::MCRichHits::const_iterator inpHit =  mcHitsInp->begin(); inpHit != mcHitsInp->end(); ++inpHit) {
        //  if( ( (*iHit)->entry() == (*inpHit)->entry()) && ( ((*iHit)-> sensDetID ()) == ((*inpHit )-> sensDetID ())  )  ) {

        //  info() <<" Found the Mc Hit  "<< inpHit- mcHitsInp->begin()<<"   "<<(*inpHit )->entry() <<"  "
        //       <<(*inpHit )-> sensDetID ()<<  endmsg;
        //    info()<<" Found the Input MCHit  with MCP "<< inpHit- mcHitsInp->begin() <<"   "
        //          <<  (*inpHit)->mcParticle() <<endmsg;


        //            const LHCb::MCParticle* atruePart =
        // m_richRecMCTruth->trueCherenkovRadiation(pixel,  (*iHit)->radiator());
        //  info()<<" Current mc part for pixel "<<atruePart <<endmsg;




        //    }

        //  }
        //end test



        //for ( LHCb::MCRichHits::const_iterator iHit = mcHits.begin();
        //      iHit != mcHits.end(); ++iHit ){

        if ( !(*iHit)->isBackground() && ( (*iHit)->isSignal()) ){
          //  if ( ( (*iHit)->isSignal()) ){
          const Rich::RadiatorType radiator = (*iHit)->radiator();
          //        info()<<" current mc hit is signal in radiator "<<radiator<<endmsg;

          // test the following for tests with data with mcopticalphoton.
          //        const LHCb::MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton(*iHit);
          // info()<<"current radiator mcphot "<<radiator<<"  "<< mcPhot <<endmsg;
          //          if ( mcPhot ){
          //          info()<<" mc optical photon exists "<<endmsg;
          //const Gaudi::XYZVector& aTkTrueMom = mcPhot->parentMomentum();
          // double aTrueCkvTheta = (double)  (mcPhot->cherenkovTheta());// not saved for now.
          // double aTkTrueMomX =   aTkTrueMom.x();
          // double aTkTrueMomY =   aTkTrueMom.y();
          // double aTkTrueMomZ =   aTkTrueMom.z();
          // double   aTkTrueMomTot= pow( (aTkTrueMomX* aTkTrueMomX+aTkTrueMomY*aTkTrueMomY+aTkTrueMomZ* aTkTrueMomZ),0.5);
          //            const LHCb::MCParticle * mcPa = m_richRecMCTruth->trueCherenkovRadiation(pixel,radiator);
          // end of test with mcopticalphoton

          const LHCb::MCParticle * mcPa = (*iHit)->mcParticle();
          // info()<<" Current MC Particle for mc hit "<<   iHit- mcHits.begin() <<"     "<<mcPa<<endmsg;

          if ( mcPa ) {
            if(  radiator == Rich::Aerogel ) {
              MI p = find( m_mcPhotVecAgel.begin(), m_mcPhotVecAgel.end(), mcPa);
              if( p ==  m_mcPhotVecAgel.end() ){
                m_mcPhotVecAgel.push_back(mcPa);
                std::vector<int> agelhitlist; agelhitlist.clear();
                agelhitlist.push_back(pixelIndex);
                m_HitTruthAssocAgel.push_back(agelhitlist);

              }else {
                m_HitTruthAssocAgel [p- m_mcPhotVecAgel.begin()].push_back(pixelIndex);

              }

            }   else if (  radiator == Rich::Rich1Gas ) { // end loop over aerogel
              MI q = find( m_mcPhotVecr1g.begin(), m_mcPhotVecr1g.end(), mcPa);
              if( q ==  m_mcPhotVecr1g.end() ){
                m_mcPhotVecr1g.push_back(mcPa);
                std::vector<int>r1ghitlist;r1ghitlist.clear();
                r1ghitlist.push_back(pixelIndex);
                m_HitTruthAssocr1g.push_back( r1ghitlist);
              }else {
                m_HitTruthAssocr1g [q-m_mcPhotVecr1g.begin()].push_back(pixelIndex);

              }
            }else if (radiator == Rich::Rich2Gas ) {   // end loop over r1g


              MI r = find( m_mcPhotVecr2g.begin(), m_mcPhotVecr2g.end(), mcPa);
              if( r ==  m_mcPhotVecr2g.end() ){
                m_mcPhotVecr2g.push_back(mcPa);
                std::vector<int>r2ghitlist;r2ghitlist.clear();
                r2ghitlist.push_back(pixelIndex);
                m_HitTruthAssocr2g.push_back( r2ghitlist);

              }else {
                m_HitTruthAssocr2g [r-m_mcPhotVecr2g.begin()].push_back(pixelIndex);

              }

            }}}}}}  // end loop over hits



  // loop over track segments
  int aNumtk=-1;
  int aNumMaxTk= (int) richSegments()->size();
  int n0s=0;    int n1s=0; int n2s=0;
  VD t0beta; VD t1beta; VD t2beta;
  t0beta.reserve(aNumMaxTk);t1beta.reserve(aNumMaxTk);t2beta.reserve(aNumMaxTk);
  VD t0gma;   VD t1gma;   VD t2gma;
  t0gma.reserve(aNumMaxTk);t1gma.reserve(aNumMaxTk);t2gma.reserve(aNumMaxTk);
  VD t0pid; VD t1pid;  VD t2pid;
  t0pid.reserve(aNumMaxTk); t1pid.reserve(aNumMaxTk);t2pid.reserve(aNumMaxTk);
  VD t0rad; VD t1rad; VD t2rad;
  t0rad.reserve(aNumMaxTk); t1rad.reserve(aNumMaxTk); t2rad.reserve(aNumMaxTk);
  VD t0nht; VD t1nht; VD t2nht;
  t0nht.reserve(aNumMaxTk); t1nht.reserve(aNumMaxTk); t2nht.reserve(aNumMaxTk);

  VD rus0; VD n0ht; rus0.clear(); rus0.reserve(aNumMaxTk);
  n0ht.clear(); n0ht.reserve(aNumMaxTk);
  VD rus1; VD n1ht;rus1.clear(); rus1.reserve(aNumMaxTk);
  n1ht.clear(); n1ht.reserve(aNumMaxTk);
  VD rus2; VD n2ht;rus2.clear(); rus2.reserve(aNumMaxTk);
  n2ht.clear(); n2ht.reserve(aNumMaxTk);




  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ){

    LHCb::RichRecSegment * segment = *iSeg;
    if ( !m_trSelector->trackSelected( segment->richRecTrack() ) ) continue;
    //    const Rich::ParticleIDType mcType = m_richRecMCTruth->mcParticleType( segment );
    // if ( Rich::Unknown == mcType ) continue;

    const Gaudi::XYZPoint & pdPoint = segment->pdPanelHitPoint();  // track proj on hpd panel in global coord
    const Gaudi::XYZPoint & pdPointLocal = segment->pdPanelHitPointLocal();
    if( !((pdPoint.x() ==0.0) && ( pdPoint.y() == 0.0 )) ) {  // avoid unphysical track coord.
      const LHCb::RichTrackSegment & tkSeg = segment->trackSegment();
      double tkMomz = tkSeg.bestMomentum().z();
      const Rich::RadiatorType rad = tkSeg.radiator();   // which radiator
      if(tkMomz >0.0 ) { // avoid tracks going backwards
        aNumtk++;
        const LHCb::MCParticle* mcpart = m_richRecMCTruth->mcParticle( segment);
        double aBeta=-100;
        double aGamma=-100;
        double aTkPid=-100000;
        VD aRingInfoAgel (3,-10.0);
        VD aRingInfoR1g  (3,-10.0);
        VD aRingInfoR2g  (3,-10.0);

        double radiusTolerence=10.0;
        double NominalMinradius= 0.0;
        double NominalMaxradius=300.0;

        if(rad == Rich::Aerogel ) radiusTolerence=20.0;


        if(mcpart) {

          aBeta = mcpart->beta();
          aGamma= mcpart->gamma();
          aTkPid= (mcpart->particleID().pid());
          double ameanTruerad=   segment->averageCKRadiusLocal(Rich::Pion);
          // test
          //
          //  if(rad ==  Rich::Aerogel  ) {
          //
          //             info()<<"rad mom expected true radius for various hypo elm pion muon kaon proton "
          //                <<rad<<"   "<<aNumtk<<"   "<<pow((tkSeg.bestMomentum().Mag2()),0.5)<<"  "<<aTkPid<<"  "
          //       << segment->averageCKRadiusLocal(Rich::Electron)<<"   "
          //      <<segment->averageCKRadiusLocal(Rich::Pion)<<"   "
          //      <<segment->averageCKRadiusLocal(Rich::Muon)<<"   "
          //      <<segment->averageCKRadiusLocal(Rich::Kaon)<<"   "
          //      <<segment->averageCKRadiusLocal(Rich::Proton)<<endmsg;
          // this test gives lot of unexpected zeros  and hence the info seems unreliable.
          //
          // }
          //
          //
          // end of test

          if(fabs(aTkPid) == 321 )  ameanTruerad=  (double)  (segment->averageCKRadiusLocal(Rich::Kaon));
          if(fabs(aTkPid) == 13 )  ameanTruerad= (double)  ( segment->averageCKRadiusLocal(Rich::Muon));
          if(fabs(aTkPid) == 11 )  ameanTruerad=  (double)  (segment->averageCKRadiusLocal(Rich::Electron));
          if(fabs(aTkPid) == 2212 )  ameanTruerad= (double)  ( segment->averageCKRadiusLocal(Rich::Proton));

          // ad hoc fix to wrong input values.
          if(  ameanTruerad != 0.0 ) {
            NominalMinradius=  ameanTruerad-radiusTolerence;
            NominalMaxradius = ameanTruerad +radiusTolerence ;

          }else {

            NominalMinradius=  0.0;
            NominalMaxradius =  rt()->RConst()->  MaxNominalRadiusForRadiatorValue(rad)+ radiusTolerence;

          }
          if( NominalMinradius <0.0 ) NominalMinradius=0.0;
          if(NominalMaxradius > ( rt()->RConst()->  MaxNominalRadiusForRadiatorValue(rad))+ radiusTolerence ){
            NominalMaxradius= (rt()->RConst()->  MaxNominalRadiusForRadiatorValue(rad)) + radiusTolerence;
          }

          // end of ad hoc fix for unreliable input values.


        }



        if(rad ==  Rich::Aerogel ) {
          n0s++;
          t0beta.push_back(aBeta);
          t0gma.push_back(aGamma);
          t0pid.push_back(aTkPid);
          MI p = find( m_mcPhotVecAgel.begin(), m_mcPhotVecAgel.end(), mcpart);
          if (p !=  m_mcPhotVecAgel.end() ) {
            VI agelhitlist =  m_HitTruthAssocAgel [p- m_mcPhotVecAgel.begin()];
            aRingInfoAgel = getTrueRingInfo( rad,  agelhitlist , pdPointLocal, NominalMinradius, NominalMaxradius  );
          }
          rus0.push_back( aRingInfoAgel[0]);
          n0ht.push_back(  aRingInfoAgel[1]);

        }else if ( rad == Rich::Rich1Gas ) {
          n1s++;
          t1beta.push_back(aBeta);
          t1gma.push_back(aGamma);
          t1pid.push_back(aTkPid);
          MI q = find( m_mcPhotVecr1g.begin(), m_mcPhotVecr1g.end(), mcpart);
          if (q !=  m_mcPhotVecr1g.end() ) {
            VI r1ghitlist = m_HitTruthAssocr1g [q-m_mcPhotVecr1g.begin()];
            aRingInfoR1g = getTrueRingInfo( rad,r1ghitlist ,  pdPointLocal, NominalMinradius, NominalMaxradius   );
          }
          rus1.push_back( aRingInfoR1g[0]);
          n1ht.push_back(  aRingInfoR1g[1]);
          // info()<<"Rich1Gas tracknum  Truerad truenumhit sumrad "<<aNumtk<<"  "<<n1s-1<<"  "<<aRingInfoR1g<<endmsg;

        }else if ( rad == Rich::Rich2Gas ) {
          n2s++;
          t2beta.push_back(aBeta);
          t2gma.push_back(aGamma);
          t2pid.push_back(aTkPid);
          MI r = find( m_mcPhotVecr2g.begin(), m_mcPhotVecr2g.end(), mcpart);
          if (r !=  m_mcPhotVecr2g.end() ) {
            VI r2ghitlist = m_HitTruthAssocr2g [r-m_mcPhotVecr2g.begin()];
            aRingInfoR2g = getTrueRingInfo( rad, r2ghitlist, pdPointLocal,   NominalMinradius, NominalMaxradius );
          }
          rus2.push_back( aRingInfoR2g[0]);
          n2ht.push_back(  aRingInfoR2g[1]);
          // info()<<"Rich2Gas track num  Truerad truenumhit sumrad "
          //      << iSeg - richSegments()->begin() <<"  " <<aNumtk<<"   "<<(n2s-1)<<"   "<<aRingInfoR2g<<endmsg;
        }}}} // end loop over segments



  rt()->MCT()->StoreBGPid(n0s,t0beta,t0gma,t0pid);
  rt()->MCT()->StoreTrueHit(rus0,n0ht);


  rt()->MCT()->StoreBGPid(n1s,t1beta,t1gma,t1pid);
  rt()->MCT()->StoreTrueHit(rus1,n1ht);


  rt()->MCT()->StoreBGPid(n2s,t2beta,t2gma,t2pid);
  rt()->MCT()->StoreTrueHit(rus2,n2ht);


  //       info()<<" AcquireMCTruthInfo  "<<endmsg;

  return  StatusCode::SUCCESS;
}

VD RichMCTruthAcquireAlg::getTrueRingInfo(int rad , VI ahitlist ,
                                          const Gaudi::XYZPoint & pdPointLocal, double aMinradius, double aMaxradius ){
  typedef LHCb::RichRecPixels::const_iterator  MP;

  double curMeanRad=0.0;
  double  curRingRadSum=0.0;
  double   curnumhit =0.0;
  for (int ih=0; ih< (int) ahitlist.size(); ++ih) {
    int pixIndex= ahitlist[ih];
    MP iPix = richPixels()->begin()+ pixIndex;
    const LHCb::RichRecPixel * pixel = *iPix;
    const Gaudi::XYZPoint & lPos = pixel->localPosition();
    double tkx = pdPointLocal.x();
    double tky = pdPointLocal.y();
    double hx= lPos.x(); double hy = lPos.y();
    // now for the Y shift in aerogel
    if(rad ==0 ) {
      double aShift =  rt()->RParam()->YAgelShift();
      if(  tky > 0.0 &&  hy < 0.0 ) {
        hy += aShift;

      }else if ( tky < 0 && hy > 0.0 ) {

        hy -= aShift;

      }

    }

    double adx = hx- tkx;
    double ady = hy - tky;
    double aradius= pow(( adx*adx + ady*ady),0.5);
    if( aradius >  aMinradius && aradius< aMaxradius) {

      curRingRadSum += aradius;
      curnumhit += 1.0;

    }



  }
  if( curnumhit  >= 1.0 ) {
    curMeanRad = curRingRadSum/ curnumhit;
  }


  VD aH ; aH.clear(); aH.resize(3);
  aH[0]=curMeanRad;
  aH[1]= curnumhit;
  aH[2]= curRingRadSum;

  return aH;

}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichMCTruthAcquireAlg::finalize() {


  debug() << "==> Finalize" << endmsg;

  return RichRingRecAlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
