// $Id: HltL0CaloPrepare.cpp,v 1.9 2008-08-10 18:27:54 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "boost/foreach.hpp"
#include <memory>
#include "Event/L0DUBase.h"
#include "Event/L0CaloCandidate.h"

// local
#include "HltL0CaloPrepare.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0CaloPrepare
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0CaloPrepare );

static L0DUBase::CaloType::Type string2type(const std::string& s) {
    typedef GaudiUtils::VectorMap<std::string,L0DUBase::CaloType::Type> map_t;
    static map_t map;
    if (map.empty()) {
#define _insert_map_(x) { map.insert( #x,  L0DUBase::CaloType::x ); }
    _insert_map_( Electron ) ; _insert_map_( Photon   );
    _insert_map_( Hadron   ) ; _insert_map_( Pi0Local );
    _insert_map_( Pi0Global) ; _insert_map_( SumEt    );
    _insert_map_( SpdMult  ) ;
    _insert_map_( HadronSlave1Out ); _insert_map_( HadronSlave2Out );
    _insert_map_( HadronSlave1In );  _insert_map_( HadronSlave2In );
    _insert_map_( SumEtSlave1Out );  _insert_map_( SumEtSlave2Out );
    _insert_map_( SumEtSlave1In );   _insert_map_( SumEtSlave2In )
   }
   map_t::const_iterator i = map.find(s);
   if (i==map.end()){ 
        throw GaudiException("Unknown L0 calo type name",s,StatusCode::FAILURE);
        return L0DUBase::CaloType::Type(0); // never reached, keep compiler happy..
   }
   return i->second;
}

void HltL0CaloPrepare::caloTypeProperty::updateHandler(Property&) {
    m_val = string2type( m_prop.value() );
}

HltL0CaloPrepare::caloTypeProperty::caloTypeProperty(HltL0CaloPrepare::caloTypeProperty& rhs) 
    : m_prop(rhs.m_prop)
    , m_val(rhs.m_val)
{ m_prop.declareUpdateHandler( &HltL0CaloPrepare::caloTypeProperty::updateHandler, this ); }

HltL0CaloPrepare::caloTypeProperty::caloTypeProperty(const std::string& s) 
    : m_prop(s)
    , m_val( string2type(s) )
{ m_prop.declareUpdateHandler( &HltL0CaloPrepare::caloTypeProperty::updateHandler, this ); }

HltL0CaloPrepare::caloTypeProperty& HltL0CaloPrepare::caloTypeProperty::operator=(const std::string& s) 
{   m_prop = s;
    m_val = string2type(s);
}
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0CaloPrepare::HltL0CaloPrepare( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selection(*this)
  , m_caloMaker(0)
  , m_histoEt(0)
  , m_histoEt1(0)
  , m_caloType("Hadron")
{
  declareProperty("MinEt", m_minEt = 3500.);
  declareProperty("CaloType", m_caloType.property() );

  declareProperty("CaloCandidatesLocation", m_caloCandidatesLocation = 
                  L0CaloCandidateLocation::Full);
                  // L0CaloCandidateLocation::Default);

  declareProperty("CaloMakerTool", m_caloMakerName = "");

  m_selection.declareProperties();
}
//=============================================================================
// Destructor
//=============================================================================
HltL0CaloPrepare::~HltL0CaloPrepare() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0CaloPrepare::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  m_selection.retrieveSelections();

  m_selection.registerSelection();

  m_histoEt = initializeHisto("Et",0.,6000.,100);
  m_histoEt1 = initializeHisto("Et1",0.,6000.,100);

  debug() << " calo candidates location " 
          << m_caloCandidatesLocation << endreq;

  m_caloMaker = (m_caloMakerName.empty() 
                    ? (ICaloSeedTool*)0
                    :tool<ICaloSeedTool>("HadronSeedTool"));

  saveConfiguration();  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0CaloPrepare::execute() {

  L0CaloCandidates* input = get<L0CaloCandidates>(m_caloCandidatesLocation);
  Tracks* output = new Tracks();
  put(output,"Hlt/Track/"+m_selection.output()->id().str());

  // BOOST_FOREACH(const L0CaloCandidate* calo, *m_selection.input<1>()) {
  BOOST_FOREACH(const L0CaloCandidate* calo, *input ) {
    if (calo->type() == m_caloType() && calo->et() > m_minEt) {
        std::auto_ptr<Track> tcalo( new Track() );
        if (m_caloMaker) {
            StatusCode sc = m_caloMaker->makeTrack(*calo,*tcalo);
            if (sc.isFailure()) return sc;
        } else                           makeTrack(*calo,*tcalo);
        addExtras(*calo,*tcalo);
        //TODO: pushes both into IHltDataSvc (m_outputTracks) and into TES (output)
        m_selection.output()->push_back(tcalo.get());
        output->insert(tcalo.release());
    }
  }

  if (m_debug) {
    // debug()  << "# Input: " << m_selection.input<1>()->size() 
    debug()  << "# Input: " << input->size() 
             << " -> # Output: " << m_selection.output()->size() << endreq;
  }
  if (m_debug) printInfo(" Calos ",*m_selection.output());

  return StatusCode::SUCCESS;
}


StatusCode HltL0CaloPrepare::finalize() {

  debug() << "==> Finalize" << endmsg;
  return HltAlgorithm::finalize();  
}

void HltL0CaloPrepare::makeTrack(const L0CaloCandidate& calo,
                                  LHCb::Track& track) {
  
  // Get energy and position of L0 calo candidate:
  double x      = calo.position().x();
  double y      = calo.position().y();
  double z      = calo.position().z();
  double ex     = calo.posTol()*(4./sqrt(12.0));
  double ey     = ex;
  double et     = calo.et();

  double sintheta = sqrt(x*x + y*y)/(sqrt(x*x + y*y + z*z));
  double e = fabs(et)/sintheta;

  debug() << " position " << calo.position() << " et " << et
          << " sintheta " << sintheta << " e " << e << endreq;  

  State state;
  state.setLocation(State::MidHCal);
  state.setState(x,y,z,ex,ey,1./e); // ??? slopeX = ex, slopY = ey ???
  track.addToStates(state);
  
}

void HltL0CaloPrepare::addExtras(const L0CaloCandidate& calo,
                                  LHCb::Track& track) {

  double ex     = calo.posTol()*(4./sqrt(12.0));
  double ey     = ex;

  BOOST_FOREACH( State* state, track.states() ) {
    if (state->location() == State::MidHCal){
      state->setTx(ex); // ???
      state->setTy(ey); // ???
      debug() << " changed slopes! " << state->slopes() << endreq;
    }
  }

  CaloCellID id = calo.id();
  LHCb::CaloCellID id1(id.calo(), id.area(), id.row()+1, id.col()   );
  LHCb::CaloCellID id2(id.calo(), id.area(), id.row()+1, id.col()+1 );
  LHCb::CaloCellID id3(id.calo(), id.area(), id.row()  , id.col()+1 );

  track.addToLhcbIDs( LHCbID(id) );
  track.addToLhcbIDs( LHCbID(id1) );
  track.addToLhcbIDs( LHCbID(id2) );
  track.addToLhcbIDs( LHCbID(id3) );
}


//=============================================================================
