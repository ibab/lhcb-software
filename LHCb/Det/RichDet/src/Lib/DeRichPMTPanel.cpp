// $Id: $
// Include files

// STL
#include <sstream>
#include <time.h>
#include <vector>
// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// local
#include "RichDet/DeRichPMTPanel.h"
#include "RichDet/DeRichSystem.h"

// MathCore files
#include "GaudiKernel/Transform3DTypes.h"

// DetDesc
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"
#include "DetDesc/TabulatedProperty.h"

// GSL
#include "gsl/gsl_math.h"

//-----------------------------------------------------------------------------

const CLID CLID_DeRichPMTPanel = 12020;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichPMTPanel::DeRichPMTPanel( const std::string & name )
  : DeRichPDPanel ( name ),
    m_DePMTModules(std::vector<IDetectorElement*> (1)),
    m_DePMTs(1,std::vector<DeRichPMT*>(2)),
    m_DePMTAnodes(1,std::vector<IDetectorElement*>(2)),
    m_PmtModulePlaneHalfSizeR1( std::vector<double> (0)),
    m_PmtModulePlaneHalfSizeR2( std::vector<double>(0)),
    m_RichPmtNumModulesInRowCol(std::vector<int>(4)),
    m_RichPmtModuleCopyNumBeginPanel(std::vector<int>(4)),
    m_RichPmtModuleCopyNumEndPanel(std::vector<int>(4)),
    m_RichPmtModuleActiveAreaHalfSize(std::vector<double>(2)),
    m_NumPmtInRowCol(std::vector<int>(2)),
    m_NumPmtModuleInRich(std::vector<int>(4)),
    m_Rich1PmtLensPresence(0),
    m_Rich1PmtLensModuleCol(std::vector<int>(0)),
    m_RichPmtModuleLensFlag(std::vector<bool>(350)),
    m_Rich1PmtPanelWithLensXSize(std::vector<double>(2)),
    m_Rich1PmtPanelWithLensYSize(std::vector<double>(2)),
    m_Rich1PmtPanelWithLensColSize(std::vector<int>(2)),
    m_RichNumLensPmtinModuleRowCol(std::vector<int>(2)),
    m_Rich2UseGrandModule(false),
    m_Rich2UseMixedModule(false), 
    m_GrandPmtModulePlaneHalfSizeR2( std::vector<double>(0)),
    m_MixedPmtModulePlaneHalfSizeR2( std::vector<double>(0)),
    m_MixedStdPmtModulePlaneHalfSizeR2( std::vector<double>(0)),
    m_RichGrandPmtModuleActiveAreaHalfSize(std::vector<double>(2)),
    m_NumGrandPmtInRowCol(std::vector<int>(2)),
    m_Rich2MixedModuleArrayColumnSize(std::vector<int>(3)),
    m_ModuleIsWithGrandPMT(std::vector<bool>(350))
{
  // Set the PD type to PMT
  m_pdType = LHCb::RichSmartID::MaPMTID;
}

//=============================================================================
// Destructor
//=============================================================================
DeRichPMTPanel::~DeRichPMTPanel() {}

//=============================================================================
// Retrieve Pointer to class defininition structure
const CLID& DeRichPMTPanel::classID()
{
  return CLID_DeRichPMTPanel;
}

//=========================================================================
//  Initialize
//=========================================================================
StatusCode DeRichPMTPanel::initialize()
{
  StatusCode sc =  StatusCode::SUCCESS;

  // store the name of the panel, without the /dd/Structure part
  const std::string::size_type pos = name().find("Rich");
  setMyName( std::string::npos != pos ? name().substr(pos) : "DeRichPMTPanel_NO_NAME" );

  MsgStream msg ( msgSvc(), "DeRichPMTPanel" );

  if ( msgLevel(MSG::DEBUG,msg) )
    msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  // set rich and side
  if ( !setRichPanelAndSide() ) return StatusCode::FAILURE;
  if ( rich() == Rich::InvalidDetector || side() == Rich::InvalidSide )
  {
    error() << "Error initializing PMT panel " << name() << "  "<<rich()<< side() <<endmsg;
    return StatusCode::FAILURE;
  }

  // register UMS dependency on local geometry
  updMgrSvc()->registerCondition( this,  geometry(), &DeRichPMTPanel::geometryUpdate );

  // get the pmtmodule and pmt  detector elements
  m_DePMTs.clear();
  m_DePMTModules.clear();
  m_DePMTAnodes.clear();

  int numCurModules = getNumModulesInThisPanel();

  m_DePMTModules.reserve(numCurModules);
  m_DePMTs.reserve(numCurModules);
  m_DePMTAnodes.reserve(numCurModules);

  sc = sc && getPanelGeometryInfo();
  if(sc == StatusCode::FAILURE ) return sc;
  
  const IDetectorElement::IDEContainer & detelems = childIDetectorElements();

  for ( IDetectorElement::IDEContainer::const_iterator det_it = detelems.begin();
        det_it != detelems.end(); ++det_it )
  {
    //info()<<"  det it names  "<<( det_it - detelems.begin() ) <<"  "<<(*det_it)->name() <<endmsg;

    if ( std::string::npos != (*det_it)->name().find("MAPMT_MODULE:") )
    {

      // get PMT Module
      SmartDataPtr<IDetectorElement> dePMTModule( dataSvc(), (*det_it)->name() );
      if ( !dePMTModule )
      {
        msg << MSG::FATAL << "Non DeRichPMT Module detector element "
            << (*det_it)->name() << endmsg;
        return StatusCode::FAILURE;
      }

      // save to list of PMT Modules
      m_DePMTModules.push_back( dePMTModule );
      int aCurrentModuleCopyNumber = getModuleCopyNumber( dePMTModule ->name() );
                  

      int aNumPmtInCurrentRichModule= (int) dePMTModule->childIDetectorElements().size();
      std::vector<DeRichPMT*> DePmtsInCurModule(aNumPmtInCurrentRichModule,(DeRichPMT*)0);
      std::vector<IDetectorElement*> DePmtAnodesInCurModule(aNumPmtInCurrentRichModule,(IDetectorElement*)0);

      
      //std::vector<DeRichPMT*> DePmtsInCurModule(m_NumPmtInRichModule,(DeRichPMT*)0);
      //std::vector<IDetectorElement*> DePmtAnodesInCurModule(m_NumPmtInRichModule,(IDetectorElement*)0);

      // register UMS dependency.
      updMgrSvc()->registerCondition( this, dePMTModule->geometry(), &DeRichPMTPanel::geometryUpdate );

      // get PMT
      // for now assume the only det elements inside a module is just the pmts and
      // that inside the pmt is the anode. This may be changed in the future if needed with
      // some modif to the following lines.

      if ( !dePMTModule->childIDetectorElements().empty() )
      {
        for ( IDetectorElement::IDEContainer::const_iterator det_it_pm= dePMTModule->childIDetectorElements().begin();
              det_it_pm !=  dePMTModule->childIDetectorElements().end(); ++det_it_pm )
        {
          if ( std::string::npos != (*det_it_pm)->name().find("MAPMT:") )
          {
            // get PMT
            SmartDataPtr<DeRichPMT> dePMT( dataSvc(), (*det_it_pm)->name() );

            if( dePMT ) {

              // DeRichPMT * dePMT = (DeRichPMT*)  (*det_it_pm);
              // register UMS dependency
              updMgrSvc()->registerCondition( this,dePMT->geometry(),&DeRichPMTPanel::geometryUpdate );
              // get the current pmt and save.
              int curPmtNum = det_it_pm - dePMTModule->childIDetectorElements().begin();
              int curPmtCopyNum= dePMT->pmtCopyNumber();
              dePMT->setPmtLensFlag( isCurrentPmtWithLens(curPmtCopyNum)   );
              dePMT->setPmtIsGrandFlag( ModuleIsWithGrandPMT(aCurrentModuleCopyNumber)  );
              DePmtsInCurModule[curPmtNum] =dePMT;

              if( ! dePMT->childIDetectorElements().empty() )
              {
                for(IDetectorElement::IDEContainer::const_iterator det_it_pm_an = dePMT->childIDetectorElements().begin();
                    det_it_pm_an !=  dePMT->childIDetectorElements().end(); ++det_it_pm_an )
                {
                  if ( std::string::npos != (*det_it_pm_an)->name().find("MAPMTAnode:") )
                  {
                    SmartDataPtr<IDetectorElement> dePmtAnode( dataSvc(), (*det_it_pm_an)->name() );

                    if (dePmtAnode)
                    {
                      //  IDetectorElement* dePmtAnode = (IDetectorElement*) (*det_it_pm_an);
                      // register UMS dependency
                      updMgrSvc()->registerCondition( this,dePmtAnode->geometry(),&DeRichPMTPanel::geometryUpdate );
                      DePmtAnodesInCurModule[curPmtNum] = dePmtAnode;
                    }
                    else
                    {
                      msg << MSG::FATAL << "Non DeRichPMT Anode detector element "
                          << (*det_it_pm_an)->name() << endmsg;
                      return StatusCode::FAILURE;
                    }
                  }// end test anode name

                } // end loop over  anodes in a pmt. We assume there is only 1 det elem inside a pmt
                // which is the anode.
              }// end test on pt child det elem

            }
            else
            {      // end test existence of pmt
              msg << MSG::FATAL << "No DeRichPMT  detector element "
                  << (*det_it_pm)->name() << endmsg;
              return StatusCode::FAILURE;
            }

          }// end test pmt name

        } // end loop over pmts in  a module

        m_DePMTs.push_back( DePmtsInCurModule);
        m_DePMTAnodes.push_back(DePmtAnodesInCurModule);

      }
      else
      {
        msg << MSG::FATAL << "Problem getting PMT Det elem from " << (*det_it)->name()
            << endmsg;
        return StatusCode::FAILURE;
      }
    } // end check on module

  } // end loop over modules

  // trigger first UMS update
  const StatusCode update = updMgrSvc()->update(this);

  //msg << MSG::DEBUG << "Panel Initialised" << endmsg;

  return update;
}


//=========================================================================
//  getFirstDeRich
//=========================================================================
DetectorElement * DeRichPMTPanel::getFirstDeRich() const
{
  SmartDataPtr<DetectorElement> afterMag(dataSvc(),"/dd/Structure/LHCb/AfterMagnetRegion");
  if ( !afterMag )
  {
    error() << "Could not load AfterMagnetRegion det elem" << endmsg;
  }

  std::string firstRichLoc;
  if ( afterMag->exists("RichDetectorLocations") )
    firstRichLoc = afterMag->paramVect<std::string>("RichDetectorLocations")[0];
  else
    firstRichLoc = DeRichLocations::Rich1;

  SmartDataPtr<DetectorElement> deRich(dataSvc(), firstRichLoc );
  if ( !deRich )
  {
    error() << "Could not load DeRich for DeRichPMTPanel" << endmsg;
  }

  return deRich;
}

//=========================================================================
int DeRichPMTPanel::getNumModulesInThisPanel()
{
  int aNumMod = 0;
  if(rich() == Rich::Rich1 )
  {
    if(side() == Rich::top )
    {
      aNumMod = m_NumPmtModuleInRich[0];
    }
    else if (side() == Rich::bottom)
    {
      aNumMod = m_NumPmtModuleInRich[1];
    }

  }
  else if (rich() == Rich::Rich2 || rich() == Rich::Rich )
  {
    if ( side() == Rich::left )
    {
      aNumMod = m_NumPmtModuleInRich[2];
    }
    else if (side() == Rich::right)
    {
      aNumMod = m_NumPmtModuleInRich[3];
    }
  }

  return aNumMod;
}

//=========================================================================
//  generate the transforms for global <-> local frames
//=========================================================================
StatusCode DeRichPMTPanel::geometryUpdate ( )
{
  StatusCode sc =  StatusCode::SUCCESS;

  DetectorElement* firstRich = getFirstDeRich();
  if( !firstRich )return StatusCode::FAILURE;

  sc = sc && getPanelGeometryInfo();

  m_PDColumns  = param<int>("PMTColumns");   // pd colums in a panel, not used, may be phased out
  m_PDNumInCol = param<int>("PMTNumberInColumn"); //pd rows in  a panel. not used, may be phased out.
  //  m_PDMax = nPDColumns() * nPDsPerCol(); // total in a  panel.
  m_PDMax = ( getNumModulesInThisPanel()) * m_NumPmtInRichModule;

  double  aOffset=0.0;
  Gaudi::XYZVector aDir(0.0,0.0,0.0);
  Gaudi::XYZPoint aPon(0.0,0.0,0.0);
  int sign =1;

  if ( rich() == Rich::Rich1 )
  {

    if(  side() == Rich::top )
    {
      const std::vector<double>& aPmtR1P0DetPlaneDirCos=firstRich->param<std::vector<double> >("Rich1TopPmtDetectorPlaneDirCos");
      const std::vector<double>& aPmtR1P0DetPlanePoint  = firstRich->param<std::vector<double> >("Rich1TopPmtDetPlanePoint");
      aOffset = fabs (m_PmtModulePlaneHalfSizeR1[1]) ;
      aDir = Gaudi::XYZVector(aPmtR1P0DetPlaneDirCos[0],aPmtR1P0DetPlaneDirCos[1], aPmtR1P0DetPlaneDirCos[2]);
      aPon = Gaudi::XYZPoint( aPmtR1P0DetPlanePoint[0], aPmtR1P0DetPlanePoint[1],  aPmtR1P0DetPlanePoint[2]);

    }
    else if ( side () == Rich::bottom )
    {
      sign =-1;
      const std::vector<double>& aPmtR1P1DetPlaneDirCos=firstRich->param<std::vector<double> >("Rich1BotPmtDetectorPlaneDirCos");
      const std::vector<double>& aPmtR1P1DetPlanePoint = firstRich->param<std::vector<double> >("Rich1BotPmtDetPlanePoint");
      aOffset = fabs( m_PmtModulePlaneHalfSizeR1[3]);
      aDir = Gaudi::XYZVector(aPmtR1P1DetPlaneDirCos[0],aPmtR1P1DetPlaneDirCos[1], aPmtR1P1DetPlaneDirCos[2]);
      aPon = Gaudi::XYZPoint( aPmtR1P1DetPlanePoint[0], aPmtR1P1DetPlanePoint[1],  aPmtR1P1DetPlanePoint[2]);
    }

    m_detectionPlane = Gaudi::Plane3D(aDir, aPon);

    // Also create a plane at the exterior surface of the pmt quartz window.
    double aZShiftR1 =  firstRich->param<double> ("Rich1PmtDetPlaneZInPmtPanel");
    const Gaudi::XYZPoint localpointP(0.0,0.0,aZShiftR1-m_RichPmtQuartzThickness);
    const Gaudi::XYZPoint localpointQ(0.0,100.0,aZShiftR1-m_RichPmtQuartzThickness);
    const Gaudi::XYZPoint localpointR(50.0,50.0,aZShiftR1-m_RichPmtQuartzThickness);
    const Gaudi::XYZPoint globalpointP(geometry()->toGlobal(localpointP));
    const Gaudi::XYZPoint globalpointQ(geometry()->toGlobal(localpointQ));
    const Gaudi::XYZPoint globalpointR(geometry()->toGlobal(localpointR));
    m_detectionPlane_exterior = Gaudi::Plane3D(globalpointP,globalpointQ,globalpointR);



  }
  else if ( rich() == Rich::Rich2 || rich() == Rich::Rich )
  {

    //It does not seem to be straightforward,  to get the detection plane param
    //directly from the xmldb for RICH2. So create
    // the actual detection plane using the Z Shift wrt the current panel.
    double aZShiftR2 =  firstRich->param<double> ("Rich2PmtDetPlaneZInPmtPanel");
    // create three random points on the detection plane, which can define the plane.
    const Gaudi::XYZPoint localpointA(0.0,0.0,aZShiftR2);
    const Gaudi::XYZPoint localpointB(0.0,100.0,aZShiftR2);
    const Gaudi::XYZPoint localpointC(50.0,50.0,aZShiftR2);
    const Gaudi::XYZPoint globalpointA(geometry()->toGlobal(localpointA));
    const Gaudi::XYZPoint globalpointB(geometry()->toGlobal(localpointB));
    const Gaudi::XYZPoint globalpointC(geometry()->toGlobal(localpointC));

    m_detectionPlane = Gaudi::Plane3D(globalpointA,globalpointB,globalpointC);


    // Also create  a  second plane at the exterior surface of the PmtQuartzWindow.
    const Gaudi::XYZPoint localpointP(0.0,0.0,aZShiftR2-m_RichPmtQuartzThickness);
    const Gaudi::XYZPoint localpointQ(0.0,100.0,aZShiftR2-m_RichPmtQuartzThickness);
    const Gaudi::XYZPoint localpointR(50.0,50.0,aZShiftR2-m_RichPmtQuartzThickness);
    const Gaudi::XYZPoint globalpointP(geometry()->toGlobal(localpointP));
    const Gaudi::XYZPoint globalpointQ(geometry()->toGlobal(localpointQ));
    const Gaudi::XYZPoint globalpointR(geometry()->toGlobal(localpointR));
    m_detectionPlane_exterior = Gaudi::Plane3D(globalpointP,globalpointQ,globalpointR);


    aPon = globalpointA;

    if(  side() == Rich::left )
    {
      aOffset = fabs (m_PmtModulePlaneHalfSizeR2[0]) ;
      if(m_Rich2UseGrandModule) {
        aOffset = fabs (m_GrandPmtModulePlaneHalfSizeR2[0]) ;
        if(m_Rich2UseMixedModule) {
           aOffset = fabs (m_MixedPmtModulePlaneHalfSizeR2[0]) ;       
        }
        
      }
      
      
    }
    else if ( side () == Rich::right )
    {
      sign=-1;
      aOffset = fabs( m_PmtModulePlaneHalfSizeR2[2]);
      if(m_Rich2UseGrandModule) {

        aOffset = fabs( m_GrandPmtModulePlaneHalfSizeR2[2]);
        if( m_Rich2UseMixedModule) {
          aOffset = fabs (m_MixedPmtModulePlaneHalfSizeR2[2]) ;       
        }
        
      }
      
    }
  }



  m_localOffset = aOffset;

  m_detPlaneZ = aPon.z();
  // m_detPlaneZ = geometry()->toLocal(aPon.z()); // it seems the local Z coord is to be saved,
  // looking at the hpd version  ?
  //  if so, uncomment this line and comment out the previous line.

  //  const ROOT::Math::Translation3D localTranslation = ROOT::Math::Translation3D(aPon.x(),sign*aOffset,aPon.z());
  const ROOT::Math::Translation3D localTranslation = 
    (rich() == Rich::Rich1) ?  ROOT::Math::Translation3D(aPon.x(),sign*aOffset,aPon.z())
    : ROOT::Math::Translation3D(sign*aOffset,aPon.y(), aPon.z());


  m_globalToPDPanelTransform = localTranslation * geometry()->toLocalMatrix();
  m_PDPanelToGlobalTransform = m_globalToPDPanelTransform.Inverse();

  m_localPlane = geometry()->toLocalMatrix() * m_detectionPlane;



  m_localPlaneNormal = m_localPlane.Normal();



  // m_localPlane2 =

  return sc;
}

StatusCode DeRichPMTPanel::smartID( const Gaudi::XYZPoint& globalPoint,
                                    LHCb::RichSmartID& id ) const
{
  const std::vector<int> aHitChannelVec = findPMTArraySetup(globalPoint);
  return setRichPmtSmartID( aHitChannelVec, id ) ;
}

StatusCode DeRichPMTPanel::setRichPmtSmartID(const std::vector<int>& aPmtHitChannel,
                                             LHCb::RichSmartID& id )const
{
  id.setIDType(LHCb::RichSmartID::MaPMTID);
  id.setPD(aPmtHitChannel[0],aPmtHitChannel[1]);
  id.setPixelRow(aPmtHitChannel[3]);
  id.setPixelCol(aPmtHitChannel[2]);
  id.setPanel(side());
  id.setRich(rich());

  return StatusCode::SUCCESS;
}

StatusCode DeRichPMTPanel::getPanelGeometryInfo()
{
  StatusCode sc =  StatusCode::SUCCESS;


  if(  ( m_PmtModulePlaneHalfSizeR1.empty() ) ||
       ( m_PmtModulePlaneHalfSizeR2.empty() )  )
  {
    DetectorElement * firstRich = getFirstDeRich();
    if ( !firstRich )
    {
      error() << "Cannot locate first RICH detector" << endmsg;
      return StatusCode::FAILURE;
    }

    m_RichPmtNumModulesInRowCol.resize(4);
    m_RichPmtNumModulesInRowCol[0]=1;
    m_RichPmtNumModulesInRowCol[1]=1;

    // keep some the RICH1 papameters inside the if block
    if ( rich() == Rich::Rich1 )
    {
      m_PmtModulePlaneHalfSizeR1 = firstRich->param<std::vector<double> >("Rich1PMTModulePlaneHalfSize");
      int aRich1PmtNumModulesInRow = firstRich->param<int> ("Rich1NumberOfModulesInRow" );
      int aRich1PmtNumModulesInCol = firstRich->param<int> ("Rich1NumberOfModulesInCol" );
      m_RichPmtNumModulesInRowCol[0]=aRich1PmtNumModulesInRow;
      m_RichPmtNumModulesInRowCol[1]=aRich1PmtNumModulesInCol;
    }
    m_Rich2UseGrandModule=false;
    m_Rich2UseMixedModule=false;
    m_Rich2ArrayConfig=0;
    if(firstRich->exists("Rich2PMTArrayConfig") ){
      m_Rich2ArrayConfig=firstRich->param<int>("Rich2PMTArrayConfig");
      if(m_Rich2ArrayConfig >= 1 ) {
        m_Rich2UseGrandModule=true;
        if(m_Rich2ArrayConfig == 2 ) {
          m_Rich2UseMixedModule=true;
        }
        
      }
    }
    

    m_PmtModulePlaneHalfSizeR2 = firstRich->param<std::vector<double> >("Rich2PMTModulePlaneHalfSize");
    m_PmtModulePitch = firstRich->param<double>("RichPmtModulePitch");
    int aRich2PmtNumModulesInRow = firstRich->param<int> ("Rich2NumberOfModulesInRow" );
    int aRich2PmtNumModulesInCol = firstRich->param<int> ("Rich2NumberOfModulesInCol" );
    m_RichPmtNumModulesInRowCol[2]=aRich2PmtNumModulesInRow;
    m_RichPmtNumModulesInRowCol[3]=aRich2PmtNumModulesInCol;

    // info()<< "DeRichPmtplane  getPanelGeometryInfo  m_RichPmtNumModulesInRowCol "
    //       <<  m_RichPmtNumModulesInRowCol <<endmsg;


    
    m_RichPmtModuleCopyNumBeginPanel=
      firstRich->param<std::vector<int> > ("RichPmtModuleNumBeginInPanels");
    m_RichPmtModuleCopyNumEndPanel=
      firstRich->param<std::vector<int> > ("RichPmtModuleNumEndInPanels");
    m_RichPmtModuleActiveAreaHalfSize=
      firstRich->param<std::vector<double> >("RichPMTModuleActiveAreaHalfSize" );
 
    // m_PmtModulePitch = firstRich->param<double>("RichPmtModulePitch");

    m_PmtPitch=firstRich->param<double>("RichPmtPitch");


    m_NumPmtInRowCol.resize(2);
    m_NumPmtInRowCol[0] = firstRich->param<int> ("RichPmtNumInModuleRow");
    m_NumPmtInRowCol[1] = firstRich->param<int> ("RichPmtNumInModuleCol");
    m_NumPmtInRichModule = firstRich->param<int> ("RichTotNumPmtInModule");

    int aRich1NumModules=  firstRich->param<int> ("Rich1TotNumModules" );
    int aRich2NumModules=  firstRich->param<int> ("Rich2TotNumModules" );
    m_Rich2TotNumStdModules=aRich2NumModules;
    m_totNumPmtModuleInRich = aRich1NumModules + aRich2NumModules;

    m_NumPmtModuleInRich.resize(4);
    m_NumPmtModuleInRich[0]=aRich1NumModules/2; //rich1top
    m_NumPmtModuleInRich[1]=aRich1NumModules/2; //rich1bottom
    m_NumPmtModuleInRich[2]=aRich2NumModules/2; //rich2left
    m_NumPmtModuleInRich[3]=aRich2NumModules/2; //rich2right

    m_PmtAnodeXSize = firstRich->param<double>("RichPmtAnodeXSize");
    m_PmtAnodeYSize = firstRich->param<double>("RichPmtAnodeYSize");

    m_PmtPixelGap = firstRich->param<double>("RichPmtPixelGap" );
    m_PmtPixelsInRow = firstRich->param<int>("RichPmtNumPixelCol" );
    m_PmtPixelsInCol = firstRich->param<int>("RichPmtNumPixelRow" );
    m_PmtAnodeXEdge = -0.5*(m_PmtAnodeXSize+m_PmtPixelGap);
    m_PmtAnodeYEdge = -0.5*(m_PmtAnodeYSize+m_PmtPixelGap);
    m_AnodeXPixelSize = firstRich->param<double> ("RichPmtPixelXSize" );
    m_AnodeYPixelSize = firstRich->param<double> ("RichPmtPixelYSize" );
    m_PmtAnodeEffectiveXPixelSize= m_AnodeXPixelSize+m_PmtPixelGap;
    m_PmtAnodeEffectiveYPixelSize= m_AnodeYPixelSize+m_PmtPixelGap;
    m_PmtMasterLateralSize = firstRich->param<double>("RichPmtMasterLateralSize" );
    m_Rich1TotNumPmts = firstRich->param<int>("Rich1TotNumPmt");
    m_Rich2TotNumPmts = firstRich->param<int>("Rich2TotNumPmt");
    m_Rich2TotNumStdPmts = m_Rich2TotNumPmts ;
    m_RichPmtQuartzThickness = firstRich->param<double>("RichPmtQuartzZSize" );
    m_RichPmtQuartzLocalZInPmt= firstRich->param<double>("RichPmtQuartzZPosInPmt");

    if(m_Rich2UseGrandModule) {
      if(firstRich->exists("Rich2GrandPMTModulePlaneHalfSize") ){
        

        m_GrandPmtModulePlaneHalfSizeR2 = firstRich->param<std::vector<double> >("Rich2GrandPMTModulePlaneHalfSize");
        m_GrandPmtModulePitch = firstRich->param<double>("RichGrandPmtModulePitch");
        m_RichGrandPmtModuleActiveAreaHalfSize=
           firstRich->param<std::vector<double> >("RichGrandPMTModuleActiveAreaHalfSize" );
        m_GrandPmtPitch=firstRich->param<double>("RichGrandPmtPitch");
        
        m_Rich2TotNumGrandModules=  firstRich->param<int> ("Rich2TotNumGrandModules" );
        m_Rich2TotNumStdModules=0;
        m_totNumPmtModuleInRich = aRich1NumModules + m_Rich2TotNumGrandModules;  
        m_NumPmtModuleInRich[2]=m_Rich2TotNumGrandModules/2; //rich2left
        m_NumPmtModuleInRich[3]=m_Rich2TotNumGrandModules/2; //rich2right





        m_GrandPmtAnodeXSize = firstRich->param<double>("RichGrandPmtAnodeXSize");
        m_GrandPmtAnodeYSize = firstRich->param<double>("RichGrandPmtAnodeYSize");

        m_GrandPmtPixelGap = firstRich->param<double>("RichGrandPmtPixelGap" );
        m_GrandPmtPixelsInRow = firstRich->param<int>("RichGrandPmtNumPixelCol" );
        m_GrandPmtPixelsInCol = firstRich->param<int>("RichGrandPmtNumPixelRow" );
        m_GrandPmtAnodeXEdge = -0.5*(m_GrandPmtAnodeXSize+m_GrandPmtPixelGap);
        m_GrandPmtAnodeYEdge = -0.5*(m_GrandPmtAnodeYSize+m_GrandPmtPixelGap);
        m_GrandAnodeXPixelSize = firstRich->param<double> ("RichGrandPmtPixelXSize" );
        m_GrandAnodeYPixelSize = firstRich->param<double> ("RichGrandPmtPixelYSize" );
        m_GrandPmtAnodeEffectiveXPixelSize= m_GrandAnodeXPixelSize+m_GrandPmtPixelGap;
        m_GrandPmtAnodeEffectiveYPixelSize= m_GrandAnodeYPixelSize+m_GrandPmtPixelGap;
        m_GrandPmtMasterLateralSize = firstRich->param<double>("RichGrandPmtMasterLateralSize" );
        m_GrandNumPmtInRichModule = firstRich->param<int> ("RichTotNumGrandPmtInModule");


        m_NumGrandPmtInRowCol.resize(2);
        m_NumGrandPmtInRowCol[0] = firstRich->param<int> ("RichGrandPmtNumInModuleRow");
        m_NumGrandPmtInRowCol[1] = firstRich->param<int> ("RichGrandPmtNumInModuleCol");
        m_Rich2TotNumGrandPmts = firstRich->param<int>  ( "Rich2TotNumGrandPmt"    );
        m_Rich2TotNumStdPmts=0;
        if(m_Rich2UseMixedModule) {
          if( firstRich->exists("Rich2MixedPMTModulePlaneHalfSize") ){           
            m_MixedPmtModulePlaneHalfSizeR2 = firstRich->param<std::vector<double> >("Rich2MixedPMTModulePlaneHalfSize");
            m_MixedStdPmtModulePlaneHalfSizeR2 =firstRich->param<std::vector<double> >("Rich2MixedStdPMTModulePlaneHalfSize" );
            m_Rich2TotNumStdModules  =   firstRich->param<int> ("Rich2TotNumStdModules" ); 
            m_totNumPmtModuleInRich = aRich1NumModules + m_Rich2TotNumGrandModules+m_Rich2TotNumStdModules;
            m_NumPmtModuleInRich[2]=(m_Rich2TotNumGrandModules+m_Rich2TotNumStdModules)/2; //rich2left
            m_NumPmtModuleInRich[3]=(m_Rich2TotNumGrandModules+m_Rich2TotNumStdModules)/2; //rich2right
            m_Rich2TotNumStdPmts = firstRich->param<int>  ( "Rich2TotNumStdPmt"    );
            if( firstRich->exists("Rich2MixedNumModulesArraySetup") )
               m_Rich2MixedModuleArrayColumnSize =firstRich ->param<std::vector<int> >("Rich2MixedNumModulesArraySetup");            
            
          }
          
          
          
        }
        

        

      }
      
    }
    

    // Setup flags for large PMTs

    RichSetupMixedSizePmtModules();
   
    


    // setup the Lens Flag.


    m_totNumPmtModuleInRich1     = firstRich-> param<int> ("Rich1TotNumModules" );

    m_Rich1PmtLensPresence=0;
    m_Rich1PmtLensModuleCol.clear();

    if(exists("Rich1PmtLensPresence") ){
      m_Rich1PmtLensPresence  = firstRich->  param<int>("Rich1PmtLensPresence");
    }

    if(m_Rich1PmtLensPresence >= 1 ) {

      m_Rich1PmtLensModuleCol = firstRich->  param<std::vector<int> >("Rich1PmtLensModuleColumns");
      m_PmtMasterWithLensLateralSize = firstRich->param<double>("RichLensPmtMasterLateralSize" );
      m_PmtModuleWithLensPitch = firstRich->param<double>("RichLensPmtModulePitch");
      m_PmtLensPitch=firstRich->param<double>("RichLensPmtPitch");
      m_Rich1PmtPanelWithLensXSize = firstRich->param<std::vector<double> > ("Rich1PMTModuleLensPlaneXEdge");
      m_Rich1PmtPanelWithLensYSize = firstRich->param<std::vector<double> > ( "Rich1PMTModuleLensPlaneYEdge"   );

      m_Rich1PmtPanelWithLensColSize = firstRich->param<std::vector<int> > ( "Rich1PmtLensModuleColumnsSize"  );

      m_RichNumLensPmtinModuleRowCol.resize(2);

      m_RichNumLensPmtinModuleRowCol[0] =  firstRich->param<int> ( "RichLensPmtNumInModuleRow" );
      m_RichNumLensPmtinModuleRowCol[1] =  firstRich->param<int> ( "RichLensPmtNumInModuleCol" );
      m_Rich1LensMagnificationFactor = firstRich->param<double> ("RichPmtLensMagnficationFactor"    );

      Rich1SetupPMTModulesWithLens();

    }else {
      for (int i=0; i< (int)m_RichPmtModuleLensFlag.size(); ++i ) {
        m_RichPmtModuleLensFlag[i]=false;
      }

      
    }

    
  }
  
  

  return sc;
}

void DeRichPMTPanel::Rich1SetupPMTModulesWithLens()
{

  int aRowR1 =  2*m_RichPmtNumModulesInRowCol[0];
  int aColR1 =  m_RichPmtNumModulesInRowCol[1];
  m_RichPmtModuleLensFlag.clear();
  m_RichPmtModuleLensFlag.reserve(aRowR1*aColR1);
  m_RichPmtModuleLensFlag.resize(aRowR1*aColR1);

  //m_RichPmtModuleLensFlag.assign(aRowR1*aColR1, false);

  for (int r=0; r<aRowR1  ; ++r ) {
    for(int c=0; c<aColR1 ; ++c ){
      int m=(r*aColR1)+c;
      m_RichPmtModuleLensFlag[m]=false;
      if( (int) (m_Rich1PmtLensModuleCol.size()) > 0) {

        std::vector<int>::const_iterator location= find(m_Rich1PmtLensModuleCol.begin(),
                                                        m_Rich1PmtLensModuleCol.end(), c);
        if(location != m_Rich1PmtLensModuleCol.end() ) m_RichPmtModuleLensFlag[m]=true;
      }



    }

  }


}



bool DeRichPMTPanel::isCurrentPmtModuleWithLens(const int aModuleNum)
{
  bool aPresence=false;
  if( aModuleNum < m_totNumPmtModuleInRich1 ) {
    aPresence = m_RichPmtModuleLensFlag[aModuleNum];
  }
  return  aPresence;

}
bool DeRichPMTPanel::isCurrentPmtWithLens(const int aPMTNum)
{

  const int aModuleNum = aPMTNum/m_NumPmtInRichModule;
  return isCurrentPmtModuleWithLens(aModuleNum);

}
void DeRichPMTPanel::RichSetupMixedSizePmtModules() {
  
  m_ModuleIsWithGrandPMT.assign(m_totNumPmtModuleInRich,false);
  
  // Now set the flags for module according to the geometry setup

  if(m_Rich2UseGrandModule ) {

    int aTotNumModInRich1= m_NumPmtModuleInRich[0]+m_NumPmtModuleInRich[1];

    for (int im=aTotNumModInRich1; im<m_totNumPmtModuleInRich; ++im) {
        if(m_Rich2ArrayConfig == 1 ) {
          m_ModuleIsWithGrandPMT[im]=true;
          
        }else if ( m_Rich2ArrayConfig == 2 ) { 
  
          int imLocal= im - aTotNumModInRich1;
          if (imLocal > m_NumPmtModuleInRich[2] ) imLocal -=  m_NumPmtModuleInRich[2];
          int aColNum = imLocal/(m_RichPmtNumModulesInRowCol[3]);
          int aRowNum = imLocal - aColNum*(m_RichPmtNumModulesInRowCol[3]);
          if( (aRowNum < m_Rich2MixedModuleArrayColumnSize[0] ) || 
              (aRowNum >= ( m_Rich2MixedModuleArrayColumnSize[0]+ m_Rich2MixedModuleArrayColumnSize[1] ) ) ) {
            m_ModuleIsWithGrandPMT[im] = true;
          }
          
        }

        //info()<<" All grandPmtRich2Arrayflags  im  ModuleIsWithGrandPMT  "
        //     <<   m_Rich2ArrayConfig<< "   "<<m_Rich2UseGrandModule<<"   "<< m_Rich2UseMixedModule 
        //      <<   "     "<<im<<"   "<< m_ModuleIsWithGrandPMT[im] <<endmsg;
     
      
    }// end loop over modules in rich2
    
  } // end if rich2useGrandModule 

  
}
                           




int DeRichPMTPanel::getPmtNumFromRowCol(int PRow, int PCol ) const
{
  //for values outside the range, set the closest value to the
  // corresponding edges.

  if ( PRow < 0 ) PRow=0;
  if ( PCol < 0 ) PCol =0;
  if ( PRow >= m_NumPmtInRowCol[1] ) PRow = m_NumPmtInRowCol[1]-1;
  if ( PCol >= m_NumPmtInRowCol[0] ) PCol = m_NumPmtInRowCol[0]-1;

  return ( PCol + ( PRow*m_NumPmtInRowCol[0] ) );
}
int DeRichPMTPanel::getLensPmtNumFromRowCol(int PRow, int PCol ) const
{
  //for values outside the range, set the closest value to the
  // corresponding edges.

  if ( PRow < 0 ) PRow=0;
  if ( PCol < 0 ) PCol =0;
  if ( PRow >= m_RichNumLensPmtinModuleRowCol[1] ) PRow =m_RichNumLensPmtinModuleRowCol[1] -1;
  if ( PCol >= m_RichNumLensPmtinModuleRowCol[0] ) PCol =m_RichNumLensPmtinModuleRowCol[0]-1;

  return ( PCol + ( PRow*m_RichNumLensPmtinModuleRowCol[0] ) );
}

int DeRichPMTPanel::getGrandPmtNumFromRowCol(int PRow, int PCol ) const
{
  //for values outside the range, set the closest value to the
  // corresponding edges.

  if ( PRow < 0 ) PRow=0;
  if ( PCol < 0 ) PCol =0;
  if ( PRow >= m_NumGrandPmtInRowCol[1] ) PRow = m_NumGrandPmtInRowCol[1]-1;
  if ( PCol >= m_NumGrandPmtInRowCol[0] ) PCol = m_NumGrandPmtInRowCol[0]-1;

  return ( PCol + ( PRow*m_NumGrandPmtInRowCol[0] ) );
}



std::vector<int> DeRichPMTPanel::getPmtRowColFromPmtNum( const int aPmtNum )
{
  const int aPRow = (int) (aPmtNum/m_NumPmtInRowCol[0]);
  const int aPCol =  aPmtNum - ( aPRow*m_NumPmtInRowCol[0]);
  std::vector<int> aRP (2);
  aRP[0] = aPRow;
  aRP[1] = aPCol;
  return aRP;
}

std::vector<int> DeRichPMTPanel::getGrandPmtRowColFromPmtNum( const int aPmtNum )
{
  const int aPRow = (int) (aPmtNum/m_NumGrandPmtInRowCol[0]);
  const int aPCol =  aPmtNum - ( aPRow*m_NumGrandPmtInRowCol[0]);
  std::vector<int> aRP (2);
  aRP[0] = aPRow;
  aRP[1] = aPCol;
  return aRP;
}



int DeRichPMTPanel::getPmtModuleNumFromRowCol( int MRow, int MCol ) const
{
  int aMNum = -1;
  //set the closest Row Col. This means if the row col exceeds the edges set them to those
  // at the closest edge.

  //  info()<<" DeRichPmtPanel getPmtModuleNumFromRowCol "<< MRow <<"  "<<MCol<<endmsg;
  // info()<<"DeRichPmtPanel rich side "<<rich()<<"   "<<side()<<"   "<<endmsg;
  // info()<<"DeRichPmtPanel StartPmtModuleNum In panel "<< m_RichPmtModuleCopyNumBeginPanel <<endmsg;

  if ( MRow< 0 ) MRow = 0;
  if ( MCol< 0 ) MCol = 0;

  if ( rich() == Rich::Rich1 )
  {
    if(MRow >= m_RichPmtNumModulesInRowCol[1]) MRow = m_RichPmtNumModulesInRowCol[1]-1;
    if(MCol >=  m_RichPmtNumModulesInRowCol[0]) MCol = m_RichPmtNumModulesInRowCol[0]-1;

    aMNum = MCol + ( MRow*m_RichPmtNumModulesInRowCol[0] );

    if(side() == Rich::top)
    {
      aMNum += m_RichPmtModuleCopyNumBeginPanel[0] ;
    }
    else if (side() == Rich::bottom)
    {
      aMNum += m_RichPmtModuleCopyNumBeginPanel[1] ;
    }

  }
  else if ( rich() == Rich::Rich2 || rich() == Rich::Rich )
  {
    if(MRow >= m_RichPmtNumModulesInRowCol[3]) MRow = m_RichPmtNumModulesInRowCol[3]-1;
    if(MCol >=  m_RichPmtNumModulesInRowCol[2]) MCol = m_RichPmtNumModulesInRowCol[2]-1;

    aMNum = MRow + ( MCol*m_RichPmtNumModulesInRowCol[3] );
    if(side() == Rich::left) {
      aMNum += m_RichPmtModuleCopyNumBeginPanel[2];

    }
    else if (side() == Rich::right )
    {
      aMNum += m_RichPmtModuleCopyNumBeginPanel[3];
    }

  }

  return aMNum;
}

int DeRichPMTPanel::PmtModuleNumInPanelFromModuleNum(const int aMnum) const
{
  int aM = -1;
  if ( rich() == Rich::Rich1 )
  {
    if(side() == Rich::top)
    {
      aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[0];
    }
    else if (side() == Rich::bottom)
    {
      aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[1];
    }
  }
  else if ( rich() == Rich::Rich2 || rich() == Rich::Rich )
  {
    if ( side() == Rich::left)
    {
      aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[2];
    }
    else if (side() == Rich::right )
    {
      aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[3];
    }
  }
  return aM;
}

int DeRichPMTPanel::PmtModuleNumInPanelFromModuleNumAlone(const int aMnum  ) const
{
  int aM = -1;
  if ( aMnum  >= m_RichPmtModuleCopyNumBeginPanel[0] &&
       aMnum  <= m_RichPmtModuleCopyNumEndPanel[0] )
  {
    aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[0];
  }
  else if ( aMnum  >= m_RichPmtModuleCopyNumBeginPanel[1] &&
            aMnum  <= m_RichPmtModuleCopyNumEndPanel[1] )
  {
    aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[1];
  }
  else if ( aMnum  >= m_RichPmtModuleCopyNumBeginPanel[2] &&
            aMnum  <= m_RichPmtModuleCopyNumEndPanel[2] )
  {
    aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[2];
  }
  else if ( aMnum  >= m_RichPmtModuleCopyNumBeginPanel[3] &&
            aMnum  <= m_RichPmtModuleCopyNumEndPanel[3] )
  {
    aM = aMnum - m_RichPmtModuleCopyNumBeginPanel[3];
  }
  return aM;
}

std::vector<int> DeRichPMTPanel::PmtModuleRowColFromModuleNumInPanel(const int aMnum )
{
  int MRow=-1;
  int MCol=-1;

  if ( rich() == Rich::Rich1 )
  {
    MRow = (int) (aMnum/m_RichPmtNumModulesInRowCol[0]);
    MCol= aMnum -MRow*m_RichPmtNumModulesInRowCol[0];
  }
  else if( rich() == Rich::Rich2 || rich() == Rich::Rich )
  {
    MCol = (int) (aMnum/m_RichPmtNumModulesInRowCol[3]);
    MRow = aMnum - MCol*m_RichPmtNumModulesInRowCol[3];
  }
  std::vector<int> aM (2);
  aM[0]= MRow;
  aM[1]= MCol;

  return aM;
}

std::vector<int> DeRichPMTPanel::findPMTArraySetup(const Gaudi::XYZPoint& aGlobalPoint) const
{
  std::vector<int> aCh (4,0);

  const Gaudi::XYZPoint inPanel( geometry()->toLocal(aGlobalPoint) );
  const double x =  inPanel.x();
  const double y =  inPanel.y();

  //  info()<<"Now in findPMTArraySetup  cood inPanel coord "<<inPanel<<"    Rich=  "<<rich()
  //        <<  "    side= "<<side()<<"   modulepitch "<<m_PmtModulePitch << "    GrandModulePitch "<< m_GrandPmtModulePitch
  //        <<"  "<< aGlobalPoint << "  x y    Lensflag  "<< x <<"   "<< y << "  " << m_Rich1PmtLensPresence<< endmsg;



  int  aModuleCol=-1;
  int  aModuleRow=-1;
  int  aModuleNum = -1;
  int  aModuleNumInPanel=-1;
  int  aModuleWithLens=0;

  if ( rich() == Rich::Rich1 )
  {
    if(side() == Rich::top)
    {
      if( m_Rich1PmtLensPresence   ) {
        if( x > m_Rich1PmtPanelWithLensXSize[0] && x <  m_Rich1PmtPanelWithLensXSize[1] ) {
          aModuleCol = ((int) (fabs( (x-m_Rich1PmtPanelWithLensXSize[0])/m_PmtModulePitch )))+ m_Rich1PmtPanelWithLensColSize[2];
          aModuleRow = (int) (fabs( (y-m_PmtModulePlaneHalfSizeR1[1])/m_PmtModulePitch));

        }else if (x < m_Rich1PmtPanelWithLensXSize[0] ) {

          aModuleCol = (int) (fabs( (x-m_Rich1PmtPanelWithLensXSize[2])/m_PmtModuleWithLensPitch ));
          aModuleRow = (int) (fabs( (y-m_Rich1PmtPanelWithLensYSize[0])/m_PmtModuleWithLensPitch ));
          aModuleWithLens=1;

        }else if ( x >  m_Rich1PmtPanelWithLensXSize[1] ) {

          aModuleCol = ( (int) (fabs( (x-m_Rich1PmtPanelWithLensXSize[4])/m_PmtModuleWithLensPitch )))
            +m_Rich1PmtPanelWithLensColSize[0]+ m_Rich1PmtPanelWithLensColSize[2] ;
          aModuleRow = (int) (fabs( (y-m_Rich1PmtPanelWithLensYSize[0])/m_PmtModuleWithLensPitch ));
          aModuleWithLens=1;

        }


      } else {

        aModuleCol = (int) (fabs( (x-m_PmtModulePlaneHalfSizeR1[0])/m_PmtModulePitch ));
        aModuleRow = (int) (fabs( (y-m_PmtModulePlaneHalfSizeR1[1])/m_PmtModulePitch));
      }

      aModuleNum= getPmtModuleNumFromRowCol(aModuleRow,aModuleCol);
      aModuleNumInPanel=aModuleNum-m_RichPmtModuleCopyNumBeginPanel[0];


    }

    else if ( side() == Rich::bottom)
    {
      if( m_Rich1PmtLensPresence   ) {


        if( x > m_Rich1PmtPanelWithLensXSize[0] && x <  m_Rich1PmtPanelWithLensXSize[1] ) {
          aModuleCol = ((int) (fabs( (x-m_Rich1PmtPanelWithLensXSize[1])/m_PmtModulePitch )))+ m_Rich1PmtPanelWithLensColSize[1];
          aModuleRow = (int) (fabs( (y-m_PmtModulePlaneHalfSizeR1[3])/m_PmtModulePitch));

        }else if (x > m_Rich1PmtPanelWithLensXSize[1] ) {

          aModuleCol = (int) (fabs( (x-m_Rich1PmtPanelWithLensXSize[5])/m_PmtModuleWithLensPitch ));
          aModuleRow = (int) (fabs( (y-m_Rich1PmtPanelWithLensYSize[0])/m_PmtModuleWithLensPitch ));
          aModuleWithLens=1;

        }else if ( x <  m_Rich1PmtPanelWithLensXSize[0] ) {

          aModuleCol = ( (int) (fabs( (x-m_Rich1PmtPanelWithLensXSize[3])/m_PmtModuleWithLensPitch )))
            +m_Rich1PmtPanelWithLensColSize[0]+ m_Rich1PmtPanelWithLensColSize[1] ;
          aModuleRow = (int) (fabs( (y-m_Rich1PmtPanelWithLensYSize[0])/m_PmtModuleWithLensPitch ));
          aModuleWithLens=1;

        }




      }else {


        aModuleCol = (int) ( fabs( (x-m_PmtModulePlaneHalfSizeR1[2])/m_PmtModulePitch));
        aModuleRow = (int) ( fabs( (y-m_PmtModulePlaneHalfSizeR1[3])/m_PmtModulePitch));
      }


      aModuleNum= getPmtModuleNumFromRowCol(aModuleRow,aModuleCol);
      aModuleNumInPanel=aModuleNum-m_RichPmtModuleCopyNumBeginPanel[1];
    }
  }
  else if ( rich() == Rich::Rich2 || rich() == Rich::Rich )
  {
    if(side() == Rich::left)
    {
      if( (  !m_Rich2UseGrandModule )  ) {
        
        aModuleCol = (int) (fabs( (x-m_PmtModulePlaneHalfSizeR2[0])/m_PmtModulePitch ));
        aModuleRow = (int) (fabs( (y-m_PmtModulePlaneHalfSizeR2[1])/m_PmtModulePitch));
      }else {
        if( !m_Rich2UseMixedModule ) {          

            aModuleCol = (int) (fabs( (x-m_GrandPmtModulePlaneHalfSizeR2[0])/m_GrandPmtModulePitch ));
            aModuleRow = (int) (fabs( (y-m_GrandPmtModulePlaneHalfSizeR2[1])/m_GrandPmtModulePitch));
        
        }else {
               
          if( fabs(y) <  fabs(m_MixedStdPmtModulePlaneHalfSizeR2 [1]) ) {
            
             aModuleCol = (int) (fabs( (x-m_MixedStdPmtModulePlaneHalfSizeR2[0])/m_PmtModulePitch ));
             aModuleRow = (int) (fabs( (y-m_MixedStdPmtModulePlaneHalfSizeR2[1])/m_PmtModulePitch));
             aModuleRow += m_Rich2MixedModuleArrayColumnSize[0];
          }else {
                           
             aModuleCol = (int) (fabs( (x-m_MixedPmtModulePlaneHalfSizeR2[0])/m_GrandPmtModulePitch ));
             if( y < m_MixedStdPmtModulePlaneHalfSizeR2 [1] ) {
                
               aModuleRow = (int) (fabs( (y-m_MixedPmtModulePlaneHalfSizeR2[1])/m_GrandPmtModulePitch));
             }else if (y >= fabs(m_MixedStdPmtModulePlaneHalfSizeR2 [1])) {
               aModuleRow = (int) (fabs( (y- (fabs(m_MixedStdPmtModulePlaneHalfSizeR2 [1])))/m_GrandPmtModulePitch));
               aModuleRow +=  ( m_Rich2MixedModuleArrayColumnSize[0]+    m_Rich2MixedModuleArrayColumnSize[1]);
             }
             
             
          }       

          
        } // end check on UseMixed module.        
        
      } // end check on any GrandModule use
      
      
      

      aModuleNum= getPmtModuleNumFromRowCol(aModuleRow,aModuleCol);
      aModuleNumInPanel=aModuleNum-m_RichPmtModuleCopyNumBeginPanel[2];
    }  // end of rich2 side left
    
    else if (side() == Rich::right)
    {
      if( (  ! m_Rich2UseGrandModule )  ) {
        aModuleCol = (int) (fabs( (x-m_PmtModulePlaneHalfSizeR2[2])/m_PmtModulePitch ));
        aModuleRow = (int) (fabs( (y-m_PmtModulePlaneHalfSizeR2[3])/m_PmtModulePitch));
      }else {

        if( !  m_Rich2UseMixedModule ) {
          
           aModuleCol = (int) (fabs( (x-m_GrandPmtModulePlaneHalfSizeR2[2])/m_GrandPmtModulePitch ));
           aModuleRow = (int) (fabs( (y-m_GrandPmtModulePlaneHalfSizeR2[3])/m_GrandPmtModulePitch));
        }else {
          if( fabs(y) < fabs( m_MixedStdPmtModulePlaneHalfSizeR2 [3]) ) {

            //  info()<<" Rich2 Module right x y stdEdge Pitch "<< x <<"  "<<y<<"  "<<m_MixedStdPmtModulePlaneHalfSizeR2 <<"  "
            //      << m_PmtModulePitch<<endmsg;
            

             aModuleCol = (int) (fabs( (x-m_MixedStdPmtModulePlaneHalfSizeR2[2])/m_PmtModulePitch ));
             aModuleRow = (int) (fabs( (y-m_MixedStdPmtModulePlaneHalfSizeR2[3])/m_PmtModulePitch));
             aModuleRow += m_Rich2MixedModuleArrayColumnSize[0];
            
          }else {
            //   info()<<" Rich2 Module right x y GrandEdge Pitch "<< x <<"  "<<y<<"  "<<m_MixedPmtModulePlaneHalfSizeR2 <<"  "
            //      << m_GrandPmtModulePitch<<endmsg;
          
             aModuleCol = (int) (fabs( (x-m_MixedPmtModulePlaneHalfSizeR2[2])/m_GrandPmtModulePitch ));
             if( y > m_MixedStdPmtModulePlaneHalfSizeR2 [3] ) {
                
               aModuleRow = (int) (fabs( (y-m_MixedPmtModulePlaneHalfSizeR2[3])/m_GrandPmtModulePitch));
             }else if (y <=  (-1.0*m_MixedStdPmtModulePlaneHalfSizeR2 [3] ) ) {
               aModuleRow = (int) (fabs(   (y- (-1.0*m_MixedStdPmtModulePlaneHalfSizeR2 [3]))/m_GrandPmtModulePitch));
               aModuleRow +=  ( m_Rich2MixedModuleArrayColumnSize[0]+    m_Rich2MixedModuleArrayColumnSize[1]);
             }
             
          }
          
          
          
          
        } // end check on mixed module
        
      } // end check on any GrandModule use
      
      aModuleNum= getPmtModuleNumFromRowCol(aModuleRow,aModuleCol);
      aModuleNumInPanel=aModuleNum-m_RichPmtModuleCopyNumBeginPanel[3];
    } // end rich2 side right
  }
  

  // info() <<"findPmtarray Module col row num "<< rich()<<"  "<<side()<<"   " <<aModuleCol<<"   "<<aModuleRow<<"  "
  //        <<aModuleNum<<"  "<<aModuleNumInPanel<<" modulelens  "<< aModuleWithLens   << endmsg;

  if( aModuleNum >-1 )
  {
    const Gaudi::XYZPoint inModule( ((m_DePMTModules[aModuleNumInPanel])->
                                     geometry()->toLocalMatrix())*aGlobalPoint);

    const double xp= inModule.x();
    const double yp= inModule.y();
    int aPmtCol =-1;
    int aPmtRow =-1;
    int  aPmtNum=-1;

    if(aModuleWithLens== 0 ) {

      if(  ( rich() == Rich::Rich2 )  &&  ( ModuleIsWithGrandPMT(aModuleNum))  ) {
         aPmtCol = (int) (fabs ((xp-m_RichGrandPmtModuleActiveAreaHalfSize[0])/m_GrandPmtPitch));
         aPmtRow = (int) (fabs ((yp-m_RichGrandPmtModuleActiveAreaHalfSize[1])/m_GrandPmtPitch));
         aPmtNum = getGrandPmtNumFromRowCol(aPmtRow,aPmtCol);
       
        
      }else {
        
      

        aPmtCol = (int) (fabs ((xp-m_RichPmtModuleActiveAreaHalfSize[0])/m_PmtPitch));
        aPmtRow = (int) (fabs ((yp-m_RichPmtModuleActiveAreaHalfSize[1])/m_PmtPitch));
        aPmtNum = getPmtNumFromRowCol(aPmtRow,aPmtCol);
      
      }
      
    }else {
      aPmtCol = (int) (fabs  ((xp-(0.5*m_PmtMasterWithLensLateralSize))/m_PmtModuleWithLensPitch));
      aPmtRow = (int) (fabs ((yp-(0.5*m_PmtMasterWithLensLateralSize))/m_PmtModuleWithLensPitch));
      aPmtNum = getLensPmtNumFromRowCol(aPmtRow,aPmtCol);


    }


    if ( aPmtNum > -1 )
    {
      aCh [0]=aModuleNum;
      aCh [1]=aPmtNum;

      //       id.setPD(aModuleNum,aPmtNum);

      const Gaudi::XYZPoint inPmtAnode( (m_DePMTAnodes [aModuleNumInPanel] [aPmtNum ]) ->
                                        geometry() ->toLocalMatrix() *aGlobalPoint);
      // info()<<"findArraysetup inPmtAnode "<<inPmtAnode<<endmsg;

      double  xpi = inPmtAnode.x();
      double  ypi = inPmtAnode.y();


      if(aModuleWithLens > 0 ){
        xpi = xpi/m_Rich1LensMagnificationFactor;
        ypi = ypi/m_Rich1LensMagnificationFactor;

      }
      int aPmtPixelCol =0;
      int aPmtPixelRow =0;
      

      if(  ( rich() == Rich::Rich2 )  &&   ( ModuleIsWithGrandPMT(aModuleNum))  ) {
        aPmtPixelCol = (int) (fabs ((xpi - m_GrandPmtAnodeXEdge)/m_GrandPmtAnodeEffectiveXPixelSize));
        aPmtPixelRow = (int) (fabs ((ypi - m_GrandPmtAnodeYEdge)/m_GrandPmtAnodeEffectiveYPixelSize));    
     
       if( aPmtPixelCol >=  m_GrandPmtPixelsInCol )aPmtPixelCol= m_GrandPmtPixelsInCol-1;
       if( aPmtPixelRow >=  m_GrandPmtPixelsInRow )aPmtPixelRow = m_GrandPmtPixelsInRow-1;

       
     }else {
       
     
       int aPmtPixelCol = (int) (fabs ((xpi - m_PmtAnodeXEdge)/m_PmtAnodeEffectiveXPixelSize));
       int aPmtPixelRow = (int) (fabs ((ypi - m_PmtAnodeYEdge)/m_PmtAnodeEffectiveYPixelSize));    
     
       if( aPmtPixelCol >=  m_PmtPixelsInCol )aPmtPixelCol= m_PmtPixelsInCol-1;
       if( aPmtPixelRow >=  m_PmtPixelsInRow )aPmtPixelRow = m_PmtPixelsInRow-1;
     }
     
      if(aPmtPixelCol < 0 ) aPmtPixelCol=0;
      if(aPmtPixelRow  < 0) aPmtPixelRow=0;


      // info() <<"  pixel col row from local coord to store "<<aPmtPixelCol <<"   "<<aPmtPixelRow<<endmsg;


      aCh[2] = aPmtPixelCol;
      aCh[3] = aPmtPixelRow;

      //id.setPixelRow(aPmtPixelRow);
      // id.setPixelCol(aPmtPixelCol);


    }
    else
    {
      error() << "DeRichPmtPanel : Inadmissible PMT number. Rich side Module Col Row pmt col row  "
              <<rich()<<"   "<< side() << "   "<< aModuleCol <<"  "
              <<aModuleRow<<"  "<<aPmtCol<<"   "<<aPmtRow<< endmsg;
    }

  }
  else
  {
    error() << "DeRichPmtPanel : Inadmissible PMT Module number. Rich side Module Col Row   "
            << rich() <<"   "<< side() << "   "<< aModuleCol <<"  "<<aModuleRow<<endmsg;
  }

  // info()<<" DeRichPanel FindPMTArray aCh "<<aCh<<endmsg;

  return aCh;
}

/// Returns the intersection point with the detector plane given a vector and a point.
LHCb::RichTraceMode::RayTraceResult
DeRichPMTPanel::detPlanePoint( const Gaudi::XYZPoint& pGlobal,
                               const Gaudi::XYZVector& vGlobal,
                               Gaudi::XYZPoint& hitPosition,
                               LHCb::RichSmartID& smartID,
                               const LHCb::RichTraceMode mode ) const
{
  Gaudi::XYZPoint panelIntersection = Gaudi::XYZPoint(0.0,0.0,0.0);  // define a dummy point and fill correctly later.

  StatusCode sc = getPanelInterSection(pGlobal,vGlobal, panelIntersection,hitPosition);

  if ( sc == StatusCode::FAILURE)
  {
    return  LHCb::RichTraceMode::RayTraceFailed;
  }

  std::vector<int> aC = findPMTArraySetup( hitPosition );
  sc = sc && setRichPmtSmartID(aC,smartID );
  if ( sc == StatusCode::FAILURE) 
  {
    return  LHCb::RichTraceMode::RayTraceFailed;
  }
  

  bool isInPanelAcc = isInPmtPanel( panelIntersection  ) ;
  // the following line to be modifed after getting the new values for
  // RichTracemode.

  LHCb::RichTraceMode::RayTraceResult aTr =
    (isInPanelAcc) ? (LHCb::RichTraceMode::InHPDPanel) : LHCb::RichTraceMode::OutsideHPDPanel ;

  return ( (mode.detPlaneBound() == LHCb::RichTraceMode::RespectHPDPanel) ? aTr : LHCb::RichTraceMode::InHPDPanel );
}

LHCb::RichTraceMode::RayTraceResult
DeRichPMTPanel::PDWindowPoint( const Gaudi::XYZVector& vGlobal,
                               const Gaudi::XYZPoint& pGlobal,
                               Gaudi::XYZPoint& windowPointGlobal,
                               LHCb::RichSmartID& smartID,
                               const LHCb::RichTraceMode mode ) const
{
  Gaudi::XYZPoint panelIntersection = Gaudi::XYZPoint(0.0,0.0,0.0);  // define a dummy point and fill correctly later.
  StatusCode sc = getPanelInterSection(pGlobal,vGlobal, panelIntersection,windowPointGlobal);
  if(sc == StatusCode::FAILURE) {
    return  LHCb::RichTraceMode::RayTraceFailed;
  }

  std::vector<int> aC = findPMTArraySetup(windowPointGlobal  );
  sc = sc && setRichPmtSmartID(aC,smartID );
  if(sc == StatusCode::FAILURE) {
    return  LHCb::RichTraceMode::RayTraceFailed;
  }


  bool isInPanelAcc = isInPmtPanel( panelIntersection  ) ;
  bool isInPmtAcc=false;
  bool isInPmtAnodeAcc = false;
  if(isInPanelAcc)
  {
    int aModuleNumInPanel = PmtModuleNumInPanelFromModuleNumAlone(aC[0]);
    const Gaudi::XYZPoint coordinPmt( (m_DePMTs [aModuleNumInPanel] [aC[1]]) ->
                                      geometry() ->toLocalMatrix() *windowPointGlobal );

    bool CoordIsOnLens  = (m_DePMTs [aModuleNumInPanel] [aC[1]]) -> PmtLensFlag();
    const Gaudi::XYZPoint coordinPmtA = CoordIsOnLens ? DemagnifyFromLens(coordinPmt): coordinPmt;
    const bool flagGrandPmt= ModuleIsWithGrandPMT(aC[0]);
    
    isInPmtAcc = isInPmt(coordinPmtA ,flagGrandPmt  );
    if(isInPmtAcc )
    {
      const Gaudi::XYZPoint coordinPmtAnode( (m_DePMTAnodes [aModuleNumInPanel] [aC[1]]) ->
                                             geometry() ->toLocalMatrix() * windowPointGlobal );
      isInPmtAnodeAcc = isInPmtAnodeLateralAcc(coordinPmtAnode,flagGrandPmt );
    }

  }

  // the following line to be modifed after getting the new values for
  // RichTracemode.
  LHCb::RichTraceMode::RayTraceResult res = LHCb::RichTraceMode::InHPDTube;
  if( !isInPanelAcc) {
    res = LHCb::RichTraceMode::OutsideHPDPanel;
  }else {
    if( isInPmtAnodeAcc ) {
      res = LHCb::RichTraceMode::InHPDTube;
    }else {
      res = LHCb::RichTraceMode::InHPDPanel;
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "mode used " <<mode<<endmsg;

  return res;
}
Gaudi::XYZPoint DeRichPMTPanel::DemagnifyFromLens(const Gaudi::XYZPoint aLensPoint) const
{
  double aX = aLensPoint.x()/ m_Rich1LensMagnificationFactor ;
  double aY = aLensPoint.y()/ m_Rich1LensMagnificationFactor ;
  return Gaudi::XYZPoint(aX,aY,aLensPoint.z());

}

StatusCode DeRichPMTPanel::getPanelInterSection ( const Gaudi::XYZPoint& pGlobal,
                                                  const Gaudi::XYZVector& vGlobal ,
                                                  Gaudi::XYZPoint& panelIntersection,
                                                  Gaudi::XYZPoint& panelIntersectionGlobal ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  const Gaudi::XYZVector vInPanel( geometry()->toLocalMatrix() * vGlobal );

  // find the intersection with the detection plane
  const double scalar = vInPanel.Dot(m_localPlaneNormal);

  if ( fabs(scalar) < 1e-5 )
  {
    //sc =  sc &&  StatusCode::FAILURE;
    sc =  StatusCode::FAILURE;
  }
  else
  {
    // transform point to the PMTPanel coordsystem.
    const Gaudi::XYZPoint pInPanel( geometry()->toLocal(pGlobal) );



    // get panel intersection point
    const double distance = -m_localPlane.Distance(pInPanel) / scalar;



    panelIntersection = Gaudi::XYZPoint ( pInPanel + distance*vInPanel );
    panelIntersectionGlobal =geometry()->toGlobal( panelIntersection );


  }

  return sc;
}

bool DeRichPMTPanel::isInPmtPanel(const Gaudi::XYZPoint& aPointInPanel ) const
{
  bool inAcc= false;
  double x = aPointInPanel.x();
  double y = aPointInPanel.y();

  if ( rich() == Rich::Rich1 )
  {
    if ( side() == Rich::top )
    {
      if( ( fabs(x) <  fabs(m_PmtModulePlaneHalfSizeR1[0]) )  &&
          (fabs (y) <  fabs(m_PmtModulePlaneHalfSizeR1[1]) ) )  inAcc=true;
    }
    else if (side() == Rich::bottom )
    {
      if ( ( fabs(x) <  fabs(m_PmtModulePlaneHalfSizeR1[2]) )  &&
           (fabs (y) <  fabs(m_PmtModulePlaneHalfSizeR1[3]) ) )  inAcc=true;
    }

  }
  else if (rich() == Rich::Rich2 || rich() == Rich::Rich )
  {
    if( side() == Rich::left )
    {
      if(   m_Rich2UseGrandModule ) {

        if( !   m_Rich2UseMixedModule ) { 
    
          if ( ( fabs(x) <  fabs(m_GrandPmtModulePlaneHalfSizeR2[0]) )  &&
               (fabs (y) <  fabs(m_GrandPmtModulePlaneHalfSizeR2[1]) ) )  inAcc=true;
      
        } else {

          if ( ( fabs(x) <  fabs(m_MixedPmtModulePlaneHalfSizeR2[0]) )  &&
               (fabs (y) <  fabs(m_MixedPmtModulePlaneHalfSizeR2[1]) ) )  inAcc=true;
          
        }       
        
      } else {
        
      
       if ( ( fabs(x) <  fabs(m_PmtModulePlaneHalfSizeR2[0]) )  &&
           (fabs (y) <  fabs(m_PmtModulePlaneHalfSizeR2[1]) ) )  inAcc=true;
       
      }
      
      
    }
    else if (side () == Rich::right )
    {

      if(  m_Rich2UseGrandModule ) {

        if( !   m_Rich2UseMixedModule ) { 
    

         if ( ( fabs(x) <  fabs(m_GrandPmtModulePlaneHalfSizeR2[2]) )  &&
              (fabs (y) <  fabs(m_GrandPmtModulePlaneHalfSizeR2[3]) ) )  inAcc=true;
         
        }else {
        
          if ( ( fabs(x) <  fabs(m_MixedPmtModulePlaneHalfSizeR2[2]) )  &&
               (fabs (y) <  fabs(m_MixedPmtModulePlaneHalfSizeR2[3]) ) )  inAcc=true;        
        }
        
        
      }else {
        
      

      if ( ( fabs(x) <  fabs(m_PmtModulePlaneHalfSizeR2[2]) )  &&
           (fabs (y) <  fabs(m_PmtModulePlaneHalfSizeR2[3]) ) )  inAcc=true;

      
      }
      
    }

  }

  return inAcc;
}


bool DeRichPMTPanel::isInPmt(const Gaudi::XYZPoint& aPointInPmt , const bool aFlagGrandPMT ) const
{
  const double xp = aPointInPmt.x();
  const double yp = aPointInPmt.y();
  const double aPmtH = ( (aFlagGrandPMT)   && (rich() == Rich::Rich2)  ) ?  (m_GrandPmtMasterLateralSize/2.0 )  
    :  (m_PmtMasterLateralSize/2.0);
  return ( (fabs(xp) <  aPmtH ) && (fabs (yp) < aPmtH ) );
}

bool DeRichPMTPanel::isInPmtAnodeLateralAcc(const Gaudi::XYZPoint& aPointInPmtAnode, const bool aFlagGrandPMT  ) const
{
  const double xp = aPointInPmtAnode.x();
  const double yp = aPointInPmtAnode.y();
  bool  aflgpx = true;
  if(  aFlagGrandPMT   && (rich() == Rich::Rich2) ) {

    aflgpx = ( fabs(xp) < fabs( m_GrandPmtAnodeXEdge ) ) && ( fabs(yp) < fabs( m_GrandPmtAnodeYEdge ) );
      
  }else {
    aflgpx = ( fabs(xp) < fabs( m_PmtAnodeXEdge ) ) && ( fabs(yp) < fabs( m_PmtAnodeYEdge ) );
    
  }
  const bool aflgpxC= aflgpx;
  
  
  return aflgpxC;
}


unsigned int DeRichPMTPanel::pdNumber( const LHCb::RichSmartID& smartID ) const
{
  //  info()<<"derich pmt panel smartid "<<smartID <<endmsg;

  return ( smartID.rich() == rich() && smartID.panel() == side() ?
           smartID.pdCol() * m_NumPmtInRichModule  + smartID.pdNumInCol() :
           nPDs() + 1 );
}
bool DeRichPMTPanel::pdGrandSize( const LHCb::RichSmartID& smartID ) const 
{
  return ModuleIsWithGrandPMT(smartID.pdCol());
}


//const DeRichPD* DeRichPMTPanel::dePD( const unsigned int PmtCopyNumber ) const
//{
//  const DeRichPD * dePmt = NULL;

//  if ( ((int) PmtCopyNumber) < ( m_Rich1TotNumPmts + m_Rich2TotNumPmts  ) )
//  {
//   const unsigned int Mnum = (int) (PmtCopyNumber/m_NumPmtInRichModule);
//    const unsigned int MNumInCurPanel = PmtModuleNumInPanelFromModuleNumAlone(Mnum);
//    const unsigned int Pnum =  PmtCopyNumber - ( Mnum * m_NumPmtInRichModule);

//    if ( MNumInCurPanel >= m_DePMTs.size() ||
//         Pnum >= m_DePMTs[MNumInCurPanel].size() )
//    {
//      std::ostringstream mess;
//      mess << "Inappropriate PMT numbers " << MNumInCurPanel << " " << Pnum;
//      throw GaudiException( mess.str(), "*DeRichPMTPanel*",StatusCode::FAILURE);
//    }
//
//    dePmt = m_DePMTs [ MNumInCurPanel ] [ Pnum ];
//
//  }
//  else
//  {
//    std::ostringstream mess;
//    mess << "Inappropriate PmtNumber : " << PmtCopyNumber;
//    throw GaudiException( mess.str(),"*DeRichPMTPanel*",StatusCode::FAILURE);
//  }

//  return dePmt;
//}

const DeRichPD* DeRichPMTPanel::dePD( const unsigned int PmtCopyNumber ) const
{

  const DeRichPD* aPD =  dePMT(  PmtCopyNumber );

  return aPD;
}


const DeRichPMT* DeRichPMTPanel::dePMT( const unsigned int PmtCopyNumber ) const
{
  const DeRichPMT * dePmt = NULL;

  int aTotNumPmt= m_Rich1TotNumPmts + m_Rich2TotNumPmts;
  //  if(rich() == Rich::Rich2 && m_Rich2UseGrandModule ) {
  //    aTotNumPmt= m_Rich1TotNumPmts + m_Rich2TotNumPmts;
  // }
  

  if ( ((int) PmtCopyNumber) < ( aTotNumPmt  ) )
  {
    const unsigned int Mnum = (int) (PmtCopyNumber/m_NumPmtInRichModule);
    const unsigned int MNumInCurPanel = PmtModuleNumInPanelFromModuleNumAlone(Mnum);
    const unsigned int Pnum =  PmtCopyNumber - ( Mnum * m_NumPmtInRichModule);
    // info()<<"DeRichPMTPanel  current rich side pmtcopynum "<<rich()<<"  "<<side()<<"  "<<PmtCopyNumber<<  endmsg;

    //        info()<<"dePmtpanel depmt mNum "<<Mnum << endmsg;
    //  info()<<"depmts sizes "<< (int) m_DePMTs.size()
    //     <<"  "<< m_DePMTs[MNumInCurPanel].size()<<endmsg;


    if ( MNumInCurPanel >= m_DePMTs.size() ||
         Pnum >= m_DePMTs[MNumInCurPanel].size() )
    {
      std::ostringstream mess;
      mess << "DeRichPMTPanel: Inappropriate PMT module and pmt numbers " << MNumInCurPanel << " " << Pnum;

      throw GaudiException( mess.str(), "*DeRichPMTPanel*",StatusCode::FAILURE);
    }else {


      dePmt = m_DePMTs [ MNumInCurPanel ] [ Pnum ];
    }

  }
  else
  {
    std::ostringstream mess;
    mess << "DeRichPMTPanel: Inappropriate PmtcopyNumber : " << PmtCopyNumber;
    throw GaudiException( mess.str(),"*DeRichPMTPanel*",StatusCode::FAILURE);
  }

  return dePmt;
}

Gaudi::XYZPoint DeRichPMTPanel::detPointOnAnode( const LHCb::RichSmartID smartID ) const
{
  const DeRichPMT * aPMT = dePMT( pdNumber( smartID ) );
  return aPMT->detPointOnAnode(smartID);
}

//  return a list with all the valid readout channels (smartIDs)
//=========================================================================
StatusCode
DeRichPMTPanel::readoutChannelList ( LHCb::RichSmartID::Vector& readoutChannels ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  int CurPanelNum = 0 ;
  if ( rich() == Rich::Rich1 )
  {
    if( side() == Rich::top )
    {
      CurPanelNum=0 ;
    }
    else if (side() == Rich::bottom )
    {
      CurPanelNum=1 ;
    }
  }
  else if (rich() == Rich::Rich2 || rich() == Rich::Rich )
  {
    if( side() == Rich::left )
    {
      CurPanelNum=2 ;
    }
    else if (side() == Rich::right )
    {
      CurPanelNum=3 ;
    }
  }

  int aBeginM=m_RichPmtModuleCopyNumBeginPanel[CurPanelNum];
  int aEndM = m_RichPmtModuleCopyNumEndPanel[CurPanelNum];
  // int aNumMod = aEndM-aBeginM+1;

  for (int iM=aBeginM ; iM <= aEndM; ++iM )
  {
    for (int iP=0; iP < m_NumPmtInRichModule; ++iP )
    {
      for (int iPx =0; iPx < m_PmtPixelsInRow ; ++iPx )
      {
        for (int iPy=0; iPy <m_PmtPixelsInCol; ++iPy )
        {
          const LHCb::RichSmartID id(rich(), side(), iP, iM, iPy, iPx, LHCb::RichSmartID::MaPMTID );
          readoutChannels.push_back (id );
        }
      }
    }
  }

  return StatusCode::SUCCESS;
}

int DeRichPMTPanel::sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const
{
  // Create a RichSmartID for this RICH and panel

  LHCb::RichSmartID id(rich(), side(), 0, 0, 0, 0,LHCb::RichSmartID::MaPMTID );
  // set the remaining fields from the position
  return ( smartID(globalPoint,id).isSuccess() ?
           id : LHCb::RichSmartID( rich(), side(), 0, 0, 0, 0,LHCb::RichSmartID::MaPMTID)   );
}


//=========================================================================
//  setRichPanelAndSide
//=========================================================================
StatusCode DeRichPMTPanel::setRichPanelAndSide ( )
{
  const Gaudi::XYZPoint zero(0.0, 0.0, 0.0);
  const Gaudi::XYZPoint centreGlobal(geometry()->toGlobal(zero));

  // Work out what Rich/panel I am
  if ( name().find("Rich1") != std::string::npos )
  {
    m_rich = Rich::Rich1;
    if ( centreGlobal.y() > 0.0 )
    {
      m_side = Rich::top;
    }
    else
    {
      m_side = Rich::bottom;
    }
  }
  // Rich2
  else if ( name().find("Rich2") != std::string::npos )
  {
    m_rich = Rich::Rich2;
    if ( centreGlobal.x() > 0.0 )
    {
      m_side = Rich::left;
    }
    else
    {
      m_side = Rich::right;
    }
  }
  // Single Rich
  else if ( name().find("Rich/") != std::string::npos )
  {
    m_rich = Rich::Rich;
    if ( centreGlobal.x() > 0.0 )
    {
      m_side = Rich::left;
    }
    else
    {
      m_side = Rich::right;
    }
  }
  // problem
  else
  {
    error() << "Cannot identify Rich/side" << endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}
//=========================================================================
//  seModulecipy number
//=========================================================================

int DeRichPMTPanel::getModuleCopyNumber ( std::string aModuleName){

  int anumber=-1;
   const std::string::size_type pos2 = aModuleName.find(":");
   if ( std::string::npos == pos2 ){    error() << "A PMTModule  without a number!   " <<aModuleName<< endmsg;
     
   }else {
     anumber = atoi( aModuleName.substr(pos2+1).c_str() );
   }
   
   return anumber;
   

}





                           

