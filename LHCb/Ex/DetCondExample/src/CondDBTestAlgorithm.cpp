// $Id: CondDBTestAlgorithm.cpp,v 1.11 2005-08-31 16:02:49 marcocle Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDetDataSvc.h"

#include "DetDesc/Condition.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/DetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/LVolume.h"

#include "DetDesc/IUpdateManagerSvc.h"

// local
#include "CondDBTestAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CondDBTestAlgorithm
//
// 2005-06-23 : Marco Clemencic
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CondDBTestAlgorithm>          s_factory ;
const        IAlgFactory& CondDBTestAlgorithmFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CondDBTestAlgorithm::CondDBTestAlgorithm( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_LHCb_cond(NULL),
    m_Hcal_cond(NULL),
    m_LHCb_temp(-1e20),
    m_Hcal_temp(-1e20),
    m_avg_temp(-1e20),
    m_evtCount(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
CondDBTestAlgorithm::~CondDBTestAlgorithm() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CondDBTestAlgorithm::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  info() << "Retrieve the LHCb detector /dd/Structure/LHCb" << endmsg;
  SmartDataPtr<DetectorElement> lhcb( detSvc(), "/dd/Structure/LHCb" );

  
  info() << "*** register conditions ***" << endmsg;
  try {
    m_ums = svc<IUpdateManagerSvc>("UpdateManagerSvc",true);
    
    m_ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&CondDBTestAlgorithm::i_updateCacheLHCb);
    m_ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",&CondDBTestAlgorithm::i_updateCache);
    m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&CondDBTestAlgorithm::i_updateCacheHcal);
    m_ums->registerCondition(this,"/dd/SlowControl/Hcal/scHcal",&CondDBTestAlgorithm::i_updateCache);
    m_ums->registerCondition(this,"/dd/SlowControl/LHCb/scLHCb",NULL);
    m_ums->registerCondition(this,"/dd/Properties/TestFunction");
    m_ums->registerCondition(this,"/dd/Alignment/Velo/Module01");
    m_ums->registerCondition(this,"/dd/Alignment/Velo/Module12");
    m_ums->update(this);
  }
  catch (GaudiException){
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CondDBTestAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;

  info() << "-------------------------------------" << endmsg;
  info() << "Event" << dynamic_cast<IDetDataSvc*>(detSvc())->eventTime() << endmsg;
  info() << "Temperature check: LHCb = " << m_LHCb_temp << endmsg;
  info() << "                   Hcal = " << m_Hcal_temp << endmsg;
  info() << "                   avg  = " << m_avg_temp << endmsg;
  info() << *(get<TabulatedProperty>(detSvc(),"/dd/Properties/TestFunction")) << endmsg;
  info() << "-------------------------------------" << endmsg;
  
  ++m_evtCount;

  StatusCode sc;
  
  // Retrieve the LHCb detector element
  info() << "Retrieve the LHCb detector /dd/Structure/LHCb" << endmsg;
  SmartDataPtr<DetectorElement> lhcb( detSvc(), "/dd/Structure/LHCb" );
  sc = i_analyse( lhcb );
  if( !sc.isSuccess() ) return sc;

  // Retrieve alignment for the LHCb detector
  info() << "Retrieve the alignment Condition for the LHCb detector" << endmsg;
  Condition* alLHCb = lhcb->condition("Alignment");
  sc = i_analyse( alLHCb );
  if( !sc.isSuccess() ) return sc;

  // Retrieve slowControl for the LHCb detector
  info() << "Retrieve the slowControl Condition for the LHCb detector" << endmsg;
  Condition* scLHCb = lhcb->condition("SlowControl");
  sc = i_analyse( scLHCb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;
  if (m_evtCount == 4){
    // change the value and trigger a re-cache
    info() << "Forcing a re-cache..." << endmsg;
    scLHCb->param<double>("Temperature") = scLHCb->param<double>("Temperature")/2.0;
    info() << "Temperature set to " << scLHCb->param<double>("Temperature") << endmsg;
    info() << "UpdateManagerSvc::invalidate() ==> Condition data are considered no more valid" << endmsg;
    m_ums->invalidate(scLHCb);
  } else if (m_evtCount == 5){
    // trigger a re-load from CondDB
    info() << "Forcing a re-load from CondDB..." << endmsg;
    info() << "ValidDataObject::forceUpdateMode() + UpdateManagerSvc::invalidate()" << endmsg;
    scLHCb->forceUpdateMode();
    m_ums->invalidate(scLHCb);
  }

  // Retrieve logical volume for the LHCb detector
  info() << "Retrieve the logical volume for the LHCb detector" << endmsg;
  LVolume* lvLHCb = dynamic_cast<LVolume*>( (ILVolume*)(lhcb->geometry()->lvolume()) );
  sc = i_analyse( lvLHCb );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve the Hcal detector element
  info() << "Retrieve the Hcal detector /dd/Structure/LHCb/Hcal" << endmsg;
  SmartDataPtr<DetectorElement> hcal( detSvc(), "/dd/Structure/LHCb/Hcal" );
  sc = i_analyse( hcal );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve slowControl for the Hcal detector
  info() << "Retrieve the slowControl Condition for the Hcal detector" << endmsg;
  Condition* scHcal = hcal->condition("SlowControl");
  sc = i_analyse( scHcal );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Retrieve the Dummy detector element
  info() << "Test DummyDE detector /dd/Structure/LHCb/Dummy" << endmsg;
  SmartDataPtr<DetectorElement> dummy( detSvc(), "/dd/Structure/LHCb/Dummy" );
  sc = i_analyse( dummy );
  if( !sc.isSuccess() ) return StatusCode::FAILURE;

  // Test alignment (and cool::ChannelId)
  info() << "Test AlignmentConditions detector /dd/Alignment/Velo/ModuleXX" << endmsg;
  AlignmentCondition* m01 = getDet<AlignmentCondition>("/dd/Alignment/Velo/Module01");
  AlignmentCondition* m12 = getDet<AlignmentCondition>("/dd/Alignment/Velo/Module12");

  //  info() << m01->name() << " dPosXYZ = " << m01->param<std::vector<double> >("dPosXYZ")
  //         << " transformation = \n"
  info() << m01->name() << ":\n"
         << m01->printParams()
         << "\n transformation =  \n"
         << m01->matrix()[0][0] << " " << m01->matrix()[0][1] << " " << m01->matrix()[0][2] << " " << m01->matrix()[0][3] << "\n"
         << m01->matrix()[1][0] << " " << m01->matrix()[1][1] << " " << m01->matrix()[1][2] << " " << m01->matrix()[1][3] << "\n"
         << m01->matrix()[2][0] << " " << m01->matrix()[2][1] << " " << m01->matrix()[2][2] << " " << m01->matrix()[2][3] << "\n"
         << m01->matrix()[3][0] << " " << m01->matrix()[3][1] << " " << m01->matrix()[3][2] << " " << m01->matrix()[3][3] << "\n"
         << endmsg;

  info() << m12->name() << ":\n"
         << m12->printParams()
         << "\n transformation =  \n"
         << m12->matrix()[0][0] << " " << m12->matrix()[0][1] << " " << m12->matrix()[0][2] << " " << m12->matrix()[0][3] << "\n"
         << m12->matrix()[1][0] << " " << m12->matrix()[1][1] << " " << m12->matrix()[1][2] << " " << m12->matrix()[1][3] << "\n"
         << m12->matrix()[2][0] << " " << m12->matrix()[2][1] << " " << m12->matrix()[2][2] << " " << m12->matrix()[2][3] << "\n"
         << m12->matrix()[3][0] << " " << m12->matrix()[3][1] << " " << m12->matrix()[3][2] << " " << m12->matrix()[3][3] << "\n"
         << endmsg;

  // Event processing completed
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CondDBTestAlgorithm::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=========================================================================
//  Anlyze the contnt of a DataObject
//=========================================================================
StatusCode CondDBTestAlgorithm::i_analyse( DataObject* pObj ) {
  if( 0 != pObj ) {
    info() << "Successfully retrieved" << endmsg;
  } else {
    error() << "Could not retrieve this DataObject" << endmsg;
    return StatusCode::FAILURE;
  }

  ValidDataObject *pVDO= dynamic_cast<ValidDataObject*>( pObj );

  if( 0 != pVDO ) {
    info() << "Object is valid in ["
           << std::dec << pVDO->validSince().absoluteTime()
      //<< "(0x" << std::hex << pVDO->validSince().absoluteTime() << "),"
           << "," 
           << std::dec << pVDO->validTill().absoluteTime()
      //<< "(0x" << std::hex << pVDO->validTill().absoluteTime() << ")"
           << "[" << endmsg;
    
    if ( pVDO->isValid( svc<IDetDataSvc>("DetectorDataSvc")->eventTime() ) ) {
      info() << "(currently valid)" << endmsg;
    }
    else {
      error() << "Object is not valid at the current event time! (" << svc<IDetDataSvc>("DetectorDataSvc")->eventTime()
              << ")" << endmsg;
      return StatusCode::FAILURE;
    }
    ParamValidDataObject *pPVDO= dynamic_cast<ParamValidDataObject*>( pObj );
    if ( 0 != pPVDO ){
      info() << "It is a ParamValidDataObject with " << pPVDO->paramNames().size() << " params" << endmsg;
      info() << "Parameters:\n" << pPVDO->printParams() << endmsg;
    }
  } else {
    info() << "(not a ValidDataObject)" << endmsg;
  }
  
  return StatusCode::SUCCESS;  
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBTestAlgorithm::i_updateCacheLHCb ( ) {
  info() << "i_updateCacheLHCb() called!" << endmsg;
  if (m_LHCb_cond == NULL){
    try {
      m_LHCb_cond = getDet<Condition>("/dd/SlowControl/LHCb/scLHCb");
    }
    catch (GaudiException){
      return StatusCode::FAILURE;
    }
  }
  m_LHCb_temp = m_LHCb_cond->param<double>("Temperature");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBTestAlgorithm::i_updateCacheHcal ( ) {
  info() << "i_updateCacheHcal() called!" << endmsg;
  if (m_Hcal_cond == NULL){
    try {
      m_Hcal_cond = getDet<Condition>("/dd/SlowControl/Hcal/scHcal");
    }
    catch (GaudiException){
      return StatusCode::FAILURE;
    }
  }
  m_Hcal_temp = m_Hcal_cond->param<double>("Temperature");
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode CondDBTestAlgorithm::i_updateCache ( ) {
  info() << "i_updateCache() called!" << endmsg;

  if (m_LHCb_cond == NULL){
    try {
      m_LHCb_cond = getDet<Condition>("/dd/SlowControl/LHCb/scLHCb");
    }
    catch (GaudiException){
      return StatusCode::FAILURE;
    }
  }

  if (m_Hcal_cond == NULL){
    try {
      m_Hcal_cond = getDet<Condition>("/dd/SlowControl/Hcal/scHcal");
    }
    catch (GaudiException){
      return StatusCode::FAILURE;
    }
  }
  m_avg_temp = (m_LHCb_cond->param<double>("Temperature")+m_Hcal_cond->param<double>("Temperature"))/2.;
  return StatusCode::SUCCESS;
}
//=============================================================================
