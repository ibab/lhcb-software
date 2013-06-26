//
// $Id:
// ============================================================================
//CLHEP
// Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SystemOfUnits.h"


#include "CaloDet/DeCalorimeter.h"

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
  , m_calo(NULL)
{ declareProperty( "Detector"         , m_nameOfDetector );};
//=============================================================================
// Standard destructor
//=============================================================================
CaloDigitChannel::~CaloDigitChannel() {};
//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloDigitChannel::initialize() {
 
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if(msgLevel(MSG::DEBUG)) debug() << " ====> Initialize" << endmsg;

  if ( "Ecal" ==  m_nameOfDetector) {
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
    m_inputData      =  LHCb::CaloDigitLocation::Ecal ;
    if(msgLevel(MSG::DEBUG)) {
      debug() << "Detector name == ECAL  " << m_nameOfDetector << endmsg; 
      debug() << "Input data :  "          << m_inputData      << endmsg;
    }  
  }

  if ( "Hcal" ==  m_nameOfDetector) {
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
    m_inputData      =  LHCb::CaloDigitLocation::Hcal ;
    if(msgLevel(MSG::DEBUG)) {
      debug() << " Detector Name == HCAL  " << m_nameOfDetector << endmsg; 
      debug() << "Input data  ==> Normal  " << m_inputData      << endmsg;
    }
  }
  
  if ( "Spd" ==  m_nameOfDetector) {
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
    m_inputData      =  LHCb::CaloDigitLocation::Spd ;
    if(msgLevel(MSG::DEBUG)) {
      debug() << "This is the Spd  " <<  m_nameOfDetector << endmsg; 
      debug() << "Input data  ==> Normal  " <<m_inputData << endmsg;
    } 
  }
  
  if ( "Prs" ==  m_nameOfDetector) {
    m_calo    = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
    m_inputData      =  LHCb::CaloDigitLocation::Prs ;
    if(msgLevel(MSG::DEBUG)) {
      debug() << "Input data :  "    << m_inputData      << endmsg;
      debug() << "This is the Prs  " << m_nameOfDetector << endmsg;
    } 
  }
  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitChannel::execute() {
  if(msgLevel(MSG::DEBUG)) debug() << " >>> Execute" << endmsg;
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


  if(!exist<LHCb::CaloDigits>( m_inputData )) return StatusCode::SUCCESS;
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
      index.push_back(id.index());  
      area.push_back(id.area());
      col.push_back(id.col());
      row.push_back(id.row());
      if(msgLevel(MSG::DEBUG)) {
        debug() << " x " << x  << endmsg;
        debug() << " y  " << y << endmsg;
        debug() << " id  "  << id << endmsg; 
        debug() << " col "  << col << endmsg;
        debug() << " row "  << row << endmsg;
        debug() << "The Number of Digits is : " << nbDigit<< endmsg;
      } 
    }
  }
  tuple->farray("index" ,index,"Ndigits",7000 );
  tuple->farray("x",x,"Ndigits",7000);
  tuple->farray("y",y,"Ndigits",7000);
  tuple->farray("z",z,"Ndigits",7000);
  tuple->farray("energy",energy,"Ndigits",7000);
  tuple->farray("area", area,"Ndigits",7000);
  tuple->farray("col", col,"Ndigits",7000);
  tuple->farray("row", row,"Ndigits",7000);
  
  
  sc = tuple->write();
  info() << "written tuple" << endmsg ; 
  
  return sc ; 
  
};
