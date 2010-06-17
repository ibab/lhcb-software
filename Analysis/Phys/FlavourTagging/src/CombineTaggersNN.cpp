#include "CombineTaggersNN.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineTaggersNN v1.0
//
// 2010-06-10 : Marc Grabalosa 
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( CombineTaggersNN );

//=============================================================================
CombineTaggersNN::CombineTaggersNN( const std::string& type,
				    const std::string& name,
				    const IInterface* parent ):
  GaudiTool ( type, name, parent ) { 
  declareInterface<ICombineTaggersTool>(this);

  declareProperty( "OmegaMaxBin", m_omegamaxbin = 0.36 );
  declareProperty( "OmegaScale",  m_omegascale  = 0.06 );
  declareProperty( "ProbMin",     m_ProbMin     = 0.55 );
  declareProperty( "P0_NN",     m_P0_NN     = 0.502625 ); 
  declareProperty( "P1_NN",     m_P1_NN     = 0.504699 );
  declareProperty( "ProbMin_NN",     m_ProbMin_NN     = 0.55 );
  declareProperty( "WeightsFile", m_WeightsFile= "/afs/cern.ch/user/m/mgrabalo/TMVAClassification_MLP.weights.xml");
  declareProperty( "TmvaMethod",  m_TmvaMethod = "MLP method");
}

CombineTaggersNN::~CombineTaggersNN(){}

//=============================================================================
StatusCode CombineTaggersNN::initialize() { 

  //  m_reader_comb = new TMVA::Reader(0);
  info()<<"The weights file is at "<<m_WeightsFile<<endreq;
  m_reader_comb = new TMVA::Reader("!Color");
  m_reader_comb->AddVariable("pmu", &pmu);
  m_reader_comb->AddVariable("pe", &pe);
  m_reader_comb->AddVariable("pk", &pk);
  m_reader_comb->AddVariable("pss", &pss);
  m_reader_comb->AddVariable("pvtx", &pvtx);
  m_reader_comb->BookMVA( m_TmvaMethod, m_WeightsFile );
  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode CombineTaggersNN::finalize()   { 

  if(m_reader_comb) delete m_reader_comb;
  return StatusCode::SUCCESS; 
}

//=============================================================================
int CombineTaggersNN::combineTaggers(FlavourTag& theTag, 
				     std::vector<Tagger*>& vtg ){
  if( vtg.empty() ) return 0;

  info() << "#################Combine Taggers NN##############"<< endreq;
  double tagdecision=0;
  int vtgsize = vtg.size();
  int abstagsum = 0;
  pmu=pe=pk=pss=pvtx=0.5; //NN variables (.h)
  int mutag, etag, ktag, sstag, vtxtag;
  mutag=etag=ktag=sstag=vtxtag=0;
  double pnsum=0.5;
  double probNN=0;

  //obtain pn(omega) and tagdec for each tagger
  debug()<<"vtg size: "<<vtgsize<<endreq;
  for( int i = 0; i != vtgsize; i++ ) { //multiply all probabilities
    debug()<<"i: "<<i<<", vtgtype: "<<vtg.at(i)->type()<<", dec:"<<vtg.at(i)->decision()<<endreq;
    if (!vtg.at(i)->decision()) continue;
    if ((vtg.at(i))->type()==2) {
      pmu = 1-(vtg.at(i))->omega(); //probability of 'right' mu
      mutag = vtg.at(i)->decision(); 
      debug()<<"muon -> pmu: "<<pmu<<", mutag:"<<mutag<<endreq;
    }
    if ((vtg.at(i))->type()==3) {
      pe = 1-(vtg.at(i))->omega(); //probability of 'right' e
      etag = vtg.at(i)->decision(); 
      debug()<<"ele -> pe: "<<pe<<", etag:"<<etag<<endreq;
    }
    if ((vtg.at(i))->type()==4) {
      pk = 1-(vtg.at(i))->omega(); //probability of 'right' k
      ktag = vtg.at(i)->decision(); 
      debug()<<"kaon -> pk: "<<pk<<", ktag:"<<ktag<<endreq;
    }
    if ((vtg.at(i))->type()==5) {
      pss = 1-(vtg.at(i))->omega(); //probability of 'right' kaonSS
      sstag = vtg.at(i)->decision(); 
      debug()<<"kaonSS -> pss: "<<pss<<", sstag:"<<sstag<<endreq;
    }
    if ((vtg.at(i))->type()==6){
      pss = 1-(vtg.at(i))->omega(); //probability of 'right' pionSS
      sstag = vtg.at(i)->decision(); 
      debug()<<"pionSS -> pss: "<<pss<<", sstag:"<<sstag<<endreq;
    }
    if ((vtg.at(i))->type()==10){
      pvtx = 1-(vtg.at(i))->omega(); //probability of 'right' vtx
      vtxtag = vtg.at(i)->decision(); 
      debug()<<"pvtx: "<<pvtx<<", vtxtag:"<<vtxtag<<endreq;
    }
    abstagsum = abs(mutag)+abs(etag)+abs(ktag)+abs(sstag)+abs(vtxtag);
  }
  debug() << "abstagsum = " << abstagsum <<endreq;

  //only one tagger
  if (abstagsum==1){
    debug() <<"(pmu, pe, pk, pss, pvtx)*tag: "<<pmu*mutag<<" "<<pe*etag<<" "<<pk*ktag<<" "<<pss*sstag<<" "<<pvtx*vtxtag<<" "<<endreq;
    pnsum=pmu+pe+pk+pss+pvtx-2;//sum all taggers minus 2(4 taggers inactive)
    tagdecision=mutag+etag+ktag+sstag+vtxtag;
    info() << "pnsum: "<<pnsum<<", tagdecision: "<<tagdecision<<endreq;
    //throw away poorly significant tags
    if(pnsum < m_ProbMin) {
      pnsum = 0.50;
      tagdecision = 0;
    }
  }

  //more than one tagger
  if (abstagsum>1){
    debug() <<"(pmu, pe, pk, pss, pvtx)*tag: "<<pmu*mutag<<" "<<pe*etag<<" "<<pk*ktag<<" "<<pss*sstag<<" "<<pvtx*vtxtag<<" "<<endreq;
    //ch p[i] for tagdec=-1
    if (mutag<0) pmu=1-pmu;
    if (etag<0) pe=1-pe;
    if (ktag<0) pk=1-pk;
    if (sstag<0) pss=1-pss;
    if (vtxtag<0) pvtx=1-pvtx;
    debug() <<"(changed pmu, pe, pk, pss, pvtx)*tag: "<<pmu*mutag<<" "<<pe*etag<<" "<<pk*ktag<<" "<<pss*sstag<<" "<<pvtx*vtxtag<<" "<<endreq;
    probNN = m_reader_comb -> EvaluateMVA( m_TmvaMethod );
    double probPlus = m_P0_NN + m_P1_NN*(probNN);//tuning NN
    if (probPlus>1) probPlus=1;
    if (probPlus<0) probPlus=0;
    if (probNN>0.) {tagdecision = +1, pnsum=probPlus;}
    if (probNN<0.) {tagdecision = -1, pnsum=1-probPlus;}
    info() << "probNN: " << probNN <<", probPlus: " << probPlus <<", pnsum: "<<pnsum<<", tagdecision: "<<tagdecision<<endreq;
    //throw away poorly significant tags
    if(pnsum < m_ProbMin_NN) {
      pnsum = 0.50;
      tagdecision = 0;
    }
  }
  
  //sort decision into categories ------------------
  //cat=1 will be least reliable, cat=5 most reliable
  //ProbMin is a small offset to adjust for range of pnsum
  int category = 0;
  double omega = fabs(1-pnsum); 
  if(      omega > m_omegamaxbin                ) category=1;
  else if( omega > m_omegamaxbin-m_omegascale   ) category=2;
  else if( omega > m_omegamaxbin-m_omegascale*2 ) category=3;
  else if( omega > m_omegamaxbin-m_omegascale*3 ) category=4;
  else                                            category=5;
  if( !tagdecision ) category=0;

  //info
  info() << "Final 1-w = " << pnsum <<endreq;
  info() << "TagDecision = " << tagdecision <<endreq;
  info() << "Category = " << category <<endreq;
  info() << "Omega = " << omega <<endreq;

  ///fill FlavourTag object
  if(      tagdecision ==  1 ) theTag.setDecision( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag.setDecision( FlavourTag::b );
  else theTag.setDecision( FlavourTag::none );
  theTag.setCategory( category );
  theTag.setOmega( 1-pnsum );

  //fill in taggers info into FlavourTag object
  for(int j=0; j!=vtgsize; ++j) {
    int itag = vtg.at(j)->decision();
    if(itag) theTag.addTagger(*(vtg.at(j)));
  }

  return category;
}
//=============================================================================
