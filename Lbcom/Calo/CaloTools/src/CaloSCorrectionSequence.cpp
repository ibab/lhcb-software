// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ObjectVector.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "CaloKernel/CaloCellID.h"
#include "CaloKernel/CaloTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
//#include "Event/CaloDigitStatus.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
#include "Event/CaloDataFunctor.h"

#include "CaloSCorrectionSequence.h"

// ============================================================================
/** @file CaloSCorrectionSequence.cpp
 *
 *  Implementation file for class : CaloSCorrectionSequence
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   20/06/2002
 */
// ============================================================================


// ============================================================================
/** @var CaloSCorrectionSequenceFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloSCorrectionSequence>         s_factory ;
const        IToolFactory&CaloSCorrectionSequenceFactory = s_factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @see   CaloTool
 *  @see    AlgTool 
 *  @param type    type of the tool  (?)
 *  @param name    name of the concrete instance 
 *  @param parent  pointer to parent object (algorithm, service or tool)
 */
// ============================================================================
CaloSCorrectionSequence::CaloSCorrectionSequence(const std::string& type, 
                                                 const std::string& name, 
                                                 const IInterface* parent) 
  : CaloTool( type, name, parent )
  , m_ISCorrectionNameX()
  , m_ISCorrectionNameY()
  , m_ISCorrectionBorderName() {
  declareInterface<ICaloHypoTool>(this);
  declareProperty("SCorrectionsX",m_ISCorrectionNameX);
  declareProperty("SCorrectionsY",m_ISCorrectionNameY);
  declareProperty("SCorrectionborder",m_ISCorrectionBorderName = "");
}
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloSCorrectionSequence::~CaloSCorrectionSequence() {}
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSCorrectionSequence::initialize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "intialize() has been called" << endreq;
  /// initialize the base class
  StatusCode sc = CaloTool::initialize();
  if( sc.isFailure() ) { 
    Error("Could not initialize the base class ",sc);
    return StatusCode::FAILURE;
  }
  ///
  if( 0 == det() ) {
    if( 0 == detSvc() ) { 
      Error("Detector Data Service is invalid!"); 
      return StatusCode::FAILURE;
    }
    ///
    SmartDataPtr<DeCalorimeter> calo( detSvc() , detName() );
    if( !calo ) {
      Error("Could not locate detector='"+detName()+"'");
      return StatusCode::FAILURE;
    }
    /// set detector
    setDet( calo );
  }
  if( 0 == det() ) {
    Error("DeCalorimeter* points to NULL!");
    return StatusCode::FAILURE;
  }
  /// FunctionTools initialisation
  if (m_ISCorrectionNameX.size()!=3) {
    log << MSG::INFO << "number of tools along X/area !=3 " 
        << endreq;
    return StatusCode::FAILURE;
  }
  m_SameXY = false;
  if (m_ISCorrectionNameY.size()==0) {
    log << MSG::INFO << "number of tools along Y/area ==0 " 
        << "assuming same as along X"
        << endreq;
    m_SameXY=true;
  } else {  
    if (m_ISCorrectionNameY.size()!=3) {
      log << MSG::INFO << "number of tools along Y/area !=0 and !=3 "
          << endreq;
      return StatusCode::FAILURE;
    }
  }
  if (m_SameXY==true) {
    log << MSG::INFO << "SCorrection tools: " << endreq;
    int i=0;
    std::vector<std::string>::iterator ToolName;
    for (ToolName=m_ISCorrectionNameX.begin();
         ToolName!=m_ISCorrectionNameX.end();
         ++ToolName,i++) {
      log << MSG::INFO << " area:" << i
          << " name:" << *ToolName
          << endreq;
      ICaloSCorrectionFunction* mytool = tool(*ToolName,mytool);
      if (mytool==0) {return StatusCode::FAILURE;}
      m_ISCorrectionX.push_back(mytool);
      m_ISCorrectionY.push_back(mytool);
    }
  } else {
     log << MSG::INFO << "SCorrection tools along X: " << endreq;
     int i=0;      
     std::vector<std::string>::iterator ToolName;
     for (ToolName=m_ISCorrectionNameX.begin();
          ToolName!=m_ISCorrectionNameX.end();
         ++ToolName,i++) {
      log << MSG::INFO << " area:" << i
          << " name:" << *ToolName
          << endreq;
      ICaloSCorrectionFunction* mytool = tool(*ToolName,mytool);
      if (mytool==0) {return StatusCode::FAILURE;}
      m_ISCorrectionX.push_back(mytool);
     }
     log << MSG::INFO << "SCorrection tools along Y: " << endreq;
     i=0;
     for (ToolName=m_ISCorrectionNameY.begin();
          ToolName!=m_ISCorrectionNameY.end();
         ++ToolName,i++) {
      log << MSG::INFO << " area:" << i
          << " name:" << *ToolName
          << endreq;
      ICaloSCorrectionFunction* mytool = tool(*ToolName,mytool);
      if (mytool==0) {return StatusCode::FAILURE;}
      m_ISCorrectionY.push_back(mytool);
     }
  }  

  m_Border=true;
  if ((m_ISCorrectionBorderName == "no")||(m_ISCorrectionBorderName == "")) {
    m_Border=false;
    log << MSG::INFO << " NO border modification" << endreq;
  } else {
    log << MSG::INFO << " border" 
        << " name:" << m_ISCorrectionBorderName
        << endreq;
    m_ISCorrectionBorder = tool(m_ISCorrectionBorderName,m_ISCorrectionBorder);
  }
  return StatusCode::SUCCESS;
}
// ============================================================================

// ============================================================================
/** standard finalization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSCorrectionSequence::finalize()
{
  MsgStream log(msgSvc(), name());
  log << MSG::VERBOSE << "finalize() has been called" << endreq;
  /// remove detector
  setDet( (const DeCalorimeter*) 0 );
  /// finalize the  the base class
  return CaloTool::finalize ();
}
// ============================================================================

// ============================================================================
/** The main processing method
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloSCorrectionSequence::process( CaloHypo* hypo ) const
{ return (*this) ( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloSCorrectionSequence::operator() ( CaloHypo* hypo ) const {
  StatusCode sc;
  MsgStream logmsg(msgSvc(), name());
  logmsg << MSG::VERBOSE << "process() has been called" << endreq;
  if (hypo==0) {return StatusCode::FAILURE;}
  if (hypo->hypothesis()!=CaloHypotheses::Photon) {return StatusCode::FAILURE;}
  SmartRefVector<CaloCluster> clusters = hypo->clusters();
  if (clusters.size()!=1) {return StatusCode::FAILURE;}
  CaloCluster *cluster;
  for (SmartRef<CaloCluster> *clusterloop = clusters.begin();
       clusterloop!=clusters.end();
       ++clusterloop) {
    if (clusterloop!=0) {cluster = *(clusterloop);}
  }
  std::vector<CaloClusterEntry> sac = cluster->entries();
  double maxenergy=0.;
  SmartRef<CaloDigit> seed;
  logmsg << MSG::VERBOSE << "looping on digits:" << endreq;
  for (CaloClusterEntry* i=sac.begin();i!=sac.end();++i) {
    if (i==0) {continue;}
    SmartRef<CaloDigit> j=i->digit();
    if (j==0) {continue;}
    if (j->e()*i->fraction()>maxenergy) {
      maxenergy=j->e()*i->fraction();
      seed=j;
    }
    logmsg << MSG::VERBOSE << "e()= " << j->e()*i->fraction() << endreq;
  }
  if (seed==0) {return StatusCode::FAILURE;}
  if (maxenergy==0) {return StatusCode::FAILURE;}
  logmsg << MSG::VERBOSE << "seed energy: " << seed->e() << endreq;
  logmsg << MSG::VERBOSE << "seed cell: " << seed->cellID() << endreq;
  double xseed=det()->cellX(seed->cellID())*mm;
  logmsg << MSG::VERBOSE << "seed cell x: " << xseed << endreq;
  double yseed=det()->cellY(seed->cellID())*mm;
  logmsg << MSG::VERBOSE << "seed cell y: " << yseed << endreq;
  int rowseed=seed->cellID().row();
  logmsg << MSG::VERBOSE << "seed cell raw: " << rowseed << endreq;
  int colseed=seed->cellID().col();
  logmsg << MSG::VERBOSE << "seed cell col: " << colseed << endreq;
  unsigned int areaseed=seed->cellID().area();
  logmsg << MSG::VERBOSE << "seed cell area: " << areaseed << endreq;
  if (areaseed>2) {
    return StatusCode::FAILURE;
  }
  double sizeseed=det()->cellSize(seed->cellID())*mm;
  logmsg << MSG::VERBOSE << "seed cell size: " << sizeseed << endreq;
  if (sizeseed<0.1) {
    return StatusCode::FAILURE;
  }
  bool border=false;
  int numberofneighbor = 0;
  for (const CaloCellID *cellloop = 
         det()->neighborCells(seed->cellID()).begin();
       cellloop!=det()->neighborCells(seed->cellID()).end();
       ++cellloop) {
    numberofneighbor++;
    if (areaseed!=cellloop->area()) {border=true;}
  }
  if (numberofneighbor!=8) {border=true;}

  double E[3][3];
  {
  for (int i=0;i<3;i++) {
    for (int j=0;j<3;j++) {E[i][j]=0.0;}
  }
  }
  { for (CaloClusterEntry* i=sac.begin();i!=sac.end();++i) {
    if (i==0) {continue;}
    SmartRef<CaloDigit> j=i->digit();
    if (j==0) {continue;}
    int row=j->cellID().row()-rowseed+1;
    int col=j->cellID().col()-colseed+1;
    if ((row>=0)&&(row<=2)&&(col>=0)&&(col<=2)) {
      E[col][row]=j->e()*i->fraction();
    }
  }}
  {for (int i=0;i<3;i++) {
    logmsg << MSG::DEBUG << "|" << E[2][i]
        << "|" << E[1][i] << "|" << E[0][i] << endreq;
  }}
  double energy=0.;
  double eleft=0.,evert=0.,eright=0.;
  double ebottom=0.,ehori=0.,etop=0.;
  {for (int i=0;i<3;i++) {
    for (int j=0;j<3;j++) {
      if (i==0) {eleft+=E[i][j];}
      if (i==1) {evert+=E[i][j];}
      if (i==2) {eright+=E[i][j];}
      if (j==0) {ebottom+=E[i][j];}
      if (j==1) {ehori+=E[i][j];}
      if (j==2) {etop+=E[i][j];}
      energy+=E[i][j];
    }
  }}
  logmsg << MSG::VERBOSE << "energy: " << energy << endreq;

  double x=0.,y=0.;
  double xprime=0.,yprime=0.;
  if (border==false) {
    logmsg << MSG::VERBOSE << "calling X-tool[" << areaseed << "]..." << endreq;
    logmsg << MSG::VERBOSE << "eleft:" << eleft 
           << " evert:" << evert << " eright:" << eright << endreq;
    sc = m_ISCorrectionX[areaseed]->
      calculate(eleft,evert,eright,energy,xseed,yseed,x);
    if (sc!=StatusCode::SUCCESS) {return sc;}
    sc = m_ISCorrectionX[areaseed]->
      calculateprime(eleft,evert,eright,energy,xseed,yseed,xprime);
    if (sc!=StatusCode::SUCCESS) {return sc;}
    logmsg << MSG::VERBOSE << "x:" << x << endreq;
    logmsg << MSG::VERBOSE << "calling Y-tool[" << areaseed << "]..." << endreq;
    sc = m_ISCorrectionY[areaseed]->
      calculate(ebottom,ehori,etop,energy,xseed,yseed,y);
    if (sc!=StatusCode::SUCCESS) {return sc;}
    sc = m_ISCorrectionY[areaseed]->
      calculateprime(ebottom,ehori,etop,energy,xseed,yseed,yprime);
    if (sc!=StatusCode::SUCCESS) {return sc;}
    logmsg << MSG::VERBOSE << "y:" << y << endreq;
  } else {
    if (m_Border==true) {
      logmsg << MSG::VERBOSE << "calling X-tool on border..." << endreq;
      sc = m_ISCorrectionBorder->
        calculate(eleft,evert,eright,energy,xseed,yseed,x);
      if (sc!=StatusCode::SUCCESS) {return sc;}
      sc = m_ISCorrectionBorder->
        calculateprime(eleft,evert,eright,energy,xseed,yseed,xprime);
      if (sc!=StatusCode::SUCCESS) {return sc;}
      logmsg << MSG::VERBOSE << "x:" << x << endreq;
      logmsg << MSG::VERBOSE << "calling Y-tool on border..." << endreq;
      sc = m_ISCorrectionBorder->
        calculate(ebottom,ehori,etop,energy,xseed,yseed,y);
      if (sc!=StatusCode::SUCCESS) {return sc;}
      sc = m_ISCorrectionBorder->
        calculateprime(ebottom,ehori,etop,energy,xseed,yseed,yprime);
      if (sc!=StatusCode::SUCCESS) {return sc;}
      logmsg << MSG::VERBOSE << "y:" << y << endreq;
    }
  }
  x*=sizeseed;
  x+=xseed;
  y*=sizeseed;
  y+=yseed;
  // X/Y/E: pass data to CaloPosition
  CaloPosition* position = new CaloPosition();
  //CaloPosition* position  = hypo->position();
  logmsg << MSG::VERBOSE << "CaloPosition created..." << endreq;
  HepVector localposition(3);
  localposition(CaloPosition::X)=x;
  localposition(CaloPosition::X)=y;
  localposition(CaloPosition::E)=energy;
  position->setParameters(localposition);
  logmsg << MSG::VERBOSE << "X/Y/E updated..." << endreq;

  HepSymMatrix localcovariance(3,1);
  //HepSymMatrix localcovariance((hypo->position())->covariance());
  /*
  localcovariance(CaloPosition::E,CaloPosition::E)=
    ((hypo->position())->covariance())(CaloPosition::E,CaloPosition::E);
  localcovariance(CaloPosition::E,CaloPosition::X)=xprime
    *((hypo->position())->covariance())(CaloPosition::E,CaloPosition::X);
  localcovariance(CaloPosition::E,CaloPosition::Y)=yprime
    *((hypo->position())->covariance())(CaloPosition::E,CaloPosition::Y);
  localcovariance(CaloPosition::X,CaloPosition::X)=xprime*xprime
    *((hypo->position())->covariance())(CaloPosition::X,CaloPosition::X);
  localcovariance(CaloPosition::X,CaloPosition::Y)=xprime*yprime
    *((hypo->position())->covariance())(CaloPosition::Y,CaloPosition::Y);
  localcovariance(CaloPosition::Y,CaloPosition::Y)=yprime*yprime
    *((hypo->position())->covariance())(CaloPosition::Y,CaloPosition::Y);
    */
  position->setCovariance(localcovariance);  
  /*
  //CaloPosition *old = hypo->position();
  //HepSymMatrix oldcov = ((hypo->position())->covariance());
  //HepSymMatrix cov = (hypo->position())->covariance();
  CaloPosition* old = hypo->position();
  
  //CaloPosition::Covariance oldcov(hypo->position()->covariance());  
  logmsg << MSG::VERBOSE 
         << ":" << position->covariance()
         << ":" << old->covariance()
         << endreq;
  */
  logmsg << MSG::VERBOSE << "Covariance updated..." << endreq;
  hypo->setPosition(position);  
  logmsg << MSG::VERBOSE << "CaloHypo updated..." << endreq;

  return StatusCode::SUCCESS;
}
