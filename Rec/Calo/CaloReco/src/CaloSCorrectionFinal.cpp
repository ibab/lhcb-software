// Include files
#include <numeric>
#include <algorithm>
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

#include "CaloSCorrectionFinal.h"

#include "math.h"

// ============================================================================
/** @file CaloSCorrectionFinal.cpp
 *
 *  Implementation file for class : CaloSCorrectionFinal
 *
 *  @author Richard Beneyton beneyton@in2p3.fr
 *  @date   14/11/2002
 */
// ============================================================================

#ifdef WIN32
namespace std 
{
    template <class T>
		inline const T& max( const T& a , const T& b )
	{ return (a<b) ? b : a ;};

};
#endif 

// ============================================================================
/** @var CaloSCorrectionFinalFactory
 *  Mandatory declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloSCorrectionFinal>         s_factory ;
const        IToolFactory&CaloSCorrectionFinalFactory = s_factory ; 
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
CaloSCorrectionFinal::CaloSCorrectionFinal(const std::string& type, 
                                           const std::string& name, 
                                           const IInterface* parent) 
  : CaloTool( type, name, parent )
  , m_Coeff_area_0_X()
  , m_Coeff_area_0_Y()
  , m_Coeff_area_1_X()
  , m_Coeff_area_1_Y()
  , m_Coeff_area_2_X()
  , m_Coeff_area_2_Y() 
  , m_a2GeV(ResA * ResA * GeV)
  , m_b2(ResB * ResB)
  , m_s2gain(GainS * GainS )
  , m_s2incoherent(NoiseIn * NoiseIn)
  , m_s2coherent(NoiseCo * NoiseCo) {
  declareInterface<ICaloHypoTool>(this);
  declareProperty("Coeff0X"     ,m_Coeff_area_0_X);
  declareProperty("Coeff0Y"     ,m_Coeff_area_0_Y);
  declareProperty("Coeff1X"     ,m_Coeff_area_1_X);
  declareProperty("Coeff1Y"     ,m_Coeff_area_1_Y);
  declareProperty("Coeff2X"     ,m_Coeff_area_2_X);
  declareProperty("Coeff2Y"     ,m_Coeff_area_2_Y);
  declareProperty("a2GeV"       ,m_a2GeV         );
  declareProperty("b2"          ,m_b2            );
  declareProperty("s2gain"      ,m_s2gain        );
  declareProperty("s2incoherent",m_s2incoherent  );
  declareProperty("s2coherent"  ,m_s2coherent    );
}
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
CaloSCorrectionFinal::~CaloSCorrectionFinal() {}
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloTool 
 *  @see  AlgTool
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSCorrectionFinal::initialize()
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
  /// coeffs initialisation
  if (m_Coeff_area_0_X.size()!=3) {
    if (m_Coeff_area_0_X.size()==0) {
      m_Coeff_area_0_X.push_back(0.5431);
      m_Coeff_area_0_X.push_back(0.08069);
      m_Coeff_area_0_X.push_back(-0.001040);
    } else {
      return StatusCode::FAILURE;
    }
  }
  if (m_Coeff_area_0_Y.size()!=3) {
    if (m_Coeff_area_0_Y.size()==0) {
      m_Coeff_area_0_Y.push_back(0.5571);
      m_Coeff_area_0_Y.push_back(0.09588);
      m_Coeff_area_0_Y.push_back(-0.0006399);
    } else {
      return StatusCode::FAILURE;
    }
  }
  if (m_Coeff_area_1_X.size()!=3) {
    if (m_Coeff_area_1_X.size()==0) {
      m_Coeff_area_1_X.push_back(0.6476);
      m_Coeff_area_1_X.push_back(0.1787);
      m_Coeff_area_1_X.push_back(-0.002058);
    } else {
      return StatusCode::FAILURE;
    }
  }
  if (m_Coeff_area_1_Y.size()!=3) {
    if (m_Coeff_area_1_Y.size()==0) {
      m_Coeff_area_1_Y.push_back(0.6683);
      m_Coeff_area_1_Y.push_back(0.2044);
      m_Coeff_area_1_Y.push_back(-0.002140);
    } else {
      return StatusCode::FAILURE;
    }
  }
  if (m_Coeff_area_2_X.size()!=3) {
    if (m_Coeff_area_2_X.size()==0) {
      m_Coeff_area_2_X.push_back(0.6258);
      m_Coeff_area_2_X.push_back(0.1770);
      m_Coeff_area_2_X.push_back(-0.001298);
    } else {
      return StatusCode::FAILURE;
    }
  }
  if (m_Coeff_area_2_Y.size()!=3) {
    if (m_Coeff_area_2_Y.size()==0) {
      m_Coeff_area_2_Y.push_back(0.7194);
      m_Coeff_area_2_Y.push_back(0.2356);
      m_Coeff_area_2_Y.push_back(-0.0001821);
    } else {
      return StatusCode::FAILURE;
    }
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
StatusCode CaloSCorrectionFinal::finalize()
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
StatusCode CaloSCorrectionFinal::process( CaloHypo* hypo ) const
{ return (*this) ( hypo ); };
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @see ICaloHypoTool
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode CaloSCorrectionFinal::operator() ( CaloHypo* hypo ) const {

  MsgStream logmsg(msgSvc(), name());
  logmsg << MSG::VERBOSE << "process() has been called" << endreq;
  int i,j,k,l;

  if (hypo==0) {return StatusCode::FAILURE;}
  if (hypo->hypothesis()!=CaloHypotheses::Photon) {return StatusCode::FAILURE;}
  SmartRefVector<CaloCluster> clusters = hypo->clusters();
  double oldenergy = hypo->e();
  logmsg << MSG::VERBOSE << "old_energy="
         << oldenergy << endreq;
  if (clusters.size()!=1) {return StatusCode::FAILURE;}
  CaloCluster *cluster;
  SmartRef<CaloCluster> *clusterloop;
  for (clusterloop = clusters.begin();
       clusterloop!=clusters.end();
       ++clusterloop) {
    if (clusterloop!=0) {cluster = *(clusterloop);}
  }
  std::vector<CaloClusterEntry> sac = cluster->entries();
  double maxenergy=0.;
  SmartRef<CaloDigit> seed;
  logmsg << MSG::VERBOSE << "looping on digits:" << endreq;
  CaloClusterEntry* cce;
  for (cce=sac.begin();cce!=sac.end();++cce) {
    if (cce==0) {continue;}
    SmartRef<CaloDigit> cd=cce->digit();
    if (cd==0) {continue;}
    if (cd->e()*cce->fraction()>maxenergy) {
      maxenergy=cd->e()*cce->fraction();
      seed=cd;
    }
    logmsg << MSG::VERBOSE << "e()= " << cd->e()*cce->fraction() << endreq;
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
  {
    for (const CaloCellID *cellloop = 
           det()->neighborCells(seed->cellID()).begin();
         cellloop!=det()->neighborCells(seed->cellID()).end();
         ++cellloop) {
      numberofneighbor++;
      if (areaseed!=cellloop->area()) {border=true;}
    }
  }
  if (numberofneighbor!=8) {border=true;}

  // 'diagonal' covariance allocation
  double E[3][3];
  double cov_ii[3][3];
  double gain[3][3];
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      E[i][j]=0.0;
      cov_ii[i][j]=0.0;
      gain[i][j]=0.0;
    }
  }
  for (cce=sac.begin();cce!=sac.end();++cce) {
    if (cce==0) {continue;}
      SmartRef<CaloDigit> cd=cce->digit();
      if (cd==0) {continue;}
      int row=cd->cellID().row()-rowseed+1;
      int col=cd->cellID().col()-colseed+1;
      if ((row>=0)&&(row<=2)&&(col>=0)&&(col<=2)) {
        E[col][row]=cd->e()*cce->fraction();        
        // intrinsic resolution 
        cov_ii[col][row] = fabs(cd->e()*cce->fraction())*m_a2GeV; 
        if( 0 != m_b2 ) {
          cov_ii[col][row]+=
            cd->e()*cce->fraction()*cd->e()*cce->fraction()*m_b2;
        }  
        //  gain fluctuation
        if( 0 != m_s2gain) {
          cov_ii[col][row]+=
            cd->e()*cce->fraction()*cd->e()*cce->fraction()*m_s2gain;
        }
        //  noise (both coherent and incoherent) 
        if( 0 != (m_s2coherent + m_s2incoherent) ) { 
          gain[col][row]   = det()->cellGain(cd->cellID()); 
          cov_ii[col][row] += (m_s2coherent + m_s2incoherent) 
            * gain[col][row] * gain[col][row] ; 
        }
      }
  }
  
  // non diagonal covariance allocation
  double cov_ij[3][3][3][3];
  for (cce=sac.begin();cce!=sac.end();++cce) {
    if (cce==0) {continue;}
    SmartRef<CaloDigit> cd=cce->digit();
    if (cd==0) {continue;}
    int row=cd->cellID().row()-rowseed+1;
    int col=cd->cellID().col()-colseed+1;
    if ((row>=0)&&(row<=2)&&(col>=0)&&(col<=2)) {
      E[col][row]=cd->e()*cce->fraction();
      if( 0 == m_s2coherent ) { continue ; } 
      CaloClusterEntry* cce2;
      for (cce2=sac.begin();cce2!=sac.end();++cce2) {
        if (cce2==0) {continue;}
        SmartRef<CaloDigit> cd2=cce2->digit();
        if (cd2==0) {continue;}
        int row_l=cd2->cellID().row()-rowseed+1;
        int col_l=cd2->cellID().col()-colseed+1;
        if ((row_l>=0)&&(row_l<=2)&&(col_l>=0)&&(col_l<=2)) {
          if ((row_l!=row)&&(col_l!=col)) {
            cov_ij[col][row][col_l][row_l] = m_s2coherent 
              * gain[col][row] * gain[col_l][row_l] ;
          }
        }
      }
    }
  }

  // print matrix
  for (i=0;i<3;i++) {
    logmsg << MSG::DEBUG << "|" << E[2][i]
           << "|" << E[1][i] << "|" << E[0][i] << endreq;
  }
  
  // matrix energy definition and allocation
  // attention: supress 2 cell on 9!!!
  double Eleft[3][3],Evert[3][3],Eright[3][3];
  double Ebottom[3][3],Ehori[3][3],Etop[3][3];
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      Eleft[i][j]=Evert[i][j]=Eright[i][j]=0.;
      Ebottom[i][j]=Ehori[i][j]=Etop[i][j]=0.;
    }
  }
  //Eleft
  if (std::min(E[0][0],std::min(E[0][1],E[0][1]))==E[0][0])
    {Eleft[0][1]=Eleft[0][2]=1.;}
  if (std::min(E[0][0],std::min(E[0][1],E[0][1]))==E[0][1])
    {Eleft[0][0]=Eleft[0][2]=1.;}
  if (std::min(E[0][0],std::min(E[0][1],E[0][1]))==E[0][2])
    {Eleft[0][0]=Eleft[0][1]=1.;}
  //Evert
  Evert[1][0]=Evert[1][1]=Evert[1][1]=1.;
  //Eright
  if (std::min(E[2][0],std::min(E[2][1],E[2][1]))==E[2][0])
    {Eright[2][1]=Eright[2][2]=1.;}
  if (std::min(E[2][0],std::min(E[2][1],E[2][1]))==E[2][1])
    {Eright[2][0]=Eright[2][2]=1.;}
  if (std::min(E[2][0],std::min(E[2][1],E[2][1]))==E[2][2])
    {Eright[2][0]=Eright[2][1]=1.;}
  //Ebottom
  if (std::max(E[0][0],std::max(E[1][0],E[2][0]))==E[0][0])
    {Ebottom[1][0]=Ebottom[2][0]=1.;}
  if (std::max(E[0][0],std::max(E[1][0],E[2][0]))==E[1][0])
    {Ebottom[0][0]=Ebottom[2][0]=1.;}
  if (std::max(E[0][0],std::max(E[1][0],E[2][0]))==E[2][0])
    {Ebottom[0][0]=Ebottom[1][0]=1.;}
  //Ehori
  Ehori[0][1]=Ehori[1][1]=Ehori[2][1]=1.;
  //Etop
  if (std::max(E[0][2],std::max(E[1][2],E[2][2]))==E[0][2])
    {Etop[1][2]=Etop[2][2]=1.;}
  if (std::max(E[0][2],std::max(E[1][2],E[2][2]))==E[1][2])
    {Etop[0][2]=Etop[2][2]=1.;}
  if (std::max(E[0][2],std::max(E[1][2],E[2][2]))==E[2][2])
    {Etop[0][2]=Etop[1][2]=1.;}

  // simple calcul (will disappear)
  double energy=0.;
  double eleft=0.,evert=0.,eright=0.;
  double ebottom=0.,ehori=0.,etop=0.;
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      energy +=           1.*E[i][j];
      eleft  +=  Eleft[i][j]*E[i][j];
      evert  +=  Evert[i][j]*E[i][j];
      eright += Eright[i][j]*E[i][j];
      ebottom+=Ebottom[i][j]*E[i][j];
      ehori  +=  Ehori[i][j]*E[i][j];
      etop   +=   Etop[i][j]*E[i][j];
    }
  }
  logmsg << MSG::VERBOSE << "energy: " << energy << endreq;

  // little mirroring of coeffs, to avoid plenty of times same code...
  double coeff0X,coeff1X,coeff2X;
  double coeff0Y,coeff1Y,coeff2Y;
  if (areaseed==0) {
    coeff0X=m_Coeff_area_0_X[0];
    coeff1X=m_Coeff_area_0_X[1];
    coeff2X=m_Coeff_area_0_X[2];
    coeff0Y=m_Coeff_area_0_Y[0];
    coeff1Y=m_Coeff_area_0_Y[1];
    coeff2Y=m_Coeff_area_0_Y[2];
  } else if (areaseed==1) {
    coeff0X=m_Coeff_area_1_X[0];
    coeff1X=m_Coeff_area_1_X[1];
    coeff2X=m_Coeff_area_1_X[2];
    coeff0Y=m_Coeff_area_1_Y[0];
    coeff1Y=m_Coeff_area_1_Y[1];
    coeff2Y=m_Coeff_area_1_Y[2];
  } else if (areaseed==2) {
    coeff0X=m_Coeff_area_2_X[0];
    coeff1X=m_Coeff_area_2_X[1];
    coeff2X=m_Coeff_area_2_X[2];
    coeff0Y=m_Coeff_area_2_Y[0];
    coeff1Y=m_Coeff_area_2_Y[1];
    coeff2Y=m_Coeff_area_2_Y[2];
  }
  
  // definition and allocation of 3x3 matrix for num and denom for x and y
  // !!!!
  // HERE is the S Correction formula
  // !!!!
  double matrix_numerator_X[3][3],matrix_denominator_X[3][3];
  double matrix_numerator_Y[3][3],matrix_denominator_Y[3][3];
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      matrix_numerator_X[i][j]=
        coeff0X*(Eright[i][j]-Eleft[i][j]);
      matrix_denominator_X[i][j]=
        Eright[i][j]+
        (coeff1X+coeff2X*log(energy/1000.))
        *Evert[i][j]
        +Eleft[i][j];
      matrix_numerator_Y[i][j]=
        coeff0Y*(Eright[i][j]-Eleft[i][j]);
      matrix_denominator_Y[i][j]=
        Eright[i][j]+
        (coeff1Y+coeff2Y*log(energy/1000.))
        *Evert[i][j]
        +Eleft[i][j];
    }
  }

  // calcul of num and denom for x and y
  double numerator_X,denominator_X;
  double numerator_Y,denominator_Y;
  numerator_X=0.;
  denominator_X=0.;
  numerator_Y=0.;
  denominator_Y=0.;
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      numerator_X+=matrix_numerator_X[i][j];
      denominator_X+=matrix_denominator_X[i][j];
      numerator_Y+=matrix_numerator_Y[i][j];
      denominator_Y+=matrix_denominator_Y[i][j];
    }
  }
  double x=numerator_X/denominator_X;
  double y=numerator_Y/denominator_Y;
  x*=sizeseed;
  x+=xseed;
  y*=sizeseed;
  y+=yseed;
  
  // update cluster parameters  
  { // V.B.
    if( 0 == hypo->position() ) 
      { hypo->setPosition( cluster->position().clone() ) ; }
  }
  CaloPosition::Parameters& parameters = hypo->position()->parameters();
  // keep old energy from ECorr
  parameters( CaloPosition::E ) = energy ;
  parameters( CaloPosition::X ) = x ;
  parameters( CaloPosition::Y ) = y ;
  logmsg << MSG::VERBOSE << "X/Y/E updated..." << endreq;

  double cov_numerator_X[3][3],cov_denominator_X[3][3];
  double cov_numerator_Y[3][3],cov_denominator_Y[3][3];
  for (i=0;i<3;i++) {
    for (j=0;j<3;j++) {
      cov_numerator_X[i][j]=0.;
      cov_denominator_X[i][j]=0.;
      cov_numerator_Y[i][j]=0.;
      cov_denominator_Y[i][j]=0.;
      for (k=0;i<3;i++) {
        for (l=0;j<3;j++) {
          cov_numerator_X[i][j]+=
            matrix_numerator_X[i][j]*cov_ij[i][j][k][l];
          cov_denominator_X[i][j]+=
            matrix_denominator_X[i][j]*cov_ij[i][j][k][l];
          cov_numerator_Y[i][j]+=
            matrix_numerator_Y[i][j]*cov_ij[i][j][k][l];
          cov_denominator_Y[i][j]+=
            matrix_denominator_Y[i][j]*cov_ij[i][j][k][l];
        }
      }
    }
  }
  // I need to find correct math formulas...
  double CovXX=0.;
  double CovXY=0.;
  double CovEX=0.;
  double CovYY=0.;
  double CovEY=0.;
  double CovEE=0.;
  

  // update cluster matrix   
  CaloPosition::Covariance& covariance = hypo->position()->covariance();
  covariance( CaloPosition::X , CaloPosition::X ) = CovXX ;
  covariance( CaloPosition::Y , CaloPosition::X ) = CovXY ;
  covariance( CaloPosition::E , CaloPosition::X ) = CovEX ;
  covariance( CaloPosition::Y , CaloPosition::Y ) = CovYY ;
  covariance( CaloPosition::E , CaloPosition::Y ) = CovEY ;
  covariance( CaloPosition::E , CaloPosition::E ) = CovEE ;



  logmsg << MSG::VERBOSE << "Covariance updated..." << endreq;
  logmsg << MSG::VERBOSE << "CaloHypo updated..." << endreq;
  return StatusCode::SUCCESS;
}
