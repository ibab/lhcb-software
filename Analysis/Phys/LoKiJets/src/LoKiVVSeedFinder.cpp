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

  m_dist       = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
  if ( !m_dist ) {
    err() << "Unable to Retrieve LoKi::DistanceCalculator" << endreq;
    return StatusCode::FAILURE;
  }  
  
  m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ;
  if ( !m_combiner) {
    err() << "Unable to Retrieve Default  ParticleCombiner" << endreq;
    return StatusCode::FAILURE;
  }
  
  m_fitter     = tool<IVertexFit>("OfflineVertexFitter");
  if ( !m_fitter ) {
    err() << "Unable to Retrieve Default VertexFitter" << endreq;
    return StatusCode::FAILURE;
  }


  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |     \\  /\\  /                                       | "<<std::endl;
  std::cout<<" |      \\/  \\/SeedsFinder Tool for Phys Particles     | "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |       \\  /                                         | "<<std::endl;
  std::cout<<" |        \\/       EPFLausanne - 2009-11              | "<<std::endl;
  std::cout<<" |         |        A. Bay & C. Potterat              | "<<std::endl;
  std::cout<<" |         |                                          | "<<std::endl;
  std::cout<<" |                                       _     _      | "<<std::endl;
  std::cout<<" |                                  |   |_||_||_      | "<<std::endl;
  std::cout<<" |                                  |__ |  | ||__     | "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" |                                                     "<<std::endl;
  std::cout<<" |   VVSeedsFinder parameters: "<<std::endl;
  std::cout<<" |   SeedID            "<<   m_seedID      <<std::endl;	
  std::cout<<" |   SeedTriplets      "<<   m_Triplets        <<std::endl;	
  std::cout<<" |   SeedRParameter    "<<   m_r            <<std::endl;
  std::cout<<" |   PtTrackMin        "<<   m_PtTrackMin      <<std::endl;
  std::cout<<" |   PTrackMin         "<<   m_PTrackMin      <<std::endl;
  std::cout<<" |   SeedTrkChi2PerDoF "<<   m_TrkChi2DoF     <<std::endl;
  std::cout<<" |   IPmin             "<<   m_IPmin       <<std::endl;
  std::cout<<" |   Signif            "<<   m_Signif          <<std::endl;  	
  std::cout<<" |   DMK0              "<<   m_DMK0            <<std::endl;   	
  std::cout<<" |   TseedVtxMin       "<<   m_TseedVtxMin     <<std::endl; 	
  std::cout<<" |   TseedVtxMax       "<<   m_TseedVtxMax     <<std::endl;	
  std::cout<<" |   DtrakMax          "<<   m_DtrakMax        <<std::endl;     	
  std::cout<<" |   DeltaRSeeds       "<<   m_DeltaRSeeds     <<std::endl;	
  std::cout<<" |   PtSeedsMin        "<<   m_PtSeedsMin      <<std::endl;  	
  std::cout<<" |   PtMergedSeedsMin  "<<   m_PtMergedSeedsMin      <<std::endl;  
  std::cout<<" |                                                      "<<std::endl;
  std::cout<<" |                                                    | "<<std::endl;
  std::cout<<" ------------------------------------------------------ "<<std::endl;



  return StatusCode::SUCCESS ;
} 
// ===========================================================================
// find seeds
// ===========================================================================
StatusCode LoKi::VVSeedFinder::makeJets 
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const 
{

  //Select evt with only 1 PV !
  const LHCb::RecVertex::Container* verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);
  if(verts->size()!=1) return StatusCode::SUCCESS;
  LHCb::RecVertex::Container::const_iterator iv =verts->begin() ;
  LHCb::VertexBase* RecVert = (*iv)->clone();

  IJetMaker::Jets Seeds; 
  double ipl, chi2l;
  StatusCode sc;
  LHCb::Vertex vtx, vseedbest;
  double dca12, dcae12;
  //Kshort Mass
  const double MK0=497.7;  
  
  std::vector<Gaudi::XYZVector> Slopes_flag;


  LHCb::Particle::ConstVector  PartIP;
  LHCb::Particle::ConstVector all;
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
	verbose()  << "addinfo Likelihood = " << TrkLi << endmsg;
	verbose()  << "addinfo CloneDist  = " << CloneDist<< endmsg;
	verbose()  << "addinfo GhostProb  = " << GhostProb << endmsg;
	verbose()  << "addinfo Longb      = " << LongTrk << endmsg;
	verbose()  << "p Trk   = " <<  trk->momentum() / Gaudi::Units::GeV << endmsg;    
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
  
  
  
  debug()  << "nb of ghost (LiH)             : "<< testLiHi << endmsg;
  debug()  << "nb of clone                   : "<< testClone << endmsg;
  debug()  << "nb of long                    : "<< testLong << endmsg;
  debug()  << "nb of charged                 : "<< testCharged << endmsg;
  debug()  << "nb of 'photon'                : "<< (int) all.size() - double(testCharged) << endmsg;
  debug()  << "Particle ALL size             : "<< input_.size() << endmsg;
  debug()  << "Particle ALL size (JetCone)   : "<< all.size() << endmsg;
  debug()  << "Particle INPUT for JET        : "<< PartIP.size() << endmsg;


  int cntK0 = 0;


  LHCb::Particle::ConstVector::const_iterator jp, kp, ksp, lp;
  for ( jp =  PartIP.begin(); jp != PartIP.end(); jp++ ) { 
    if(KsDau.size() != 0)   
      for(ksp = KsDau.begin(); ksp!= KsDau.end(); ksp++)
	if(*ksp == *jp)	  continue;
    for ( kp = (jp+1) ; kp != PartIP.end(); kp++ ) {
      if(KsDau.size() != 0)     
	for(ksp = KsDau.begin(); ksp!= KsDau.end(); ksp++)
	  if(*ksp == *kp)    continue;
      
      sc = m_dist->distance((*jp), (*kp), dca12, dcae12);
      if(!sc) {  warning()  << "can't mesure the dist "<< endmsg;}
      if ( dca12 > m_DtrakMax) continue; // dca too large bwt the tracks

      Gaudi::LorentzVector sum = (*jp)->momentum() + (*kp)->momentum();	

      //=============================================================================
      // COMBINEPIPIMINK0: GET RIF OF K0S
      //============================================================================= 	
      if((*jp)->particleID().abspid() == 211
	 &&  (*kp)->particleID().abspid() == 211
	 && ((*jp)->charge()) * ((*kp)->charge()) < 0 ) 
	if ( fabs(sum.M()-MK0)<m_DMK0 ) {
	  KsDau.push_back(*kp);
	  KsDau.push_back(*jp);
	  cntK0++;
	  continue; 
	}      
      

      if(!m_Triplets || (int) PartIP.size()>80 ){
	//2-tracks Seed otpion (if 3-trk is requested but more than 80 trk can be used -> 2-trk)
	StatusCode scvtx = m_fitter->fit( vtx, **jp, **kp );      
	if( scvtx.isFailure() ){warning()<< "VTX Fit failed"<< endmsg;continue;}
	
	double mydz= -1;
	if (scvtx) mydz = vtx.position().z() -  RecVert->position().z();      
	if (mydz<0) continue;
	// the vtx is before the PV

	Gaudi::XYZPoint  dv =   Gaudi::XYZPoint(vtx.position() - RecVert->position());
	double dr = sqrt(dv.x()*dv.x() + dv.y()*dv.y());
	if (dr < m_DRmin || dr > m_DRmax) continue;
	// the vtx is too close or to far from the beam direction

	
	//---------------------------
	//Seed level cuts -----------
	//---------------------------
	LHCb::VertexBase* PPvtx2 = vtx.clone();   
	m_dist->distance(PPvtx2,RecVert, tof, tofe);     
	if (tof< m_TseedVtxMin || tof> m_TseedVtxMax) continue;  
	// the vtx is too close or to far from the PV (~time of flight)
	if(sum.Pt() <   m_PtSeedsMin) continue;
	//Pt too soft
	
	
	LHCb::Particle              pSeed ;
	LHCb::Particle::ConstVector daughters ;
	LHCb::Vertex                vSeed      ;
	daughters.push_back ( *jp );
	daughters.push_back ( *kp );
	
	pSeed.setParticleID     (LHCb::ParticleID( m_seedID )) ;      
	pSeed.setReferencePoint ( vtx.position() ) ;    
	pSeed.setMomentum ( sum  ) ;             
	StatusCode sc = m_combiner->combine ( daughters , pSeed , vSeed ) ;
	//save the trks as the daugthers

	if ( sc.isFailure())   Error ( "Error from momentum combiner, skip" , sc ) ;      
	Seeds.push_back(pSeed.clone());
	
      }else{
	//3-tracks Seed otpion (if 3-trk is requested but more than 80 trk can be used -> 2-trk)
	for ( lp = (kp+1) ; lp != PartIP.end(); lp++ ) {
	  if(KsDau.size() != 0)     
	    for(ksp = KsDau.begin(); ksp!= KsDau.end(); ksp++)
	      if(*ksp == *lp)    continue;
	  
	  double dca13, dcae13;
	  m_dist->distance((*jp), (*lp), dca13, dcae13);
	  if ( dca13> m_DtrakMax) continue;  
	  // dca too large btw the trks
	  
	  double dca23, dcae23;
	  m_dist->distance((*lp), (*kp), dca23, dcae23);
	  if ( dca23> m_DtrakMax) continue;     
	  // dca too large btw the trks
	  

	  
	  StatusCode scvtx = m_fitter->fit( vtx, **jp, **kp, **lp );      
	  if( scvtx.isFailure() ){warning()<< "VTX Fit failed"<< endmsg;continue;}

	  double mydz= -1;
	  if (scvtx) mydz = vtx.position().z() -  RecVert->position().z();      
	  if (mydz<0) continue;
	  // the vtx is before the PV
	  Gaudi::XYZPoint  dv =   Gaudi::XYZPoint(vtx.position() - RecVert->position());
	  double dr = sqrt(dv.x()*dv.x() + dv.y()*dv.y());
	  if (dr < m_DRmin || dr > m_DRmax) continue;
	  // the vtx is too close or to far from the beam direction

	  
	  //---------------------------
	  //Seed level cuts -----------
	  //---------------------------
	  LHCb::VertexBase* PPvtx2 = vtx.clone();   
	  m_dist->distance(PPvtx2,RecVert, tof, tofe);     
	  if (tof< m_TseedVtxMin || tof> m_TseedVtxMax) continue;  
	  // the vtx is too close or to far from the PV (~time of flight)

	  Gaudi::LorentzVector sum2 = sum + (*lp)->momentum();
	  if(sum2.Pt() <   m_PtSeedsMin) continue;
	  //Pt too soft
	  
      	  LHCb::Particle              pSeed ;
	  LHCb::Particle::ConstVector daughters ;
	  LHCb::Vertex                vSeed      ;
	  daughters.push_back ( *jp );
	  daughters.push_back ( *kp );
	  daughters.push_back ( *lp );	
	  pSeed.setParticleID     (LHCb::ParticleID( m_seedID )) ;      
	  pSeed.setReferencePoint ( vtx.position() ) ;    
	  pSeed.setMomentum ( sum  ) ;             
	  StatusCode sc = m_combiner->combine ( daughters , pSeed , vSeed ) ;
	  //save the trks as the daugthers
	  if ( sc.isFailure())   Error ( "Error from momentum combiner, skip" , sc ) ;      
	  Seeds.push_back(pSeed.clone());
	}
      }
    }//end loop kp
  }//end loop jp


  if ( Seeds.empty() ) {
    Warning ( "No Seeds from LoKiVVSeedFinder") ;
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
      debug() << "Seeds  "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
  
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
      debug() << "BSeeds "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
  
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
      debug() << "CSeeds "<<myj<<"| nb of combin Seeds:  "<<  pJet->weight()<< "  nb part in the jet:  " << pJet->daughtersVector().size() <<"  Pt:  "<< pJet->pt() << endmsg;
      debug() <<         "        | m  : "<<  pJet->momentum() << endmsg;
      debug() <<         "        | eta: "<<  pJet->momentum().eta() << "  / phi: " <<   pJet->momentum().phi() <<endmsg;
  
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
    if ( sc.isFailure())   Error ( "Error from momentum combiner, skip" , sc ) ;
 

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
  
  debug()  <<"size of  noFiltered ProtoJets           = " <<  ProtoJets.size()  << endmsg;
  
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
	    
	    Gaudi::LorentzVector v1 = seed->momentum();
	    Gaudi::LorentzVector v2 = seed2->momentum();
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
  
  
  debug()  <<"size of preFiltered ProtoJets           = " <<  ProtoJets.size()  << endmsg;
  
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
	  Gaudi::LorentzVector m1 = seed->momentum();	  
	  for ( IJetMaker::Jets::iterator ip2 = ip+1; ip2 != ProtoJets.end(); ++ip2 ){	
	    j++;
	    LHCb::Particle* seed2 = (*ip2);
	    
	    if(seed2->momentum().E() >0.){	  
	      Gaudi::LorentzVector m2 = seed2->momentum();
	      
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
	verbose() <<" FilterProtoJet merge " << endmsg;
	verbose() << "Nseed phi/eta= "<< (ProtoJets.at(J1test)->momentum()).phi() <<" " << (ProtoJets.at(J1test)->momentum()).eta() << endmsg;
	verbose() << "Nseed2phi/eta= "<< (ProtoJets.at(J2test)->momentum()).phi() <<" " << (ProtoJets.at(J2test)->momentum()).eta() << endmsg;
	verbose() << "E P1= "<< (ProtoJets.at(J1test)->momentum()).E() <<"   E P2= " << (ProtoJets.at(J2test)->momentum()).E() << endmsg;
	verbose() << "E P1/P2= "<< fabs((ProtoJets.at(J1test)->momentum()).E()-(ProtoJets.at(J2test)->momentum()).E())/(ProtoJets.at(J1test)->momentum()).E()  << endmsg;	
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
      
      
      
      
      verbose()<<" ProtoJet Size         : " << mySize<<endmsg;
      verbose()<<" ProtoJet Size         : " << ProtoJets.size()<<endmsg;
      verbose()<<" NewFilterProtoJet Size: " << NewProtoJets.size()<<endmsg;
      
      if( (int)NewProtoJets.size() !=  mySize ) testJets = true;
      
      
      ProtoJets.clear();
      ProtoJets = NewProtoJets;
      
      
      if(NewProtoJets.size() % 10 == 0)
	verbose() << "Number of filtered ProtoJets is " <<  NewProtoJets.size() << endmsg;  
      
    }
    
    debug()  <<"size of    Filtered ProtoJets           = " <<  NewProtoJets.size()  << endmsg;
    
    
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



// ============================================================================
/// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,VVSeedFinder)
// ============================================================================
// The END 
// ============================================================================
