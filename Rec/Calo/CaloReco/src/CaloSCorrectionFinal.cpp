// $Id: CaloSCorrectionFinal.cpp,v 1.12 2003-12-11 16:33:40 cattanem Exp $
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
inline double asinh ( double x )
 { return log( x + sqrt( x * x + 1. ) ) ; }
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
  , m_Coeff_border_area_0_X()
  , m_Coeff_border_area_0_Y()
  , m_Coeff_border_area_1_X()
  , m_Coeff_border_area_1_Y()
  , m_Coeff_border_area_2_X()
  , m_Coeff_border_area_2_Y()
  ///
  , m_a2GeV        ()
  , m_b2           ()
  , m_s2gain       ()
  , m_s2incoherent ()
  , m_s2coherent   ()
  ///
  , m_resA         ( 0.10 ) // calorimeter resolution (stochastic term)
  , m_resB         ( 0.01 ) // calorimeter resolution (constant term)
  , m_gainS        ( 0.01 ) // relative variantion of gain
//  , m_sigmaIn      ( 1.30 ) // sigma of incoherent noise (in channels)
  , m_sigmaIn      ( 0.00 ) // sigma of incoherent noise (in channels)
//  , m_sigmaCo      ( 0.30 ) // sigma of coherent noise
  , m_sigmaCo      ( 0.00 ) // sigma of coherent noise
{
  declareInterface<ICaloHypoTool>(this);
  declareProperty("Coeff0X",m_Coeff_area_0_X);
  declareProperty("Coeff0Y",m_Coeff_area_0_Y);
  declareProperty("Coeff1X",m_Coeff_area_1_X);
  declareProperty("Coeff1Y",m_Coeff_area_1_Y);
  declareProperty("Coeff2X",m_Coeff_area_2_X);
  declareProperty("Coeff2Y",m_Coeff_area_2_Y);
  declareProperty("Coeff0Xborder",m_Coeff_border_area_0_X);
  declareProperty("Coeff0Yborder",m_Coeff_border_area_0_Y);
  declareProperty("Coeff1Xborder",m_Coeff_border_area_1_X);
  declareProperty("Coeff1Yborder",m_Coeff_border_area_1_Y);
  declareProperty("Coeff2Xborder",m_Coeff_border_area_2_X);
  declareProperty("Coeff2Yborder",m_Coeff_border_area_2_Y);
  //
  declareProperty("ResolutionA"        , m_resA     ) ;
  declareProperty("ResolutionB"        , m_resB     ) ;
  declareProperty("RelativeGainError"  , m_gainS    ) ;
  declareProperty("NoiseIncoherent"    , m_sigmaIn  ) ;
  declareProperty("NoiseCoherent"      , m_sigmaCo  ) ;
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
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "intialize() has been called" << endreq;
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
  if (m_Coeff_area_0_X.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_0_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_0_Y.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_0_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_1_X.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_1_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_1_Y.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_1_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_2_X.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_2_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_area_2_Y.size()!=6) {
		msg << MSG::ERROR << "m_Coeff_area_2_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
	
	m_Coeff_X[0] = m_Coeff_area_0_X;
	m_Coeff_Y[0] = m_Coeff_area_0_Y;
	m_Coeff_X[1] = m_Coeff_area_1_X;
	m_Coeff_Y[1] = m_Coeff_area_1_Y;
	m_Coeff_X[2] = m_Coeff_area_2_X;
	m_Coeff_Y[2] = m_Coeff_area_2_Y;

  if (m_Coeff_border_area_0_X.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_0_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_0_Y.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_0_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_1_X.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_1_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_1_Y.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_1_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_2_X.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_2_X size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
  if (m_Coeff_border_area_2_Y.size()!=1) {
		msg << MSG::ERROR << "m_Coeff_border_area_2_Y size pb!!!" << endreq;
    return StatusCode::FAILURE;
  }
	
	m_Coeff_border_X[0] = m_Coeff_border_area_0_X;
	m_Coeff_border_Y[0] = m_Coeff_border_area_0_Y;
	m_Coeff_border_X[1] = m_Coeff_border_area_1_X;
	m_Coeff_border_Y[1] = m_Coeff_border_area_1_Y;
	m_Coeff_border_X[2] = m_Coeff_border_area_2_X;
	m_Coeff_border_Y[2] = m_Coeff_border_area_2_Y;
	

  m_a2GeV        = m_resA    * m_resA    * GeV  ;
  m_b2           = m_resB    * m_resB           ;
  m_s2gain       = m_gainS   * m_gainS          ;
  m_s2incoherent = m_sigmaIn * m_sigmaIn        ;
  m_s2coherent   = m_sigmaCo * m_sigmaCo        ;

	msg << MSG::INFO << "m_a2GeV:" << m_a2GeV << endreq;
	msg << MSG::INFO << "m_b2:" << m_b2 << endreq;

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
  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "finalize() has been called" << endreq;
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

  MsgStream msg(msgSvc(), name());
  msg << MSG::VERBOSE << "process() has been called" << endreq;
	int i,j,k,l;

	StatusCode update_data = StatusCode::SUCCESS;

  if (hypo==0) {
		msg << MSG::ERROR << "hypo==0" << endreq;
		return StatusCode::FAILURE;
	}
  if (hypo->hypothesis()!=CaloHypotheses::Photon) {
		msg << MSG::ERROR << "hypo->hypothesis()!=CaloHypotheses::Photon" << endreq;
		//return StatusCode::FAILURE;
	}
  SmartRefVector<CaloCluster> clusters = hypo->clusters();
  if (clusters.size()!=1) {
		msg << MSG::ERROR << "clusters.size()!=1" << endreq;
		return StatusCode::FAILURE;
	}
//   CaloCluster *cluster;
//   for (SmartRef<CaloCluster> *clusterloop = clusters.begin();
//        clusterloop!=clusters.end();
//        ++clusterloop) {
//     if (clusterloop!=0) {cluster = *(clusterloop);}
//   }
  typedef SmartRefVector<CaloCluster> CCs;
  CaloCluster* cluster = 0 ;
  for ( CCs::iterator clusterloop = clusters.begin();
       clusterloop!=clusters.end();
       ++clusterloop) {
    if ( 0 != *clusterloop ) {cluster = *(clusterloop);}
  }

  std::vector<CaloClusterEntry> sac = cluster->entries();
  double maxenergy=0.;
  SmartRef<CaloDigit> seed;
  msg << MSG::VERBOSE << "looping on digits:" << endreq;
  //	CaloClusterEntry* cce;
  typedef std::vector<CaloClusterEntry> CCEs ;
  CCEs::iterator cce;
  for (cce=sac.begin();cce!=sac.end();++cce) {
    SmartRef<CaloDigit> srcd=cce->digit();
    if (srcd==0) {continue;}
    if (srcd->e()*cce->fraction()>maxenergy) {
      maxenergy=srcd->e()*cce->fraction();
      seed=srcd;
    }
    msg << MSG::VERBOSE << "e()= " << srcd->e()*cce->fraction() << endreq;
  }
  if (seed==0) {
		msg << MSG::ERROR << "seed==0" << endreq;
		return StatusCode::FAILURE;
	}
  if (maxenergy==0) {
		msg << MSG::ERROR << "maxenergy==0" << endreq;
		return StatusCode::FAILURE;
	}
  msg << MSG::VERBOSE << "seed energy: " << seed->e() << endreq;
  msg << MSG::VERBOSE << "seed cell: " << seed->cellID() << endreq;
  double xseed=det()->cellX(seed->cellID())*mm;
  msg << MSG::VERBOSE << "seed cell x: " << xseed << endreq;
  double yseed=det()->cellY(seed->cellID())*mm;
  msg << MSG::VERBOSE << "seed cell y: " << yseed << endreq;
  int rowseed=seed->cellID().row();
  msg << MSG::VERBOSE << "seed cell raw: " << rowseed << endreq;
  int colseed=seed->cellID().col();
  msg << MSG::VERBOSE << "seed cell col: " << colseed << endreq;
  unsigned int areaseed=seed->cellID().area();
  msg << MSG::VERBOSE << "seed cell area: " << areaseed << endreq;
  if (areaseed>2) {
		msg << MSG::ERROR << "areaseed>2" << endreq;
		update_data = StatusCode::FAILURE;
		//return StatusCode::FAILURE;
  }
  double sizeseed=det()->cellSize(seed->cellID())*mm;
  msg << MSG::VERBOSE << "seed cell size: " << sizeseed << endreq;
  if (sizeseed<0.1) {
		msg << MSG::ERROR << "sizeseed<0.1" << endreq;
		update_data = StatusCode::FAILURE;
		//return StatusCode::FAILURE;
  }

	// update cluster parameters
	{ // V.B.
	  if( 0 == hypo->position() )
	    { hypo->setPosition( cluster->position().clone() ) ; }
	}

	CaloPosition::Parameters& oldparameters = hypo->position()->parameters();
	const double baryx = oldparameters( CaloPosition::X );
	const double baryy = oldparameters( CaloPosition::Y );
  msg << MSG::VERBOSE << "cluster barycenter position: " << baryx << "," << baryy << endreq;

	double E[3][3];
	double gain[3][3];
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {
			E[i][j]=0.0;
			gain[i][j]=0.0;
		}
	}
	for (cce=sac.begin();cce!=sac.end();++cce) {
		// if (cce==0) {continue;}
		SmartRef<CaloDigit> srcd=cce->digit();
		if (srcd==0) {continue;}
		if (srcd->cellID().area()!=areaseed) {continue;}
		int row=srcd->cellID().row()-rowseed+1;
		int col=srcd->cellID().col()-colseed+1;
		if ((row>=0)&&(row<=2)&&(col>=0)&&(col<=2)) {
			E[col][row]=srcd->e()*cce->fraction();
			//  noise (both coherent and incoherent)
			if( 0 != (m_s2coherent + m_s2incoherent) ) {
				gain[col][row]   = det()->cellGain(srcd->cellID());
			}
		}
	}

	// non diagonal covariance allocation (must be symetric)
	double cov_ij[3][3][3][3];
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {
			for (k=0;k<3;k++) {
				for (l=0;l<3;l++) {
					cov_ij[i][j][k][l]=0.;
				}
			}
		}
	}
	for (cce=sac.begin();cce!=sac.end();++cce) {
		// if (cce==0) {continue;}
		SmartRef<CaloDigit> srcd=cce->digit();
		if (srcd==0) {continue;}
		int row=srcd->cellID().row()-rowseed+1;
		int col=srcd->cellID().col()-colseed+1;
		if ((row>=0)&&(row<=2)&&(col>=0)&&(col<=2)) {
      //			CaloClusterEntry* cce2;
			CCEs::iterator cce2;
			for (cce2=sac.begin();cce2!=sac.end();++cce2) {
				// if (cce2==0) {continue;}
				SmartRef<CaloDigit> srcd2=cce2->digit();
				if (srcd2==0) {continue;}
				int row_l=srcd2->cellID().row()-rowseed+1;
				int col_l=srcd2->cellID().col()-colseed+1;
				if ((row_l>=0)&&(row_l<=2)&&(col_l>=0)&&(col_l<=2)) {
					if ((row_l!=row)&&(col_l!=col)) {
						cov_ij[col][row][col_l][row_l] = m_s2coherent*gain[col][row]*gain[col_l][row_l];
					} else {
						cov_ij[col][row][col_l][row_l] = 
							fabs(E[col][row])*m_a2GeV
							+E[col][row]*E[col][row]*(m_b2+m_s2gain)
							+gain[col][row]*gain[col][row]*m_s2coherent;
					}
				}
			}
		}
	}

	msg << MSG::VERBOSE << "energy matrix:" << endreq;
	for (i=0;i<3;i++) {
		msg << MSG::VERBOSE << "|" << E[2][i] << "|" << E[1][i] << "|" << E[0][i] << endreq;
	}

	msg << MSG::VERBOSE << "cov_ii matrix:" << endreq;
	for (i=0;i<3;i++) {
		msg << MSG::VERBOSE << "|" << cov_ij[2][i][2][i] << "|" 
			<< cov_ij[1][i][1][i] << "|" 
			<< cov_ij[0][i][0][i] << endreq;
	}

	bool border=false;
	int numberofneighbor = 0;
	// const CaloCellID* cellloop;
  CaloNeighbors::const_iterator cellloop ;
	for (cellloop = det()->neighborCells(seed->cellID()).begin();
		cellloop!=det()->neighborCells(seed->cellID()).end();
		++cellloop) {
		numberofneighbor++;
		if (areaseed!=cellloop->area()) {border=true;}
	}
	if (numberofneighbor!=8) {border=true;}

	double energy=0.;
	double eleft=0.,evert=0.,eright=0.;
	double ebottom=0.,ehori=0.,etop=0.;
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {energy+=E[i][j];}
	}
	msg << MSG::VERBOSE << "energy: " << energy << endreq;

	eleft  =E[0][0]+E[0][1]+E[0][1];
	evert  =E[1][0]+E[1][1]+E[1][1];
	eright =E[2][0]+E[2][1]+E[2][1];
	ebottom=E[0][0]+E[1][0]+E[2][0];
	ehori  =E[0][1]+E[1][1]+E[2][1];
	etop   =E[0][2]+E[1][2]+E[2][2];

	bool borderx=false;
	bool bordery=false;
	if (border) {
		if ((eleft==0.)||(eright==0.)) {borderx=true;}
		if ((ebottom==0.)||(etop==0.)) {bordery=true;}
		if ((borderx==false)&&(bordery==false)) {
			msg << MSG::ERROR << "(border)&&(borderx==false)&&(bordery==false)" << endreq;
			//update_data = StatusCode::FAILURE;
		}
	}
	msg << MSG::VERBOSE << "borderx: " << borderx << endreq;
	msg << MSG::VERBOSE << "bordery: " << bordery << endreq;

	double EDiffX[3][3],EDiffY[3][3];
	for (i=0;i<3;i++) {
		for (j=0;j<3;j++) {EDiffX[i][j]=0.;EDiffY[i][j]=0.;}
	}
	double BorderDiffX = 0 ;
  double BorderDiffY = 0 ;

	double x=0.,temp_x=0.,param_x=0.;
	if (!borderx) {
		param_x=m_Coeff_X[areaseed][2]
			+m_Coeff_X[areaseed][3]*fabs(xseed)
			+m_Coeff_X[areaseed][4]*log(energy)
			+m_Coeff_X[areaseed][5]*log(energy)*log(energy);
		const double Enum = (eright-eleft);
		const double Edenom = (eright+m_Coeff_X[areaseed][1]*evert+eleft);
		temp_x=2*m_Coeff_X[areaseed][0]*Enum/Edenom*sinh(1./param_x);
		x=sizeseed*param_x*asinh(temp_x)/2.;
		const double Difffactor = 
      sizeseed*param_x*m_Coeff_X[areaseed][0]*
      sinh(1./param_x)/sqrt(1.+temp_x*temp_x);
		// in eright
		EDiffX[2][0]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffX[2][1]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffX[2][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		// in evert
		EDiffX[1][0]=Difffactor*-m_Coeff_X[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffX[1][1]=Difffactor*-m_Coeff_X[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffX[1][2]=Difffactor*-m_Coeff_X[areaseed][1]*Enum/(Edenom*Edenom);
		// in eleft
		EDiffX[0][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffX[0][1]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffX[0][2]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		msg << MSG::VERBOSE << "X standart correction..." << endreq;
	} else {
		param_x=m_Coeff_border_X[areaseed][0];
		temp_x=(baryx-xseed)/sizeseed;
		x=sizeseed*param_x*asinh(temp_x)/2.;
		BorderDiffX=
      param_x*sinh(1./param_x)/
      sqrt(1.+ temp_x*temp_x*4.*sinh(1./param_x)*sinh(1./param_x));
		msg << MSG::VERBOSE << "X border correction..." << endreq;
	}
	x+=xseed;

	double y=0.,temp_y=0.,param_y=0.;
	if (!bordery) {
		param_y=m_Coeff_Y[areaseed][2]
			+m_Coeff_Y[areaseed][3]*fabs(yseed)
			+m_Coeff_Y[areaseed][4]*log(energy)
			+m_Coeff_Y[areaseed][5]*log(energy)*log(energy);
		const double Enum = (etop-ebottom);
		const double Edenom = (etop+m_Coeff_Y[areaseed][1]*ehori+ebottom);
		temp_y=2*m_Coeff_Y[areaseed][0]*Enum/Edenom*sinh(1./param_y);
		y=sizeseed*param_y*asinh(temp_y)/2.;
		const double Difffactor = 
      sizeseed*param_y*m_Coeff_Y[areaseed][0]*
      sinh(1./param_y)/sqrt(1.+temp_y*temp_y);
		// in etop
		EDiffY[0][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[1][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][2]=Difffactor*(Edenom-Enum)/(Edenom*Edenom);
		// in ehori
		EDiffY[0][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffY[1][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		EDiffY[2][1]=Difffactor*-m_Coeff_Y[areaseed][1]*Enum/(Edenom*Edenom);
		// in ebottom
		EDiffY[0][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[1][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		EDiffY[2][0]=Difffactor*(-Edenom-Enum)/(Edenom*Edenom);
		msg << MSG::VERBOSE << "Y standart correction..." << endreq;
	} else {
		param_y=m_Coeff_border_Y[areaseed][0];
		temp_y=(baryy-yseed)/sizeseed;
		y=sizeseed*param_y*asinh(temp_y)/2.;
		BorderDiffY=
      param_y*sinh(1./param_y)/
      sqrt(1.+temp_y*temp_y*4.*sinh(1./param_y)*sinh(1./param_y));
		msg << MSG::VERBOSE << "Y border correction..." << endreq;
	}
	y+=yseed;

	if (update_data == StatusCode::SUCCESS ) {
		CaloPosition::Parameters& parameters = hypo->position()->parameters();
		// keep old energy from ECorr
		//parameters( CaloPosition::E ) = energy ;
		parameters( CaloPosition::X ) = x ;
		parameters( CaloPosition::Y ) = y ;
		msg << MSG::VERBOSE << "X/Y/E updated..." << endreq;

		// covariance initialisation
		double CovEE = 0.;
		double CovEX = 0.;
		double CovEY = 0.;
		double CovXX = 0.;
		double CovXY = 0.;
		double CovYY = 0.;
		// I USE THE COVARIANCE DEFINITION
		for (i=0;i<3;i++) {
			for (j=0;j<3;j++) {
				for (k=0;k<3;k++) {
					for (l=0;l<3;l++) {
						CovEE+=cov_ij[i][j][k][l];
						CovEX+=cov_ij[i][j][k][l]*EDiffX[i][j];
						CovEY+=cov_ij[i][j][k][l]*EDiffY[i][j];
						CovXX+=cov_ij[i][j][k][l]*EDiffX[i][j]*EDiffX[k][l];
						CovXY+=cov_ij[i][j][k][l]*EDiffX[i][j]*EDiffY[k][l];
						CovYY+=cov_ij[i][j][k][l]*EDiffY[i][j]*EDiffY[k][l];
					}
				}
			}
		}
		CaloPosition::Covariance& covariance = hypo->position()->covariance();
		if (borderx) {
			CovEX=BorderDiffX*covariance( CaloPosition::E , CaloPosition::X );
			CovXX=BorderDiffX*BorderDiffX*covariance( CaloPosition::X , CaloPosition::X );
			CovXY=BorderDiffX*covariance( CaloPosition::X , CaloPosition::Y );
		}
		if (bordery) {
			CovEY=BorderDiffY*covariance( CaloPosition::E , CaloPosition::Y );
			CovYY=BorderDiffY*BorderDiffY*covariance( CaloPosition::Y , CaloPosition::Y );
			CovXY=BorderDiffY*covariance( CaloPosition::X , CaloPosition::Y );
		}
		if ((borderx)&&(bordery)) {
			CovXY=BorderDiffX*BorderDiffY*covariance( CaloPosition::X , CaloPosition::Y );
		}
		msg << MSG::VERBOSE << "covariance EE:" << CovEE
			<< " was:" << covariance( CaloPosition::E , CaloPosition::E ) << endreq;
		covariance( CaloPosition::E , CaloPosition::E ) = CovEE;
		msg << MSG::VERBOSE << "covariance EX:" << CovEX
			<< " was:" << covariance( CaloPosition::E , CaloPosition::X ) << endreq;
		covariance( CaloPosition::E , CaloPosition::X ) = CovEX;
		msg << MSG::VERBOSE << "covariance EY:" << CovEY
			<< " was:" << covariance( CaloPosition::E , CaloPosition::Y ) << endreq;
		covariance( CaloPosition::E , CaloPosition::Y ) = CovEY;
		msg << MSG::VERBOSE << "covariance XX:" << CovXX
			<< " was:" << covariance( CaloPosition::X , CaloPosition::X ) << endreq;
		covariance( CaloPosition::X , CaloPosition::X ) = CovXX;
		msg << MSG::VERBOSE << "covariance XY:" << CovXY
			<< " was:" << covariance( CaloPosition::X , CaloPosition::Y ) << endreq;
		covariance( CaloPosition::X , CaloPosition::Y ) = CovXY;
		msg << MSG::VERBOSE << "covariance YY:" << CovYY
			<< " was:" << covariance( CaloPosition::Y , CaloPosition::Y ) << endreq;
		covariance( CaloPosition::Y , CaloPosition::Y ) = CovYY;

		msg << MSG::VERBOSE << "covariance updated..." << endreq;
	} else {
	  msg << MSG::INFO << "something wrong detected, hypo position NOT updated..." << endreq;
	}

	msg << MSG::VERBOSE << "CaloHypo ok..." << endreq;
	return StatusCode::SUCCESS;
}
