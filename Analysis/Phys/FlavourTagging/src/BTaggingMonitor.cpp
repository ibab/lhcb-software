// $Id: BTaggingMonitor.cpp,v 1.3 2005-01-31 10:07:46 pkoppenb Exp $
// local
#include "BTaggingMonitor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BTaggingMonitor
//
// 2004-02-15 : Marco Musy
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<BTaggingMonitor>          s_factory ;
const        IAlgFactory& BTaggingMonitorFactory = s_factory ; 

//=============================================================================
BTaggingMonitor::BTaggingMonitor( const std::string& name,
				  ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ), m_tags_locations(0)
{
  declareProperty("TagsLocations", m_tags_locations);
}

//=============================================================================
BTaggingMonitor::~BTaggingMonitor() {}; 

//=============================================================================
StatusCode BTaggingMonitor::initialize() {

  m_pAsctLinks = tool<Particle2MCLinksAsct::IAsct>
                          ( "Particle2MCLinksAsct", "TagMonitor", this );
  if(0 == m_pAsctLinks) {
    fatal() << "Unable to retrieve Link Associator tool"<<endreq;
    return StatusCode::FAILURE;
  }

  m_debug = tool<IDebugTool> ( "DebugTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }

  nsele=0;
  for(int i=0; i<50; ++i) { nrt[i]=0; nwt[i]=0; }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTaggingMonitor::execute() {

  if( !filterPassed() ) {
    debug() << "Event was not selected. "<< endreq;
    setFilterPassed( false );
    return StatusCode::SUCCESS;
  }
  setFilterPassed( true );
 
  // Retrieve informations about event
  EventHeader* evt = get<EventHeader> (EventHeaderLocation::Default);
  if ( !evt ) {
    err() << "Unable to Retrieve Event" << endreq;
    return StatusCode::FAILURE;
  }

  //choose the forced B
  MCParticle* B0 = 0;
  GenMCLinks* sigL = get<GenMCLinks> (GenMCLinkLocation::Default);
  if( !sigL ) {
    err() << "Unable to Retrieve GenMCLinks" << endreq;
    return StatusCode::FAILURE; 
  }
  B0 = (*(sigL->begin()))->signal();
  if(!B0) {
    err() << "No signal B in GenMCLinks" << endreq;
    return StatusCode::FAILURE; 
  }

  int tagdecision=0, ix=0;
  int truetag = B0->particleID().pid()>0 ? 1 : -1;
  Particle* tagB = 0;

  std::vector<std::string>::const_iterator loc_i;
  for(loc_i=m_tags_locations.begin(); loc_i!=m_tags_locations.end(); loc_i++){
    debug() << "Monitoring location: "<< (*loc_i)+"/Tags" << endreq;
    SmartDataPtr<FlavourTags> tags(eventSvc(), (*loc_i)+"/Tags");
    if( !tags ) {
      debug() << "No tags found. " << endreq;
      continue;
    }

    FlavourTags::const_iterator ti;
    for( ti=tags->begin(); ti!=tags->end(); ti++ ) {

      tagB = (*ti)->taggedB();
      tagdecision = (*ti)->decision();
      ix = (*ti)->category();
      info() << "BTAGGING MON "
	     << std::setw(3) << tagdecision
	     << std::setw(3) << ix 
	     << std::setw(3) << truetag
	     << endreq;

     if( tagB ) debug() << "taggedB P="<< tagB->p()/GeV <<endreq;
     m_debug->printTree( (*ti)->taggedB() );
    }

    debug() << "Monitoring location: "<<(*loc_i)+"/Taggers" <<endreq;
    SmartDataPtr<Particles> tagCands( eventSvc(), (*loc_i)+"/Taggers" );
    if( !tagCands ) {
      debug() << "No taggers found. " << endreq;
      continue;
    }
    debug() << "NR Taggers found= " << tagCands->size() << endreq;

    Particles::const_iterator ip;
    for( ip = tagCands->begin(); ip != tagCands->end(); ip++ ){
      debug() << "tagger ID: " << (*ip)->particleID().pid()
	      << "  P=" << (*ip)->p()/GeV << endreq;
    }
  }
  
  if( tagB ) {
    nsele++;
    if     (tagdecision ==  truetag) nrt[ix]++;
    else if(tagdecision == -truetag) nwt[ix]++;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
StatusCode BTaggingMonitor::finalize(){ 

  MsgStream req( msgSvc(), name() );

  // calculate effective efficiency in categories with errors
  double rtt=0;
  double wtt=0;
  double ef_tot=0;
  double effe_tot=0;
  double epsilerr, epsilerrtot=0;

  info() << "======================================================="<<endreq;
  info() << "Summary : " <<endreq;
  info() << " Category            EFF.          Etag         Wrong TF"
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

  info()<< "---------------------------------------------------------"<<endreq;
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
  info()<< "========================================================="<<endreq;

  return StatusCode::SUCCESS; 
}
//=============================================================================

