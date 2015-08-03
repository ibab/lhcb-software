// Include files 

// from Gaudi
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloDigitFilterAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloDigitFilterAlg
//
// 2010-12-21 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloDigitFilterAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloDigitFilterAlg::CaloDigitFilterAlg( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ), m_filter(0)
{
  declareProperty("EcalFilter" ,m_ecal = 0x3); // 1 = Mask , 2=Offset , 3 = both, 0 = none
  declareProperty("HcalFilter" ,m_hcal = 0x3); // 1 = Mask , 2=Offset , 3 = both, 0 = none
  declareProperty("PrsFilter"  ,m_prs  = 0x3); // 1 = Mask , 2=Offset , 3 = both, 0 = none
  declareProperty("SpdFilter"  ,m_spd  = 0x1); // 1 = Mask , 2=Offset , 3 = both, 0 = none
}
//=============================================================================
// Destructor
//=============================================================================
CaloDigitFilterAlg::~CaloDigitFilterAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloDigitFilterAlg::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  m_filter = tool<ICaloDigitFilterTool>("CaloDigitFilterTool","FilterTool");
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloDigitFilterAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;



  if( mask(m_ecal) || offset(m_ecal) )
    if( !m_filter->cleanDigits("Ecal",offset(m_ecal),mask(m_ecal)))
      Warning("Ecal digits filtering failed",StatusCode::SUCCESS).ignore();

  if( mask(m_hcal) || offset(m_hcal) )
    if( !m_filter->cleanDigits("Hcal",offset(m_hcal),mask(m_hcal)))
      Warning("Hcal digits filtering failed",StatusCode::SUCCESS).ignore();
  
  if( mask(m_prs) || offset(m_prs) )
    if( !m_filter->cleanDigits("Prs",offset(m_prs),mask(m_prs)))
      Warning("Prs digit filtering failed",StatusCode::SUCCESS).ignore();
  
  if( mask(m_spd) || offset(m_spd) )
    if( !m_filter->cleanDigits("Spd",offset(m_spd),mask(m_spd)))
      Warning("Spd digit filtering failed",StatusCode::SUCCESS).ignore();


  return StatusCode::SUCCESS;
}

//=============================================================================

  
