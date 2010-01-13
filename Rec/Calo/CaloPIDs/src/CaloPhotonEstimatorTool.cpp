// Include files
// GaudiKernel
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IAxis.h"
#include "AIDA/IHistogram1D.h"
// DetDesc
#include "DetDesc/IGeometryInfo.h"
/// CaloDet
#include "CaloDet/DeCalorimeter.h"
// Event
#include "Event/CaloDigit.h"
#include "Event/CaloHypo.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"
/// local
#include "CaloPhotonEstimatorTool.h"


// ============================================================================
/** @file
 *
 *  Implementation file for class CaloPhotonEstimatorTool
 *  Photon Selection Tool
 *  (LHCb 2004-03)
 *
 *  @author Frédéric Machefert frederic.machefert@in2p3.fr
 *  @date   2004-15-04
 */

// ============================================================================

// Instantiation of a static factory class used by clients to create
// instances of this tool

DECLARE_TOOL_FACTORY( CaloPhotonEstimatorTool );

//
// Standard Constructor
CaloPhotonEstimatorTool::CaloPhotonEstimatorTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent)
  : GaudiTool( type, name, parent )
  , m_nameOfECAL (DeCalorimeterLocation::Ecal)
  , m_nameOfPRS  (DeCalorimeterLocation::Prs)
  , m_nameOfSPD  (DeCalorimeterLocation::Spd)
  
  , m_planePrs      ( )
  , m_planeSpd      ( )
  , m_tableLocation ( LHCb::CaloIdLocation::ClusterMatch )
  , m_table()
  , m_vertex      (   )
  
  , m_eBin               (   )
  , m_binningEPrs        (   )
  , m_binningChi2        (   )
  , m_binningShap        (   )

  , m_signalEPrsData     (   )
  , m_signalEPrsSpdData  (   )
  , m_backgrEPrsData     (   )
  , m_backgrEPrsSpdData  (   )

  , m_signalChi2Data     (   )
  , m_signalChi2SpdData  (   )
  , m_backgrChi2Data     (   )
  , m_backgrChi2SpdData  (   )
  , m_signalShapeData    (   )
  , m_signalShapeSpdData (   )
  , m_backgrShapeData    (   )
  , m_backgrShapeSpdData (   )

//  , m_monitoring         (true)
  , m_extrapol           (true)
{
  declareInterface<ICaloHypoLikelihood>( this );

  declareProperty("DetEcal",m_nameOfECAL);
  declareProperty("DetPrs",m_nameOfPRS);
  declareProperty("DetSpd",m_nameOfSPD);


  declareProperty( "TableLocation" , m_tableLocation ) ;

  declareProperty("EBin"             ,m_eBin);
  declareProperty("EPrsbin"          ,m_binningEPrs);
  declareProperty("Chi2Bin"          ,m_binningChi2);
  declareProperty("ShapBin"          ,m_binningShap);
  declareProperty("EPrsSignal"       ,m_signalEPrsData);
  declareProperty("EPrsSpdSignal"    ,m_signalEPrsSpdData);
  declareProperty("EPrsBackground"   ,m_backgrEPrsData);
  declareProperty("EPrsSpdBackground",m_backgrEPrsSpdData);
  declareProperty("Chi2Signal"       ,m_signalChi2Data);
  declareProperty("Chi2SpdSignal"    ,m_signalChi2SpdData);
  declareProperty("Chi2Background"   ,m_backgrChi2Data);
  declareProperty("Chi2SpdBackground",m_backgrChi2SpdData);
  declareProperty("ShapSignal"       ,m_signalShapeData);
  declareProperty("ShapSpdSignal"    ,m_signalShapeSpdData);
  declareProperty("ShapBackground"   ,m_backgrShapeData);
  declareProperty("ShapSpdBackground",m_backgrShapeSpdData);
  declareProperty("Extrapolation"    ,m_extrapol);

  using namespace LHCb::CaloAlgUtils;
  m_tableLocation = CaloIdLocation( "ClusterMatch" , context() );

}
// ============================================================================
/** destructor, virtual and protected
 */
// ============================================================================
CaloPhotonEstimatorTool::~CaloPhotonEstimatorTool()
{
};

// ============================================================================
/** standard initialization method
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonEstimatorTool::initialize ()
{
  debug() << "==> Initialize CaloPhotonEstimatorTool" << endmsg;

  /// initialize the base class
  StatusCode sc = GaudiTool::initialize ();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class!") ; }

  // ECAL
  SmartDataPtr<DeCalorimeter> detecal (detSvc(),m_nameOfECAL);
  if(!detecal)
    error() <<"Unable to retrieve ECAL detector "
       <<m_nameOfECAL<<endmsg;
  if (!detecal){ return StatusCode::FAILURE ;}

  // SPD
  SmartDataPtr<DeCalorimeter> detspd (detSvc(),m_nameOfSPD);
  if(!detspd)
    error() <<"Unable to retrieve SPD detector "
       <<m_nameOfSPD<<endmsg;
  if (!detspd){    return StatusCode::FAILURE ; }

  // PRS
  SmartDataPtr<DeCalorimeter> detprs (detSvc(),m_nameOfPRS);
  if(!detprs)
    error()<<"Unable to retrieve PRS detector "
       <<m_nameOfPRS<<endmsg;
  if (!detprs){ return StatusCode::FAILURE ; }

  // Convert Detectors to DeCalorimeter
  m_detEcal = (DeCalorimeter*) detecal;
  m_detSpd = (DeCalorimeter*) detspd;
  m_detPrs = (DeCalorimeter*) detprs;
  // Locate plane
  m_planeSpd = m_detSpd->plane(CaloPlane::Middle);
  m_planePrs = m_detPrs->plane(CaloPlane::Middle);


  // IP
  m_vertex=Gaudi::XYZPoint(0.,0.,0.);

  // Check data consistency
  if (m_binningEPrs.size()!=3)
  { return Error("Wrong Binning Parameter (EPrs)"); }
  if (m_binningChi2.size()!=3)
  { return Error("Wrong Binning Parameter (Chi2Tk)"); }
  if (m_binningShap.size()!=3)
  { return Error("Wrong Binning Parameter (ShShape)"); }


// Build useful vector
  int index;
  double width=
    (m_binningEPrs[ 2 ]-m_binningEPrs[ 1 ])/double(m_binningEPrs[ 0 ]);
  for ( index=0; index<=int (m_binningEPrs[ 0 ]); ++index ) {
    m_ePrsBin.push_back(double(index)*width+m_binningEPrs[ 1 ]);
	}

  width=(m_binningChi2[ 2 ]-m_binningChi2[ 1 ])/double(m_binningChi2[ 0 ]);
  for ( index=0; index<=int (m_binningChi2[ 0 ]); ++index ) {
    m_chi2Bin.push_back(double(index)*width+m_binningChi2[ 1 ]);
  }

  width=(m_binningShap[ 2 ]-m_binningShap[ 1 ])/double(m_binningShap[ 0 ]);
  for ( index=0; index<=int (m_binningShap[ 0 ]); ++index ) {
    m_shapBin.push_back(double(index)*width+m_binningShap[ 1 ]);
  }


/*
// Pdf Histograms
  if (m_monitoring){
    unsigned int nEbin=m_eBin.size()+1;
    m_histoSvc = svc<IHistogramSvc> ( "HistogramDataSvc", true );

    debug()G<<"HistogramDataSvc is loaded properly!!"<<endmsg;

    m_signalEPrs=makeHisto
      (int(m_binningEPrs[ 0 ]),m_binningEPrs[ 1 ],m_binningEPrs[ 2 ],
       100, std::string("Signal Prs noSpdHit"),
       std::string("/stat/PhotonPdf"),
       m_signalEPrsData,nEbin);
    m_signalChi2=makeHisto
      (int(m_binningChi2[ 0 ]),m_binningChi2[ 1 ],m_binningChi2[ 2 ],
       200, std::string("Signal Chi2Tk noSpdHit"),
       std::string("/stat/PhotonPdf"),
       m_signalChi2Data,nEbin);
    m_signalShape=makeHisto
      (int(m_binningShap[ 0 ]),m_binningShap[ 1 ],m_binningShap[ 2 ],
       300, std::string("Signal Shower Shape noSpdHit"),
       std::string("/stat/PhotonPdf"),
       m_signalShapeData,nEbin);

    m_signalEPrsSpd=makeHisto
      (int(m_binningEPrs[ 0 ]),m_binningEPrs[ 1 ],m_binningEPrs[ 2 ],
       110, std::string("Signal Prs SpdHit"),
       std::string("/stat/PhotonPdf"),
       m_signalEPrsData,nEbin);
    m_signalChi2Spd=makeHisto
      (int(m_binningChi2[ 0 ]),m_binningChi2[ 1 ],m_binningChi2[ 2 ],
       210, std::string("Signal Chi2Tk SpdHit"),
       std::string("/stat/PhotonPdf"),
       m_signalChi2Data,nEbin);
    m_signalShapeSpd=makeHisto
      (int(m_binningShap[ 0 ]),m_binningShap[ 1 ],m_binningShap[ 2 ],
       310, std::string("Signal Shower Shape SpdHit"),
       std::string("/stat/PhotonPdf"),
       m_signalShapeData,nEbin);

    m_backgrEPrs=makeHisto
      (int(m_binningEPrs[ 0 ]),m_binningEPrs[ 1 ],m_binningEPrs[ 2 ],
       1100, std::string("Background Prs noSpdHit"),
       std::string("/stat/PhotonPdf"),
       m_backgrEPrsData,nEbin);
    m_backgrChi2=makeHisto
      (int(m_binningChi2[ 0 ]),m_binningChi2[ 1 ],m_binningChi2[ 2 ],
       1200, std::string("Background Chi2Tk noSpdHit"),
       std::string("/stat/PhotonPdf"),
       m_backgrChi2Data,nEbin);
    m_backgrShape=makeHisto
      (int(m_binningShap[ 0 ]),m_binningShap[ 1 ],m_binningShap[ 2 ],
       1300, std::string("Background Shower Shape noSpdHit"),
       std::string("/stat/PhotonPdf"),
       m_backgrShapeData,nEbin);

    m_backgrEPrsSpd=makeHisto
      (int(m_binningEPrs[ 0 ]),m_binningEPrs[ 1 ],m_binningEPrs[ 2 ],
       1110, std::string("Background Prs SpdHit"),
       std::string("/stat/PhotonPdf"),
       m_backgrEPrsData,nEbin);
    m_backgrChi2Spd=makeHisto
      (int(m_binningChi2[ 0 ]),m_binningChi2[ 1 ],m_binningChi2[ 2 ],
       1210, std::string("Background Chi2Tk SpdHit"),
       std::string("/stat/PhotonPdf"),
       m_backgrChi2Data,nEbin);
    m_backgrShapeSpd=makeHisto
      (int(m_binningShap[ 0 ]),m_binningShap[ 1 ],m_binningShap[ 2 ],
       1310, std::string("Background Shower Shape SpdHit"),
       std::string("/stat/PhotonPdf"),
       m_backgrShapeData,nEbin);

    m_likelihood =
      m_histoSvc->book(std::string("/stat/PhotonPdf")         ,
                       10                                         ,
                       "CaloHypo Photon Likelihood Distribution"  ,
                       100,0.,1.                                   ) ;
  }
*/

  // General WARNING
  warning() << "Vertex location HARD-CODED : IP(0.,0.,0.)"<<endmsg;

  return StatusCode::SUCCESS ;
};

// ============================================================================
/** standard finalization method
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonEstimatorTool::finalize   ()
{
  return GaudiTool::finalize ();
};


// ============================================================================
double CaloPhotonEstimatorTool::likelihood(const LHCb::CaloHypo* hypo )  const
{

  // Get the relevant information - basic checks
  
  if( 0 == hypo ) Exception( " *CaloHypo* points to NULL " );


  if( 1 != hypo->clusters().size() ){
    debug() <<"Number of clusters != 1"<<endmsg;
    return -1.;
  }


  const SmartRef<LHCb::CaloCluster> cluster=hypo->clusters().front();
  if ( 0 == cluster ) Exception( " *CaloCluster* points to NULL " );
  
  if( cluster->entries().empty() ) Exception( " *CaloCluster* empty " );

  LHCb::CaloCluster::Entries::const_iterator iseed =
    LHCb::ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                        cluster->entries().end  () ,
                                        LHCb::CaloDigitStatus::SeedCell  ) ;


  if( iseed == cluster->entries().end() ) Exception( " *SeedCell* not found ");

  const LHCb::CaloDigit* seed = iseed->digit();
  if( 0 == seed ) Exception( " SeedCell *Digit* points to NULL! ");

  //***
  // Evaluate Estimator Parameters : Energy, EPrs, Chi2 and Shower Shape ...

  const LHCb::CaloPosition *pos = hypo->position() ;
  const Gaudi::XYZPoint position (pos->x(),pos->y(),pos->z());

  // Energy
  double energy  = pos->e();
  Gaudi::XYZVector hypoVect = position-m_vertex;
  double et=energy*hypoVect.Rho()/hypoVect.R();
  

  /// Chi2
  double chi2;
  // get the input data 
  if( !exist<LHCb::Calo2Track::IClusTrTable>(m_tableLocation))
    return Warning(" Input missing '" + m_tableLocation + "'",StatusCode::SUCCESS);
  m_table = get<LHCb::Calo2Track::IClusTrTable>( m_tableLocation ) ; 
  const LHCb::Calo2Track::IClusTrTable::Range range = m_table -> relations ( cluster ) ;
  if( range.empty() )  { chi2=1.e+6; }            // bad match -> large value !
  else {
    chi2 = range.front().weight();
  }

  // Shower Shape
  double shape=pos->spread()(0,0)+pos->spread()(1,1);
  //Spd hit and Prs deposit
  double eSpd=0.;
  double ePrs=0.;

  // point in the detector
  Gaudi::XYZPoint spdPoint,prsPoint;
 
  if (!m_extrapol){
    spdPoint = m_planeSpd.ProjectOntoPlane( position );
    prsPoint = m_planePrs.ProjectOntoPlane( position );
  }
  else {
    const Gaudi::XYZVector direction = position - m_vertex;
    typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>  LineType ;
    const LineType line =  LineType(m_vertex , direction );
    double mu=0;

    if( !Gaudi::Math::intersection<LineType,Gaudi::Plane3D
        ,Gaudi::XYZPoint>( line , m_planeSpd , spdPoint , mu) )
        warning() << " CAN NOT EXTRAPOLATE TO THE SPD PLANE " << endmsg;

    if( !Gaudi::Math::intersection<LineType,Gaudi::Plane3D
        ,Gaudi::XYZPoint>( line , m_planePrs , prsPoint , mu) )
        warning() << " CAN NOT EXTRAPOLATE TO THE Prs PLANE " << endmsg;
  }

  const LHCb::CaloCellID cellSpd = m_detSpd->Cell( spdPoint );
  const LHCb::CaloCellID cellPrs = m_detPrs->Cell( prsPoint );

	// Get CaloCell Deposits in the SPD and PRS
  if( !(LHCb::CaloCellID() == cellSpd) ) { // valid cell!
	  for (SmartRefVector<LHCb::CaloDigit>::const_iterator
           digit=hypo->digits().begin() ;
         digit != hypo->digits().end() ; digit++ ) {
	    if ( (*digit)->cellID() == cellSpd ) {
				eSpd=(*digit)->e();
	    }
	  }
	}

	if( !(LHCb::CaloCellID() == cellPrs)  ) { // valid cell!
	  for (SmartRefVector<LHCb::CaloDigit>::const_iterator
           digit=hypo->digits().begin() ;
         digit != hypo->digits().end() ; digit++ ) {
	    if ( (*digit)->cellID() == cellPrs ) {
		 		ePrs=(*digit)->e();
      }
	  }
  }

  // ***
  // Likelihood correspond to that point

  // Corresponding Energy bin
	unsigned int ebin = bin(energy,m_eBin);
	unsigned int eprsbin, chi2bin,shapbin;
  unsigned int ibin;
  // Initialization
	double estimator;
  double signal, backgr;
	signal=1.; 	backgr=1.;

  // SPD Hit
  if (eSpd>1) {
    eprsbin = bin(ePrs,m_ePrsBin);
    ibin    = ( ebin*(m_ePrsBin.size()+1))+ eprsbin;
    signal *= m_signalEPrsSpdData[ ibin ];
    backgr *= m_backgrEPrsSpdData[ ibin ];
    chi2bin = bin(chi2,m_chi2Bin);
    ibin    = ( ebin*(m_chi2Bin.size()+1))+ chi2bin;
    signal *= m_signalChi2SpdData[ ibin ];
    backgr *= m_backgrChi2SpdData[ ibin ];
    shapbin = bin(shape,m_shapBin);
    ibin    = ( ebin*(m_shapBin.size()+1))+ shapbin;
    signal *= m_signalShapeSpdData[ ibin ];
    backgr *= m_backgrShapeSpdData[ ibin ];
  }

  // No SPD Hit
  else {
    eprsbin = bin(ePrs,m_ePrsBin);
    ibin    = ( ebin*(m_ePrsBin.size()+1))+ eprsbin;
    signal *= signal * m_signalEPrsData[ ibin ];
    backgr *= backgr * m_backgrEPrsData[ ibin ];
    chi2bin = bin(chi2,m_chi2Bin);
    ibin    = ( ebin*(m_chi2Bin.size()+1))+ chi2bin;
    signal *= m_signalChi2Data[ ibin ];
    backgr *= m_backgrChi2Data[ ibin ];
    shapbin = bin(shape,m_shapBin);
    ibin    = ( ebin*(m_shapBin.size()+1))+ shapbin;
    signal *= m_signalShapeData[ ibin ];
    backgr *= m_backgrShapeData[ ibin ];
  }


  estimator=(signal+backgr>0.)?signal/(signal+backgr):-1.;

  debug() <<"Photon Candidate :"<<endmsg;
  debug() <<" -E       ="<<energy<<endmsg;
  debug() <<" -Et      ="<<et<<endmsg;
  debug() <<" -Spd hit ="<<eSpd<<endmsg;
  debug() <<" -EPrs    ="<<ePrs<<endmsg;
  debug() <<" -Chi2    ="<<chi2<<endmsg;
  debug() <<" -ShShape ="<<shape<<endmsg;
  debug() <<"      => estimator="<<estimator<<endmsg;

//  if (m_monitoring) {m_likelihood->fill(estimator, 1.);}

  return estimator;

};
// ============================================================================

double CaloPhotonEstimatorTool::operator() (const LHCb::CaloHypo* hypo ) const{
  return likelihood(hypo);
};

// ============================================================================

const LHCb::CaloHypo::Hypothesis& CaloPhotonEstimatorTool::hypothesis ( ) const
{
  return m_hypothesis ;
};

// ============================================================================
/*
std::vector<IHistogram1D*> CaloPhotonEstimatorTool::makeHisto(
	unsigned int bin, double xmin, double xmax,
  unsigned int nhisto, std::string hname, std::string dir,
  std::vector<double> data, unsigned int n){

  std::vector<IHistogram1D*> histoList;
	char histoname[60];

  if ( (data.size() % n) !=0 ) {
		Error("Wrong Data field.");
  }
  unsigned int isize=data.size()/n;

	double width=(xmax-xmin)/double(bin);

  for (unsigned int i=0; i<n; ++i){
		IHistogram1D* histo;
    sprintf(histoname,"%s #%i",hname.c_str(),nhisto+i);
		// debug() <<"Processing Histo ..."<<histoname<<endmsg;

    histo  = m_histoSvc->book( dir                      ,
                               nhisto+i                 ,
                               histoname                ,
                               bin, xmin, xmax           ) ;

    unsigned int istart=isize*i;
    unsigned int idata=0;
    while (idata<isize){
			double value = data [ istart + idata ];
			double xval  = xmin + (double(idata)-0.5)*width;
      histo->fill(xval,value);
      idata++;
    }
    if( 0 == histo  ) {
			Error("Histogram is not booked !");
    }
		histoList.push_back(histo);
   }
	return histoList;
}
*/

