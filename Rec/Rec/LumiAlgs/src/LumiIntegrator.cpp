// $Id: LumiIntegrator.cpp,v 1.1 2009-06-12 08:17:50 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// event model
#include "Event/LumiFSR.h"
#include "Event/LumiIntegral.h"
#include "Event/LumiCounters.h"

// local
#include "LumiIntegrator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiIntegrator
//
// 2009-03-16 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( LumiIntegrator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiIntegrator::LumiIntegrator( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_ToolName(""),
    m_count_files(0),
    m_count_events(0),
    m_duplicateFiles(0),
    m_integral(0.),
    m_lumi_scale(0.),
    m_lumi_rel_error(0.),
    m_LumiSum()
{
  declareInterface<ILumiIntegrator>(this);
  
}
//=============================================================================
// Destructor
//=============================================================================
LumiIntegrator::~LumiIntegrator() {} 

//=============================================================================
// Integrate Lumi FSR data
//=============================================================================
StatusCode LumiIntegrator::integrate( LHCb::LumiFSR& fsr ){
  m_LumiSum += fsr;
  return StatusCode::SUCCESS; 
}

StatusCode LumiIntegrator::integrate( LHCb::LumiFSR* fsr ){
  m_LumiSum += (*fsr);
  return StatusCode::SUCCESS; 
}

StatusCode LumiIntegrator::integrate( LHCb::LumiIntegral& fsr ){
  return integrate( &fsr );
}

StatusCode LumiIntegrator::integrate( LHCb::LumiIntegral* fsr ){
  const std::vector< std::string > ids = fsr->fileIDs();
  for(std::vector< std::string >::const_iterator id = ids.begin() ; id != ids.end() ; ++id ){
    if ( m_LumiSum.hasFileID(*id) ) m_duplicateFiles.push_back(*id);
  }  
  m_LumiSum += (*fsr);
  return StatusCode::SUCCESS; 
}

StatusCode LumiIntegrator::integrate( LHCb::LumiIntegral& fsr, std::vector<double> coeff, double f ){
  return integrate( &fsr, coeff, f );
}

StatusCode LumiIntegrator::integrate( LHCb::LumiIntegral* fsr, std::vector<double> coeff, double f ){
  // get fsr and sum into lumi
  LHCb::LumiIntegral::ValuePair defValue ( -1, 0. );
  for ( int key = 0; key < int(coeff.size()) && key < LHCb::LumiCounters::LastGlobal; key++ ) {
    if ( coeff[key] != 0 ) {
      if ( fsr->hasInfo(key) ) {
        LHCb::LumiIntegral::ValuePair value = fsr->info( key, defValue );
        std::string counterName = LHCb::LumiCounters::counterKeyToString( key );
        if ( value.first != -1 ) {
          m_integral += value.second * coeff[key] * f;
          debug() << "counter " << counterName << ": " << key << " " 
                  << value.first << " " << value.second 
                  << " coefficient " << coeff[key] << " integral " << m_integral << endmsg;
        }
      }
    }
  }
  return StatusCode::SUCCESS; 
}

//=============================================================================
// Set absolute scale
//=============================================================================
StatusCode LumiIntegrator::setAbsolute(double scale, double relerror) {
  m_lumi_scale = scale;
  m_lumi_rel_error = relerror;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get final result
//=============================================================================
double LumiIntegrator::lumiValue() {
  return m_integral * m_lumi_scale;
}

//=============================================================================
// Get error on result
//=============================================================================
double LumiIntegrator::lumiError() {
  return m_integral * m_lumi_scale * m_lumi_rel_error;
}

//=============================================================================
// Retrieve the integrated Lumi FSR data
//=============================================================================
const LHCb::LumiIntegral& LumiIntegrator::integral( ) const{
  return m_LumiSum; 
}

//=============================================================================
// Retrieve the duplicates
//=============================================================================
std::vector<std::string> LumiIntegrator::duplicates( ) const{
  return m_duplicateFiles;
}

//=============================================================================
// Count the number of events for a given file
//=============================================================================
void LumiIntegrator::countEvents( ) {
  m_count_events++;
}

//=============================================================================
// Retrieve the event count for a file
//=============================================================================
unsigned long LumiIntegrator::events( ) const{
  return m_count_events; 
}

//=============================================================================
// Check the event count for a file
//=============================================================================
bool LumiIntegrator::checkEvents( ) const{
  return true;
}
