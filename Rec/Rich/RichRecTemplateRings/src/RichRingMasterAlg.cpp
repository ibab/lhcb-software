// $Id: RichRingMasterAlg.cpp,v 1.3 2009-03-19 17:15:43 seaso Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RichRingMasterAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRingMasterAlg
//
// 2007-05-17 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichRingMasterAlg );

//============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRingMasterAlg::RichRingMasterAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : RichRingRecTupleAlgBase ( name , pSvcLocator ),
    m_trSelector        ( NULL )
{

  declareProperty( "RingLocation",
                   m_ringLocation = LHCb::RichRecRingLocation::TemplateRings+"All" );
  declareProperty( "RichTemplateRingNtupProduce",  m_storeNtupRadius = false);
  declareProperty( "RichTemplateRingMassFinderActivate", m_activateRingMassFinder=false);
  declareProperty( "RichTemplateSavingRingsOnTESActivate" , m_activateSavingRingsOnTES=true);


}

//=============================================================================
// Destructor
//=============================================================================
RichRingMasterAlg::~RichRingMasterAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichRingMasterAlg::initialize()
{
  StatusCode sc = RichRingRecTupleAlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  acquireTool( "TrackSelector",       m_trSelector, this );

  rt()->tmD()->init();
  rt()->tmD()->ConfigureTemplateForRadiator();
  sc= rt()->RLocTgC()->InitTargetConfigParam();
  rt()->RFFP()->InitFFPlan();

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichRingMasterAlg::execute() 
{
  debug() << "==> Execute RichRingMasterAlg " << endmsg;

  StatusCode sc = FindRingsWithTracks();

  if(m_activateRingMassFinder) {
    sc = sc && ReconstructMassForRings();
  }
  if(  m_activateSavingRingsOnTES ) {
  
    sc = sc && saveRingsInTES();
  }
  
  return sc;
}

StatusCode RichRingMasterAlg::FindRingsWithTracks()
{
  StatusCode sc= StatusCode::SUCCESS;
  sc = ResetRichEventRingReconParam();
  // loop through the radiators
  IRichRingRecToolBase* art = rt();


  for(int irad = art->RParam()->MaxRadiator(); irad >= art->RParam()->MinRadiator();    --irad){

    sc = SetRadiatorSpecificParam(irad);
    int iRich =  art->Tfm()->RichDetNumFromRadiator(irad);


    // loop through the track segments in each radiator
    VI tkMM = art->Tfm()->getTrackIndexLimits( irad);
    
    for(int itk=tkMM[0]; itk< (tkMM [1]) ; ++itk){

      // select and configure the RICH hits in the Field of Interest.
      sc = art->RLocTgC()-> SelectHitsInFoi (itk, irad, iRich);

      sc = art->RLocTDC()->ConfigureTemplateHitsForFF(irad);

      sc = art->RLocTgC()->ConfigureTargetHitsInFoiForFF(itk,irad,iRich);

      // perform FFT
      VD  aFFTarget =   art->RFFP()->ConvertToFF2d(art->RLocTgC()->RpnTarget(),
                                                   art->RLocTgC()->PpnTarget());
      VD  aFFTemplate =  art->RFFP()->ConvertToFF2d(art->RLocTDC()->RpnTemplate(),
                                                    art->RLocTDC()->PpnTemplate());


      // get Correlation
      VD   aCorr = art->Tfm()->GetCorrelation(aFFTarget, aFFTemplate);

      // perform inverse FFT
      VVD  aRPCorr =  art->RFFP()->ConvertToInvFF2d( aCorr);

      // get the peak and store the result
      VI   aRPMaxVec = art->Tfm()->FindPeakInRP( aRPCorr,irad);
      sc =  art->RLocTDC()->ScaleTemplateHits(aRPMaxVec,itk,irad);
      // determine the mean radius
      sc = art->RLocTgC()->EstimateMeanRadiusFromRawRadius( itk, irad , iRich );


      // debug Single track plots


      if( art->RParam()->ActivateSingleEvHisto() && art->RParam()-> ActivateSingleTrackDebug() ) {
        art->HSingleEv()->PlotTargetAndTemplate();
        art->HSingleEv()-> PlotInvCorr(aRPCorr);
      }
      // end of debug Single track plots.

    } // end loop over track segments in a radiator




  } // end loop over radiators

  if(m_storeNtupRadius) {

    debug()<<" Now store info in ntuple "<<endmsg;
    sc = StoreRingInfoInNtup();

  }

  return sc;
}

StatusCode RichRingMasterAlg::ResetRichEventRingReconParam(){
  rt()->RRslt()->clearReconSegmentArrays();
  rt()->RRslt()->clearResultTrackArrays();
  // rt()->RPid()->initEvent();
  return StatusCode::SUCCESS;
}

StatusCode RichRingMasterAlg::SetRadiatorSpecificParam(int irad)
{
  rt()->RLocTgC()->SetRadiatorParam(irad);
  return StatusCode::SUCCESS;
}

StatusCode RichRingMasterAlg::ReconstructMassForRings()
{
  StatusCode sc= StatusCode::SUCCESS;

  IRichRingRecToolBase* art = rt();

  // loop through the radiators
  for(int irad = art->RParam()->MaxRadiator(); irad >= art->RParam()->MinRadiator();    --irad){

    // loop through the tracksegments in each radiator
    VI tkMM = art->Tfm()->getTrackIndexLimits( irad);

    for(int itk=tkMM[0]; itk< (tkMM [1]) ; ++itk){
      art->RMass()->RichReconMassForTrackSegment(itk, irad );

    }
  }

  return sc;
}

StatusCode RichRingMasterAlg::StoreRingInfoInNtup()
{
  StatusCode sc= StatusCode::SUCCESS;
  Tuple m_RingTuple = nTuple(250,"RichRingTuple");
  int maxNumHit=2000;
  int maxNumTk=1000;

  for(int irad = rt()->RParam()->MaxRadiator(); irad >= rt()->RParam()->MinRadiator();    --irad){
    VI tkMM = rt()->Tfm()->getTrackIndexLimits( irad);
    int iRich = rt()->Tfm()->RichDetNumFromRadiator(irad);
    int aNumHit = rt()->tgD()->NumHitsTarget(iRich);
    // int aNumTk = rt()->tgD()->NumChRdTrackValue(irad);
    VD tkX; tkX.clear(); tkX.reserve(200);
    VD tkY; tkY.clear(); tkY.reserve(200);
    VD tkRadius; tkRadius.clear(); tkRadius.reserve(200);
    VD tkMom; tkMom.clear(); tkMom.reserve(200);
    for(int itk=tkMM[0]; itk< (tkMM [1]) ; ++itk){
      double aRadius =  rt()->RRslt()->TrackFoundMeanRadiusValue(itk,irad);
      double aRecMom =  rt()->tgD()-> TrackRecMomValue(itk,irad);
      double atkX= rt()->tgD()->TrackXcInpValue(itk,irad);
      double atkY= rt()->tgD()->TrackYcInpValue(itk,irad);
      tkX.push_back(atkX);
      tkY.push_back(atkY);
      tkRadius.push_back(aRadius);
      tkMom.push_back(aRecMom);

    }

    std::vector<double> htX; htX.clear(); htX.reserve(maxNumHit);
    std::vector<double> htY; htY.clear(); htY.reserve(maxNumHit);
    for (int ih=0; ih< aNumHit; ih++){
      htX.push_back(rt()->tgD()->XHitInpValue(ih,iRich));
      htY.push_back(rt()->tgD()->YHitInpValue(ih,iRich));
    }


    //info()<<" Now in store info to Ntuple NumHit "<<aNumHit<<endmsg;

    m_RingTuple->column("Radiator", irad);
    m_RingTuple->column("NumTk",(int) tkX.size());
    m_RingTuple->column("NumHit",(int) htX.size());
    m_RingTuple->farray("TkXHit",tkX,"tkXLength",maxNumTk);
    m_RingTuple->farray("TkYHit",tkY,"tkXLength",maxNumTk);
    m_RingTuple->farray("RecRadius",tkRadius,"tkRadiusLength", maxNumTk);
    m_RingTuple->farray("RecMom",tkMom,"tkMomLength",maxNumTk);
    m_RingTuple ->farray("XHitCoord",htX,"XLength",maxNumHit);
    m_RingTuple ->farray("YHitCoord",htY,"YLength",maxNumHit);
    sc=m_RingTuple ->write();
  }

  return sc;
}

StatusCode RichRingMasterAlg::saveRingsInTES(){
  StatusCode sc= StatusCode::SUCCESS;

  LHCb::RichRecRings * rings = getRings( m_ringLocation );

  // Loop over radiators
  for(int irad = rt()->RParam()->MaxRadiator(); irad >= rt()->RParam()->MinRadiator();    --irad){
    VI tkMM = rt()->Tfm()->getTrackIndexLimits( irad);
    int iRich = rt()->Tfm()->RichDetNumFromRadiator(irad);

    for(int itk=tkMM[0]; itk< (tkMM [1]) ; ++itk){  // loop over selected RichRecSegments

      double atkX= rt()->tgD()->TrackXcInpValue(itk,irad);
      double atkY= rt()->tgD()->TrackYcInpValue(itk,irad);

      // Create a new Ring object
      LHCb::RichRecRing * newRing = new LHCb::RichRecRing();

      rings->insert(newRing);
      // set detector information
      // for now assign Panel according to where the track intersection point is,
      // although the ring could cover more than one panel.
      if(iRich==0){
        newRing->setRich(Rich::Rich1);
        if(atkY > 0.0 ) {
          newRing->setPanel(Rich::top);
        }else {
          newRing->setPanel(Rich::bottom);
        }

      }else {
        newRing->setRich(Rich::Rich2);
        if(atkX > 0.0 ) {
          newRing->setPanel(Rich::left);
        }else {
          newRing->setPanel(Rich::right);
        }
      }
      if(irad ==2)newRing->setRadiator(Rich::Rich2Gas);
      if(irad ==1)newRing->setRadiator(Rich::Rich1Gas);
      if(irad ==0)newRing->setRadiator(Rich::Aerogel);

      //set ring type info
      newRing->setType      ( LHCb::RichRecRing::TemplateRing );
      newRing->setAlgorithm ( LHCb::RichRecRing::Template     );

      // set ring centre coordinate and ring radius.
      const Gaudi::XYZPoint centrePointLocal= rt()->tgD()->CTrackInp (itk,irad);
      int aInvIndex= rt()->tgD()->TrackInvIndexValue(itk,irad);
      LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin()+aInvIndex;
      const LHCb::RichRecSegment * segment = *iSeg;
      const Gaudi::XYZPoint & centrePointGlobal = segment->pdPanelHitPoint();
      double aRadius =  rt()->RRslt()->TrackFoundMeanRadiusValue(itk,irad);

      newRing->setCentrePointLocal ( centrePointLocal );
      newRing->setCentrePointGlobal( centrePointGlobal );
      newRing->setRadius(aRadius);
      newRing->setRichRecSegment(segment);

      // now to fill the mass hypothesis. For now just using the closest mass.
      // this needs improvement in the future.
      double amass= rt()->RRslt()->TrackSegReconMassValue(itk,irad);
      int ahyp= rt()->RConst()->estimatePidFromMassAlone(amass);
      Rich::ParticleIDType aType = Rich::Pion;
      if(ahyp <0 )            { aType=Rich::Unknown;
      }else if  (ahyp ==0 )   { aType=Rich::Electron;
      }else if  (ahyp == 1)  { aType=Rich::Muon;
      }else if  (ahyp == 2 ) { aType= Rich::Pion;
      }else if  (ahyp == 3 ) { aType = Rich::Kaon;
      }else if  (ahyp == 4 ) { aType = Rich::Proton;
      }
      const Rich::ParticleIDType aTypeA=aType;
      newRing->setMassHypo(aTypeA);
      // end of part which needs improvement in future.

      // Also to be added the RichRecPixels on each ring.

    } // end loop over RichRecSegments in the current radiator


  }// end loop over radiators


  return sc;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichRingMasterAlg::finalize() 
{
  debug() << "==> Finalize" << endmsg;

  //  if( rt()->RParam()-> WriteOutAuxiliaryDebugHisto()  ){
  //   rt()->HSingleEv()-> WriteOutDebugHistogramsToAuxiliaryFile();
  // }
  rt()->RFFP()->ReleaseFFTWArrays();

  return RichRingRecTupleAlgBase::finalize();  // must be called after all other actions
}

//=============================================================================
