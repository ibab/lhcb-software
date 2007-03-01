#include "CombineTaggersProbability.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineTaggersProbability v1.3
//
// 2006-10-02 : Marco Musy 
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( CombineTaggersProbability );

//=============================================================================
CombineTaggersProbability::CombineTaggersProbability( const std::string& type,
						      const std::string& name,
						      const IInterface* parent ):
  GaudiTool ( type, name, parent ) { 
  declareInterface<ICombineTaggersTool>(this);

  declareProperty( "OmegaMaxBin", m_omegamaxbin = 0.36 );
  declareProperty( "OmegaScale",  m_omegascale  = 0.06 );
  declareProperty( "ProbMin",     m_ProbMin = 0.52 );

}
CombineTaggersProbability::~CombineTaggersProbability(){}
StatusCode CombineTaggersProbability::initialize() { return StatusCode::SUCCESS; }
StatusCode CombineTaggersProbability::finalize()   { return StatusCode::SUCCESS; }

//=============================================================================
int CombineTaggersProbability::combineTaggers(FlavourTag& theTag, 
					      std::vector<Tagger*>& vtg ){
  if( vtg.empty() ) return 0;

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
  debug() << "Final 1-w = " << pnsum <<endreq;
    
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
