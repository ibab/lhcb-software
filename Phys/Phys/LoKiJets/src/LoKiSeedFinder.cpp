// $Id: LoKiSeedFinder.cpp,v 1.2 2009-12-14 12:34:33 cocov Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKiSeedFinder.h"
// ============================================================================
/** @file
 *  Implementation file for class  LoKi::SeedFinder
 *  @author Cedric POTTERAT   cedric.potterat@cern.ch
 *  @date   2011-01-31
 */
// ============================================================================
/*  standard initialization of the tool
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::SeedFinder::initialize ()
{
  const StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }

  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;

  m_dist     = tool<IDistanceCalculator>("LoKi::DistanceCalculator");

  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;

  m_fitter   = tool<IVertexFit>("OfflineVertexFitter");

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() <<" ------------------------------------------------------ "<<endmsg;
    debug() <<" |                                                    | "<<endmsg;
    debug() <<" |         SeedsFinder Tool for Phys Particles        | "<<endmsg;
    debug() <<" |                                                    | "<<endmsg;
    debug() <<" ------------------------------------------------------ "<<endmsg;
    debug() <<" |                                                    | "<<endmsg;
    debug() <<" |                                                      "<<endmsg;
    debug() <<" |   SeedsFinder parameters: "                           <<endmsg;
    debug() <<" |   SeedID                "<<   m_seedID                    <<endmsg;
    debug() <<" |   SeedTriplets          "<<   m_Triplets                  <<endmsg;
    debug() <<" |   PtTrackMin            "<<   m_PtTrackMin                <<endmsg;
    debug() <<" |   PTrackMin             "<<   m_PTrackMin                 <<endmsg;
    debug() <<" |   TrkChi2PerDoF         "<<   m_TrkChi2DoF                <<endmsg;
    debug() <<" |   IPmin                 "<<   m_IPmin                     <<endmsg;
    debug() <<" |   Signif                "<<   m_Signif                    <<endmsg;
    debug() <<" |   DMK0                  "<<   m_DMK0                      <<endmsg;
    debug() <<" |   TseedVtxMin           "<<   m_TseedVtxMin               <<endmsg;
    debug() <<" |   TseedVtxMax           "<<   m_TseedVtxMax               <<endmsg;
    debug() <<" |   DtrakMax              "<<   m_DtrakMax                  <<endmsg;
    debug() <<" |   PtSeedsMin            "<<   m_PtSeedsMin                <<endmsg;
    debug() <<" |   SeedsVtxMaxChi2PerDoF "<<   m_SeedsMaxChi2DoF           <<endmsg;
    debug() <<" |                                                      "<<endmsg;
    debug() <<" |                                                    | "<<endmsg;
    debug() <<" ------------------------------------------------------ "<<endmsg;
  }

  return sc;
}

// ===========================================================================
// find seeds
// ===========================================================================

StatusCode LoKi::SeedFinder::makeJets
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const
{

  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  if(verts->size()!=1) return StatusCode::SUCCESS;
  LHCb::RecVertex::Container::const_iterator iv =verts->begin() ;

  const LHCb::RecVertex vtx = **iv ;

  makeJets(input_,  vtx, jets_ );

  return StatusCode::SUCCESS ;
}


StatusCode LoKi::SeedFinder::makeJets
( const IJetMaker::Input& input_ ,const LHCb::RecVertex& RecVert_ ,  IJetMaker::Jets& jets_ ) const
{


  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  LHCb::RecVertex::Container::const_iterator iv  ;
  LHCb::VertexBase* RecVert =  RecVert_.clone();

  std::vector<LHCb::VertexBase*> PVs;

  for(iv = verts->begin() ; iv != verts->end() ; iv++)
    if((*iv)->position() != RecVert->position()) PVs.push_back((*iv)->clone());




  IJetMaker::Jets Seeds;
  IJetMaker::Jets SeedsPurged;

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

  if(m_PVveto)
  {
    for(iv = verts->begin() ; iv != verts->end() ; ++iv )
    {
      RemoveTracks( PartIP, **iv);
    }
  }

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
    return Warning ( "Not enough good part for seeding", StatusCode::SUCCESS) ;
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
  }
  else
  {
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
          if ( sc.isFailure())   Error ( "Error from momentum combiner, skip" , sc ) ;
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
      if ( sc.isFailure())   Error ( "Error from momentum combiner, skip" , sc ) ;
      Seeds.push_back(pSeed.clone());



    }//end loop kp
  }//end loop jp



  if ( Seeds.empty() ) {
    Warning ( "No Seeds from LoKi::SeedFinder") ;
    return StatusCode::SUCCESS ;
  }

  //=============================================================================
  // SORT THE SEEDS
  //=============================================================================

  if     (m_sort == 1) std::stable_sort(Seeds.begin(), Seeds.end(),sortPt() );
  else if(m_sort == 2) std::stable_sort(Seeds.begin(), Seeds.end(),sortE() );
  else if(m_sort == 3) std::stable_sort(Seeds.begin(), Seeds.end(),sortEta() );
  else if(m_sort == 4) std::stable_sort(Seeds.begin(), Seeds.end(),sortDauPt() );
  else if(m_sort == 5) std::stable_sort(Seeds.begin(), Seeds.end(),sortCHi2NDoF() );
  else if(m_sort == 6) std::stable_sort(Seeds.begin(), Seeds.end(),sortDirAng() );
  else if(m_sort == 7) std::stable_sort(Seeds.begin(), Seeds.end(),sortCOMBO() );

  //remove seed who share a track with a higher racked seed
  int myj= 0;

  for (   IJetMaker::Jets::iterator ijet =  Seeds.begin() ;  Seeds.end() != ijet ; ++ijet ) {
    myj++;


    bool samedau = false;
    LHCb::Particle*  Seed1 = *ijet;
    if(Seed1->daughters().size()<2) continue;

    const LHCb::Particle* dau11(0);
    const LHCb::Particle* dau12(0);
    const LHCb::Particle* dau13(0);

    dau11 = Seed1->daughters().at(0).target();
    dau12 = Seed1->daughters().at(1).target();

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Seeds  "<<myj<<"| nb of combin Seeds:  "<<   Seed1->weight()<< "  nb part in the jet:  " <<  Seed1->daughtersVector().size() <<"  Pt:  "<<  Seed1->pt() << endmsg;
      verbose() <<         "        | m  : "<<   Seed1->momentum() << endmsg;
      verbose() <<         "        | eta: "<<   Seed1->momentum().eta() << "  / phi: " <<    Seed1->momentum().phi() <<endmsg;
      verbose() <<         "        |dau1: "<<  dau11->pt()<<endmsg;
      verbose() <<         "        |dau2: "<<  dau12->pt()<<endmsg;
    }

    if(Seed1->daughters().size()>2){
      dau13 = Seed1->daughters().at(2).target();
      if ( msgLevel(MSG::VERBOSE) ) verbose() <<         "        |dau3: "<<  dau13->pt()<<endmsg;
    }

    Gaudi::XYZVector seedd = Gaudi::XYZVector(
                                              Gaudi::XYZPoint(Seed1->endVertex()->position()) -
                                              Gaudi::XYZPoint(Seed1->referencePoint())
                                              );


    double dotprod = seedd.Unit().Dot(Seed1->momentum().Vect().Unit());


    double a    =  std::fabs(std::acos (dotprod));

    double combo1 = 1000*1000*(a * Seed1->endVertex()->chi2PerDoF())/(Seed1->daughtersVector().size()*Seed1->pt()*Seed1->pt());

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() <<         "        |PAng: "<<   a<<endmsg;
      verbose() <<         "        |Chi2: "<<   Seed1->endVertex()->chi2()<<endmsg;
      verbose() <<         "        |nDoF: "<<   Seed1->endVertex()->nDoF()<<endmsg;
      verbose() <<         "        |C/N : "<<   Seed1->endVertex()->chi2PerDoF()<<endmsg;
      verbose() <<         "        |COMB: "<<   combo1<<endmsg;
    }


    for (IJetMaker::Jets::iterator ijet2 =  SeedsPurged.begin() ;  SeedsPurged.end() != ijet2 ; ++ijet2 ) {


      LHCb::Particle*  Seed2 = *ijet2;

      const LHCb::Particle* dau21(0);
      const LHCb::Particle* dau22(0);
      const LHCb::Particle* dau23(0);

      dau21 = Seed2->daughters().at(0).target();
      dau22 = Seed2->daughters().at(1).target();
      if(Seed2->daughters().size()>2)  dau23 = Seed2->daughters().at(2).target();

      //     double R = getDeltaR(Seed1, Seed2);

      //     if(R < 0.5){//start the merging process

      if(( dau11 == dau21 ) ||
         ( dau12 == dau22 ) ||
         ( dau11 == dau22 ) ||
         ( dau12 == dau21 ) ){
        samedau = true;
        break;
      }
      if(Seed1->daughters().size()>2)
        if(( dau13 == dau21 ) ||
           ( dau13 == dau22 ) ){
          samedau = true;
          break;
        }
      if(Seed2->daughters().size()>2)
        if(( dau11 == dau23 ) ||
           ( dau12 == dau23 ) ){
          samedau = true;
          break;
        }
      if(Seed2->daughters().size()>2 && Seed1->daughters().size()>2)
        if ( dau13 == dau23 ){
          samedau = true;
          break;
        }

      //     }
    }
    if(samedau) continue;

    SeedsPurged.push_back(Seed1);

  }




  myj= 0;
  for ( IJetMaker::Jets::iterator ijet =  SeedsPurged.begin() ;
        SeedsPurged.end() != ijet ; ++ijet )
  {
    LHCb::Particle* pJet = *ijet;

    myj++;

    if( msgLevel(MSG::DEBUG) )
    {
      debug() << "SeedsP "<<myj<<"| nb of combin Seeds:  "
              <<  pJet->weight()<< "  nb part in the jet:  "
              << pJet->daughtersVector().size() <<"  Pt:  "
              << pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta()
              << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
    }

    const LHCb::Particle* dau11(0);
    const LHCb::Particle* dau12(0);
    dau11 = pJet->daughters().at(0).target();
    dau12 = pJet->daughters().at(1).target();


    if( msgLevel(MSG::DEBUG) )
    {
      debug() <<         "        |dau1: "<<  dau11->pt()<<endmsg;
      debug() <<         "        |dau2: "<<  dau12->pt()<<endmsg;
    }

    if(pJet->daughters().size()>2){
      const LHCb::Particle* dau13(0);
      dau13 = pJet->daughters().at(2).target();
      if( msgLevel(MSG::DEBUG) )
        debug() <<         "        |dau3: "<<  dau13->pt()<<endmsg;
    }



    Gaudi::XYZVector seedd = Gaudi::XYZVector(
                                              Gaudi::XYZPoint(pJet->endVertex()->position()) -
                                              Gaudi::XYZPoint(pJet->referencePoint())
                                              );


    double dotprod = seedd.Unit().Dot(pJet->momentum().Vect().Unit());


    double a    =  std::fabs(std::acos (dotprod));


    if( msgLevel(MSG::DEBUG) )
    {
      debug() <<         "        |dirA: "<<   a<<endmsg;
      debug() <<         "        |Chi2: "<<   pJet->endVertex()->chi2()<<endmsg;
      debug() <<         "        |nDoF: "<<   pJet->endVertex()->nDoF()<<endmsg;
      debug() <<         "        |C/N : "<<   pJet->endVertex()->chi2PerDoF()<<endmsg;
    }

    double combo1 = 1000*1000*(a * pJet->endVertex()->chi2PerDoF())/(pJet->daughtersVector().size()*pJet->pt()*pJet->pt());

    if( msgLevel(MSG::DEBUG) )
      debug() <<         "        |COMB: "<<   combo1<<endmsg;


  }



  if ( statPrint() || msgLevel ( MSG::INFO ) )
  { counter ( "#seeds" ) += SeedsPurged.size() ; }

  jets_ = SeedsPurged ;




  return StatusCode::SUCCESS ;
}





//=============================================================================
// Remove trks form a PV
//=============================================================================

void LoKi::SeedFinder::RemoveTracks(LHCb::Particle::ConstVector & particles,
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



//=============================================================================
// GETDELTAR: CALCULATES DELTAR
//=============================================================================
double LoKi::SeedFinder::getDeltaR(LHCb::Particle *p1, LHCb::Particle *p2) const {

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




// ============================================================================
/// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,SeedFinder)
// ============================================================================
// The END
// ============================================================================
