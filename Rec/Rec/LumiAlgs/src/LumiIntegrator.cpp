// Include files 

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
DECLARE_TOOL_FACTORY( LumiIntegrator )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiIntegrator::LumiIntegrator( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_ToolName(""),
    m_count_events(0),
    m_duplicateFiles(0),
    m_integral(0.),
    m_lumi_scale(0.),
    m_lumi_rel_error(0.),
    m_LumiSum()
{
  declareInterface<ILumiIntegrator>(this);
  m_ToolName = name;
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
// Accumulate mu values per file
//=============================================================================
StatusCode LumiIntegrator::accumulate_mu( LHCb::LumiIntegral& fsr, LHCb::TimeSpanFSR* 
                                          timeSpanFSR, int mukey, std::vector<double> coeff, double f ){
  return accumulate_mu( &fsr, timeSpanFSR, mukey, coeff, f );
}

StatusCode LumiIntegrator::accumulate_mu( LHCb::LumiIntegral* fsr, LHCb::TimeSpanFSR* timeSpanFSR, int mukey, \
                                          std::vector<double> coeff, double f ){
  // collect mu
  ILumiIntegrator::muTuple mT;

  mT.run = 0;  
  mT.guid = "NF";
  mT.time0 = 0;
  mT.time1 = 0;
  mT.deltaLumi = 0;
  mT.norm = 0;
  mT.mu = 0;

  // run and file-id
  const std::vector<unsigned int> runNumbers = fsr->runNumbers();
  const std::vector<std::string> fileIDs = fsr->fileIDs();
  if ( runNumbers.size() > 0 ) {
    mT.run = runNumbers[0];
    mT.guid = fileIDs[0];
  }
  // get fsr and get lumi and mu for this piece
  double munorm = 0;
  std::string thisCounter("NC");
  //  int thisCoef(0);
  LHCb::LumiIntegral::ValuePair defValue ( -1, 0. );
  for ( int key = 0; key < int(coeff.size()) && key < LHCb::LumiCounters::LastGlobal; key++ ) {
    if ( fsr->hasInfo(key) ) {
      LHCb::LumiIntegral::ValuePair value = fsr->info( key, defValue );
      std::string counterName = LHCb::LumiCounters::counterKeyToString( key );
      // collect all mu's
      mT.valuePairs.push_back(value);
      mT.keys.push_back(key);
      // collect chosen mu
      if ( coeff[key] != 0 || mukey == key) {
        if ( value.first != -1 ) {
          mT.deltaLumi += value.second * coeff[key] * f;
          if ( mukey == key ) {
            thisCounter = counterName;
            //            thisCoef = coeff[key];
            munorm = value.first;
            mT.norm = munorm;
            if ( munorm > 0 ) {
              mT.mu = value.second / munorm;
            } else {
              mT.mu = 0;
            }
          }
        }
      }
    }
  }
  // time span
  if ( timeSpanFSR != NULL ) {
    mT.time0 = timeSpanFSR->earliest();
    mT.time1 = timeSpanFSR->latest();
  }
  // accumulate
  m_muTuple.push_back(mT);
  // printout
  if ( m_ToolName.find("RawLumiIntegrator") == std::string::npos ) info() << "MU";
  else info() << "RAW";
  info() << ": RUN " << mT.run  << " GUID " << mT.guid << " " 
         << "T " << mT.time0 << "-" << mT.time1 << " "
         << "dL " << mT.deltaLumi << " N " << mT.norm << " MU " << mT.mu << " " 
         << endmsg;

  return StatusCode::SUCCESS; 
}

std::vector<ILumiIntegrator::muTuple> LumiIntegrator::muValues( ) {
  return m_muTuple;
}

//=============================================================================
// Set/get absolute scale
//=============================================================================
StatusCode LumiIntegrator::setAbsolute(double scale, double relerror) {
  m_lumi_scale = scale;
  m_lumi_rel_error = relerror;
  return StatusCode::SUCCESS;
}
double  LumiIntegrator::absolute() {
  return m_lumi_scale;
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
// Get values from mu calculation
//=============================================================================
ulonglong LumiIntegrator::muFiles() { return m_muTuple.size(); }
ulonglong LumiIntegrator::muKeys(unsigned long index) { return m_muTuple[index].keys.size(); }
ulonglong LumiIntegrator::muRun(unsigned long index) { return (ulonglong)m_muTuple[index].run; }
std::string LumiIntegrator::muGuid(unsigned long index) { return m_muTuple[index].guid; }
ulonglong LumiIntegrator::muTime0(unsigned long index) { return m_muTuple[index].time0; }
ulonglong LumiIntegrator::muTime1(unsigned long index) { return m_muTuple[index].time1; }
double LumiIntegrator::muDeltaLumi(unsigned long index) { return m_muTuple[index].deltaLumi; }
double LumiIntegrator::muNorm(unsigned long index) { return m_muTuple[index].norm; }
double LumiIntegrator::muMu(unsigned long index) { return m_muTuple[index].mu; }
int LumiIntegrator::muCounter(unsigned long index, unsigned long key) { return m_muTuple[index].keys[key]; }
std::string LumiIntegrator::muCounterName(unsigned long index, unsigned long key) { 
  return LHCb::LumiCounters::counterKeyToString( m_muTuple[index].keys[key] ); 
}
double LumiIntegrator::muCounterNorm(unsigned long index, unsigned long key) { 
  return m_muTuple[index].valuePairs[key].first; 
}
double LumiIntegrator::muCounterValue(unsigned long index, unsigned long key) { 
  return m_muTuple[index].valuePairs[key].second; 
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
