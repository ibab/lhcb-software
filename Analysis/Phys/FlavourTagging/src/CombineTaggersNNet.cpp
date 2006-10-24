// $Id: CombineTaggersNNet.cpp,v 1.3 2006-10-24 10:16:44 jpalac Exp $
#include "CombineTaggersNNet.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineTaggersNNet v1.3
//
// 2006-10-02 : Marco Musy 
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( CombineTaggersNNet );

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
StatusCode CombineTaggersNNet::finalize()   { return StatusCode::SUCCESS; }

//=============================================================================
int CombineTaggersNNet::combineTaggers(FlavourTag& theTag, 
				       std::vector<Tagger*>& vtg ){
  if( vtg.empty() ) return 0;

  int category=0;
  double tagdecision=0;
  double pnsum_a= 0.50;  //hypothesis of truetag=+1
  double pnsum_b= 0.50;  //hypothesis of truetag=-1
  int vtgsize = vtg.size();
  for( int i = 0; i != vtgsize; i++ ) { //multiply all probabilities
    double mtag = vtg.at(i)->decision();
    if(!mtag) continue;
    double pn   = 1-(vtg.at(i))->omega(); //probability of 'right'
    pnsum_a *= ((1-mtag)/2 + mtag*pn ); // p
    pnsum_b *= ((1+mtag)/2 - mtag*pn ); //(1-p)
    debug()<<"i="<<i<<" itag="<<mtag<<" pn="<<pn<<endreq;
  }
  if(pnsum_a > pnsum_b) tagdecision = +1;
  if(pnsum_a < pnsum_b) tagdecision = -1;
  //normalise probability to the only two possible flavours:
  double pnsum = std::max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b);

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
  if(tagdecision) category = ( (int)(10.0*dpnsum/fabs(1.-dpnsum)) ) +1;
  if(category>5) category=5;
  if(category<0) category=0;

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
