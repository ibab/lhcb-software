// $Id: BTaggingChecker.cpp,v 1.6 2006-11-03 20:51:40 musy Exp $
// local
#include "BTaggingChecker.h"
#include "Kernel/StringUtils.h"

//--------------------------------------------------------------------------
// Implementation file for class : BTaggingChecker
//
// 2004-02-15 : Marco Musy
//--------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( BTaggingChecker );

//==========================================================================
BTaggingChecker::BTaggingChecker( const std::string& name,
				  ISvcLocator* pSvcLocator )
  : DVAlgorithm ( name , pSvcLocator ) {
  declareProperty("TagsLocation", 
		  m_tags_location = FlavourTagLocation::Default );
}

//==========================================================================
BTaggingChecker::~BTaggingChecker() {}; 

//==========================================================================
StatusCode BTaggingChecker::initialize() {

  m_debug = tool<IDebugTool> ( "DebugTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }

  nsele=0;
  for(int i=0; i<50; ++i) { nrt[i]=0; nwt[i]=0; }

  return StatusCode::SUCCESS;
};

//==========================================================================
// Main execution
//==========================================================================
StatusCode BTaggingChecker::execute() {
 
  setFilterPassed( false );

  ///////////////////////////////////////
  // Find the forced B
  MCParticle* B0 = findBForcedToDecay();
  ///////////////////////////////////////

  int tagdecision=0, ix=0;
  int truetag = B0->particleID().pid()>0 ? 1 : -1;

  //look in TagLocation
  FlavourTags* tags(0);
  if( exist<FlavourTags>( m_tags_location ) ) {
    tags = get<FlavourTags>( m_tags_location );
  } else {
    debug() << "No tags found in " << m_tags_location << endreq;
    return StatusCode::SUCCESS;
  }
  debug()<< tags->size()<<" tags found in "<<m_tags_location << endreq;

  FlavourTags::const_iterator ti;
  for( ti=tags->begin(); ti!=tags->end(); ti++ ) {

    tagdecision = (*ti)->decision();
    ix = (*ti)->category();

    info() << "BTAGGING MON "
	   << std::setw(3) << truetag
	   << std::setw(3) << tagdecision
	   << std::setw(3) << ix
	   << endreq;

    if( ! tagdecision ) continue;

    m_debug->printTree( (*ti)->taggedB() );
  }

  //count rights and wrongs
  nsele++;
  if     (tagdecision ==  truetag) nrt[ix]++;
  else if(tagdecision == -truetag) nwt[ix]++;

  setFilterPassed( true );
  return StatusCode::SUCCESS;
};

//==========================================================================
StatusCode BTaggingChecker::finalize(){ 

  MsgStream req( msgSvc(), name() );

  // calculate effective efficiency in categories with errors
  double rtt=0;
  double wtt=0;
  double ef_tot=0;
  double effe_tot=0;
  double epsilerr, epsilerrtot=0;

  info()<<"======================================================="<<endreq;
  info()<< std::setw(40)<< "Summary: EXCLUSIVE BTAGGING PERFORMANCE " <<endmsg; 
  info()<< "Summary : " <<endreq;
  info()<< " Category            EFF.          Etag         Wrong TF"
	<< "      r       w       "<<endreq;

  for( int it=1; it < 13; it++ ) {

    double rtag = nrt[it];               // right
    double wtag = nwt[it];               // wrong
    double utag = nsele-rtag-wtag;       // untagged

    if(rtag+wtag == 0) continue;         //empty category

    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;           // tagging efficiency
    double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
    if(rtag<wtag) epsil= -epsil;

    rtt      += rtag;
    wtt      += wtag;
    ef_tot   += eftag;
    effe_tot += epsil;

    //errors on efficiency and omega
    double eftag_err= sqrt((rtag*utag + utag*wtag)/nsele)/nsele;
    double omtag_err= sqrt( rtag*wtag /(rtag+wtag) ) / (rtag+wtag);

    epsilerr = sqrt((pow(rtag - wtag,2)*
                     (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
                      *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
                    /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
    epsilerrtot = sqrt(pow(epsilerrtot,2)+pow(epsilerr,2));

    req.setf(std::ios::fixed);
    info()<<std::setw(2)<< "  " << it << "     "
	  <<std::setprecision(2)<<std::setw(8)
	  << epsil*100 << "+-" << epsilerr*100
	  <<" "<<std::setw(8)<< eftag*100 << "+-" <<eftag_err*100
	  <<" "<<std::setprecision(1)<<std::setw(8)
	  << omtag*100 << "+-" <<omtag_err*100
	  <<" "<<std::setw(7)<< (int) rtag
	  <<" "<<std::setw(7)<< (int) wtag
	  << endreq;
  }

  //calculate global tagging performances -------------------------------

  //equivalent value of the wrong tag fraction
  double avw_invert=(1-sqrt(fabs(effe_tot)/ef_tot))/2;
  if(effe_tot<0) avw_invert= 1-avw_invert;
  double utt = nsele-rtt-wtt;
  double eftot_err= sqrt((rtt*utt + utt*wtt)/nsele)/nsele;
  double avw_invert_err= sqrt((rtt*wtt*(rtt*rtt + rtt*utt + wtt*(utt + wtt)))/
			      ( pow(rtt + wtt,4) *(rtt + utt + wtt)));

  info()<<"---------------------------------------------------------"<<endreq;
  info()<< "Total nr of events =  "<<std::setw(5) << nsele << endreq;
  info()<< "Tagging efficiency =  "
	<<std::setprecision(2)<<std::setw(5)
	<< ef_tot*100 << " +/- "<<eftot_err*100<< " %"<< endreq;
  info()<< "Wrong Tag fraction =  "
	<<std::setprecision(2)<<std::setw(5)
	<< avw_invert*100 << " +/- " <<avw_invert_err*100 << " %"<< endreq;
  info()<< "EFFECTIVE COMB. TE =  "
	<<std::setprecision(2)<<std::setw(5)
	<< effe_tot*100 << " +/- "<<epsilerrtot*100<< " %"<< endreq;
  info()<<"==========END OF EXCLUSIVE BTAGGING PERFORMANCE ======="<<endmsg;
  info()<<"========================================================="<<endreq;

  return StatusCode::SUCCESS; 
}
//==========================================================================
MCParticle* BTaggingChecker::findBForcedToDecay( void ) {

  MCParticle* mcSignal = 0;
  SmartDataPtr<HepMCEvents> hepVect(eventSvc(), HepMCEventLocation::Default);
  if ( ! hepVect ) return mcSignal;
  for( std::vector<LHCb::HepMCEvent*>::iterator q=hepVect->begin();
       q!=hepVect->end(); ++q ) {
    for ( HepMC::GenEvent::particle_iterator 
	    p  = (*q)->pGenEvt()->particles_begin();
	    p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( (*p)->status() != 889 ) continue;
      mcSignal = associatedofHEP(*p);
      if(mcSignal) break; 
    }
  }
  return mcSignal;
}
MCParticle* BTaggingChecker::associatedofHEP(HepMC::GenParticle* hepmcp) {

  SmartDataPtr<MCParticles> mcpart (eventSvc(), MCParticleLocation::Default );
  int mid = hepmcp->pdg_id();
  double mothmom   = hepmcp->momentum().vect().mag();
  double moththeta = hepmcp->momentum().vect().theta();
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( mid == (*imc)->particleID().pid() ) {
      if( fabs(mothmom - (*imc)->momentum().P())< 1.0){
	if( fabs(moththeta -(*imc)->momentum().Theta())< 0.0001){
	  return (*imc);
	}
      }
    }
  }
  return 0;
}
//==========================================================================
