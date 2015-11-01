// $Id: TupleToolDecay.cpp,v 1.4 2010-01-26 15:39:26 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolDecay.h"
//#include "Kernel/IParticleTupleTool.h"

#include "LoKi/Trees.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "GaudiKernel/Algorithm.h"

#include <boost/lexical_cast.hpp>

using namespace Gaudi;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleTool
//
// 2007-11-02 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolDecay )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolDecay::TupleToolDecay( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
    : GaudiTool( type, name , parent )
    , m_hasMatched( false )
    , m_myName(name)
    , m_dkFinder(NULL)
    , m_mcdkFinder(NULL)
    , m_isMC(false)
    , m_mcdecayTree( Decays::Trees::Invalid_<const LHCb::MCParticle*>() )
    , m_decayTree  ( Decays::Trees::Invalid_<const LHCb::Particle*>()   )
{
  declareInterface<ITupleToolDecay>(this);
  declareProperty( "ToolList", m_stufferList );
  declareProperty( "InheritTools", m_inheritTools = true );
  declareProperty( "UseLoKiDecayFinders", m_useLoKiDecayFinders = true );
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolDecay::~TupleToolDecay() {}

//=============================================================================

StatusCode TupleToolDecay::initializeDecay( const std::string& dcy, bool isMC )
{
  if ( !GaudiTool::initialize() ) return StatusCode::FAILURE;

  m_isMC = isMC ;

  if (msgLevel(MSG::DEBUG)) debug() << "TupleToolDecay::initialize "
                                    << dcy << " " << isMC << endmsg ;

  std::string pname; // sets the name to "ToolName" and not to "AlgoParent.ToolName"
  const Algorithm* alg = dynamic_cast<const Algorithm*>( parent() );
  if( alg ) pname = alg->name();

  if( pname.empty() )
    Warning("Did someone change the name structure ???, your printout will not be nice..." );
  else {
    m_myName = m_myName.substr( pname.size()+1 );
  }

  bool test = true ;
  if ( m_useLoKiDecayFinders ) {
    if ( m_isMC ) {
      Decays::IMCDecay* mcdecaytool = tool<Decays::IMCDecay>( "LoKi::MCDecay", this );
      if ( mcdecaytool != NULL ) {
        m_mcdecayTree = mcdecaytool->tree(dcy);
      }
      test = m_mcdecayTree.valid();
    } else {
      Decays::IDecay* decaytool = tool<Decays::IDecay>( "LoKi::Decay", this );
      if ( decaytool != NULL ) {
        m_decayTree = decaytool->tree(dcy);
      }
      test = m_decayTree.valid();
    }
  } else {
    if (m_isMC){
      m_mcdkFinder = tool<IMCDecayFinder>("MCDecayFinder", this );
      test &= m_mcdkFinder->setDecay( dcy );
    } else {
      m_dkFinder = tool<IDecayFinder>("DecayFinder", this );
      test &= m_dkFinder->setDecay( dcy );
    }
  }

  if (msgLevel(MSG::DEBUG)) debug() << "Initialized " << name()
                                    << " with decay " << decay() << endmsg;

  std::sort( m_stufferList.begin(), m_stufferList.end() );
  m_stufferList.erase(std::unique(m_stufferList.begin(),m_stufferList.end()),
                      m_stufferList.end());

  return StatusCode(test);
}

//=============================================================================
// get decay
//=============================================================================
std::string TupleToolDecay::decay() const
{
  if ( m_useLoKiDecayFinders ) {
    return ( m_isMC ? m_mcdecayTree.toString() : m_decayTree.toString() );
  } else {
    return ( m_isMC ? m_mcdkFinder->decay() : m_dkFinder->decay() );
  }
}

std::string TupleToolDecay::getInfo() const { return name() + " :" + decay(); }
void TupleToolDecay::printInfo() const { info() << getInfo() << endmsg; }

IDecayFinder* TupleToolDecay::decayFinder() const {return m_dkFinder;}
IMCDecayFinder* TupleToolDecay::mcDecayFinder() const {return m_mcdkFinder;}

bool TupleToolDecay::hasMatched() const { return m_hasMatched; }
void TupleToolDecay::hasMatched( bool state ) { m_hasMatched = state; }

bool TupleToolDecay::inheritTools() const { return m_inheritTools; }

const std::string& TupleToolDecay::getName() const { return m_myName; }
const std::vector<std::string>& TupleToolDecay::getStuffers() const { return m_stufferList; }
