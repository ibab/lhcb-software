#include "CombineTaggersProbabilityOS.h"
#include "taggingutils.h"

CombineTaggersProbabilityOS::CombineTaggersProbabilityOS() {

  declareProperty( "CombineTaggersProb_omegamaxbin_OS", m_omegamaxbin_OS = 0.38);
  declareProperty( "CombineTaggersProb_omegascale_OS",  m_omegascale_OS  = 0.07);
  declareProperty( "CombineTaggersProb_ProbMin_OS",     m_ProbMin_OS     = 0.56);

  declareProperty( "CombineTaggersProb_ProbMin_OS_P0_Cal_OS",   m_P0_Cal_OS   = 0.364); 
  declareProperty( "CombineTaggersProb_ProbMin_OS_P1_Cal_OS",   m_P1_Cal_OS   = 1.08 ); 
  declareProperty( "CombineTaggersProb_ProbMin_OS_Eta_Cal_OS",  m_Eta_Cal_OS  = 0.338);

  theTag = new FlavourTag();
}

FlavourTag* CombineTaggersProbabilityOS::combineTaggers( Taggers& vtg ) {
  theTag->reset();

  theTag->setTaggers(vtg);

  if( vtg.empty() ) return theTag;

  double tagdecision=0;
  double pnsum_a= 0.50;  //hypothesis of truetag=+1
  double pnsum_b= 0.50;  //hypothesis of truetag=-1
  int vtgsize = vtg.size();
  for( int i = 0; i != vtgsize; i++ ) { //multiply all probabilities
    if(! vtg.at(i)) continue;
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

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  debug() << " Before pn="<< pnsum <<" w="<<1-pnsum<<endreq;
  pnsum = 1 - m_P0_Cal_OS - m_P1_Cal_OS * ( (1-pnsum)-m_Eta_Cal_OS);
  debug() << " OS pn="<< pnsum <<" w="<<1-pnsum<<endreq;

  //throw away poorly significant tags
  if(pnsum < m_ProbMin_OS) {
    pnsum = 0.50;
    tagdecision = 0;
  }

  //sort decision into categories ------------------
  //cat=1 will be least reliable, cat=5 most reliable
  //ProbMin is a small offset to adjust for range of pnsum
  int category = 0;
  double omega = fabs(1-pnsum);
  if(      omega > m_omegamaxbin_OS                   ) category=1;
  else if( omega > m_omegamaxbin_OS-m_omegascale_OS   ) category=2;
  else if( omega > m_omegamaxbin_OS-m_omegascale_OS*2 ) category=3;
  else if( omega > m_omegamaxbin_OS-m_omegascale_OS*3 ) category=4;
  else                                            category=5;
  if( !tagdecision ) category=0;

  ///fill FlavourTag object
  if(      tagdecision ==  1 ) theTag->setDecision( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag->setDecision( FlavourTag::b );
  else theTag->setDecision( 0 );
  theTag->setCategory( category );
  theTag->setOmega( 1-pnsum );

  return theTag;
}
