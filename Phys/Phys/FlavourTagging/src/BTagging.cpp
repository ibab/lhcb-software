// Include files 
#include "GaudiKernel/Property.h"
// local
#include "BTagging.h"

//-----------------------------------------------------------------------
// Implementation file for class : BTagging
//
// Author: Marco Musy
//-----------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( BTagging )

//=======================================================================
  BTagging::BTagging(const std::string& name,
                     ISvcLocator* pSvcLocator):
    DaVinciAlgorithm(name, pSvcLocator),
    m_TagLocation("FlavourTags")
{
  declareProperty( "TagOutputLocation", m_TagLocation );
}

BTagging::~BTagging() {}

//=======================================================================
// Main execution
//=======================================================================
StatusCode BTagging::execute()
{
  for ( std::vector<std::string>::const_iterator iLoc = inputLocations().begin();
        iLoc != inputLocations().end(); ++iLoc )
  {
    performTagging(*iLoc);
  }
  setFilterPassed( true );
  return StatusCode::SUCCESS;
}

//=======================================================================

void BTagging::performTagging(const std::string & location)
{

  const std::string particlesLocation = location+"/Particles";

  const Particle::Range parts = getIfExists<Particle::Range>( particlesLocation );
  if ( parts.empty() )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "No particles found at " << particlesLocation << endmsg;
    return;
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Will tag "<< parts.size() << " B hypos!" <<endmsg;

  // Make new FT object container
  FlavourTags * tags = new FlavourTags;

  //-------------- loop on signal B candidates from selection
  for ( Particle::Range::const_iterator icandB = parts.begin(); 
        icandB != parts.end(); ++icandB )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Found Particle of type " << (*icandB)->particleID() << endmsg;
    if ( (*icandB)->particleID().hasBottom() )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "About to tag candidate B of mass="
                << (*icandB)->momentum().M()/GeV <<" GeV"<<endmsg;

      FlavourTag * theTag = new FlavourTag;

      //--------------------- TAG IT ---------------------
      //use tool for tagging by just specifing the signal B
      const StatusCode sc = flavourTagging() -> tag( *theTag, *icandB );

      //use tool for tagging if you want to specify the Primary Vtx
      //const StatusCode sc = flavourTagging() -> tag( *theTag, *icandB, PVertex );

      //use tool for tagging if you want to specify a list of particles
      //const StatusCode sc = flavourTagging() -> tag( *theTag, *icandB, PVertex, vtags );
      //--------------------------------------------------
      if ( sc.isFailure() )
      {
        Error( "Tagging Tool returned error." ).ignore();
        delete theTag;
        continue;
      }
      else
      {
        tags->insert(theTag);
      }

      //--- PRINTOUTS ---
      //print the information in theTag
      if ( msgLevel(MSG::DEBUG) )
      {
        const int tagdecision = theTag->decision();
        if(tagdecision) debug() << "Flavour guessed: "
                                << (tagdecision>0 ? "b":"bbar")<<endmsg;
        debug() << "estimated omega= " << theTag->omega() <<endmsg;
        const Particle* tagB = theTag->taggedB();
        if( tagB ) debug() << "taggedB p="<< tagB->p()/GeV <<endmsg;

        ///print Taggers information
        const std::vector<Tagger>& mytaggers = theTag->taggers();
        for ( std::vector<Tagger>::const_iterator itag = mytaggers.begin();
              itag != mytaggers.end(); ++itag )
        {
          std::string tts;
          switch ( itag->type() )
          {
          case Tagger::none        : tts="none";        break;
          case Tagger::unknown     : tts="unknown";     break;
          case Tagger::OS_Muon     : tts="OS_Muon";     break;
          case Tagger::OS_Electron : tts="OS_Electron"; break;
          case Tagger::OS_Kaon     : tts="OS_Kaon";     break;
          case Tagger::SS_Kaon     : tts="SS_Kaon";     break;
          case Tagger::SS_Pion     : tts="SS_Pion";     break;
          case Tagger::SS_PionBDT  : tts="SS_PionBDT";  break;
          case Tagger::SS_Proton   : tts="SS_Proton";  break;
          case Tagger::jetCharge   : tts="jetCharge";   break;
          case Tagger::OS_jetCharge: tts="OS_jetCharge";break;
          case Tagger::SS_jetCharge: tts="SS_jetCharge";break;
          case Tagger::VtxCharge   : tts="VtxCharge";   break;
          case Tagger::Topology    : tts="Topology";    break;
          case Tagger::OS_nnetKaon : tts="OS_nnetKaon"; break;
          case Tagger::SS_nnetKaon : tts="SS_nnetKaon"; break;
          case Tagger::OS_Charm    : tts="OS_Charm";    break;
          }
          debug() << "--> tagger type: " << tts <<endmsg;
          debug() << "    decision = "
                  << (itag->decision() > 0? "b":"bbar") <<endmsg;
          debug() << "    omega    = " << itag->omega() <<endmsg;

          if ( msgLevel(MSG::VERBOSE) )
          {
            const SmartRefVector<LHCb::Particle>& taggerparts = itag->taggerParts();
            for ( SmartRefVector<LHCb::Particle>::const_iterator kp = taggerparts.begin();
                  kp != taggerparts.end(); ++kp ) 
            {
              verbose() << "    ID:" <<std::setw(4)<< (*kp)->particleID().pid()
                        << " p= "  << (*kp)->p()/GeV << endmsg;
            }
          }

        }
      }

    }
  }

  // Output to TES, if tags is not empty
  if ( tags->empty() )
  {
    delete tags;
  }
  else
  {
    put( tags, location+"/"+m_TagLocation );
  }
  
}

//==========================================================================
