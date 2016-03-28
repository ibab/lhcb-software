#include <functional>
#include "boost/algorithm/string/predicate.hpp"
#include "Kernel/SelectionLine.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/VectorMap.h"
#include "HltBase/IHltData.h"
#include "HltBase/IHltInspector.h"
#include "HltBase/IHltRegister.h"
#include "HltBase/HltSelection.h"

namespace {
    const Gaudi::StringKey major1{ "Hlt1SelectionID" };
    const Gaudi::StringKey major2{ "Hlt2SelectionID" };
}

namespace Hlt
{

class Line : public ::Selection::Line
{
  public:
    Line( const std::string& name, ISvcLocator* pSvcLocator );
    ~Line( ) override = default;

    StatusCode initialize() override;
    int numberOfCandidates() const override;
    std::unique_ptr<std::function<unsigned()>> numberOfCandidates( const Algorithm* ) const override;
    const std::pair<std::string, unsigned>& id() const override;

  private:
    void SetupSelections();
    std::pair<std::string, unsigned> SetupID();
    Hlt::IData& dataSvc() const;
    Hlt::IInspector& inspectionSvc() const;
    Hlt::IRegister& regSvc() const;

    mutable Hlt::IData* m_hltSvc;
    mutable Hlt::IInspector* m_inspectionSvc;
    mutable Hlt::IRegister* m_regSvc;
    const Hlt::Selection* m_selection;

    std::pair<std::string, unsigned> m_id;
};

}

//-----------------------------------------------------------------------------
// 2008-12-05 : Gerhard Raven
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_NAMESPACE_ALGORITHM_FACTORY( Hlt, Line )

Hlt::Line::Line( const std::string& name, ISvcLocator* pSvcLocator )
    : ::Selection::Line( name, pSvcLocator )
    , m_hltSvc{nullptr}
    , m_inspectionSvc{nullptr}
    , m_regSvc{nullptr}
    , m_selection{nullptr}
{
}

StatusCode Hlt::Line::initialize()
{
    StatusCode sc = ::Selection::Line::initialize();
    SetupSelections();
    m_id = SetupID();
    if ( m_id.second==0 ) {
        error() << " Invalid ID for " << decisionName() << " -- refusing to continue " << endmsg;
        sc = StatusCode::FAILURE;
    }
    return sc;
}

Hlt::IRegister& Hlt::Line::regSvc() const
{
    if ( !m_regSvc ) m_regSvc = svc<Hlt::IRegister>( "Hlt::Service" );
    Assert( m_regSvc, " no Hlt::Service??" );
    return *m_regSvc;
}

Hlt::IData& Hlt::Line::dataSvc() const
{
    if ( !m_hltSvc ) m_hltSvc = svc<Hlt::IData>( "Hlt::Service" );
    Assert( m_hltSvc, " no Hlt::Service??" );
    return *m_hltSvc;
}

Hlt::IInspector& Hlt::Line::inspectionSvc() const
{
    if ( !m_inspectionSvc ) m_inspectionSvc = svc<Hlt::IInspector>( "Hlt::Service" );
    Assert( m_inspectionSvc, " no Hlt::Service??" );
    return *m_inspectionSvc;
}

int Hlt::Line::numberOfCandidates() const
{
    return m_selection ? m_selection->size() : -1;
}

std::unique_ptr<std::function<unsigned int()>> Hlt::Line::numberOfCandidates( const Algorithm* algorithm ) const
{
    // This method is called in Selection::Line::initialize() at a point where
    // algorithms() would return an empty vector, so we can't use it here.
    // In fact, anything initialized in Hlt::Line::initialize() cannot be used here
    Hlt::IInspector::SelList selections;
    if (inspectionSvc().outputs(algorithm, selections)) {
        // just pick up the last one (don't know what to do if more than one anyway!)
        const auto* sel = selections.back();
        return std::unique_ptr<std::function<unsigned int()>>{
            new std::function<unsigned int()>{ [sel](){ return sel->size(); } }
        };
    }
    return std::unique_ptr<std::function<unsigned int()>>{ nullptr };
}


void Hlt::Line::SetupSelections()
{
    Hlt::IRegister::Lock lock{ &regSvc(), this };
    Gaudi::StringKey key{ decisionName() };
    if ( inspectionSvc().hasSelection( key ) ) {
        lock->registerInput( key, this );
        m_selection = dataSvc().selection( key, this );
    }
}

const std::pair<std::string, unsigned>& Hlt::Line::id() const
{
    return m_id;
}

std::pair<std::string, unsigned> Hlt::Line::SetupID()
{
    const Gaudi::StringKey *major = ( boost::algorithm::starts_with(decisionName(), "Hlt1" ) ) ? &major1
                                  : ( boost::algorithm::starts_with(decisionName(), "Hlt2" ) ) ? &major2 
                                  :   nullptr;
    if (!major)  {
        error() << " Decision does not start with Hlt1 or Hlt2 " << decisionName()
                << " ???? " << endmsg;
        return { std::string{}, 0 };
    }

    auto key = annSvc().value( *major, decisionName() );

    if ( !key ) {
        error() << " DecisionName=" << decisionName() << " not known under major "
                  << *major << endmsg;
        return { std::string{}, 0 };
    }
    return *key;
}
