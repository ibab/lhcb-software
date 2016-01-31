// $Id$
// ============================================================================
// Include files
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IANNSvc.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
#include "LoKi/HltBase.h"
// ============================================================================
/** @file
 *  Implementation file for class Hlt::Base
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-19
 */
// ============================================================================
/*  standard contructor
 *  @param name algorithm instance name
 *  @param pSvc pointer to Service Locator
 */
// ============================================================================
Hlt::Base::Base( const std::string& name, //      algorithm instance name
                 ISvcLocator* pSvc )      //   pointer to Service Locator
    : GaudiHistoAlg( name, pSvc )
      // services
      ,
      m_regSvc( nullptr ),
      m_hltSvc( nullptr ),
      m_annSvc( nullptr )
{
    //
    Assert( setProperty( "RegisterForContextService", true ).isSuccess(),
            "Unable to enforce the registration for Algorithm Context Service" );
    //
}
// ============================================================================
// virtual and protected destructor
// ============================================================================
Hlt::Base::~Base()
{
}
// ============================================================================
// initialize the algorithm
// ============================================================================
StatusCode Hlt::Base::initialize()
{
    // initialize the base
    StatusCode sc = GaudiHistoAlg::initialize();
    if ( sc.isFailure() ) return sc; // RETURN
    // set the Lock

    // locate LoKi Service
    svc<LoKi::ILoKiSvc>( "LoKiSvc", true );
    // check other importat services
    Assert( hltSvc(), "Unable to aquire Hlt Data     Service" );
    Assert( regSvc(), "Unable to aquire Hlt Register Service" );
    //
    return StatusCode::SUCCESS;
}
// ============================================================================
// finalize the algorithm
// ============================================================================
StatusCode Hlt::Base::finalize()
{
    // disable the services
    m_regSvc = nullptr;
    m_hltSvc = nullptr;
    // finalize the base class
    return GaudiHistoAlg::finalize();
}
// ============================================================================
// accessor to Hlt Registration Service
// ============================================================================
Hlt::IRegister* Hlt::Base::regSvc() const
{
    if ( !m_regSvc ) m_regSvc = svc<Hlt::IRegister>( "Hlt::Service", true );
    return m_regSvc;
}
// ============================================================================
// accessor to Hlt Data Service
// ============================================================================
Hlt::IData* Hlt::Base::hltSvc() const
{
    if ( !m_hltSvc ) m_hltSvc = svc<Hlt::IData>( "Hlt::Service", true );
    return m_hltSvc;
}
// ============================================================================
// accessor to Assigned Numbers & Names service
// ============================================================================
IANNSvc* Hlt::Base::annSvc() const
{
    if ( !m_annSvc ) m_annSvc = svc<IANNSvc>( "Hlt::Service", true );
    return m_annSvc;
}
// ============================================================================
// name   -> number for "InfoID" using IANNSvc
// ============================================================================
int Hlt::Base::hltInfoID( const std::string& name ) const
{
    auto i = annSvc()->value( Gaudi::StringKey( std::string( "InfoID" ) ), name );
    Assert( bool(i), " request for unknown Info ID" );
    return i->second;
}
// ============================================================================
// number -> name   for "InfoID" using IANNSvc
// ============================================================================
std::string Hlt::Base::hltInfoName( int id ) const
{
    auto i = annSvc()->value( Gaudi::StringKey( std::string( "InfoID" ) ), id );
    Assert( bool(i), " request for unknown Info ID" );
    return i->first;
}
// ============================================================================
// get or create the TES-container of Hlt-Candidates
// ============================================================================
Hlt::Candidate::Container*
Hlt::Base::hltCandidates( const std::string& location ) const
{
    IDataProviderSvc* s = evtSvc();
    return getOrCreate<Hlt::Candidate::Container, Hlt::Candidate::Container>(
        s, location );
}
// ============================================================================
// get or create the TES-container of Hlt-Stages
// ============================================================================
Hlt::Stage::Container* Hlt::Base::hltStages( const std::string& location ) const
{
    IDataProviderSvc* s = evtSvc();
    return getOrCreate<Hlt::Stage::Container, Hlt::Stage::Container>( s, location );
}
// ============================================================================
// get or create the TES-container of Hlt-DiMuons
// ============================================================================
Hlt::L0DiMuonCandidate::Container*
Hlt::Base::hltDiMuons( const std::string& location ) const
{
    IDataProviderSvc* s = evtSvc();
    return getOrCreate<Hlt::L0DiMuonCandidate::Container,
                       Hlt::L0DiMuonCandidate::Container>( s, location );
}
// ============================================================================
// get or create the TES-container of Hlt-MultiTracks
// ============================================================================
Hlt::MultiTrack::Container*
Hlt::Base::hltMultiTracks( const std::string& location ) const
{
    IDataProviderSvc* s = evtSvc();
    return getOrCreate<Hlt::MultiTrack::Container, Hlt::MultiTrack::Container>(
        s, location );
}
// ============================================================================
// The END
// ============================================================================
