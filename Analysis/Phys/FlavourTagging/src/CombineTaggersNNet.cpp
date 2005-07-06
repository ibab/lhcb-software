// $Id: CombineTaggersNNet.cpp,v 1.1 2005-07-06 00:36:02 musy Exp $
#include "CombineTaggersNNet.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineTaggersNNet v1.3
//
// 2005-07-05 : Marco Musy 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CombineTaggersNNet>          s_factory ;
const        IToolFactory& CombineTaggersNNetFactory = s_factory ; 

//=============================================================================
CombineTaggersNNet::CombineTaggersNNet( const std::string& type,
					const std::string& name,
					const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<ICombineTaggersTool>(this);

  declareProperty( "ProbMin", m_ProbMin = 0.52 );
}
CombineTaggersNNet::~CombineTaggersNNet(){}
StatusCode CombineTaggersNNet::initialize() { return StatusCode::SUCCESS; }
StatusCode CombineTaggersNNet::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
int CombineTaggersNNet::combineTaggers(FlavourTag& theTag, 
				       std::vector<Tagger*>& vtg ){

  //itag is now the individual B-flavour guess of each separate tagger:
  int catt=0;
  double tagdecision=0, pnsum=0.50;
  std::vector<int> itag;
  std::vector<double> pn;
  for( int j=0; j!=5; j++ ) itag.push_back(0);
  for( int j=0; j!=5; j++ ) itag.at(j) = (vtg.at(j))->decision();
 
  for( int j=0; j!=5; j++ ) pn.push_back(0.50);
  for( int j=0; j!=5; j++ ) pn.at(j) = 1-(vtg.at(j))->omega();
  
  //---------------------------
  //Make final tagging decision
  double pnsum_a= 0.50;             //hypothesis of truetag=+1
  double pnsum_b= 0.50;             //hypothesis of truetag=-1
  for( int i = 0; i != 5; i++ ) {   //multiply all probabilities
   double mtag = itag.at(i);
    pnsum_a *= ((1-mtag)/2+mtag* pn.at(i) ); // p
    pnsum_b *= ((1+mtag)/2-mtag* pn.at(i) ); //(1-p)
    debug()<<"i="<<i<<" itag="<<mtag<<" pn="<<pn.at(i)<<endreq;
  }
  if(pnsum_a > pnsum_b) tagdecision = +1;
  if(pnsum_a < pnsum_b) tagdecision = -1;
  //normalise probability to the only two possible flavours:
  pnsum = std::max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b);

  //throw away poorly significant tags
  if(pnsum < m_ProbMin) {
    pnsum = 0.50;
    tagdecision = 0;
  }
  debug() << "pnsum=" << pnsum <<endreq;
    
  //sort decision into categories
  //cat=1 will be least reliable, cat=5 most reliable
  //ProbMin is a small offset to adjust for range of pnsum
  double dpnsum = pnsum-m_ProbMin;
  if(tagdecision) catt = ( (int)(10.0*dpnsum/fabs(1.-dpnsum)) ) +1;
  if(catt>5) catt=5;
  if(catt<0) catt=0;

  ///fill FlavourTag object
  if(      tagdecision ==  1 ) theTag.setDecision( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag.setDecision( FlavourTag::b );
  else theTag.setDecision( FlavourTag::none );
  theTag.setCategory( catt );
  theTag.setOmega( 1-pnsum );
  //fill in taggers info into FlavourTag object
  for( int j=0; j!=5; j++ ) if(itag.at(j)) theTag.addTagger(*(vtg.at(j))); 

  return catt;
}
//=============================================================================
