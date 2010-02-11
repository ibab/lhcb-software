// $Id: BTaggingChecker.cpp,v 1.17 2010-02-11 18:39:15 musy Exp $
#include "BTaggingChecker.h"

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
  : DVAlgorithm ( name , pSvcLocator ),
    m_debug(0)
    , m_forcedBtool(0) 
    , m_bkg(0)
{
  declareProperty("TagsLocation", 
                  m_tags_location = FlavourTagLocation::Default );
  declareProperty( "RequireHltTrigger", m_requireHltTrigger = false );
}

//==========================================================================
BTaggingChecker::~BTaggingChecker() {}; 

//==========================================================================
StatusCode BTaggingChecker::initialize() {

  StatusCode sc = DVAlgorithm::initialize() ;

  m_debug = tool<IPrintDecayTreeTool> ( "PrintDecayTreeTool", this );
  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );

  nsele=0;
  for(int i=0; i<50; ++i) { nrt[i]=0; nwt[i]=0; }

  return sc ;
};

//==========================================================================
// Main execution
//==========================================================================
StatusCode BTaggingChecker::execute() {
 
  setFilterPassed( false );

  ////////////////////////////////////////////////////
  // Find the forced B
  const MCParticle* BS = m_forcedBtool->forcedB();
  ////////////////////////////////////////////////////

  if(!BS) {
    warning()<<"BTaggingChecker called but no B was forced to decay in MC"<<endreq;
    return StatusCode::SUCCESS;
  }
  
  int tagdecision=0, ix=0;
  int truetag = BS->particleID().pid()>0 ? 1 : -1;

  //look in TagLocation
  FlavourTags* tags(0);
  if( exist<FlavourTags>( m_tags_location ) ) {
    tags = get<FlavourTags>( m_tags_location );
  } else {
    debug() << "No tags found in " << m_tags_location << endreq;
    return StatusCode::SUCCESS;
  }
  debug()<< tags->size()<<" tags found in "<<m_tags_location << endreq;
  
  const Particle* P=0;
  int bkgcat = -1;
  double pmax=-1;
  FlavourTags::const_iterator ti;
  for( ti=tags->begin(); ti!=tags->end(); ti++ ) {

    if((*ti)->taggedB()->p()>pmax) {
      P = (*ti)->taggedB();
      pmax=(*ti)->taggedB()->p();
      tagdecision = (*ti)->decision();
      ix          = (*ti)->category();
    }
    
    if(msgLevel(MSG::DEBUG)) m_debug->printTree( (*ti)->taggedB() );
  }

  if(P) if(m_bkg) if( ! P->isBasicParticle() ){
    bkgcat = (int)(m_bkg->category( P ));
    debug() << "BackgroundCategory decision  "<< bkgcat << endreq;
  }
 
  //----------------------------------------------------------------------
  long L0Decision = 0;
  long HLT1Decision = 0;
  long HLT2Decision = 0;
  int trig=-1;
  if( exist<L0DUReport>(L0DUReportLocation::Default) ) {
    L0DUReport* l0 = get<L0DUReport>(L0DUReportLocation::Default);
    if(l0) L0Decision  = l0->decision();
    trig = L0Decision;
  }
  if (m_requireHltTrigger) {
    const HltDecReports* decReports =
      get<HltDecReports>(LHCb::HltDecReportsLocation::Default);
    if(decReports) {
      HLT1Decision = (int) decReports->decReport("Hlt1Global")->decision();
      HLT2Decision = (int) decReports->decReport("Hlt2Global")->decision();
      trig += HLT2Decision*100 + HLT1Decision*10;
    }
  }

  //----------------------------------------------------------------------
  info() << "BTAGGING MON "<< std::setw(3) << trig << std::setw(4) << truetag 
         << std::setw(4) << bkgcat << endreq;

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
  info()<< std::setw(40)<< "Summary: EXCLUSIVE TAGGING PERFORMANCE " <<endmsg; 
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
  double avw_invert_err= sqrt( rtt*wtt /(rtt+wtt) ) / (rtt+wtt);

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
  info()<<"==========END OF EXCLUSIVE TAGGING PERFORMANCE ======="<<endmsg;
  info()<<"========================================================="<<endreq;

  return DVAlgorithm::finalize() ; 
}

