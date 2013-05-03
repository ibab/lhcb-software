// Include files 
#include "GaudiKernel/DeclareFactoryEntries.h"
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
  //look in location where Selection has put the B candidates
  if ( !exist<LHCb::Particle::Range>(location+"/Particles") )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug()<<("No selection found in "+ location+"/Particles")<<endreq;
    return;
  }

  const Particle::Range parts = get<Particle::Range>( location+"/Particles" );
  if ( parts.empty() )
  {
    Warning("No particles found at "+ location+"/Particles",
            StatusCode::SUCCESS,10).ignore();
    return;
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " Will tag "<< parts.size() << " B hypos!" <<endreq;

  // Make new FT object container
  FlavourTags * tags = new FlavourTags;
  // Output to TES
  const std::string tagLocation = location+"/"+m_TagLocation;
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Putting FlavourTags in " << tagLocation << endmsg;
  put( tags, tagLocation );

  //-------------- loop on signal B candidates from selection
  Particle::Range::const_iterator icandB;
  for ( icandB = parts.begin(); icandB != parts.end(); icandB++)
  {
    if ( (*icandB)->particleID().hasBottom() )
    {
      if ( msgLevel(MSG::DEBUG) )
        debug() << "About to tag candidate B of mass="
                << (*icandB)->momentum().M()/GeV <<" GeV"<<endreq;

      FlavourTag * theTag = new FlavourTag;

      //--------------------- TAG IT ---------------------
      //use tool for tagging by just specifing the signal B
      StatusCode sc = flavourTagging() -> tag( *theTag, *icandB );

      //use tool for tagging if you want to specify the Primary Vtx
      //StatusCode sc = flavourTagging() -> tag( *theTag, *icandB, PVertex );

      //use tool for tagging if you want to specify a list of particles
      //StatusCode sc = flavourTagging() -> tag( *theTag, *icandB, PVertex, vtags );
      //--------------------------------------------------
      if (!sc)
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
                                << (tagdecision>0 ? "b":"bbar")<<endreq;
        debug() << "estimated omega= " << theTag->omega() <<endreq;
        const Particle* tagB = theTag->taggedB();
        if( tagB ) debug() << "taggedB p="<< tagB->p()/GeV <<endreq;

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
          case Tagger::jetCharge   : tts="jetCharge";   break;
          case Tagger::OS_jetCharge: tts="OS_jetCharge";break;
          case Tagger::SS_jetCharge: tts="SS_jetCharge";break;
          case Tagger::VtxCharge   : tts="VtxCharge";   break;
          case Tagger::Topology    : tts="Topology";    break;
          }
          debug() << "--> tagger type: " << tts <<endreq;
          debug() << "    decision = "
                  << (itag->decision() > 0? "b":"bbar") <<endreq;
          debug() << "    omega    = " << itag->omega() <<endreq;

          if ( msgLevel(MSG::VERBOSE) )
          {
            const SmartRefVector<LHCb::Particle>& taggerparts = itag->taggerParts();
            for ( SmartRefVector<LHCb::Particle>::const_iterator kp = taggerparts.begin();
                  kp != taggerparts.end(); ++kp ) 
            {
              verbose() << "    ID:" <<std::setw(4)<< (*kp)->particleID().pid()
                        << " p= "  << (*kp)->p()/GeV << endreq;
            }
          }

        }
      }

    }
  }

}

//==========================================================================
