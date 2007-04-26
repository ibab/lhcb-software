
//-----------------------------------------------------------------------------
/** @file ProtoParticleFilterBase.cpp
 *
 * Implementation file for tool ProtoParticleFilterBase
 *
 * CVS Log :-
 * $Id: ProtoParticleFilterBase.cpp,v 1.6 2007-04-26 09:27:39 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// local
#include "Kernel/ProtoParticleFilterBase.h"

// boost
#include "boost/assign/list_of.hpp"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleFilterBase::ProtoParticleFilterBase( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool       ( type, name , parent ),
    m_selectionOpts ( )
{

  // tool interface
  declareInterface<IProtoParticleFilter>(this);

  // job options

  // Maybe find a better way to do this ?
  const int pos = name.find_last_of(".");
  if ( pos>0 )
  {
    const std::string NAME = to_upper(name.substr(pos+1));
    if      ( NAME == "PION" )
    {
      m_selectionOpts.push_back("");
    }

    else if ( NAME == "MUON" )
    {
      m_selectionOpts.push_back("RequiresDet='MUON' CombDLL(mu-pi)>'-8.0'");
    }
    else if ( NAME == "ELECTRON" )
    {
      m_selectionOpts.push_back("RequiresDet='CALO' CombDLL(e-pi)>'0.0'");
    }
    else if ( NAME == "KAON" )
    {
      m_selectionOpts.push_back("RequiresDet='RICH' CombDLL(k-pi)>'2.0' CombDLL(k-p)>'-2.0'");
    }
    else if ( NAME == "PROTON" )
    {
      m_selectionOpts.push_back("RequiresDet='RICH' CombDLL(p-pi)>'3.0'");
    }
  }
  declareProperty( "Selection", m_selectionOpts );

}

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleFilterBase::~ProtoParticleFilterBase() {}

//=============================================================================

StatusCode ProtoParticleFilterBase::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // setup selection
  sc = defineSelections();

  return sc;
}

StatusCode ProtoParticleFilterBase::finalize()
{
  // Perform some generic finalisation tasks
  return GaudiTool::finalize();
}

StatusCode ProtoParticleFilterBase::defineSelections()
{
  debug() << "Defining ProtoParticle selection criteria" << endreq;

  if ( !m_selectionOpts.empty() )
  {
    // Loop over all selections
    for ( SelectionDescs::const_iterator iSel = m_selectionOpts.begin();
          iSel != m_selectionOpts.end(); ++iSel )
    {
      const StatusCode sc = decodeSelOpts(*iSel);
      if ( sc.isFailure() ) return sc;
    }
  }
  else
  {
    return Warning( "No ProtoParticle selections defined", StatusCode::SUCCESS );
  }

  return StatusCode::SUCCESS;
}

StatusCode ProtoParticleFilterBase::decodeSelOpts( const std::string & description )
{
  // seperate into detector and cut data
  info() << "Selection criteria : " << description << endreq;

  // Create a new ProtoParticleSelection for this description
  m_protoSels.push_back( ProtoParticleSelection() );
  ProtoParticleSelection & protoSel = m_protoSels.back();

  // first decode "=" selections to get detector criteria
  Parser detTokens;
  detTokens.analyse( description, "=" );
  for ( Parser::Results::const_iterator i = detTokens.results().begin();
        i != detTokens.results().end(); ++i )
  {
    const std::string & tag   = to_upper((*i).tag());
    const std::string & value = (*i).value();
    debug() << " -> Detector Requirement : " << tag << " = " << value << endreq;
    const ProtoParticleSelection::DetectorRequirements * detreq = createDetReq(tag,value);
    if ( NULL != detreq )
    {
      protoSel.addToDetReqs(detreq);
    }
    else
    {
      m_protoSels.pop_back();
      return Error( "Failed to decode detector requirement : "+tag+" = "+value );
    }
  }

  // next decode the cuts themselves

  // list of possible delimiter types
  static const std::vector<std::string> delims = boost::assign::list_of("<")(">");

  // loop over all delimiters
  for ( std::vector<std::string>::const_iterator iDelim = delims.begin();
        iDelim != delims.end(); ++iDelim )
  {
    // Find tokens for current delimitor
    Parser cutTokens;
    detTokens.analyse( description, *iDelim );
    for ( Parser::Results::const_iterator iT = detTokens.results().begin();
          iT != detTokens.results().end(); ++iT )
    {
      const std::string & tag   = to_upper((*iT).tag());
      const std::string & value = (*iT).value();
      // proccess this cut
      debug() << " -> Cut : " << tag << " " << *iDelim << " " << value << endreq;
      const ProtoParticleSelection::Cut * cut = createCut( tag, *iDelim, value );
      if ( cut )
      {
        protoSel.addToCuts( cut );
      }
      else
      {
        m_protoSels.pop_back();
        return Error( "Failed to decode cut : "+tag+" "+(*iDelim)+" "+value );
      }
    }
  }

  return StatusCode::SUCCESS;
}

// CRJ : Might be a neater way to do this, but it works
bool ProtoParticleFilterBase::Parser::analyse( const std::string & desc,
                                               const std::string & delim )
{
  // clear results
  m_results.clear();

  // First separate according to spaces
  std::vector<std::string> values;
  std::string tmp(desc);
  bool goOn(true);
  while ( goOn )
  {
    const int pos = tmp.find_first_of( " " );
    if ( pos>0 )
    {
      values.push_back( tmp.substr(0,pos) );
      tmp = tmp.substr(pos+1);
    }
    else { goOn = false; }
  }
  if ( !tmp.empty() ) { values.push_back(tmp); }

  // loop over string cuts
  for ( std::vector<std::string>::const_iterator iSel = values.begin();
        iSel != values.end(); ++iSel )
  {
    //const int pos = (*iSel).find_first_of( delim );
    const int pos = (*iSel).find( delim, 0 );
    if ( pos>0 )
    {
      const std::string tag   = (*iSel).substr(0,pos);
      std::string       value = (*iSel).substr(pos+delim.size());
      // strip ' from value
      const std::string first = value.substr(0,1);
      if ( "'" == first )
      {
        const int size = value.size();
        value = value.substr(1,size-2);
      }
      m_results.push_back( Result(tag,value) );
    }
  }

  return true;
}

bool
ProtoParticleFilterBase::isSatisfied( const ProtoParticle* const & proto ) const
{
  // Protect against NULL pointers
  if ( !proto ) { Warning( "Null ProtoParticle pointer" ); return false; }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Applying all cuts to ProtoParticle " << proto->key() << " : "
              << *proto << endreq;
  }

  bool selected = false;

  // Loop over all protoparticle selections
  for ( ProtoParticleSelection::Vector::const_iterator iSel = protoSels().begin();
        iSel != protoSels().end(); ++iSel )
  {
    // test the detector requirements for this selection
    bool OK = true;
    for ( ProtoParticleSelection::DetectorRequirements::Vector::const_iterator iDet
            = (*iSel).detReqs().begin();
          iDet != (*iSel).detReqs().end(); ++iDet )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "  -> Applying DetReq " << (*iDet)->description() << endreq;
      if ( !(*iDet)->isSatisfied(proto) )
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "   -> Requirement failed" << endreq;
        OK = false;    // failed one detector requirement
        break;         // abort other checks
      }
    }
    // are detector requirements OK
    if ( !OK ) continue;

    // Det conditions OK, continue with testing the cuts
    OK = true;
    for ( ProtoParticleSelection::Cut::Vector::const_iterator iCut = (*iSel).cuts().begin();
          iCut != (*iSel).cuts().end(); ++iCut )
    {
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "  -> Applying cut " << (*iCut)->description() << endreq;
      if ( !(*iCut)->isSatisfied(proto) )
      {
        if ( msgLevel(MSG::VERBOSE) ) verbose() << "   -> Cut failed" << endreq;
        OK = false;     // cut is not satisfied
        break;          // abort checking other cuts
      }
    }

    // did we pass the cuts ?
    if ( OK )
    {
      selected = true; // cuts OK
      if ( msgLevel(MSG::VERBOSE) ) verbose() << "    -> Passed Selection" << endreq;
      break;           // selections are exclusive, so if pass one abort checking others.
    }

  } // loop over selections

  return selected;
}
