#include "TagJetsWithSeedFinder.h"
//#include "boost/algorithm/string/replace.hpp"




StatusCode  TagJetsWithSeedFinder::initialize ()
{
  
  
  StatusCode sc = DVAlgorithm::initialize(); 
  
  if ( sc.isFailure() ) return sc;
  cnttagjet=0;
  cntuntagjet=0;
  cntstagjet=0;

  cnttagseed=0;
  cntuntagseed=0;
  cntstagseed=0;

  evtcnt = 0;
  seedcnt=0;
  seedcntnZ = 0;
  jetcntnZ = 0;
  // stdstring m_inputLocations = "Phys/"+m_JetName+"/Particles";
  // location_tag =   m_inputLocations;
  // location_stag = m_inputLocations;
  // location_untag = m_inputLocations;
  // location_tag_s = m_inputLocations;
  // table_loc = m_inputLocations;

  std::string m_inputLocations = "Phys/"+m_JetName;
  location_tag =   m_inputLocations+"Tag/Particles";
  location_stag = m_inputLocations+"STag/Particles";
  location_untag = m_inputLocations+"UnTag/Particles";
  location_tag_s = m_inputLocations+"Tag/StdSeeds/Particles";
  table_loc = m_inputLocations+"Tag/Jets2SeedsRelations";



  //  boost::replace_last(location_tag,"/Particles","Tag/Particles");
  // boost::replace_last(location_stag,"/Particles","STag/Particles");
  // boost::replace_last(location_untag,"/Particles","UnTag/Particles");
  // boost::replace_last(location_tag_s,"/Particles","Tag/StdSeeds/Particles");
  // boost::replace_last(table_loc,"/Particles","Tag/Jets2SeedsRelations");



  return StatusCode::SUCCESS ;
} ;




StatusCode TagJetsWithSeedFinder::execute()
{ 
  LHCb::Particles*   seeds(0);
  // LHCb::Particles*   jets(0);

  
  //  m_seedfromjet =   m_seedfromjet_save ;
  

  LHCb::Particle::ConstVector jets = this->i_particles();  
  
  bool locseed = exist<LHCb::Particles>( m_seedLoc );
  if(!locseed && !m_seedfromjet){
    warning() << "Unable to find seed at '"
	      <<  m_seedLoc  << "'" << endreq;
    warning() << "The seed will be created with the jet daughters"<< endreq;
    setFilterPassed(true);
    return StatusCode::SUCCESS ;
  }else if(locseed){
    seeds = get<LHCb::Particles>( m_seedLoc );
    if( !seeds ){
      debug()<< "Unable to get seeds at '"
	     << m_seedLoc  << "'" << endreq;
      setFilterPassed(true);
      return StatusCode::SUCCESS ;
    }
  }
  
  
  
  
  evtcnt ++;
  
  if(!m_seedfromjet) seedcnt += (int)seeds->size();
  if(!m_seedfromjet)  if((int)seeds->size() != 0) seedcntnZ++;
  if((int)jets.size() != 0) jetcntnZ++;
  
  
  LHCb::Particles* TagJets = new LHCb::Particles();
  LHCb::Particles* UnTagJets = new LHCb::Particles();
  LHCb::Particles* STagJets = new LHCb::Particles();
  
  LHCb::Particles* TagSeeds = new LHCb::Particles();
  
  Table* table = new Table();
  
 
  
  put(TagJets, location_tag );
  if(!m_seedfromjet)   put(STagJets, location_stag );
  put(UnTagJets, location_untag );
  put(TagSeeds, location_tag_s );
  put ( table , table_loc ) ; 
  

  debug() << "writeOnTES ------TAGJETS--------------"<< endreq;
  debug() << "writeOnTES at "<< location_tag << endreq;
  if(!m_seedfromjet)    debug() << "writeOnTES ----SEMITAGJETS------------"<< endreq;
  if(!m_seedfromjet)     debug() << "writeOnTES at "<< location_stag << endreq;
  debug() << "writeOnTES ----UNTAGJETS--------------"<< endreq;
  debug() << "writeOnTES at "<< location_untag << endreq;
 
  
  
  LHCb::Particle::ConstVector::iterator ijet;

  
  if( m_seedfromjet ){
    //CREAT SEED WITH JETS DAU, TAG A JET IF A SEED IS FOUND WITH ITS DAUGHTERS
    
    debug() << "CREAT SEED WITH JETS DAU, TAG A JET IF A SEED IS FOUND WITH ITS DAUGHTERS"<< endreq;
    int jetnum = 0;
    for ( ijet = jets.begin(); jets.end() != ijet ;++ijet ) {
      jetnum++;
      const LHCb::Particle* i_jet = *ijet;
      LHCb::Particle* jet = i_jet->clone();

      LHCb::Particle::ConstVector jetDaus = jet->daughtersVector();

      IJetMaker::Jets Seeds;     
      //create seeds with the input particles
      if(!m_seedFinder){
	//select your prefered SeedFinder
	m_seedFinder = tool<IJetMaker>( m_seedFinderName, this );
	if(!m_seedFinder){
	  err() << "Unable to Retrieve Default SeedFinder" << endreq;
	  return StatusCode::FAILURE;
	}
	//	Assert ( m_seedFinder->parent() == this , "It is not a private tool!" ) ;
	//	bool fitVtx = true;
	//	StatusCode sc = Gaudi::Utils::setProperty ( m_seedFinder , "_fitVtxFromInputTracks" ,fitVtx ) ; 
	//	Assert ( sc.isSuccess() , "Unbale to set property 'MyOption'" ) ; 
      }
      //tag,seed
      if(jet->referencePoint() !=  Gaudi::XYZPoint(0,0,0)){
	LHCb::RecVertex jet_vert_ref;
	jet_vert_ref.setPosition(jet->referencePoint());
	m_seedFinder->makeJets(jetDaus, jet_vert_ref, Seeds);   
      }else{
	m_seedFinder->makeJets(jetDaus, Seeds);   
      }

      if(Seeds.size() > 0 ){
	debug() << "TAGJETS"<< endreq;
	TagJets->insert(jet);
	cnttagjet++;
	cnttagseed+=Seeds.size() ;

	for(IJetMaker::Jets::iterator iseed = Seeds.begin(); iseed!=Seeds.end(); iseed++){
	  TagSeeds->insert(*iseed);
	  table->relate(jet,*iseed,1);
	}
	
      }else{
	debug() << "UNTAGJETS"<< endreq;
	UnTagJets->insert(jet);
	cntuntagjet++;
      }
    }   
  }else{    

    LHCb::Particles::iterator iseed;
    LHCb::Particle::ConstVector::iterator ijetdau;
    LHCb::Particle::ConstVector::iterator  iseeddau;    
    
    for ( ijet = jets.begin(); jets.end() != ijet ;++ijet ) {
      bool tagjet = false;
      bool stagjet = false;
      int testDau;
      const LHCb::Particle* i_jet = *ijet;
      LHCb::Particle* jet = i_jet->clone();
      LHCb::Particle::ConstVector jetDaus;
      jetDaus.insert(jetDaus.end(), jet->daughters().begin(),jet->daughters().end()) ;
      if(jetDaus.size()==0) continue;      
      for ( iseed = seeds->begin(); seeds->end() != iseed ;++iseed ) {
	testDau = 0;
	LHCb::Particle* seed = *iseed;

	if(seed->referencePoint() != jet->referencePoint() ) continue;

	LHCb::Particle::ConstVector seedDaus;
	seedDaus.insert(seedDaus.end(), seed->daughters().begin(),seed->daughters().end()) ;	
	if(seedDaus.size()==0) continue;	
	for ( iseeddau = seedDaus.begin(); seedDaus.end() != iseeddau ;++iseeddau ){
	  const  LHCb::Particle* seedd = *iseeddau;	  
	  for ( ijetdau = jetDaus.begin(); jetDaus.end() != ijetdau ;++ijetdau ){	    
	    const  LHCb::Particle* jetd = *ijetdau;	  	    
	    if( seedd->proto()->track() != jetd->proto()->track() ) continue;
	    
	    testDau++;	  
	    
	    if(testDau == (int) seedDaus.size()){
	      tagjet = true; 
	      break;
	    }
	    
	    if((double)testDau >= (double)seedDaus.size()/2. )
	      stagjet = true;
	    
	  }//dau jet
	  if(tagjet){
	    cnttagseed++;
	    break;}
	}//dau seed
      	table->relate(jet,seed,(double)testDau/(double)seedDaus.size());
	
      }//seed  
      if(tagjet) {
	debug() << "TAGJETS"<< endreq;
	TagJets->insert(jet);
	cnttagjet++;
      }else if(stagjet){
	debug() << "SEMITAGJETS"<< endreq;
	STagJets->insert(jet);
	cntstagjet++;
      }else{
	debug() << "UNTAGJETS"<< endreq;
	UnTagJets->insert(jet);
	cntuntagjet++;
      }
    }//jet
  }
  
  setFilterPassed(true);
  return StatusCode::SUCCESS ;
}



//=============================================================================
// Finalization
//=============================================================================
StatusCode TagJetsWithSeedFinder::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  info()<< " " <<endmsg;
  info()<< "=========== JET TAGING COUNTERS ===========" <<endmsg;
  info()<< " " <<endmsg;
  info()<< "                  nb of evts: "<< evtcnt <<endmsg;
  info()<< "   nb of evt w jet.size  > 0: "<<jetcntnZ<<endmsg;
  if(!m_seedfromjet)  info()<< "   nb of evt w seed.size > 0: "<<seedcntnZ<<endmsg;
  info()<< "                  nb of jets: "<< cntuntagjet+cntstagjet+ cnttagjet <<endmsg;
  if(!m_seedfromjet)  info()<< "                 nb of seeds: "<< seedcnt <<endmsg;
  info()<< "   mutli of jets (evt/w.1.j): "<< double(cntuntagjet+ cnttagjet+ cntstagjet)/(double)evtcnt <<" / "<<double(cntuntagjet+cnttagjet+ cntstagjet)/((double) jetcntnZ)<<endmsg;
  if(!m_seedfromjet)  info()<< " mutli of seeds (evt/ w.1.s): "<< double(seedcnt)/(double)evtcnt <<" / "<<(double)seedcnt/(double)seedcntnZ <<endmsg;

  info()<< "              nb of tag jets: "<< double(cnttagjet) <<endmsg;
  info()<< "            part of tag jets: "<< double(cnttagjet)/(double)(cntuntagjet+ cnttagjet+cntstagjet) <<endmsg;
  info()<<  "  mutli of seeds per tag jet: "<< double(cnttagseed)/(double)cnttagjet <<endmsg;

  if(!m_seedfromjet)  info()<< "          nb of semitag jets: "<< double(cntstagjet) <<endmsg;
  if(!m_seedfromjet)  info()<< "        part of semitag jets: "<< double(cntstagjet)/(double)(cntuntagjet+ cnttagjet+cntstagjet) <<endmsg;
  info()<< "            nb of untag jets: "<< double(cntuntagjet)<<endmsg;
  info()<< "          part of untag jets: "<< double(cntuntagjet)/(double)(cntuntagjet+ cnttagjet+cntstagjet) <<endmsg;
  info()<< " " <<endmsg;
  info()<< "========= END JET TAGING COUNTERS ========" <<endmsg;
  info()<< " " <<endmsg;

  return StatusCode::SUCCESS;

}

