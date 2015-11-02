// $Id: LoKiVVSeedFinder.cpp,v 1.2 2009-12-14 12:34:33 potterat Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKiVVSeedFinder.h"
// ============================================================================
/** @file
 *  Implementation file for class  LoKi::VVSeedFinder
 *  @author Cedric POTTERAT   cedric.potterat@cern.ch
 *  @date   2011-01-31
 */
// ============================================================================
/*  standard initialization of the tool
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::VVSeedFinder::initialize ()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }

  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;

  m_dist       = tool<IDistanceCalculator>("LoKi::DistanceCalculator");
  if ( !m_dist ) {
    err() << "Unable to Retrieve LoKi::DistanceCalculator" << endmsg;
    return StatusCode::FAILURE;
  }

  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;
  if ( !m_combiner) {
    err() << "Unable to Retrieve Default  ParticleCombiner" << endmsg;
    return StatusCode::FAILURE;
  }

  m_fitter     = tool<IVertexFit>("OfflineVertexFitter");
  if ( !m_fitter ) {
    err() << "Unable to Retrieve Default VertexFitter" << endmsg;
    return StatusCode::FAILURE;
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug()<<" ------------------------------------------------------ "<<endmsg;
    debug()<<" |                                                    | "<<endmsg;
    debug()<<" |     \\  /\\  /                                       | "<<endmsg;
    debug()<<" |      \\/  \\/SeedsFinder Tool for Phys Particles     | "<<endmsg;
    debug()<<" |                                                    | "<<endmsg;
    debug()<<" |       \\  /                                         | "<<endmsg;
    debug()<<" |        \\/       EPFLausanne - 2009-11              | "<<endmsg;
    debug()<<" |         |        A. Bay & C. Potterat              | "<<endmsg;
    debug()<<" |         |                                          | "<<endmsg;
    debug()<<" |                                       _     _      | "<<endmsg;
    debug()<<" |                                  |   |_||_||_      | "<<endmsg;
    debug()<<" |                                  |__ |  | ||__     | "<<endmsg;
    debug()<<" ------------------------------------------------------ "<<endmsg;
    debug()<<" |                                                    | "<<endmsg;
    debug()<<" |                                                     "<<endmsg;
    debug()<<" |   VVSeedsFinder parameters: "<<endmsg;
    debug()<<" |   SeedID            "<<   m_seedID      <<endmsg;
    debug()<<" |   SeedTriplets      "<<   m_Triplets        <<endmsg;
    debug()<<" |   SeedRParameter    "<<   m_r            <<endmsg;
    debug()<<" |   PtTrackMin        "<<   m_PtTrackMin      <<endmsg;
    debug()<<" |   PTrackMin         "<<   m_PTrackMin      <<endmsg;
    debug()<<" |   SeedTrkChi2PerDoF "<<   m_TrkChi2DoF     <<endmsg;
    debug()<<" |   IPmin             "<<   m_IPmin       <<endmsg;
    debug()<<" |   Signif            "<<   m_Signif          <<endmsg;
    debug()<<" |   DMK0              "<<   m_DMK0            <<endmsg;
    debug()<<" |   TseedVtxMin       "<<   m_TseedVtxMin     <<endmsg;
    debug()<<" |   TseedVtxMax       "<<   m_TseedVtxMax     <<endmsg;
    debug()<<" |   DtrakMax          "<<   m_DtrakMax        <<endmsg;
    debug()<<" |   DeltaRSeeds       "<<   m_DeltaRSeeds     <<endmsg;
    debug()<<" |   PtSeedsMin        "<<   m_PtSeedsMin      <<endmsg;
    debug()<<" |   PtMergedSeedsMin  "<<   m_PtMergedSeedsMin      <<endmsg;
    debug()<<" |                                                      "<<endmsg;
    debug()<<" |                                                    | "<<endmsg;
    debug()<<" ------------------------------------------------------ "<<endmsg;
  }


  return StatusCode::SUCCESS ;
}
// ===========================================================================
// find seeds
// ===========================================================================
StatusCode LoKi::VVSeedFinder::makeJets
( const IJetMaker::Input& input_ ,IJetMaker::Jets& jets_ ) const
{

  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  if(verts->size()!=1) return StatusCode::SUCCESS;
  LHCb::RecVertex::Container::const_iterator iv =verts->begin() ;
  const LHCb::RecVertex vtx = **iv ;
  makeJets(input_,  vtx, jets_ );
  return StatusCode::SUCCESS ;

}


StatusCode LoKi::VVSeedFinder::makeJets
( const IJetMaker::Input& input_ , const LHCb::RecVertex& RecVert_  , IJetMaker::Jets& jets_ ) const
{

  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  LHCb::RecVertex::Container::const_iterator iv  ;
  LHCb::VertexBase* RecVert =  RecVert_.clone();

  std::vector<LHCb::VertexBase*> PVs;

  for(iv = verts->begin() ; iv != verts->end() ; iv++)
    if((*iv)->position() != RecVert->position()) PVs.push_back((*iv)->clone());



  IJetMaker::Jets Seeds;
  double ipl, chi2l;
  StatusCode sc;
  LHCb::Vertex vtx, vseedbest;
  double dca12, dcae12;
  //Kshort Mass
  const double MK0=497.7;

  std::vector<Gaudi::XYZVector> Slopes_flag;



  LHCb::Particle::ConstVector  PartIPK0Sub;
  LHCb::Particle::ConstVector  PartIP;
  LHCb::Particle::ConstVector  all;
  LHCb::Particle::ConstVector  KsDau;


  double tof = 0, tofe =0 ;

  int testLiHi = 0;
  int testClone = 0;
  int testLong = 0;
  int testCharged= 0;



  //----------------------------------------//
  // test the particles                     //
  //----------------------------------------//


  for (  LHCb::Particle::ConstVector::const_iterator ip = input_.begin() ;
         input_.end() != ip ; ++ip )     {
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) { Warning ( "Invalid input particle" ) ; continue ; }
    const LHCb::ProtoParticle * myProto = p->proto();
    if(myProto != NULL){
      const LHCb::Track* trk   = myProto->track();
      if( trk != NULL ){
        if( trk->flags() == 1 || trk->flags() == 4) continue;
        // == 1 : backward, == 4 clone
        if( trk->flags() == 1) continue;
        double CloneDist = trk->info(LHCb::Track::CloneDist,9999.);
        double TrkLi =     trk->likelihood();
        double GhostProb = trk->ghostProbability();
        double LongTrk =   trk->type();
        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose()  << "addinfo Likelihood = " << TrkLi << endmsg;
          verbose()  << "addinfo CloneDist  = " << CloneDist<< endmsg;
          verbose()  << "addinfo GhostProb  = " << GhostProb << endmsg;
          verbose()  << "addinfo Longb      = " << LongTrk << endmsg;
          verbose()  << "p Trk   = " <<  trk->momentum() / Gaudi::Units::GeV << endmsg;
        }
        if(CloneDist < 9999.) testClone++;
        if(TrkLi     < -60. ) testLiHi++;
        if(LongTrk   ==  3  ) testLong++;
        //----------------------------------------//
        // discard clones and Ghost               //
        //----------------------------------------//
        if(CloneDist < 9999.) continue;
        if(TrkLi     < -60. ) continue;
      }
    }


    bool m_FilterPart = true;
    //----------------------------------------//
    // check if 1 particle is not twice on    //
    // the TES with the slopes                //
    //----------------------------------------//
    bool flag_same = false;
    if  (m_FilterPart){
      if(Slopes_flag.size() !=0){
        for(int k = 0; k< (int)Slopes_flag.size();k++){
          if(Slopes_flag.at(k) == p->slopes()){
            flag_same = true;
            if ( msgLevel(MSG::VERBOSE) )
              verbose()  << "same slopes !!  s1: "  <<Slopes_flag.at(k)  << "   s2: " << p->slopes() <<endmsg;
            break;
          }
        }
      }
      if(!flag_same){
        Slopes_flag.push_back(p->slopes());
      }
      else{
        continue;
      }
    }


    //----------------------------------------//
    // save all the part that arrived here    //
    // as the global container                //
    //----------------------------------------//
    all.push_back(p);

    if( p->charge() == 0 ) continue;
    testCharged ++;
    if( p->proto()==NULL ) continue;
    if( p->proto()->track()==NULL ) continue;
    if( p->proto()->track()->type() != LHCb::Track::Long ) continue;
    if( p->proto()->track()->chi2PerDoF() >  m_TrkChi2DoF ) continue;

    sc = m_dist->distance(p, RecVert, ipl, chi2l);

    if( p->p() <  m_PTrackMin  ) continue;
    if( p->pt()  <  m_PtTrackMin    ) continue;
    if( ipl <  m_IPmin) continue;
    if( sqrt(chi2l) <  m_Signif) continue;

    //----------------------------------------//
    // save all the charge, with a good Pt    //
    // and a good IP/IPE as the input         //
    // particles for the Cone Jet Algo        //
    //----------------------------------------//


    PartIP.push_back(p);
  }


  //----------------------------------------//
  // discard the trk coming any VTX         //
  //----------------------------------------//

  if(m_PVveto)
    for(iv = verts->begin() ; iv != verts->end() ; iv++)
      RemoveTracks( PartIP, **iv);



  if ( msgLevel(MSG::DEBUG) )
  {
    debug()  << "nb of ghost (LiH)             : "<< testLiHi << endmsg;
    debug()  << "nb of clone                   : "<< testClone << endmsg;
    debug()  << "nb of long                    : "<< testLong << endmsg;
    debug()  << "nb of charged                 : "<< testCharged << endmsg;
    debug()  << "nb of 'photon'                : "<< (int) all.size() - double(testCharged) << endmsg;
    debug()  << "Particle ALL size             : "<< input_.size() << endmsg;
    debug()  << "Particle ALL size (JetCone)   : "<< all.size() << endmsg;
    debug()  << "Particle INPUT for JET        : "<< PartIP.size() << endmsg;
  }

  int cntK0 = 0;


  //=============================================================================
  // COMBINEPIPIMINK0: GET RIF OF K0S
  //=============================================================================
  for (  LHCb::Particle::ConstVector::const_iterator ip = PartIP.begin() ;
         PartIP.end() != ip ; ++ip )     {
    const LHCb::Particle* p = *ip ;
    bool testK = false;

    for (  LHCb::Particle::ConstVector::const_iterator kp = KsDau.begin() ;
           KsDau.end() != kp ; ++kp ) if(*ip == *kp){testK = true; break;}

    if(!testK)
      for (  LHCb::Particle::ConstVector::const_iterator ip2 = ip+1 ;
             PartIP.end() != ip2 ; ++ip2 )     {
        const LHCb::Particle* p2 = *ip2 ;

        sc = m_dist->distance((p), (p2), dca12, dcae12);
        if(!sc) {  warning()  << "can't mesure the dist "<< endmsg;}
        if ( dca12 > m_DtrakMax) continue; // dca too large bwt the tracks

        Gaudi::LorentzVector sum = (p)->momentum() + (p2)->momentum();

        //=============================================================================
        // COMBINEPIPIMINK0: GET RIF OF K0S
        //=============================================================================
        if((p)->particleID().abspid() == 211
           &&  (p2)->particleID().abspid() == 211
           && ((p)->charge()) * ((p2)->charge()) < 0 )
          if ( fabs(sum.M()-MK0)<m_DMK0 ) {
            cntK0++;
            testK = true;

            KsDau.push_back(p);
            KsDau.push_back(p2);

          }
      }

    if(testK) continue;
    PartIPK0Sub.push_back(p);
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    debug()  << "nb K0                         : "<<  cntK0 << endmsg;
    debug()  << "Particle INPUT for JET no K0  : "<< PartIPK0Sub.size() << endmsg;
  }


  if(PartIPK0Sub.size() <2){
    Warning ( "Not enough good part for seeding").ignore() ;
    return StatusCode::SUCCESS ;
  }

  Gaudi::XYZPoint  BL_P =  Gaudi::XYZPoint(0,0,0);
  Gaudi::LorentzVector BL_M = Gaudi::LorentzVector (0,0,1,0);

  if( exist<LHCb::Particle::Range>("/Event/BeamLine") ){
    const LHCb::Particle::Range BL = get<LHCb::Particle::Range>( "/Event/BeamLine" );
    const LHCb::Particle* tmp = *(BL.begin());
    BL_P =  Gaudi::XYZPoint( tmp->referencePoint() );
    BL_M  = Gaudi::LorentzVector (tmp->momentum() );
    //   m_BeamLine->setMomentum( tmp->momentum() );
    if( msgLevel(MSG::DEBUG) )
      debug()<<"Beam line position "<<  BL_P
             <<" direction " << BL_M << endmsg;
  } else {
    if( msgLevel(MSG::DEBUG) )
      debug()<<"No Beam line found at "<< "/Event/BeamLine" << endmsg;

    BL_P = Gaudi::XYZPoint( RecVert->position() );

    if( msgLevel(MSG::DEBUG) )
      debug()<<"Beam line position "<<  BL_P
             <<" direction " << BL_M << endmsg;
  }


  //COMBIN PART

  LHCb::Particle::ConstVector::const_iterator jp, kp, ksp, lp;
  for ( jp =  PartIPK0Sub.begin(); jp != PartIPK0Sub.end(); jp++ ) {
    for ( kp = jp+1; kp != PartIPK0Sub.end(); kp++ ) {

      sc = m_dist->distance((*jp), (*kp), dca12, dcae12);
      if(!sc) {  warning()  << "can't mesure the dist "<< endmsg;}
      if ( dca12 > m_DtrakMax) continue; // dca too large bwt the tracks

      Gaudi::LorentzVector sum = (*jp)->momentum() + (*kp)->momentum();


      if( m_Triplets){

        //add Try 3-tracks Seed otpion
        for ( lp = (kp+1) ; lp != PartIPK0Sub.end(); lp++ ) {


          double dca13, dcae13;
          m_dist->distance((*jp), (*lp), dca13, dcae13);
          if ( dca13> m_DtrakMax) continue;                 // dca too large btw the trks

          double dca23, dcae23;
          m_dist->distance((*lp), (*kp), dca23, dcae23);
          if ( dca23> m_DtrakMax) continue;                 // dca too large btw the trks

          StatusCode scvtx = m_fitter->fit( vtx, **jp, **kp, **lp );
          if( scvtx.isFailure() ){warning()<< "VTX Fit failed"<< endmsg;continue;}

          if(vtx.chi2PerDoF() > m_SeedsMaxChi2DoF) continue;
          //cut on chi2
          double mydz= -1;
          if (scvtx) mydz = vtx.position().z() -  RecVert->position().z();
          if (mydz<0) continue;
          // the vtx is before the PV

          //intersection of the beam line with the XY plane,
          //find the lambda parameter of the line.
          double lambda = (vtx.position().z() -  BL_P.z()) /
            BL_M.z();

          //find x and y of intersection point
          double x = BL_P.x()
            + lambda *  BL_M.x();
          double y = BL_P.y()
            + lambda * BL_M.y();

          x -= vtx.position().x(); y -= vtx.position().y();

          double dr = sqrt(x*x + y*y);
          if (dr < m_DRmin || dr > m_DRmax) continue;

          // the vtx is too close or to far from the beam direction

          //---------------------------
          //Seed level cuts -----------
          //---------------------------
          LHCb::VertexBase* PPvtx2 = vtx.clone();
          m_dist->distance(PPvtx2,RecVert, tof, tofe);
          if (tof< m_TseedVtxMin || tof> m_TseedVtxMax) continue;
          // the vtx is too close or to far from the PV (~time of flight)
          bool PVveto = false;
          if(m_PVveto)
            for( std::vector<LHCb::VertexBase*>::iterator pv = PVs.begin(); pv!=PVs.end(); pv++ ){
              m_dist->distance(PPvtx2,*pv, tof, tofe);
              if (tof< m_TseedVtxMinAnyPV){PVveto=true; break;}
            }
          if(PVveto) continue;

          Gaudi::LorentzVector sum2 = sum + (*lp)->momentum();
          if(sum2.Pt() <   m_PtSeedsMin) continue;
          //Pt too soft

          LHCb::Particle::ConstVector daughters2 ;
          LHCb::Particle              pSeed2 ;
          LHCb::Vertex                vSeed2 ;

          daughters2.push_back ( *lp );
          daughters2.push_back ( *kp );
          daughters2.push_back ( *jp );
          pSeed2.setParticleID     (LHCb::ParticleID( m_seedID )) ;
          pSeed2.setReferencePoint ( RecVert->position() ) ;
          pSeed2.setMomentum ( sum2  ) ;

          StatusCode sc = m_combiner->combine ( daughters2 , pSeed2 , vSeed2 ) ;

          pSeed2.setEndVertex (  vtx.clone() ) ;
          //save the trks as the daugthers
          //remove
          if ( sc.isFailure() ) Warning ( "Error from momentum combiner, skip", sc, 0 ).ignore() ;
          Seeds.push_back(pSeed2.clone());
          //remove lp to test another part


        }//end loop lp
      }

      //2-seed
      LHCb::Particle              pSeed ;
      LHCb::Particle::ConstVector daughters ;
      LHCb::Vertex                vSeed ;


      StatusCode scvtx = m_fitter->fit( vtx, **jp, **kp );

      if( scvtx.isFailure() ){warning()<< "VTX Fit failed"<< endmsg;continue;}

      if(vtx.chi2PerDoF() > m_SeedsMaxChi2DoF) continue;

      double mydz= -1;
      if (scvtx) mydz = vtx.position().z() -  RecVert->position().z();
      if (mydz<0) continue;
      // the vtx is before the PV

      // Gaudi::XYZPoint  dv =   Gaudi::XYZPoint(vtx.position() - m_BeamLine->referencePoint().position());

      //intersection of the beam line with the XY plane,
      //find the lambda parameter of the line.
      double lambda = (vtx.position().z() -  BL_P.z()) /
        BL_M.z();

      //find x and y of intersection point
      double x = BL_P.x()
        + lambda *  BL_M.x();
      double y = BL_P.y()
        + lambda * BL_M.y();

      x -= vtx.position().x(); y -= vtx.position().y();

      double dr = sqrt(x*x + y*y);
      if (dr < m_DRmin || dr > m_DRmax) continue;
      // the vtx is too close or to far from the beam direction

      //---------------------------
      //Seed level cuts -----------
      //---------------------------
      LHCb::VertexBase* PPvtx2 = vtx.clone();
      m_dist->distance(PPvtx2,RecVert, tof, tofe);
      if (tof< m_TseedVtxMin || tof> m_TseedVtxMax) continue;
      // the vtx is too close or to far from the PV (~time of flight)

      bool PVveto = false;
      if(m_PVveto)
        for( std::vector<LHCb::VertexBase*>::iterator pv = PVs.begin(); pv!=PVs.end(); pv++ ){
          m_dist->distance(PPvtx2,*pv, tof, tofe);
          if (tof< m_TseedVtxMinAnyPV){PVveto=true; break;}
        }
      if(PVveto) continue;

      if(sum.Pt() <  m_PtSeedsMin) continue;
      //Pt too soft

      daughters.push_back ( *jp );
      daughters.push_back ( *kp );

      pSeed.setParticleID     (LHCb::ParticleID( m_seedID )) ;
      pSeed.setReferencePoint ( RecVert->position() ) ;
      pSeed.setMomentum ( sum  ) ;
      StatusCode sc = m_combiner->combine ( daughters , pSeed , vSeed ) ;
      //save the trks as the daugthers
      pSeed.setEndVertex ( vtx.clone() ) ;
      if ( sc.isFailure()) Warning ( "Error from momentum combiner, skip", sc, 0 ).ignore() ;
      Seeds.push_back(pSeed.clone());



    }//end loop kp
  }//end loop jp





  if ( Seeds.empty() ) {
    Warning ( "No Seeds from LoKiVVSeedFinder").ignore() ;
    return StatusCode::SUCCESS ;
  }

  //=============================================================================
  // SORT THE SEEDS
  //=============================================================================
  if(m_sort == 1) std::sort(Seeds.begin(), Seeds.end(),sortPt() );
  if(m_sort == 2) std::sort(Seeds.begin(), Seeds.end(),sortE() );
  if(m_sort == 3) std::sort(Seeds.begin(), Seeds.end(),sortEta() );
  if(m_sort == 4) std::sort(Seeds.begin(), Seeds.end(),sortDauPt() );

  IJetMaker::Jets  ProtoJets , MergeProtoJets ;
  ProtoJets.reserve( Seeds.size() ) ;
  MergeProtoJets.reserve( Seeds.size() ) ;



  int myj= 0;
  for (   IJetMaker::Jets::iterator ijet =  Seeds.begin() ;  Seeds.end() != ijet ; ++ijet )
  {
    LHCb::Particle* pJet = *ijet;

    myj++;
    if( msgLevel(MSG::DEBUG) )
    {
      debug() << "Seeds  "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
    }

  }




  //=============================================================================
  // BUILDPROTOJETS: BUILDS JETS AROUND THE SEEDS FORMED WITH THE PRIMARY VERTEX AND THE VERTEXCOMBINATIONS
  //=============================================================================
  ProtoJets = JetCone( m_r , Seeds, input_);


  myj= 0;
  for (   IJetMaker::Jets::iterator ijet = ProtoJets.begin() ; ProtoJets.end() != ijet ; ++ijet )
  {
    LHCb::Particle* pJet = *ijet;

    myj++;
    if( msgLevel(MSG::DEBUG) )
    {
      debug() << "BSeeds "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
    }

  }

  //=============================================================================
  // FILTERPROTOJETS: MERGES JETS WHICH ARE TOO CLOSE OR THE SAME
  //=============================================================================
  MergeProtoJets = FilterProtoJets(m_DeltaRSeeds,  ProtoJets);



  if ( statPrint() || msgLevel ( MSG::INFO ) )
  { counter ( "#seeds" ) += Seeds.size() ; }

  jets_ = MergeProtoJets ;


  myj= 0;
  for (   IJetMaker::Jets::iterator ijet = MergeProtoJets.begin() ; MergeProtoJets.end() != ijet ; ++ijet )
  {
    LHCb::Particle* pJet = *ijet;

    myj++;
    if( msgLevel(MSG::DEBUG) )
    {
      debug() << "CSeeds "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
    }

  }


  return StatusCode::SUCCESS ;
}






//=============================================================================
// JETCONE: THE HEART OF EVERYTHING, BUILDS A JET AROUND A SEED
//=============================================================================
IJetMaker::Jets LoKi::VVSeedFinder::JetCone( const  double &Rmax,
                                             IJetMaker::Jets &seeds,
                                             const IJetMaker::Input &inputs) const{

  LHCb::Particle::Vector pJetVec ;

  IJetMaker::Jets::const_iterator iseeds;

  for ( iseeds=seeds.begin(); iseeds != seeds.end(); ++iseeds) {

    const LHCb::Particle* pSeed = *iseeds;

    LHCb::Particle              pJet ;
    LHCb::Vertex                vJet      ;
    LHCb::Particle::ConstVector daughters ;

    pJet.setParticleID     (LHCb::ParticleID(9999999)) ;

    pJet.setReferencePoint ( pSeed->referencePoint() ) ;

    const double PI=3.14159265;

    double  phiSeed = pSeed->momentum().phi();  double etaSeed = pSeed->momentum().eta();
    Gaudi::LorentzVector ptot =  Gaudi::LorentzVector(0., 0., 0. ,0.);

    LHCb::Particle::ConstVector::const_iterator ip;

    for ( ip=inputs.begin(); ip != inputs.end(); ++ip) {


      const LHCb::Particle* myPart = *ip;
      Gaudi::LorentzVector p1 =  myPart->momentum();

      double phiPart = myPart->momentum().phi();
      double etaPart   = myPart->momentum().eta();
      double Dphi = fabs(phiSeed-phiPart);
      if (Dphi>PI) { Dphi = 2*PI - Dphi; };
      double R = sqrt( Dphi*Dphi + (etaSeed-etaPart)*(etaSeed-etaPart) );

      if (R>Rmax)      continue;
      ptot = ptot + p1;

      daughters.push_back ( myPart ) ;

    }

    if ( daughters.empty())  Warning("Empty list of of daughter particles, skip it");


    StatusCode sc = m_combiner->combine ( daughters , pJet , vJet ) ;
    if ( sc.isFailure()) Warning ( "Error from momentum combiner, skip", sc, 0 ).ignore() ;


    pJet.setMomentum        ( Gaudi::LorentzVector ( ptot )  ) ;
    pJetVec.push_back( pJet.clone() );

  }



  return pJetVec;

}

//=============================================================================
// JETCONE: END
//=============================================================================



//=============================================================================
// FILTERPROTOJETS: MERGES JETS WHICH ARE TOO CLOSE OR THE SAME
//=============================================================================
IJetMaker::Jets  LoKi::VVSeedFinder::FilterProtoJets(const double &DeltaRSeeds,
                                                     IJetMaker::Jets  &ProtoJets) const  {
  //----------------------------------------------------------------------
  // filter protojets which are too close or the same-------- ------------
  //----------------------------------------------------------------------

  //  const double PI=3.14159265;
  // erase the 6th element
  //  myvector.erase (myvector.begin()+5);

  if( msgLevel(MSG::DEBUG) ) debug()  <<"size of  noFiltered ProtoJets           = " <<  ProtoJets.size()  << endmsg;

  IJetMaker::Jets NewProtoJets;
  IJetMaker::Jets NewProtoJets2;

  bool testJets = true;
  int mySize = 0;
  int k = 0;
  if(m_preFilter){
    for ( IJetMaker::Jets::iterator ip = ProtoJets.begin(); ip != ProtoJets.end(); ++ip ){
      LHCb::Particle* seed = (*ip);
      if(seed->momentum().E() >0.){
        int j = 0;
        for ( IJetMaker::Jets::iterator ip2 = ip+1; ip2 != ProtoJets.end(); ++ip2 ){
          j++;

          LHCb::Particle* seed2 = (*ip2);
          if(seed2->momentum().E() >0.){

            double testR = getDeltaR(seed,seed2);

            if( testR <  0.00005 ){
              //    if( testR <  0.05 || (fabs(v1.E() - v2.E())/v1.E() <  0.05 && testR < 0.1)){
              //SAME JET !
              ProtoJets.at(k)->setMomentum(Gaudi::LorentzVector(seed->momentum()));

              double myWeight  = seed->weight()+seed2->weight();

              ProtoJets.at(k)->eraseInfo(LHCb::Particle::Weight);
              ProtoJets.at(k)->addInfo(LHCb::Particle::Weight,myWeight);

              seed->eraseInfo(LHCb::Particle::Weight);
              seed->addInfo(LHCb::Particle::Weight,myWeight);

              seed->setMomentum(Gaudi::LorentzVector(seed->momentum()));
              ProtoJets.at(k+j)->setMomentum(Gaudi::LorentzVector(0.,0.,0.,0.));


            }

          }

        }

      }
      k++;
    }
  }
  for (  IJetMaker::Jets::iterator ip = ProtoJets.begin(); ip != ProtoJets.end(); ++ip ){

    LHCb::Particle* seed = (*ip);
    if(seed->momentum().E() >0.){
      NewProtoJets.push_back(seed);
    }
  }

  ProtoJets.clear();
  ProtoJets = NewProtoJets;

  if( msgLevel(MSG::DEBUG) ) debug()  <<"size of preFiltered ProtoJets           = " <<  ProtoJets.size()  << endmsg;

  if(m_jetFilter){
    while(testJets){

      NewProtoJets.clear();
      mySize = (int) ProtoJets.size();
      testJets = false;
      int k = 0;
      int J1test = 0;
      int J2test = 0;
      double testDR = 100.0;

      NewProtoJets = ProtoJets;

      for ( IJetMaker::Jets::iterator ip = ProtoJets.begin(); ip != ProtoJets.end(); ++ip ){
        LHCb::Particle* seed = (*ip);

        if(seed->momentum().E() >0.){
          int j = 0;
          for ( IJetMaker::Jets::iterator ip2 = ip+1; ip2 != ProtoJets.end(); ++ip2 ){
            j++;
            LHCb::Particle* seed2 = (*ip2);

            if(seed2->momentum().E() >0.){

              double    R = getDeltaR(seed, seed2);

              if (R < DeltaRSeeds && R < testDR ) {

                testDR = R;
                J1test = k;
                J2test = j+k;


              }
            }
          }
        }
        k++;
      }


      if(testDR < DeltaRSeeds){
        if( msgLevel(MSG::VERBOSE) )
        {
          verbose() <<" FilterProtoJet merge " << endmsg;
          verbose() << "Nseed phi/eta= "<< (ProtoJets.at(J1test)->momentum()).phi() <<" " << (ProtoJets.at(J1test)->momentum()).eta() << endmsg;
          verbose() << "Nseed2phi/eta= "<< (ProtoJets.at(J2test)->momentum()).phi() <<" " << (ProtoJets.at(J2test)->momentum()).eta() << endmsg;
          verbose() << "E P1= "<< (ProtoJets.at(J1test)->momentum()).E() <<"   E P2= " << (ProtoJets.at(J2test)->momentum()).E() << endmsg;
          verbose() << "E P1/P2= "<< fabs((ProtoJets.at(J1test)->momentum()).E()-(ProtoJets.at(J2test)->momentum()).E())/(ProtoJets.at(J1test)->momentum()).E()  << endmsg;
        }
        if(testDR > LHCb::Math::looseTolerance || !m_preFilter ){

          NewProtoJets.at(J1test)->setMomentum(Gaudi::LorentzVector((ProtoJets.at(J1test)->momentum() + ProtoJets.at(J2test)->momentum())));


          const  LHCb::Particle::ConstVector DaughtersJ2 = ProtoJets.at(J2test)->daughtersVector();
          for ( LHCb::Particle::ConstVector::const_iterator ip3 = DaughtersJ2.begin() ; ip3 !=  DaughtersJ2.end(); ++ip3 ){
            const LHCb::Particle* d1 = *ip3;
            NewProtoJets.at(J1test)->addToDaughters(d1);
          }

          double myWeight  = ProtoJets.at(J1test)->weight()+ProtoJets.at(J2test)->weight();
          double myCL = -1;

          if(ProtoJets.at(J1test)->momentum().E() > ProtoJets.at(J2test)->momentum().E()){
            NewProtoJets.at(J1test)->setReferencePoint( ProtoJets.at(J1test)->referencePoint());
            myCL = ProtoJets.at(J1test)->confLevel();
          } else{
            NewProtoJets.at(J1test)->setReferencePoint( ProtoJets.at(J2test)->referencePoint());
            myCL = ProtoJets.at(J2test)->confLevel();
          }
          NewProtoJets.at(J1test)->eraseInfo(LHCb::Particle::Weight);
          NewProtoJets.at(J1test)->addInfo(LHCb::Particle::Weight,myWeight);
          NewProtoJets.at(J1test)->eraseInfo( LHCb::Particle::ConfLevel);
          NewProtoJets.at(J1test)->addInfo(LHCb::Particle::ConfLevel,myCL);

        }else{

          //Same jet... nothing to do.... just erase one.


        }

        NewProtoJets.erase( NewProtoJets.begin()+J2test);
      }



      if( msgLevel(MSG::VERBOSE) )
      {
        verbose()<<" ProtoJet Size         : " << mySize<<endmsg;
        verbose()<<" ProtoJet Size         : " << ProtoJets.size()<<endmsg;
        verbose()<<" NewFilterProtoJet Size: " << NewProtoJets.size()<<endmsg;
      }

      if( (int)NewProtoJets.size() !=  mySize ) testJets = true;


      ProtoJets.clear();
      ProtoJets = NewProtoJets;


      if(NewProtoJets.size() % 10 == 0)
        if( msgLevel(MSG::VERBOSE) )verbose() << "Number of filtered ProtoJets is " <<  NewProtoJets.size() << endmsg;

    }

    if( msgLevel(MSG::DEBUG) ) debug()  <<"size of    Filtered ProtoJets           = " <<  NewProtoJets.size()  << endmsg;


    NewProtoJets.clear();
    for ( IJetMaker::Jets::iterator ip2 = ProtoJets.begin(); ip2 != ProtoJets.end(); ++ip2 ){
      if((*ip2)->momentum().Pt() < m_PtMergedSeedsMin ) continue;
      NewProtoJets.push_back((*ip2));
    }
  }

  return NewProtoJets;
}
//=============================================================================
// END OF FILTERPROTOJETS
//=============================================================================





//=============================================================================
// GETDELTAR: CALCULATES DELTAR
//=============================================================================
double LoKi::VVSeedFinder::getDeltaR(LHCb::Particle *p1, LHCb::Particle *p2) const {

  const double PI=3.14159265;
  double R,Dphi,phi1,phi2,e1,e2;
  phi1 = p1->momentum().phi();
  phi2 = p2->momentum().phi();
  e1 = p1->momentum().eta();
  e2 = p2->momentum().eta();
  Dphi = fabs(phi1-phi2);
  if (Dphi>PI) { Dphi = 2*PI - Dphi; };
  R = sqrt( Dphi*Dphi + (e1-e2)*(e1-e2) );
  return R;

}

//=============================================================================
// Remove trks form a PV
//=============================================================================







void LoKi::VVSeedFinder::RemoveTracks(LHCb::Particle::ConstVector & particles,
                                      const LHCb::RecVertex PV ) const{

  //Remove all tracks from the PV
  LHCb::Particle::ConstVector tmp;
  SmartRefVector< LHCb::Track >::const_iterator iPV = PV.tracks().begin();
  int endkey = PV.tracks().back()->key();

  for (  LHCb::Particle::ConstVector::const_iterator i = particles.begin();
         i != particles.end();++i) {
    if( (*i)->proto()->track() == NULL ) continue;
    const LHCb::Track * tk = (*i)->proto()->track();

    while( ((*iPV)->key() < tk->key()) && (*iPV)->key() != endkey ){
      iPV++;
    }
    if( (*iPV)->key() == tk->key() ){
      if( (*iPV)->key() != endkey ) iPV++;
      continue;
    }
    tmp.push_back( *i );
  }
  //Copy back tmp to particles
  particles = tmp;

}


// ============================================================================
/// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,VVSeedFinder)
// ============================================================================
// The END
// ============================================================================
