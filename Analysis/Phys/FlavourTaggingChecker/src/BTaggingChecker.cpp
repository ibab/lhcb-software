// $Id: BTaggingChecker.cpp,v 1.18 2010-02-11 21:41:58 musy Exp $
#include "BTaggingChecker.h"

//--------------------------------------------------------------------------
// Implementation file for class : BTaggingChecker
//
// 2004-02-15 : Marco Musy
//--------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( BTaggingChecker )

//==========================================================================
BTaggingChecker::BTaggingChecker( const std::string& name,
                                  ISvcLocator* pSvcLocator )
: DaVinciAlgorithm ( name , pSvcLocator ),
  m_debug(0)
  , m_forcedBtool(0)
  , m_bkg(0)
{
  declareProperty("TagsLocation",
                  m_tags_location = FlavourTagLocation::Default );
  declareProperty( "RequireHltTrigger", m_requireHltTrigger = false );
}

//==========================================================================
BTaggingChecker::~BTaggingChecker() {}

//==========================================================================
StatusCode BTaggingChecker::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize() ;
  if ( sc.isFailure() ) return sc;

  m_debug = tool<IPrintDecayTreeTool> ( "PrintDecayTreeTool", this );
  //m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  m_forcedBtool = tool<IBDecayTool> ( "BDecayTool", this );
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );

  nsele=0;
  for(int i=0; i<50; ++i) { nrt[i]=0; nwt[i]=0; nrtag[i]=0; nwtag[i]=0; }

  return sc ;
}

//==========================================================================
// Main execution
//==========================================================================
StatusCode BTaggingChecker::execute()
{
  setFilterPassed( false );

  ////////////////////////////////////////////////////
  // Find the forced B
  const MCParticle* BS = m_forcedBtool->forcedB();
  ////////////////////////////////////////////////////

  if ( !BS )
  {
    return Warning( "Called but no B was forced to decay in MC",
                    StatusCode::SUCCESS );
  }

  int tagdecision=0, ix=0;
  int truetag = BS->particleID().pid()>0 ? 1 : -1;

  //look in TagLocation
  const FlavourTags * tags = getIfExists<FlavourTags>( m_tags_location );
  if ( !tags )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "No tags found in " << m_tags_location << endmsg;
    return StatusCode::SUCCESS;
  }
  if ( msgLevel(MSG::DEBUG) )
    debug()<< tags->size()<<" tags found in "<<m_tags_location << endmsg;

  const Particle* P = NULL;
  int bkgcat = -1;
  double pmax=-1;
  for( FlavourTags::const_iterator ti = tags->begin(); ti != tags->end(); ++ti )
  {

    if ( (*ti)->taggedB()->p() > pmax )
    {
      P = (*ti)->taggedB();
      pmax=(*ti)->taggedB()->p();
      tagdecision = (*ti)->decision();
      ix = 0;
      if ( !tagdecision ) continue;
      double omega=(*ti)->omega();
      if(      omega > 0.40   ) ix=1;
      else if( omega > 0.33   ) ix=2;
      else if( omega > 0.26   ) ix=3;
      else if( omega > 0.19   ) ix=4;
      else                      ix=5;
      const std::vector<Tagger>& mytaggers = (*ti)->taggers();

      for ( std::vector<Tagger>::const_iterator itag = mytaggers.begin();
            itag != mytaggers.end(); ++itag )
      {
        const int taggdec = itag->decision();
        /*
        if(taggdec==truetag) ++nrtag[(int) itag->type()];
        else                 ++nwtag[(int) itag->type()];
        */
        switch ( itag->type() )
        {
        case Tagger::OS_Muon    : (taggdec==truetag ? ++nrtag[1] : ++nwtag[1]); break;
        case Tagger::OS_Electron: (taggdec==truetag ? ++nrtag[2] : ++nwtag[2]); break;
        case Tagger::OS_Kaon    : (taggdec==truetag ? ++nrtag[3] : ++nwtag[3]); break;
        case Tagger::SS_Kaon    : (taggdec==truetag ? ++nrtag[4] : ++nwtag[4]); break;
        case Tagger::SS_Pion    : (taggdec==truetag ? ++nrtag[5] : ++nwtag[5]); break;
        case Tagger::VtxCharge  : (taggdec==truetag ? ++nrtag[6] : ++nwtag[6]); break;
        case Tagger::OS_nnetKaon: (taggdec==truetag ? ++nrtag[7] : ++nwtag[7]); break;
        case Tagger::SS_nnetKaon: (taggdec==truetag ? ++nrtag[8] : ++nwtag[8]); break;
        case Tagger::OS_Charm   : (taggdec==truetag ? ++nrtag[9] : ++nwtag[9]); break;
        case Tagger::SS_Proton  : (taggdec==truetag ? ++nrtag[10]: ++nwtag[10]); break;
        case Tagger::SS_PionBDT : (taggdec==truetag ? ++nrtag[11]: ++nwtag[11]); break;
        }
      }
      if(msgLevel(MSG::DEBUG)) m_debug->printTree( (*ti)->taggedB() );
    }

  }

  if ( P && m_bkg && !P->isBasicParticle() )
  {
    bkgcat = (int)(m_bkg->category( P ));
    if ( msgLevel(MSG::DEBUG) )
      debug() << "BackgroundCategory decision  "<< bkgcat << endmsg;
  }

  //----------------------------------------------------------------------
  long long L0Decision = 0;
  long long HLT1Decision = 0;
  long long HLT2Decision = 0;
  int trig = -1;
  L0DUReport* l0 = getIfExists<L0DUReport>(L0DUReportLocation::Default);
  if ( l0 )
  {
    L0Decision = l0->decision();
    trig = L0Decision;
  }
  if ( m_requireHltTrigger )
  {
    const HltDecReports* decReports =
      getIfExists<HltDecReports>(LHCb::HltDecReportsLocation::Default);
    if ( decReports )
    {
      HLT1Decision = (int) decReports->decReport("Hlt1Global")->decision();
      HLT2Decision = (int) decReports->decReport("Hlt2Global")->decision();
      trig += HLT2Decision*100 + HLT1Decision*10;
    }
  }

  //----------------------------------------------------------------------
  if ( msgLevel(MSG::DEBUG) )
    debug() << "BTAGGING MON "<< std::setw(3) << trig << std::setw(4) << truetag
            << std::setw(4) << bkgcat << endmsg;

  //count rights and wrongs in categories
  ++nsele;
  if     (tagdecision ==  truetag) ++nrt[ix];
  else if(tagdecision == -truetag) ++nwt[ix];

  setFilterPassed( true );
  return StatusCode::SUCCESS;
}

//==========================================================================
StatusCode BTaggingChecker::finalize()
{
  MsgStream req( msgSvc(), name() );
  req.setf(std::ios::fixed);

  // calculate effective efficiency in categories with errors
  double rtt(0), wtt(0), rtag(0), wtag(0), utag(0);
  double ef_tot(0), effe_tot(0);
  double epsilerr(0), epsilerrtot(0);

  info()<<"======================================================="<<endmsg;
  info()<< std::setw(40)<< "Summary: EXCLUSIVE TAGGING PERFORMANCE " <<endmsg;
  info()<< " Category            EFF.          Etag         Wrong TF"
        << "      r       w       "<<endmsg;

  for( int it=1; it < 24; ++it ) {
    rtag = wtag = 0;
    std::string cats;
    //  --- PID categories not in use ----
    //if(it== 1) cats =  "   mu only";
    //if(it== 2) cats =  "    e only";
    //if(it== 3) cats =  "    k only";
    //if(it== 4) cats =  "    mu + k";
    //if(it== 5) cats =  "     e + k";
    //if(it== 6) cats =  "  vtx only";
    //if(it== 7) cats =  "     ps/ks";
    //if(it== 8) cats =  "   mu + ks";
    //if(it== 9) cats =  "    e + ks";
    //if(it==10) cats =  "    k + ks";
    //if(it==11) cats =  "   mu+k+ks";
    //if(it==12) cats =  "    e+k+ks";

    if(it==13) { cats =  "    OS muon"; rtag = nrtag[1]; wtag = nwtag[1]; }
    if(it==14) { cats =  "    OS elec"; rtag = nrtag[2]; wtag = nwtag[2]; }
    if(it==15) { cats =  "    OS kaon"; rtag = nrtag[3]; wtag = nwtag[3]; }
    if(it==16) { cats =  "    SS kaon"; rtag=  nrtag[4]; wtag = nwtag[4]; }
    if(it==17) { cats =  "    SS pion"; rtag=  nrtag[5]; wtag = nwtag[5]; }
    if(it==18) { cats =  "   VertexCh"; rtag = nrtag[6]; wtag = nwtag[6]; }
    if(it==19) { cats =  "OS nnetkaon"; rtag = nrtag[7]; wtag = nwtag[7]; }
    if(it==20) { cats =  "SS nnetkaon"; rtag=  nrtag[8]; wtag = nwtag[8]; }
    if(it==21) { cats =  "   OS Charm"; rtag = nrtag[9]; wtag = nwtag[9]; }
    if(it==22) { cats =  "  SS Proton"; rtag=  nrtag[10];wtag = nwtag[10]; }
    if(it==23) { cats =  " SS pionBDT"; rtag=  nrtag[11];wtag = nwtag[11]; }
    if(it<13) cats =  "  Category ";
    else if(it==13)
      info()<<"---------------------------------------------------------"<<endmsg;

    if(it<13) { rtag = nrt[it]; wtag = nwt[it]; }

    if(rtag+wtag == 0) continue; //empty category

    utag = nsele-rtag-wtag;       // untagged
    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;                 // tagging efficiency
    double epsil = eftag*(1-2*omtag)*(1-2*omtag);     // effective efficiency
    //    if(rtag<wtag) epsil= -epsil;  // Who wrote this???

    if(it<13){
      rtt      += rtag;
      wtt      += wtag;
      ef_tot   += eftag;
      effe_tot += epsil;
    }

    //errors on efficiency and omega
    double eftag_err= sqrt((rtag*utag + utag*wtag)/nsele)/nsele;
    double omtag_err= sqrt( rtag*wtag /(rtag+wtag) ) / (rtag+wtag);
    double diff_tag = (rtag - wtag);
    double sum_tag  = (rtag + wtag);


    epsilerr = sqrt(( diff_tag*diff_tag*
                     (-(diff_tag*diff_tag*sum_tag)+nsele
                      *(rtag*rtag +14*rtag*wtag+ wtag*wtag)))
                    /(nsele*nsele*nsele*sum_tag*sum_tag*sum_tag));
    if(it<13) epsilerrtot = sqrt(epsilerrtot*epsilerrtot+epsilerr*epsilerr);

    //PRINT: ----------------------------------
    req.setf(std::ios::fixed);
    req.setf(std::ios::fixed);
    if(it<13) {
      info()<<std::setprecision(2)<<std::setw(8)<< it;
    } else info()<<" ** ";
    info()<< cats
          <<" "<<std::setw(8)<< epsil*100 << "+-" << epsilerr*100
          <<" "<<std::setw(8)<< eftag*100 << "+-" <<eftag_err*100
          <<" "<<std::setw(8)<< omtag*100 << "+-" <<omtag_err*100
          <<" "<<std::setw(7)<< (int) rtag
          <<" "<<std::setw(7)<< (int) wtag
          << endmsg;
  }




  //calculate global tagging performances -------------------------------

  //equivalent value of the wrong tag fraction
  double avw_invert=(1-sqrt(fabs(effe_tot)/ef_tot))/2;
  if(effe_tot<0) avw_invert= 1-avw_invert;
  double utt = nsele-rtt-wtt;
  double eftot_err= sqrt((rtt*utt + utt*wtt)/nsele)/nsele;
  double avw_invert_err= sqrt( rtt*wtt /(rtt+wtt) ) / (rtt+wtt);

  info() << "---------------------------------------------------------"<< endmsg;
  info() << "Tagging efficiency =  "<<std::setw(5)
         << ef_tot*100 << " +/- "<<eftot_err*100<< " %"<< endmsg;
  info() << "Wrong Tag fraction =  "<<std::setw(5)
         << avw_invert*100 << " +/- " <<avw_invert_err*100 << " %"<< endmsg;
  info() << "EFFECTIVE COMB. TE =  "<<std::setw(5)
         << effe_tot*100 << " +/- "<<epsilerrtot*100<< " %"
         << "     (Total events= "<<std::setw(5) << int(nsele) <<")"<< endmsg;
  info() << "=========================================================\n\n";

  return DaVinciAlgorithm::finalize() ;
}

