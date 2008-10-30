//
// $Id:
// ============================================================================
//CLHEP
// Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"


#include "CaloDet/DeCalorimeter.h"//Test

//#include "Kernel/CaloCellID.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"



// Event/CaloEvent
#include "Event/MCCaloHit.h" 
#include "Event/MCCaloDigit.h" 
#include "Event/CaloDigit.h" 

#include "Event/MCHeader.h"

// local
#include "CaloDigitChannel.h"


using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class CaloDigitChannel
//
// 28/03/2007 : Yasmine Amhis
//-----------------------------------------------------------------------------
DECLARE_ALGORITHM_FACTORY( CaloDigitChannel );

// Standard creator
CaloDigitChannel::CaloDigitChannel( const std::string& name, 
					ISvcLocator* pSvcLocator) 
  : GaudiTupleAlg ( name , pSvcLocator            )
{ declareProperty( "Detector"         , m_nameOfDetector );};
//=============================================================================
// Standard destructor
//=============================================================================
CaloDigitChannel::~CaloDigitChannel() {};
//=============================================================================
//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloDigitChannel::initialize() {
 
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  debug() << " ====> Initialize" << endreq;
  
  bool normal =  "TAE" != context() && "" == rootInTES();
  if ( "Ecal" ==  m_nameOfDetector) {
    debug() << "Detector name == ECAL  " <<  m_nameOfDetector << endmsg; 
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    if ( normal ) {
      m_inputData      =  LHCb::CaloDigitLocation::Ecal ;
 
      debug() << "Input data  ==> Normal  " <<m_inputData    <<endmsg;
    }
    
    if(! normal)  {
      m_inputData      =  rootInTES() + LHCb::CaloDigitLocation::Ecal ;
      debug() << "Input  data  ==> TAE Context  " << m_inputData     << endmsg; }
  }
  
  if ( "Hcal" ==  m_nameOfDetector) {
    debug() << " Detector Name == HCAL  " <<  m_nameOfDetector << endmsg; 
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    if ( normal ) {
      m_inputData      =  LHCb::CaloDigitLocation::Hcal ;
      
      debug() << "Input data  ==> Normal  " <<m_inputData    <<endmsg;
    }
    
    if(! normal)  {
      m_inputData      =  rootInTES() + LHCb::CaloDigitLocation::Hcal ;
      debug () << "Input   data  ==> TAE Context  " << m_inputData     << endmsg; }
  }  
  
  
  if ( "Spd" ==  m_nameOfDetector) {
    debug() << "This is the Spd  " <<  m_nameOfDetector << endmsg; 
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
    
    if ( normal ) {
      m_inputData      =  LHCb::CaloDigitLocation::Spd ;
      debug() << "Input data  ==> Normal  " <<m_inputData    <<endmsg;
    }
    
    
    if(! normal)  {
      m_inputData  =  rootInTES() + LHCb::CaloDigitLocation::Spd ;
      debug() << "Input     data  ==> TAE Context  " << m_inputData     << endmsg; }
  }  
  
  
  if ( "Prs" ==  m_nameOfDetector) {
    debug() << "This is the ECAL  " <<  m_nameOfDetector << endmsg; 
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    if ( normal ) {
      m_inputData      =  LHCb::CaloDigitLocation::Prs ;
      debug() << "Input data ==> Normal  " <<m_inputData    <<endmsg;
    }
    
    
    if(! normal)  {
      m_inputData      =  rootInTES() + LHCb::CaloDigitLocation::Prs ;
      debug() << "Input data  ==> TAE Context  " << m_inputData     << endmsg; }
  }  
  
  
  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitChannel::execute() {
  debug() << " >>> Execute" << endreq;
  StatusCode sc = StatusCode::SUCCESS ;
  Tuple tuple = nTuple("Channel");
  double nbDigit  = 0.;
  double energyDigit=0;
  std::vector<double> energy;
  std::vector<double> index;
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> z;
  std::vector<double> cellGain;
  std::vector<double> cellTime; 
  std::vector<double> area;
  std::vector<double> col;
  std::vector<double> row;
  if(!exist<LHCb::CaloDigits>( m_inputData ))return StatusCode::SUCCESS;
  
  LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( m_inputData );
  
  if( 0 != digits ) { 
    LHCb::CaloDigits::const_iterator dig;  // LHCb Calo Digits
    
    for ( dig = digits->begin() ; digits->end() != dig ; ++dig ) {
      nbDigit  += 1.;
      LHCb::CaloCellID id    = (*dig)->cellID(); 
      
     
      energyDigit = (*dig)->e();
      
      energy.push_back(energyDigit );
      x.push_back(m_calo->cellX(id));
      y.push_back(m_calo->cellY(id));
      z.push_back(m_calo->cellZ(id));
      cellGain.push_back(m_calo->cellGain(id));
      cellTime.push_back(m_calo->cellTime(id));
      index.push_back(id.index());  
      area.push_back(id.area());
      col.push_back(id.col());
      row.push_back(id.row());
      debug() << " x " << x  << endmsg;
      debug() << " y  " << y << endmsg;
      debug() << " id  "  << id << endmsg; 
      debug() << " col "  << col << endmsg;
      debug() << " row "  << row << endmsg;
      debug() << "The Number of Digits is : " << nbDigit<< endmsg;
    }
    
    
  }
  tuple->array("index" ,index );
  tuple->array("x",x);
  tuple->array("y",y);
  tuple->array("z",z);
  tuple->array("cellTime",cellTime);
  tuple->array("cellGain",cellGain);
  tuple->array("energy",energy);
  tuple->array("area", area);
  tuple->array("col", col);
  tuple->array("row", row);
  sc = tuple->write();
  always() << "written tuple" << endmsg ; 
  
  return sc ; 
  
};
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloDigitChannel::finalize() {
  
  debug() << " ===> Finalize" << endreq;
  return GaudiTupleAlg::finalize(); 
}

