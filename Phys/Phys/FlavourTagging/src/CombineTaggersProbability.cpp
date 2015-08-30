#include "CombineTaggersProbability.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineTaggersProbability v2.
//
// 2011-01-18 : Marco Musy & Marc Grabalosa
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( CombineTaggersProbability )

//=============================================================================
  CombineTaggersProbability::CombineTaggersProbability( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent ):
    GaudiTool ( type, name, parent )
{
  declareInterface<ICombineTaggersTool>(this);

  declareProperty( "ProbMin",     m_ProbMin     = 0.5);
  declareProperty( "ProbMin_OS",  m_ProbMin_OS  = 0.5);
  // Tuning Moriond 2012
  //declareProperty( "P0_Cal_OS",   m_P0_Cal_OS   = 0.392);
  //declareProperty( "P1_Cal_OS",   m_P1_Cal_OS   = 0.921);
  //declareProperty( "Eta_Cal_OS",  m_Eta_Cal_OS  = 0.363);
  // Tuning 1fb-1 paper
  //declareProperty( "P0_Cal_OS",   m_P0_Cal_OS   = 0.392);
  //declareProperty( "P1_Cal_OS",   m_P1_Cal_OS   = 0.953);
  //declareProperty( "Eta_Cal_OS",  m_Eta_Cal_OS  = 0.362);
  // Tuning for Reco14 -- Combination of mu,e,K,Vtx cut based taggers
  declareProperty( "P0_Cal_OS",   m_P0_Cal_OS   = 0.390);
  declareProperty( "P1_Cal_OS",   m_P1_Cal_OS   = 0.899);
  declareProperty( "Eta_Cal_OS",  m_Eta_Cal_OS  = 0.365);
  // Tuning for Reco14 -- Combination of mu,e,nnetK,Vtx 
  declareProperty( "P0_Cal_OSnnet",   m_P0_Cal_OSnnet   = 0.4168);
  declareProperty( "P1_Cal_OSnnet",   m_P1_Cal_OSnnet   = 0.838);
  declareProperty( "Eta_Cal_OSnnet",  m_Eta_Cal_OSnnet  = 0.3962);
  
}

CombineTaggersProbability::~CombineTaggersProbability(){}

StatusCode CombineTaggersProbability::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;
  
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Comb OS calib ctt: P0_Cal "<<m_P0_Cal_OS<<", P1_Cal "<<m_P1_Cal_OS<<endmsg;
  
  return sc;
}

//=============================================================================
int CombineTaggersProbability::combineTaggers(FlavourTag& theTag,
                                              std::vector<Tagger*>& vtg ,
                                              int signalType, bool flag_nnetTaggers, bool flag_CharmTagger)
{
  if( vtg.empty() ) return 0;

  double tagdecision=0;
  double pnsum_a= 0.50;  //hypothesis of truetag=+1
  double pnsum_b= 0.50;  //hypothesis of truetag=-1
  int vtgsize = vtg.size();

  for( int i = 0; i != vtgsize; ++i ) 
  { //multiply all probabilities
    if(! vtg.at(i)) continue;
    if( vtg.at(i)->type() == (Tagger::SS_Pion)   || 
        vtg.at(i)->type() == (Tagger::SS_PionBDT)||
        vtg.at(i)->type() == (Tagger::SS_Proton) || 
        vtg.at(i)->type() == (Tagger::SS_Kaon)   || 
        vtg.at(i)->type() == (Tagger::SS_nnetKaon) 
        ) continue; // Just combine the prob of OS taggers
    
    if(!flag_nnetTaggers &&  vtg.at(i)->type() == (Tagger::OS_nnetKaon) ) continue;
    if(flag_nnetTaggers  &&  vtg.at(i)->type() == (Tagger::OS_Kaon)     ) continue;
    if(!flag_CharmTagger &&  vtg.at(i)->type() == (Tagger::OS_Charm)    ) continue;
    
    const double mtag = vtg.at(i)->decision();
    if(!mtag) continue;
    const double pn   = 1-(vtg.at(i))->omega(); //probability of 'right'
    pnsum_a *= ((1-mtag)/2 + mtag*pn ); // p
    pnsum_b *= ((1+mtag)/2 - mtag*pn ); //(1-p)
  }
  if(pnsum_a > pnsum_b) tagdecision = +1;
  if(pnsum_a < pnsum_b) tagdecision = -1;
  //normalise probability to the only two possible flavours:
  double pnsum = std::max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b);
  
  if ( msgLevel(MSG::DEBUG) )
    debug() << " Before pn="<< pnsum <<" w="<<1-pnsum<<endmsg;

  //Calibration (w=1-pn) w' = p0 + p1(w-eta)
  if(!flag_nnetTaggers && !flag_CharmTagger)  
    pnsum = 1 - m_P0_Cal_OS - m_P1_Cal_OS * ( (1-pnsum)-m_Eta_Cal_OS);
  else if(flag_nnetTaggers && !flag_CharmTagger)  
    pnsum = 1 - m_P0_Cal_OSnnet - m_P1_Cal_OSnnet * ( (1-pnsum)-m_Eta_Cal_OSnnet);
  else 
  {
  if ( msgLevel(MSG::INFO) )
    info() << " WARNING: fix the calibration parameters for this configuration of combination nnetTaggers="<<flag_nnetTaggers<<" flagCharmTagger="<<flag_CharmTagger<< endmsg;
  }
  

  if ( msgLevel(MSG::DEBUG) )
    debug() << " OS pn="<< pnsum <<" w="<<1-pnsum<<endmsg;

  //throw away poorly significant tags
  if(pnsum < m_ProbMin_OS || tagdecision == 0)
  {
    pnsum = 0.50;
    tagdecision = 0;
  }
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Final OS 1-w = " << pnsum <<endmsg;

  ///fill FlavourTag object
  if(      tagdecision ==  1 ) theTag.setDecisionOS( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag.setDecisionOS( FlavourTag::b );
  else theTag.setDecisionOS( FlavourTag::none );
  theTag.setOmegaOS( 1-pnsum );

  //###################################
  //Now fill SS+OS combination
  double SSeOS_tagdecision=tagdecision;
  double SSeOS_pnsum_a= ((1-tagdecision)/2 + tagdecision*pnsum ); //hypothesis of truetag=+1
  double SSeOS_pnsum_b= ((1+tagdecision)/2 - tagdecision*pnsum );;  //hypothesis of truetag=-1
  for( int i = 0; i != vtgsize; ++i ) 
  { //multiply all probabilities
    if(! vtg.at(i)) continue;
    if( (signalType == 1 && vtg.at(i)->type() == (Tagger::SS_Pion)) ||
        (signalType == 1 && vtg.at(i)->type() == (Tagger::SS_Proton)) ||
        (signalType == 2 && !flag_nnetTaggers && vtg.at(i)->type() == (Tagger::SS_Kaon)) ||
        (signalType == 2 && flag_nnetTaggers && vtg.at(i)->type() == (Tagger::SS_nnetKaon))
        )
    {
      const double mtagss = vtg.at(i)->decision();
      if(!mtagss) continue;
      const double pnss   = 1-(vtg.at(i))->omega(); //probability of 'right'
      SSeOS_pnsum_a *= ((1-mtagss)/2 + mtagss*pnss ); // p
      SSeOS_pnsum_b *= ((1+mtagss)/2 - mtagss*pnss ); //(1-p)
    }
  }
  if(SSeOS_pnsum_a > SSeOS_pnsum_b) SSeOS_tagdecision = +1;
  if(SSeOS_pnsum_a < SSeOS_pnsum_b) SSeOS_tagdecision = -1;
  //normalise probability to the only two possible flavours:
  double SSeOS_pnsum = std::max(SSeOS_pnsum_a,SSeOS_pnsum_b) /(SSeOS_pnsum_a + SSeOS_pnsum_b);

  //throw away poorly significant tags
  if(SSeOS_pnsum < m_ProbMin ||  SSeOS_tagdecision == 0)
  {
    SSeOS_pnsum = 0.50;
    SSeOS_tagdecision = 0;
  }
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Final 1-w = " << SSeOS_pnsum <<endmsg;

  ///fill FlavourTag object
  if(      SSeOS_tagdecision ==  1 ) theTag.setDecision( FlavourTag::bbar );
  else if( SSeOS_tagdecision == -1 ) theTag.setDecision( FlavourTag::b );
  else theTag.setDecision( FlavourTag::none );
  theTag.setOmega( 1-SSeOS_pnsum );

  //fill in taggers info into FlavourTag object
  for(int j=0; j!=vtgsize; ++j)
  {
    const int itag = vtg.at(j)->decision();
    if(itag) theTag.addTagger(*(vtg.at(j)));
  }

  return theTag.decision();
}
//=============================================================================
