// $Id: HepMCJets2HepMCJets.cpp,v 1.1 2009-12-14 12:34:33 cocov Exp $
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IMatterVeto.h"           
// ============================================================================
// Event 
// ============================================================================
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"
#include <VeloDet/DeVeloSensor.h>
#include <VeloDet/DeVeloRType.h>
#include "VeloDet/DeVelo.h"
#include "DetDesc/Condition.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;


class MatterVetoTool
    : public virtual IMatterVeto
    , public         GaudiTool
{
  // ========================================================================
  /// friend factory for instantiation 
  friend class ToolFactory<MatterVetoTool> ;
public: 

  // ========================================================================
  /// standard initialization of the tool
  StatusCode initialize() ;
  StatusCode finalize() { return GaudiTool::finalize ();};
  bool isInMatter( const  Gaudi::XYZPoint &point ) const ;

protected:
  // ========================================================================
  /// standard constructor
  MatterVetoTool 
  ( const std::string& type   , ///< tool type ??? 
    const std::string& name   , ///< tool name 
    const IInterface*  parent ) 
    : GaudiTool ( type, name , parent )
  {
    //
    declareInterface<IMatterVeto>(this);
    //
    declareProperty("UseEnlargedMatterVeto", m_useEnlargedMatterVeto=false);
    //
    //this->registering();
    //this->InitialiseGeoInfo();
    
  } 
  /// virtual protected destructor 
  virtual ~MatterVetoTool() {} 
  // ========================================================================
private:
  // flag to enable the enlarged matter veto
  bool m_useEnlargedMatterVeto;
  // ========================================================================
  // default constructor is disabled 
  MatterVetoTool () ;
  // copy constructor is disabled 
  MatterVetoTool ( const MatterVetoTool& ) ;
  // assignement operator is disabled 
  MatterVetoTool & operator=( const MatterVetoTool& ) ;
  // ========================================================================
private:
  // ======================================================================== 
  StatusCode i_cacheGeo();
  bool IsInMaterialBoxLeft(const Gaudi::XYZPoint &)const;///<Point in material region in Left halfbox
  bool IsInMaterialBoxRight(const Gaudi::XYZPoint &)const;///<Point in material region in Right halfbox

  Gaudi::Transform3D m_toVeloLFrame; ///< to transform to local velo L frame
  Gaudi::Transform3D m_toVeloRFrame; ///< to transform to local velo R frame
  std::vector<Gaudi::XYZPoint > m_LeftSensorsCenter;
  std::vector<Gaudi::XYZPoint > m_RightSensorsCenter;
  Condition* m_motionSystem;
  
  

  // ========================================================================
};

 // end of namespace LoKi 


// ============================================================================
// standard initialization of the tool 
// ============================================================================
StatusCode MatterVetoTool::initialize() 
{ 
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  if( !existDet<DataObject>(detSvc(),"Conditions/Online/Velo/MotionSystem") ){
    Warning("VELO motion system not in conditions DB", 
            StatusCode::SUCCESS).ignore();
    //m_useConditions = false;
  }else{
    registerCondition("Conditions/Online/Velo/MotionSystem",m_motionSystem,
                      &MatterVetoTool::i_cacheGeo);
    sc = runUpdate();
    if(!sc) return sc;
  }

  return StatusCode::SUCCESS;

  //m_updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  //return GaudiTool::initialize () ; 
}
//=============================================================================
// Check if particle vertex is in material
//=============================================================================
bool MatterVetoTool::isInMatter( const Gaudi::XYZPoint & point ) const {
  Gaudi::XYZPoint posloc;
  bool inMat = false;
  
  //move to local Velo half frame
  if( point.x() < 2. ){ //right half
    posloc = m_toVeloRFrame * point;
    inMat = inMat || IsInMaterialBoxRight(posloc);
  }
  if (inMat) return inMat;
  if( point.x() > -2. ){ //left half
    posloc = m_toVeloLFrame * point;
    inMat = inMat || IsInMaterialBoxLeft(posloc);
  }
  if (inMat) return inMat;
  return inMat;
}


//=============================================================================

//=============================================================================
// Initialize the geometric info
//=============================================================================
StatusCode MatterVetoTool::i_cacheGeo(){  
  //get the Velo geometry
  string velostr = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/Velo";
  const IDetectorElement* lefthalv = getDet<IDetectorElement>( velostr+"Left" );
  const IDetectorElement* righthalv =  getDet<IDetectorElement>( velostr + "Right" );
  const IGeometryInfo* halflgeominfo = lefthalv->geometry();
  const IGeometryInfo* halfrgeominfo = righthalv->geometry();
  Gaudi::XYZPoint localorigin(0,0,0);
  Gaudi::XYZPoint leftcenter = lefthalv->geometry()->toGlobal(localorigin);
  Gaudi::XYZPoint rightcenter = righthalv->geometry()->toGlobal(localorigin);
  if( msgLevel( MSG::DEBUG ) )
    debug() << "Velo global right half center "
            << rightcenter <<", left half center "<< leftcenter << endmsg;
  
  //matrix to transform to local velo frame
  m_toVeloRFrame = halfrgeominfo->toLocalMatrix() ;
  //m_toGlobalFrame = halfgeominfo->toGlobalMatrix();
  m_toVeloLFrame = halflgeominfo->toLocalMatrix() ;
  m_LeftSensorsCenter.clear();
  m_RightSensorsCenter.clear();
  DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );
  std::vector< DeVeloRType * >::const_iterator iLeftR= velo->leftRSensorsBegin() ;
  for(;iLeftR!=velo->leftRSensorsEnd();iLeftR++){
    if((*iLeftR)->isPileUp())continue;
    const Gaudi::XYZPoint localCenter(0.,0.,0.);
    const Gaudi::XYZPoint halfBoxRCenter = 
      (*iLeftR)->localToVeloHalfBox (localCenter);
    const DeVeloPhiType * phisens = (*iLeftR)->associatedPhiSensor () ;
    if(!(*iLeftR)->isPileUp()){
      const Gaudi::XYZPoint halfBoxPhiCenter = 
        phisens->localToVeloHalfBox (localCenter);
      Gaudi::XYZPoint halfBoxCenter(halfBoxRCenter.x()+(halfBoxPhiCenter.x()-halfBoxRCenter.x())/2,
                                    halfBoxRCenter.y()+(halfBoxPhiCenter.y()-halfBoxRCenter.y())/2,
                                    halfBoxRCenter.z()+(halfBoxPhiCenter.z()-halfBoxRCenter.z())/2);
      m_LeftSensorsCenter.push_back(halfBoxCenter);
    }
  }
  std::vector< DeVeloRType * >::const_iterator iRightR = 
    velo->rightRSensorsBegin() ;
  for(;iRightR!=velo->rightRSensorsEnd();iRightR++){
    const Gaudi::XYZPoint localCenter(0.,0.,0.);
    const Gaudi::XYZPoint halfBoxRCenter = 
      (*iRightR)->localToVeloHalfBox (localCenter);
    const DeVeloPhiType * phisens = (*iRightR)->associatedPhiSensor () ;
    if(!(*iRightR)->isPileUp()){
      const Gaudi::XYZPoint halfBoxPhiCenter = 
        phisens->localToVeloHalfBox (localCenter);
      Gaudi::XYZPoint halfBoxCenter(halfBoxRCenter.x()+(halfBoxPhiCenter.x()-halfBoxRCenter.x())/2,
                                    halfBoxRCenter.y()+(halfBoxPhiCenter.y()-halfBoxRCenter.y())/2,
                                    halfBoxRCenter.z()+(halfBoxPhiCenter.z()-halfBoxRCenter.z())/2);
      m_RightSensorsCenter.push_back(halfBoxCenter);
    }
  }//end sensorloop
  return StatusCode::SUCCESS;
  
}


//=============================================================================
// Check if a point is in a region containing RFFoil and sensors in the Left 
// halfbox frame
//=============================================================================

bool MatterVetoTool::IsInMaterialBoxLeft(const Gaudi::XYZPoint& point)const{
  // First get the z bin
  int regModIndex(0);
  double downlimit(-1000.),uplimit(-1000.);
  //always()<<m_LeftSensorsCenter.size()-1<<endreq;
  if(int(m_LeftSensorsCenter.size())-1<2)return false;
  for(int mod = 0 ; mod != int(m_LeftSensorsCenter.size())-1; mod++){
    downlimit=uplimit;
    uplimit=(m_LeftSensorsCenter[mod].z()+(m_LeftSensorsCenter[mod+1].z()-
                                           m_LeftSensorsCenter[mod].z())/2);
    if( point.z()>downlimit && point.z()<uplimit ){
      regModIndex=mod;
      continue;
    }
  }
  if(point.z()<800. && point.z()>uplimit)regModIndex=m_LeftSensorsCenter.size()-1;
  double r = sqrt(pow(point.x()-m_LeftSensorsCenter[regModIndex].x(),2)+pow(point.y()-m_LeftSensorsCenter[regModIndex].y(),2));
  if ( (r<5. && point.z()<370.) || (r<4.3 && point.z()>370.) ){
    return false;
  }
  // Is in the module area
  double halfModuleBoxThickness(1.75);
  if (point.z()<m_LeftSensorsCenter[regModIndex].z()+halfModuleBoxThickness 
      && point.z()>m_LeftSensorsCenter[regModIndex].z()-halfModuleBoxThickness){
    return true;
  }
  
  // depending on z:
  // in the region of small corrugation
  if(point.z()<290. && point.x()-m_LeftSensorsCenter[regModIndex].x()>4){
    // first rather large region, rather small r
    float smallerCyl = 8.;
    float RsmallerCyl = 7.;
    float largerCyl = 11.;
    float RlargerCyl = 9.;
    
    if(fabs(point.z()-m_LeftSensorsCenter[regModIndex].z())>smallerCyl
       && r < RsmallerCyl ){ 
      return false;
    }
    
    if(fabs(point.z()-m_LeftSensorsCenter[regModIndex].z())>largerCyl
       && r < RlargerCyl ){
      return false;
    }
    
  }
  if(r<12.5 && point.z()<440.){
    return true;
  }
  
  if(fabs(point.x()-m_LeftSensorsCenter[regModIndex].x())<5.5 && 
     point.z()<440.){
    return true;
  }
  
  if(fabs(point.x()-m_LeftSensorsCenter[regModIndex].x())<8.5 && 
     point.z()>440.){ 
    return true;  
  }
  
  if (m_useEnlargedMatterVeto) {
    // two modules
    if ( ( (446. < point.z() && point.z() < 453.) ||
	   (348. < point.z() && point.z() < 351.) ) && 
	 ( 5. < r && r < 44. ) )
      return true;
    // RFoil
    if ( ( 300. < point.z() && point.z() < 600.) && 
	 ( 4. < point.x() && point.x() < 8.) && 
	 ( ( -13. < point.y() && point.y() < -10. ) ||
	   ( 10. < point.y() && point.y() < -13. ) ) )
      return true;
  }

  return false;


  
}

//=============================================================================
// Check if a point is in a region containing RFFoil and sensors in the Right 
// halfbox frame
//=============================================================================

bool MatterVetoTool::IsInMaterialBoxRight(const Gaudi::XYZPoint& point) const{
  // First get the z bin
  int regModIndex(0);
  double downlimit(-1000.),uplimit(-1000.);
  if(int(m_RightSensorsCenter.size())-1<2)return false;
  for (int mod = 0 ; mod != int(m_RightSensorsCenter.size())-1; mod++){
    downlimit=uplimit;
    uplimit=(m_RightSensorsCenter[mod].z()+(m_RightSensorsCenter[mod+1].z()-m_RightSensorsCenter[mod].z())/2);
    if( point.z()>downlimit && point.z()<uplimit ){
      regModIndex=mod;
      continue;
    }
  }
  if(point.z()<800. && point.z()>uplimit)
    regModIndex=m_RightSensorsCenter.size()-1;
  // Is in vaccum clean cylinder?
  double r = sqrt(pow(point.x()-m_RightSensorsCenter[regModIndex].x(),2)+pow(point.y()-m_RightSensorsCenter[regModIndex].y(),2));
  
  // inner cylinder
  if ( (r<5. && point.z()<390.) || (r<4.3 && point.z()>390.) ){
    return false;
  }
  // is in the module area
  double halfModuleBoxThickness(1.75);
  if (point.z()<m_RightSensorsCenter[regModIndex].z()+halfModuleBoxThickness 
      && point.z()>m_RightSensorsCenter[regModIndex].z()-halfModuleBoxThickness) return true;
  // depending on z:
  // in the region of small corrugation
  if(point.z()<300. && point.x()-m_RightSensorsCenter[regModIndex].x()<-4){
    // first rather large region, rather small r
    float smallerCyl = 8.;
    float RsmallerCyl = 7.;
    float largerCyl = 11.;
    float RlargerCyl = 9.;
    
    if (fabs(point.z()-m_RightSensorsCenter[regModIndex].z())>smallerCyl
        && r < RsmallerCyl ) return false;
    if (fabs(point.z()-m_RightSensorsCenter[regModIndex].z())>largerCyl
        && r < RlargerCyl ) return false;
  }
  // Is clearly outside RFFoil part
  if (r<12.5 && point.z()<450. ) return true;
  if (point.z()<450. && fabs(point.x()-m_RightSensorsCenter[regModIndex].x())<5.5)return true;
  if (fabs(point.x()-m_RightSensorsCenter[regModIndex].x())<8.5 && point.z()>450.) return true;  

  // enlarged MatterVeto
  if (m_useEnlargedMatterVeto) {
    // one module
    if ( ( 431. < point.z() && point.z() < 439.) && 
	 ( 5. < r && r < 44. ) )
      return true;
    // RFoil
    if ( ( 300. < point.z() && point.z() < 600.) && 
	 ( 4. < point.x() && point.x() < 8.) && 
	 ( ( -13. < point.y() && point.y() < -10. ) ||
	   ( 10. < point.y() && point.y() < -13. ) ) )
      return true;
  }
  

  return false;
}


// ============================================================================
/// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY(MatterVetoTool);
// ============================================================================
// The END 
// ============================================================================
