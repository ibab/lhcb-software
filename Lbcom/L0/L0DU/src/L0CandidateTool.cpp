// $Id: L0CandidateTool.cpp,v 1.1 2006-02-18 11:29:36 ocallot Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
//
//
// ============================================================================

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
// Detector Element
#include "CaloDet/DeCalorimeter.h"
#include "DetDesc/Condition.h"
// local
#include "L0CandidateTool.h"


/** @file 
 *  Implementation file for class : L0CandidateTool
 *  
 *  purpose : extract relevant information from L0ProcessorData
 *
 * 2006-02-17 : Olivier Deschamps
 */


// ============================================================================
/*
 *   Declaration of the Tool Factory, needed for instantiation
 */
// ============================================================================
DECLARE_TOOL_FACTORY( L0CandidateTool )

// ============================================================================
L0CandidateTool::L0CandidateTool 
( const std::string& type   , 
  const std::string& name   ,
  const IInterface*  parent ) 
  : GaudiTool ( type, name , parent )
  , m_dataLocation()
  , m_adcConvert(true)
  , m_caloEtScale(20.*MeV)
  , m_muonPtScale(40.*MeV)
{
  declareProperty( "DataLocation"        , m_dataLocation );
  declareProperty( "adcConvert"          , m_adcConvert   );
  declareInterface<IL0Candidate> ( this ) ;  
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
L0CandidateTool::~L0CandidateTool () {} ;
// ============================================================================




StatusCode L0CandidateTool::initialize () 
{

  m_muonPtScale = 40.*MeV ;// ADC to MeV (hardcoded -- to be extracted from CondDB)
  //  m_caloEtScale = 20.*MeV ;// ADC to MeV (hardcoded -- to be extracted from CondDB)
  DeCalorimeter* ecal = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal ); 
    Condition* gain = ecal->condition( "Gain" );
    if ( 0 == gain ) return Error( "Condition 'Gain' not found in Ecal" );
    if ( gain->exists( "L0EtBin" ) ) {
    m_caloEtScale = gain->paramAsDouble( "L0EtBin" ) ;
    } else {
    return Error( "Parameter 'L0EtBin' not found in Ecal 'Gain'" );
    }
  return StatusCode::SUCCESS ;
};




// ============================================================================
void L0CandidateTool::setUp(const std::string& dataLocation ){
  setUp(dataLocation , false);
}
// ============================================================================
void L0CandidateTool::setUp(const std::string& dataLocation , bool convert){
  if( !m_dataLocation.empty())
    warning() << "dataLocation property for L0CandidateTool has been overwritten :" 
              << dataLocation 
              << endreq;
  m_dataLocation = dataLocation;
  m_adcConvert   = convert;
}


// ============================================================================
double L0CandidateTool::value( const unsigned int data[L0DUBase::Index::Size] ){

  double value = double( digitValue(data) ) ;
  if(m_adcConvert) value *= scale( data );
  return value;
}
// ============================================================================
double L0CandidateTool::scale(const unsigned int data[L0DUBase::Index::Size] ) {

  if( L0DUBase::Type::CaloEt  == data[ L0DUBase::Index::Scale ] )return m_caloEtScale;
  if( L0DUBase::Type::MuonPt  == data[ L0DUBase::Index::Scale ] )return m_muonPtScale;
  if( L0DUBase::Type::Digit   == data[ L0DUBase::Index::Scale ] )return 1;
  error() << "No defined type for " << data << endreq;
  return 0;
}
// ============================================================================
unsigned int L0CandidateTool::digitValue( const unsigned int data[L0DUBase::Index::Size] ){
  

  m_dataContainer = get<LHCb::L0ProcessorDatas>( m_dataLocation );


  LHCb::L0ProcessorData* fiber = m_dataContainer->object( data[ L0DUBase::Index::Fiber ]  )  ;
  
  if( 0 == fiber ){ 
    error() << "Data ( " << data << " ) not found in the container located at " << m_dataLocation << endreq;
    return 0;
  }
  
  unsigned int value = ( ( fiber->word()   &  data[L0DUBase::Index::Mask]  ) >> data[L0DUBase::Index::Shift]  ) ; 

  if( L0DUBase::Fiber::Empty != data[ L0DUBase::Index::Fiber2 ]  ) {
    LHCb::L0ProcessorData* fiber2= m_dataContainer->object( data[ L0DUBase::Index::Fiber2 ]  )  ;
    if( 0 == fiber2 ){ 
      error() << "Data ( " << data << " ) not found in the container located at " << m_dataLocation << endreq;
      return 0;
    }
    unsigned int value2 =( ( fiber2->word() & data[ L0DUBase::Index::Mask2 ]  )  >> data[ L0DUBase::Index::Shift2 ]) ;
    value |=  (value2  << data[ L0DUBase::Index::Offset ]  ); 
  }
  return value ;
}

