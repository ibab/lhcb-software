#include "GaudiAlg/GaudiAlgorithm.h"
#include "IOTCalibrationIOTool.h"
#include <string>

class OTCalibrationIO : public GaudiAlgorithm
{
public:

  OTCalibrationIO(const std::string& name,ISvcLocator* pSvcLocator) ;
  virtual ~OTCalibrationIO() ;
  virtual StatusCode initialize() ;
  virtual StatusCode execute() ;
  virtual StatusCode finalize() ;

private:

  std::string m_inputCalibFile ;
  std::string m_outputCalibFile ;
  const IOTCalibrationIOTool* m_iotool ;

} ;

#include "GaudiKernel/AlgFactory.h"
DECLARE_ALGORITHM_FACTORY( OTCalibrationIO )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTCalibrationIO::OTCalibrationIO( const std::string& name, ISvcLocator* pSvcLocator ) 
  :
  GaudiAlgorithm( name , pSvcLocator ),
  m_inputCalibFile("otcalibration.txt"),
  m_outputCalibFile("")
{
  declareProperty( "InputFileName",m_inputCalibFile) ;
  declareProperty( "OutputFileName",m_outputCalibFile) ;
}

//=============================================================================
// Destructor
//=============================================================================
OTCalibrationIO::~OTCalibrationIO() {}

//=============================================================================
// ::initialize()
//=============================================================================
StatusCode OTCalibrationIO::initialize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isSuccess() ) {
    m_iotool = tool<IOTCalibrationIOTool>("OTCalibrationIOTool",this) ;
    sc = m_iotool->readFromFile( m_inputCalibFile ) ; 
  }
  return sc; 
}

//=============================================================================
// ::execute()
//=============================================================================
StatusCode OTCalibrationIO::execute()
{
  StatusCode sc = StatusCode::SUCCESS ;

  return sc ;
}


//=============================================================================
// ::finalize()
//=============================================================================
StatusCode OTCalibrationIO::finalize()
{
  // Mandatory initialization of GaudiAlgorithm
  StatusCode sc = StatusCode::SUCCESS ;
  if( !m_outputCalibFile.empty() ) 
    sc = m_iotool->writeToFile( m_outputCalibFile ) ;
  sc = GaudiAlgorithm::finalize() ;
  return sc ;
}

