// $Id: HltAlgorithm.cpp,v 1.64 2010-08-18 09:51:11 graven Exp $
// Include files

#include "Event/Particle.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "HltBase/HltAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltAlgorithm
//
// 2006-06-15 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------
// ============================================================================
HltAlgorithm::HltAlgorithm( std::string name, ISvcLocator* pSvcLocator,
                            bool requireInputsToBeValid )
    : HltBaseAlg( std::move( name ), pSvcLocator )
    , m_requireInputsToBeValid{requireInputsToBeValid}
    , m_outputSelection{nullptr}
    , m_outputHisto{nullptr}
{

    declareProperty( "RequirePositiveInputs", m_requireInputsToBeValid );
    declareProperty( "HistogramUpdatePeriod", m_histogramUpdatePeriod = 0 );

    // TODO: in init, declare these to mon svc...
    counter( "#accept" );
    counter( "#candidates accepted" );

    // TODO: since this is not applicable to all algorithms, remove from base...
    declareProperty( "MinCandidates", m_minNCandidates = 1 );
}

HltAlgorithm::~HltAlgorithm()
{
    delete m_outputSelection;
    m_outputSelection = nullptr;
}

StatusCode HltAlgorithm::queryInterface( const InterfaceID& iid, void** ppvi )
{
    /// valid placeholder?
    if ( !ppvi ) return StatusCode::FAILURE;
    ///
    if ( Hlt::IUnit::interfaceID() == iid ) {
        *ppvi = static_cast<Hlt::IUnit*>( this );
        addRef();
        return StatusCode::SUCCESS; // RETURN
    }
    // fall back to the base
    return HltBaseAlg::queryInterface( iid, ppvi );
}

StatusCode HltAlgorithm::sysInitialize()
{
    // Bypass the initialization if the algorithm
    // has already been initialized.
    if ( Gaudi::StateMachine::INITIALIZED <= FSMState() ) return StatusCode::SUCCESS;
    // set up context such that tools can grab the algorithm...
    // kind of non-local, but kind of better (much more
    // lightweight and less invasive) than the alternative.
    // Note that while GaudiAlgorithm registers the context svc
    // in sysExectute, it doesn't do so in sysInitialize...
    Gaudi::Utils::AlgContext sentry{contextSvc(), this};
    Hlt::IRegister::Lock lock( regSvc(), this );
    return HltBaseAlg::sysInitialize();
}

StatusCode HltAlgorithm::sysFinalize()
{
    m_callbacks.clear();
    return HltBaseAlg::sysFinalize();
}

StatusCode HltAlgorithm::restart()
{
    info() << "restart of " << name() << " requested " << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode HltAlgorithm::sysExecute()
{

    StatusCode sc = StatusCode::SUCCESS;

    // switch of histogramming for this event only if so requested
    bool switchOff =
        m_histogramUpdatePeriod > 0 &&
        ( ( counter( "#accept" ).nEntries() % m_histogramUpdatePeriod ) != 0 );
    bool produced = produceHistos();
    if ( switchOff && produced ) setProduceHistos( false );

    sc = beginExecute();
    if ( sc.isFailure() ) return sc;
    sc = HltBaseAlg::sysExecute();
    if ( sc.isFailure() ) return sc;
    sc = endExecute();

    if ( switchOff && produced ) setProduceHistos( true );

    return sc;
}

StatusCode HltAlgorithm::beginExecute()
{
    if ( !m_outputSelection ) {
        error() << " no output selection !!" << endmsg;
        return StatusCode::FAILURE;
    }

    setFilterPassed( false );

    // we always process callbacks first...
    for ( auto& i : m_callbacks ) {
        StatusCode status = i->execute();
        if ( !status ) {
        }
    }

    // assert if not done properly...
    Assert( m_outputSelection->decision() == false &&
                m_outputSelection->processed() == false &&
                m_outputSelection->error() == false,
            " output already touched???" );

    bool ok = verifyInput();

    return ok ? StatusCode::SUCCESS : StatusCode::FAILURE;
}

StatusCode HltAlgorithm::endExecute()
{
    // TODO: add flushbacks here...
    auto n = m_outputSelection->size();
    counter( "#candidates accepted" ) += n;
    // for non-counting triggers, this must be done explicity by hand!!!
    if ( n >= m_minNCandidates ) m_outputSelection->setDecision( true );
    setDecision( m_outputSelection->decision() );

    if ( produceHistos() ) {
        for ( const auto& i : m_in ) {
            fill( m_inputHistos[i.second->id()], i.second->size(), 1. );
        }

        fill( m_outputHisto, m_outputSelection->size(), 1. );
    }

    if ( msgLevel( MSG::DEBUG ) )
        debug() << " output candidates " << m_outputSelection->size() << " decision "
                << m_outputSelection->decision() << " filterpassed "
                << filterPassed() << endmsg;
    return StatusCode::SUCCESS;
}

bool HltAlgorithm::verifyInput()
{
    if ( !m_requireInputsToBeValid ) return true;
    bool ok = true;
    for ( auto& i : m_in ) {
        // propagate error status!
        if ( i.second->error() ) m_outputSelection->setError( true );
        ok = ok && i.second->decision();
        if ( msgLevel( MSG::DEBUG ) )
            debug() << " input " << i.second->id() << " decision "
                    << i.second->decision() << " process status "
                    << i.second->processed() << " error status " << i.second->error()
                    << " candidates " << i.second->size() << endmsg;
    }

    if ( !ok ) {
        warning() << endmsg;
        warning() << endmsg;
        warning() << " FIXME FIXME FIXME FIXME" << endmsg;
        warning() << endmsg;
        warning() << " Empty input or false input selection!" << endmsg;
        warning() << " Most likely due to a misconfiguration" << endmsg;
        for ( const auto& i : m_in ) {
            warning() << " input selection " << i.second->id() << " decision "
                      << i.second->decision() << " processed "
                      << i.second->processed() << " error " << i.second->error()
                      << " candidates " << i.second->size() << endmsg;
        }
        warning() << endmsg;
        warning() << endmsg;
        warning() << endmsg;
        return StatusCode::FAILURE;
    }
    return ok;
}

void HltAlgorithm::setDecision( bool accept )
{
    setFilterPassed( accept );
    // the next forces the 'processed' flag to be set -- even if in some
    // cases 'accept' is obtained from m_outputSelection!!!
    // (basically, if we are of type TSelection<X>, and have no selected
    // candidates, we never explicitly set 'false', and hence would otherwise
    // not set 'processed'
    m_outputSelection->setDecision( accept );
    counter( "#accept" ) += accept;
}

// TODO: switch selection & declareInput around...
const Hlt::Selection* HltAlgorithm::declareInput( const Gaudi::StringKey& key,
                                                  const Hlt::IUnit::Client& ) const
{
    return selection( key );
}

const Hlt::Selection* HltAlgorithm::selection( const Gaudi::StringKey& key,
                                               const Hlt::IUnit::Client& ) const
{
    return selection( key );
}

class cmp_by_id
{
    Gaudi::StringKey m_id;

  public:
    cmp_by_id( const Hlt::Selection& sel ) : m_id( sel.id() )
    {
    }
    template <typename T>
    bool operator()( const T& i )
    {
        return m_id == i.second->id();
    }
};

const Hlt::Selection*
HltAlgorithm::selection( const Gaudi::StringKey& selname ) const
{
    Assert( !selname.empty(), " selection(): no selection name" );
    if ( msgLevel( MSG::DEBUG ) ) debug() << " selection: " << selname << endmsg;
    StatusCode sc = regSvc()->registerInput( selname, this );
    if ( sc.isFailure() ) {
        error() << " failed to register input " << selname << endmsg;
        Assert( 0, " selection:, failed to register input!" );
    }
    const Hlt::Selection* sel = hltSvc()->selection( selname, this );
    if ( !sel ) {
        error() << " failed to retrieve input " << selname << endmsg;
        Assert( 0, " selection:, failed to retrieve input!" );
    }
    if ( std::none_of( std::begin( m_in ), std::end( m_in ), cmp_by_id( *sel ) ) ) {
        m_in.insert( {sel->id(), sel} );
        if ( produceHistos() ) {
            bool ok = m_inputHistos.find( sel->id() ) == m_inputHistos.end();
            Assert( ok, "selection(): already input selection " + sel->id().str() );
            m_inputHistos[sel->id()] = const_cast<HltAlgorithm*>( this )
                                           ->initializeHisto( sel->id().str() );
        }
        if ( msgLevel( MSG::DEBUG ) )
            debug() << " Input selection " << sel->id() << endmsg;
    }
    if ( msgLevel( MSG::DEBUG ) )
        debug() << " retrieved selection " << sel->id() << endmsg;
    return sel;
}

StatusCode HltAlgorithm::registerOutput( Hlt::Selection* sel,
                                         const Hlt::IUnit::Client& /*client*/ ) const
{
    return registerOutput( sel );
}

StatusCode HltAlgorithm::registerOutput( Hlt::Selection* sel ) const
{
    if ( m_outputSelection ) {
        error() << "attempt to register a 2nd output selection: " << sel->id()
                << " already have " << m_outputSelection->id() << endmsg;
        return StatusCode::FAILURE;
    }
    m_outputSelection = sel;
    std::vector<const Hlt::Selection*> sels;
    sels.reserve( m_in.size() );
    for ( const auto& i : m_in ) sels.push_back( i.second );
    sel->addInputSelectionIDs( sels.begin(), sels.end() );
    if ( msgLevel( MSG::DEBUG ) )
        debug() << " Output selection " << sel->id() << endmsg;
    if ( regSvc()->registerOutput( sel, this ).isFailure() ) {
        error() << "Failed to add Selection" << sel->id() << endmsg;
        return StatusCode::FAILURE;
    }
    m_outputHisto = const_cast<HltAlgorithm*>( this )
                        ->initializeHisto( "Ncandidates", -0.5, 99.5, 100 );
    if ( msgLevel( MSG::DEBUG ) )
        debug() << " registered selection " << sel->id() << " type "
                << sel->classID() << endmsg;
    return StatusCode::SUCCESS;
}

StatusCode HltAlgorithm::registerTESInput( const Gaudi::StringKey& key,
                                           const Hlt::IUnit::Client& ) const
{
    return registerTESInput( key );
}

StatusCode HltAlgorithm::registerTESInput( const Gaudi::StringKey& key ) const
{
    auto sc = regSvc()->registerTESInput( key, this );
    if ( sc.isFailure() ) {
        return Error( "Unable to register INPUT TES location '" + key.str() + "'",
                      sc );
    }
    auto ifind = std::find( m_tes.begin(), m_tes.end(), key );
    if ( m_tes.end() == ifind ) m_tes.push_back( key );
    return StatusCode::SUCCESS;
}

const DataObject* HltAlgorithm::tes( const Hlt::IUnit::Client& /* client  */,
                                     const Hlt::IUnit::Key& location ) const
{
    // check the location
    auto ifind = std::find( m_tes.begin(), m_tes.end(), location );
    //
    Assert( m_tes.end() != ifind, "tes: anuthorized access to TES -data" );
    return hltSvc()->tes( this, location );
    //
}

Hlt::Selection* HltAlgorithm::retrieve( const Hlt::IUnit::Client& /* client */,
                                        const Gaudi::StringKey& /* key    */ ) const
{
    Error( "retrieve(): not implemented " );
    return nullptr;
}
