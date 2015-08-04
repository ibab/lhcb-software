// Include files 

// from Gaudi
#include "GaudiAlg/Tuples.h"
#include "Event/MCHit.h"

// local
#include "VeloDigiMoni.h"

// Velo Event Model
#include "Event/VeloDigit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloDigiMoni
//
// 2005-11-18 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloDigiMoni )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloDigiMoni::VeloDigiMoni( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator ),
    m_digitContainer ( LHCb::VeloDigitLocation::Default ),
    m_veloDigits ( 0 ),
    m_numberOfEvents ( 0. ),
    m_nVeloDigits ( 0. ),
    m_nVeloDigits2 ( 0. )
{
  declareProperty( "PrintInfo",  m_printInfo = false );
  declareProperty( "TestDigits", m_testDigits = true );
}
//=============================================================================
// Destructor
//=============================================================================
VeloDigiMoni::~VeloDigiMoni() {}
//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloDigiMoni::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  //
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  setHistoTopDir("Velo/");  
  //
  return (StatusCode::SUCCESS);
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloDigiMoni::execute() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  //
  m_numberOfEvents++;
  StatusCode sc=getData();
  if(sc){ 
    sc=testVeloDigit();
    if(sc.isFailure()) return ( sc );
  }else{
    return ( sc );
  }
  //
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloDigiMoni::finalize() {

  if( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  //
  if(m_numberOfEvents!=0){
  m_nVeloDigits/=m_numberOfEvents;
  m_nVeloDigits2/=m_numberOfEvents;
  double errnVeloDigits=
   sqrt((m_nVeloDigits2-(m_nVeloDigits*m_nVeloDigits))/m_numberOfEvents);
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "              - VeloDigiMoni table -                  " <<endmsg;
  info()<< "------------------------------------------------------" <<endmsg;
  info()<< "| Number of VeloDigits/Event: " << m_nVeloDigits << " +/- " 
      << errnVeloDigits <<endmsg;
  info()<< "------------------------------------------------------" <<endmsg;
  }else{
    error()<< " ==> Zero events processed! " <<endmsg;
  }
  //
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
StatusCode VeloDigiMoni::getData()
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> getData() " <<endmsg;
  //
  m_veloDigits=getIfExists<LHCb::VeloDigits>(m_digitContainer);
  if( NULL == m_veloDigits ){
    error()<< " There is no VeloDigits at " << m_digitContainer <<endmsg;
    return (StatusCode::FAILURE);
  }
  //
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> Number of VeloDigits found: "
                                    << m_veloDigits->size() <<endmsg;
  //
  return (StatusCode::SUCCESS);
}
//==============================================================================
StatusCode VeloDigiMoni::testVeloDigit()
{
  if( msgLevel(MSG::DEBUG) ) debug()<< " ==> testVeloDigits() " <<endmsg;
  //
  double size=double(m_veloDigits->size());
  m_nVeloDigits+=size;
  m_nVeloDigits2+=size*size;
  //
  plot(m_veloDigits->size(), 200, 
       "Number of VeloDigits per event ",
       0., 2000., 100);
  LHCb::VeloDigits::iterator digIt;
  for(digIt=m_veloDigits->begin(); digIt!=m_veloDigits->end(); digIt++){
    if(m_printInfo){
      info()<< " Test VeloDigit: " <<endmsg;
      info()<< " Sensor number: " << (*digIt)->sensor() <<endmsg;
      info()<< " Strip number: " << (*digIt)->strip() <<endmsg;
      info()<< " ADC value: " << (*digIt)->adcValue() <<endmsg;
    }
    plot2D((*digIt)->sensor(), (*digIt)->strip(), 201,
           "Sensor and strip number",
           0., 132., 0., 5000., 132, 100);
    plot((*digIt)->adcValue(), 202,
         "Raw ADC counts",
         0., 256., 256);
  // Test the VeloDigits2MCHits associator (linker)
    asctTool associator(evtSvc(), LHCb::VeloDigitLocation::Default+"2MCHits" );
      const Table* table=associator.direct();
      if(!table){
        error()<< "Empty table with associations" <<endmsg;
        return ( StatusCode::FAILURE );
      }
    //
    Range range1=table->relations(*digIt);
    iterator it;
    if(range1.size()!=0){
      if( msgLevel(MSG::DEBUG) ) debug()<< "Hit(s) associated to VeloDigit from strip: "
                                        << (*digIt)->strip() <<endmsg;
    }
    for(it=range1.begin(); it!=range1.end(); it++){
      const LHCb::MCHit* aHit=it->to();
      double energy=aHit->energy();
      plot(energy, 203,
           "Energy deposited in Si [eV] from VeloDigit2MCHitLinker",
           0., 300000., 100);
      if( msgLevel(MSG::DEBUG) ) debug()<< "energy from hit" << energy <<endmsg;
    }
  }
  //
  return (StatusCode::SUCCESS);
}
//==============================================================================
