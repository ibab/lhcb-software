// $Id: CombineTaggersOSTDR.cpp,v 1.2 2006-10-24 10:16:44 jpalac Exp $
#include "CombineTaggersOSTDR.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineTaggersOSTDR v1.3
//
//-----------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( CombineTaggersOSTDR )

//=============================================================================
  CombineTaggersOSTDR::CombineTaggersOSTDR( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent ) :
    GaudiTool ( type, name, parent )
{
  declareInterface<ICombineTaggersTool>(this);
}

CombineTaggersOSTDR::~CombineTaggersOSTDR(){}

//=============================================================================
int CombineTaggersOSTDR::combineTaggers(FlavourTag& theTag, 
                                        std::vector<Tagger*>& vtg, 
                                        int signalType, bool flag_nnetTaggers, bool flag_CharmTagger)
{

  if ( msgLevel(MSG::DEBUG) ) {    
    debug() << "Running CombineTaggersOSTDR::combineTaggers signalTpye" <<signalType<<endmsg;
    debug()<<"NB: flag_nnetTaggers="<<flag_nnetTaggers<<" and flag_CharmTagger="<<flag_CharmTagger<<
      " have no influence here"<<endmsg;
  }
  //Want to combine opposite side muon, electron, kaon, vertex
  int catt=0;
  double tagdecision=0;
  std::vector<Tagger*> ostaggers;
  std::vector<int> itag;
  for(int j=0; j!=4; ++j) itag.push_back(0);

  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Number of taggers = " << vtg.size() << endmsg;

  for(std::vector<Tagger*>::const_iterator iter = vtg.begin(); iter != vtg.end(); ++iter)
  {
    if((*iter)->type() == Tagger::OS_Muon)
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Muon tool present    " << endmsg;
      ostaggers.push_back(*iter);
      itag[0] = (*iter)->decision();
      if ( msgLevel(MSG::DEBUG) ) debug() << "Muon decision =      " << itag[0] << endmsg;
      if(itag[0] != 0) theTag.addTagger(**iter);
    }
    if((*iter)->type() == Tagger::OS_Electron){
      if ( msgLevel(MSG::DEBUG) ) debug() << "Electron tool present" << endmsg;
      ostaggers.push_back(*iter);
      itag[1] = (*iter)->decision();
      if ( msgLevel(MSG::DEBUG) ) debug() << "Electron decision =  " << itag[1] << endmsg;
      if(itag[1] != 0) theTag.addTagger(**iter);
    }
    if((*iter)->type() == Tagger::OS_Kaon){
      if ( msgLevel(MSG::DEBUG) ) debug() << "Kaon tool present    " << endmsg;
      ostaggers.push_back(*iter);
      itag[2] = (*iter)->decision();
      if ( msgLevel(MSG::DEBUG) ) debug() << "Kaon decision =      " << itag[2] << endmsg;
      if(itag[2] !=0 ) theTag.addTagger(**iter);
    }
    if((*iter)->type() == Tagger::VtxCharge){
      if ( msgLevel(MSG::DEBUG) ) debug() << "Vertex tool present  " << endmsg;
      ostaggers.push_back(*iter);
      itag[3] = (*iter)->decision();
      if ( msgLevel(MSG::DEBUG) ) debug() << "Vertex decision =    " << itag[3] << endmsg;
      if(itag[3] != 0) theTag.addTagger(**iter);
    }
  }

  if(itag[0])
  {
    tagdecision = itag[0];
    catt = 1;
    if(itag[2]){
      tagdecision += itag[2];
      catt=4;
    }
  }
  else if(itag[1])
  {
    tagdecision = itag[1];
    catt = 2;
    if(itag[2]){
      tagdecision += itag[2];
      catt=5;
    }
  }
  else if(itag[2])
  {
    tagdecision = itag[2];
    catt = 3;
  }
  else if(itag[3])
  {
    tagdecision = itag[3];
    catt = 6;
  }

  if(tagdecision) tagdecision = tagdecision>0 ? 1 : -1;
  if(!tagdecision) catt = 0;
  if( msgLevel(MSG::VERBOSE) ) verbose()<< " Category="<<catt<<endmsg;
  ///fill Result of FlavourTag object
  if(      tagdecision ==  1 ) theTag.setDecision( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag.setDecision( FlavourTag::b );
  else theTag.setDecision( FlavourTag::none );

  theTag.setOmega( 0.5 );

  return theTag.decision();
}
//=============================================================================
