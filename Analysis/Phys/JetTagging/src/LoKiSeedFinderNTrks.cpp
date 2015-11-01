// $Id: LoKiSeedFinderNTrks.cpp,v 1.2 2009-12-14 12:34:33 cocov Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Local 
// ============================================================================
#include "LoKiSeedFinderNTrks.h"
// ============================================================================
/** @file 
 *  Implementation file for class  LoKi::SeedFinderNTrks
 *  @author Cedric POTTERAT   cedric.potterat@cern.ch
 *  @date   2011-01-31
 */
// ============================================================================
/*  standard initialization of the tool
 *  @return status code 
 */
// ============================================================================
StatusCode LoKi::SeedFinderNTrks::initialize () 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }

  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
  


  m_dist       = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  if ( !m_dist ) {
    err() << "Unable to Retrieve LoKi::DistanceCalculator" << endmsg;
    return StatusCode::FAILURE;
  }

  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;
  if ( !m_combiner) {
    err() << "Unable to Retrieve Default  ParticleCombiner" << endmsg;
    return StatusCode::FAILURE;
  }


  m_fitter     = tool<IVertexFit>("LoKi::VertexFitter");
  if ( !m_fitter ) {
    err() << "Unable to Retrieve Default VertexFitter" << endmsg;
    return StatusCode::FAILURE;
  }



  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |      SeedsFinderNTrks Tool for Phys Particles      | "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |                                                      "<<std::endl;
  std::cout<<" |   SeedsFinder parameters: "                           <<std::endl;
  std::cout<<" |   SeedID                "<<   m_seedID                    <<std::endl;	
  std::cout<<" |   PtTrackMin            "<<   m_PtTrackMin                <<std::endl;
  std::cout<<" |   PTrackMin             "<<   m_PTrackMin                 <<std::endl;
  std::cout<<" |   TrkChi2PerDoF         "<<   m_TrkChi2DoF                <<std::endl;
  std::cout<<" |   IPmin                 "<<   m_IPmin                     <<std::endl;
  std::cout<<" |   Signif                "<<   m_Signif                    <<std::endl;  	
  std::cout<<" |   DMK0                  "<<   m_DMK0                      <<std::endl;   	
  std::cout<<" |   TseedVtxMin           "<<   m_TseedVtxMin               <<std::endl; 	
  std::cout<<" |   TseedVtxMax           "<<   m_TseedVtxMax               <<std::endl;	
  std::cout<<" |   DtrakMax              "<<   m_DtrakMax                  <<std::endl;     	
  std::cout<<" |   PtSeedsMin            "<<   m_PtSeedsMin                <<std::endl;      	
  std::cout<<" |   SeedsVtxMaxChi2PerDoF "<<   m_SeedsMaxChi2DoF           <<std::endl; 
  std::cout<<" |   SeedsVtxMaxChi2PerDoF "<<   m_SeedsMaxChi2DoF           <<std::endl;  
  std::cout<<" |   SeedsVtxMaxChi2PerDoF "<<   m_SeedsMaxChi2DoF           <<std::endl;  
  std::cout<<" |   maxDeltaChi2VtxAddTrk "<<   m_deltaChi2                 <<std::endl; 
  std::cout<<" |   maxChi2Vtx            "<<   m_maxChi2                   <<std::endl;
  std::cout<<" |                                                      "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;


  return StatusCode::SUCCESS ;
} 
// ===========================================================================
// find seeds
// ===========================================================================

StatusCode LoKi::SeedFinderNTrks::makeJets 
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const 
{ 




  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  if(verts->size()!=1) return StatusCode::SUCCESS;
  LHCb::RecVertex::Container::const_iterator iv =verts->begin() ;

  const LHCb::RecVertex vtx = **iv ;

  makeJets(input_,  vtx, jets_ );

  return StatusCode::SUCCESS ;



}


StatusCode LoKi::SeedFinderNTrks::makeJets 
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
  //Kshort Mass5
  const double MK0=497.7;  


 
  std::vector<Gaudi::XYZVector> Slopes_flag;

  LHCb::Particle::ConstVector  PartIPK0Sub;
  LHCb::Particle::ConstVector  PartIP;
  LHCb::Particle::ConstVector  all;
  LHCb::Particle::ConstVector  KsDau;


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


  
  debug()  << "nb of ghost (LiH)             : "<< testLiHi << endmsg;
  debug()  << "nb of clone                   : "<< testClone << endmsg;
  debug()  << "nb of long                    : "<< testLong << endmsg;
  debug()  << "nb of charged                 : "<< testCharged << endmsg;
  debug()  << "nb of 'photon'                : "<< (int) all.size() - double(testCharged) << endmsg;
  debug()  << "Particle ALL size             : "<< input_.size() << endmsg;
  debug()  << "Particle ALL size (JetCone)   : "<< all.size() << endmsg;
  debug()  << "Particle INPUT for JET        : "<< PartIP.size() << endmsg;




  int cntK0 = 0;
  //=============================================================================
  // COMBINEPIPIMINK0: GET RIF OF K0S
  //============================================================================= 	
  for (  LHCb::Particle::ConstVector::const_iterator ip = PartIP.begin() ; 
	 PartIP.end() != ip ; ++ip )     {
    const LHCb::Particle* p = *ip ;
    bool testK = false;
    if(0){
   
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
    }
    if(testK) continue;    
    PartIPK0Sub.push_back(p);
  }
  
  debug()  << "nb K0                         : "<<  cntK0 << endmsg;
  debug()  << "Particle INPUT for JET no K0  : "<< PartIPK0Sub.size() << endmsg;



  if(PartIPK0Sub.size() <2){
    Warning ( "Not enough good part for seeding") ;
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
    debug()<<"No Beam line found at "<< "/Event/BeamLine" << endmsg;
    
    BL_P = Gaudi::XYZPoint( RecVert->position() );

    if( msgLevel(MSG::DEBUG) )
      debug()<<"Beam line position "<<  BL_P
	     <<" direction " << BL_M << endmsg;
  }
  

  //COMBIN PART



  // StatusCode twoTrkSeeding(LHCb::Particle::ConstVector PartIPK0Sub,LHCb::Particle::ConstVector NewPartIP,LHCb::VertexBase* RecVert,  std::vector<LHCb::VertexBase*>,    Gaudi::XYZPoint  BL_P,  Gaudi::LorentzVector BL_M, IJetMaker::Jets Seeds) const;
  bool seeding2trk = true;

  
  while(seeding2trk){
    
    
    
    Seeds.clear();
    debug()  << "Particle INPUT for JET no K0  b: "<< PartIPK0Sub.size() << endmsg;

    seeding2trk = twoTrkSeeding(PartIPK0Sub,RecVert,PVs,BL_P,BL_M,Seeds);
    
    
    debug()  << "Particle INPUT for JET no K0  a1: "<< PartIPK0Sub.size() << endmsg;
    
    if ( Seeds.empty() ) 
      break;
    
    
    
    
    LHCb::Particle::Vector::iterator iseed = Seeds.begin();
    LHCb::Particle* pSeed  = *iseed;
    
    bool addtrk = true;
    while(addtrk && seeding2trk)
      addtrk = addTrktoSeed(PartIPK0Sub,pSeed);
    
    debug()  << "Particle INPUT for JET no K0  a2: "<< PartIPK0Sub.size() << endmsg;

    
    SeedsPurged.push_back(pSeed);
    
  }
  
  
  debug()  << "SeedsPurged size: "<<  SeedsPurged.size() <<endmsg;


  int myj= 0;   
  for (   IJetMaker::Jets::iterator ijet =  SeedsPurged.begin() ;  SeedsPurged.end() != ijet ; ++ijet ) 
    {
      LHCb::Particle* pJet = *ijet;

      myj++;
      debug() << "SeedsP "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
      
      int myjd= 0;   
      
      LHCb::Particle::ConstVector daus = pJet->daughtersVector(); 
      for (   LHCb::Particle::ConstVector::iterator idau =  daus.begin() ;  daus.end() != idau ; ++idau ) {
	debug() <<         "        |dau"<<myjd<<": "<<  (*idau)->pt()<<endmsg;
	myjd++;
      }
      
      
      Gaudi::XYZVector seedd = Gaudi::XYZVector(
						Gaudi::XYZPoint(pJet->endVertex()->position()) -
						Gaudi::XYZPoint(pJet->referencePoint())
						);


      double dotprod = seedd.Unit().Dot(pJet->momentum().Vect().Unit());

       
      double a    =  std::fabs(std::acos (dotprod));


      debug() <<         "        |dirA: "<<   a<<endmsg;
      debug() <<         "        |Chi2: "<<   pJet->endVertex()->chi2()<<endmsg;
      debug() <<         "        |nDoF: "<<   pJet->endVertex()->nDoF()<<endmsg;
      debug() <<         "        |C/N : "<<   pJet->endVertex()->chi2PerDoF()<<endmsg;


      double combo1 = 1000*1000*(a * pJet->endVertex()->chi2PerDoF())/(pJet->daughtersVector().size()*pJet->pt()*pJet->pt());
      debug() <<         "        |COMB: "<<   combo1<<endmsg;


    }
  


  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
    { counter ( "#seeds" ) += SeedsPurged.size() ; }
  
  jets_ = SeedsPurged ;




  return StatusCode::SUCCESS ;
}




//=============================================================================
// trk seeding
//=============================================================================

bool LoKi::SeedFinderNTrks::twoTrkSeeding(LHCb::Particle::ConstVector &PartIPK0Sub, LHCb::VertexBase* RecVert,  std::vector<LHCb::VertexBase*> PVs,   Gaudi::XYZPoint  BL_P,  Gaudi::LorentzVector BL_M, IJetMaker::Jets & Seeds) const {

  StatusCode sc;



  LHCb::Particle::ConstVector::const_iterator jp, kp, ksp, lp;
  for ( jp =  PartIPK0Sub.begin(); jp != PartIPK0Sub.end(); jp++ ) { 
    for ( kp = jp+1; kp != PartIPK0Sub.end(); kp++ ) { 
      double dca12 = 999999.;
      double dcae12 = 999999.;
      double tof = 0;
      double tofe = 9999999.;
      sc = m_dist->distance((*jp), (*kp), dca12, dcae12);
      if(!sc) {  warning()  << "can't mesure the dist "<< endmsg;}
      if ( dca12 > m_DtrakMax) continue; // dca too large bwt the tracks
      
      Gaudi::LorentzVector sum = (*jp)->momentum() + (*kp)->momentum();	

        
      //2-seed
      LHCb::Particle              pSeed ;
      LHCb::Particle::ConstVector daughters ;
      LHCb::Vertex                vSeed ;
      LHCb::Vertex vtx;

      StatusCode scvtx = m_fitter->fit( vtx, **jp, **kp );
	
      if( scvtx.isFailure() ){warning()<< "VTX Fit failed"<< endmsg;continue;}

      if(vtx.chi2PerDoF() > m_SeedsMaxChi2DoF) continue;
      
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
      
      
      
      
      if (scvtx) mydz = vtx.position().z() -  RecVert->position().z();      
      if (mydz<0) continue;
      

	
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

  LHCb::Particle::ConstVector TmpPartIP;

  if(Seeds.size() == 0) return false;

  std::sort(Seeds.begin(), Seeds.end(),sortCHi2() );  

  LHCb::Particle::Vector::iterator iseed = Seeds.begin();
  LHCb::Particle* pSeed  = *iseed;
  LHCb::Particle::ConstVector 	daughtersVector = pSeed->daughtersVector();

  for ( jp =  PartIPK0Sub.begin(); jp != PartIPK0Sub.end(); jp++ ){
    bool used = false;
    for ( kp =  daughtersVector.begin(); kp != daughtersVector.end(); kp++ ){
      if((**jp).key() == (**kp).key())
	used = true;
    }


    if(!used)
      TmpPartIP.push_back(*jp);
  }
  



  PartIPK0Sub = TmpPartIP;

  return true;



}





//=============================================================================
// add track
//=============================================================================

bool LoKi::SeedFinderNTrks::addTrktoSeed(LHCb::Particle::ConstVector & NewPartIP,LHCb::Particle *Seed) const {
  
  
  StatusCode sc;
  typedef  std::map<double, std::pair< const LHCb::Particle* , LHCb::Vertex > > CHI2VTX;
   
  CHI2VTX  chi2Vtx;
  CHI2VTX::iterator iNN;
  LHCb::Particle::ConstVector TmpPartIP;

  LHCb::Vertex* vtx = Seed->endVertex();
  double  chi2ref = vtx->chi2PerDoF();
  debug()  << "Particle INPUT add: "<< NewPartIP.size() << endmsg;

  
  LHCb::Particle::ConstVector::const_iterator jp;
  
  for ( jp =  NewPartIP.begin(); jp != NewPartIP.end(); jp++ ) { 
    double dca12 = 999999.;
    double dcae12 = 999999.;

    sc = m_dist->distance((*jp),Seed->referencePoint(), dca12, dcae12);
    
    if(!sc) {  warning()  << "can't mesure the dist "<< endmsg;}
    if ( dca12 > m_DtrakMax2) {
      
      debug()  << "dca to big "<< dca12<<endmsg;
      
      continue; // dca too large bwt the tracks
      
    }
       
    
    LHCb::Vertex vtx2;

    LHCb::Particle::ConstVector Daus = Seed->daughtersVector();
    Daus.push_back((*jp));
    StatusCode scvtx = m_fitter->fit( vtx2, Daus.begin(),Daus.end());
    if( scvtx.isFailure() ) { Warning("Error in 'add' ", sc ) ; }
    
    if(abs(chi2ref - vtx2.chi2PerDoF() ) > m_deltaChi2 ||  vtx2.chi2PerDoF()  > m_maxChi2) {
      debug()  << "chi2PerDoF() to big "<< vtx2.chi2PerDoF()<<endmsg;
   
	continue;
    }

    
    std::pair< const LHCb::Particle* , LHCb::Vertex > tmpPair;
    tmpPair.first = *jp ;
    tmpPair.second = vtx2 ;
    
 
    chi2Vtx[vtx2.chi2PerDoF()] = tmpPair;

  }
  
  
  //  best chi2
  if(chi2Vtx.empty()) return false;

  iNN = chi2Vtx.begin();
  

  LHCb::Particle::ConstVector daus = 	Seed->daughtersVector();
  
  daus.push_back(((*iNN).second).first);

  LHCb::Vertex                vSeed ;


  Seed->setMomentum ( Seed->momentum()+(((*iNN).second).first)->momentum()) ;   
   
  sc = m_combiner->combine ( daus , *Seed , vSeed ) ;

  //save the trks as the daugthers
  Seed->setEndVertex ( (((*iNN).second).second).clone() ) ;
  
  for ( jp =  NewPartIP.begin(); jp != NewPartIP.end(); jp++ )
    if((*jp)->key() != ((*iNN).second).first->key())
      TmpPartIP.push_back(*jp);
  
  
  
  NewPartIP = TmpPartIP;

  return true;
  
}



//=============================================================================
// GETDELTAR: CALCULATES DELTAR
//=============================================================================
double LoKi::SeedFinderNTrks::getDeltaR(LHCb::Particle *p1, LHCb::Particle *p2) const {

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
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,SeedFinderNTrks)
// ============================================================================
// The END 
// ============================================================================
