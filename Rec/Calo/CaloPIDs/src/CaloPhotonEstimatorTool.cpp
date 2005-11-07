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
// CaloKernel
// DetDesc
#include "DetDesc/IGeometryInfo.h"
/// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CaloEvent
#include "Kernel/CaloHypotheses.h"
#include "Event/CaloDigit.h"
#include "Event/CaloHypo.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"
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

static const ToolFactory<CaloPhotonEstimatorTool> s_factory;
const IToolFactory& CaloPhotonEstimatorToolFactory = s_factory;

//
// Standard Constructor
CaloPhotonEstimatorTool::CaloPhotonEstimatorTool(const std::string& type,
                           const std::string& name,
                           const IInterface* parent)
  : CaloTool( type, name, parent )

  , m_nameOfECAL ("/dd/Structure/LHCb/Ecal")
  , m_nameOfPRS  ("/dd/Structure/LHCb/Prs")
  , m_nameOfSPD  ("/dd/Structure/LHCb/Spd")
//  , m_nameOfECAL ("DeCalorimeterLocation::Ecal")
//  , m_nameOfPRS  ("DeCalorimeterLocation::Prs")
//  , m_nameOfSPD  ("DeCalorimeterLocation::Spd")

  , m_zPrs      ( -100.*km )
  , m_zSpd      ( -100.*km )
  , m_shiftSpd  (  30.5*mm )

//  , m_histoSvc  ( 0        )

  , m_matchType   ("AssociatorWeighted<CaloCluster,Track,float>")
  , m_matchName   ("PhotonMatch")
  , m_match       ( 0           )

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
  declareInterface<IIncidentListener> (this);
  declareInterface<ICaloHypoLikelihood>( this );

  declareProperty("DetEcal",m_nameOfECAL);
  declareProperty("DetPrs",m_nameOfPRS);
  declareProperty("DetSpd",m_nameOfSPD);

  declareProperty("ShiftSPDCell",m_shiftSpd);

  declareProperty( "MatchType" , m_matchType  ) ;
  declareProperty( "MatchName" , m_matchName  ) ;

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
}
// ============================================================================
/** destructor, virtual and protected
 */
// ============================================================================
CaloPhotonEstimatorTool::~CaloPhotonEstimatorTool()
{
  /// remove detector
  setDet( (const DeCalorimeter*) 0 );
  ///
};

// ============================================================================
/** standard initialization method
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonEstimatorTool::initialize ()
{
  MsgStream logbk( msgSvc() , name() );
  logbk << MSG::DEBUG << "==> Initialize CaloPhotonEstimatorTool" << endreq;

  /// initialize the base class
  StatusCode sc = CaloTool::initialize ();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class!") ; }

  // ECAL
  SmartDataPtr<DeCalorimeter> detecal (detSvc(),m_nameOfECAL);
  if(!detecal)
    logbk<<MSG::ERROR<<"Unable to retrieve ECAL detector "
       <<m_nameOfECAL<<endreq;
  if (!detecal){ return StatusCode::FAILURE ;}

  // SPD
  SmartDataPtr<DeCalorimeter> detspd (detSvc(),m_nameOfSPD);
  if(!detspd)
    logbk<<MSG::ERROR<<"Unable to retrieve SPD detector "
       <<m_nameOfSPD<<endreq;
  if (!detspd){    return StatusCode::FAILURE ; }

  // PRS
  SmartDataPtr<DeCalorimeter> detprs (detSvc(),m_nameOfPRS);
  if(!detprs)
    logbk<<MSG::ERROR<<"Unable to retrieve PRS detector "
       <<m_nameOfPRS<<endreq;
  if (!detprs){ return StatusCode::FAILURE ; }

  // Convert Detectors to DeCalorimeter
  m_detEcal = (DeCalorimeter*) detecal;
  m_detSpd = (DeCalorimeter*) detspd;
  m_detPrs = (DeCalorimeter*) detprs;

  const IGeometryInfo* geoinfSpd = m_detSpd->geometry() ;
  if( 0 == geoinfSpd ) { return Error("IGeotryInfo is not available!"); }
  // center of the detector in mother reference frame
  HepPoint3D centerSpd = geoinfSpd->toGlobal( HepPoint3D() );
  m_zSpd = centerSpd.z();

  const IGeometryInfo* geoinfPrs = m_detPrs->geometry() ;
  if( 0 == geoinfPrs ) { return Error("IGeotryInfo is not available!"); }
  // center of the detector in mother reference frame
  HepPoint3D centerPrs = geoinfPrs->toGlobal( HepPoint3D() );
  m_zPrs = centerPrs.z();

  // locate match
  m_match  = tool<IMatch>( m_matchType  , m_matchName ) ;
  if( 0 == m_match  ) { return StatusCode::FAILURE ; }

  // IP
  m_vertex=HepPoint3D(0.,0.,0.);

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

    logbk<<MSG::DEBUG<<"HistogramDataSvc is loaded properly!!"<<endreq;

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
  logbk<<MSG::WARNING<<"SPD scintillator offset HARD-CODED : value="
       <<m_shiftSpd<<endreq;
  logbk<<MSG::WARNING<<"Vertex location HARD-CODED : IP(0.,0.,0.)"<<endreq;

  return StatusCode::SUCCESS ;
};

// ============================================================================
/** standard finalization method
 *  @return status code
 */
// ============================================================================
StatusCode CaloPhotonEstimatorTool::finalize   ()
{
  /// remove detector
  setDet( (const DeCalorimeter*) 0 );
  /// finalize the  the base class
  return CaloTool::finalize ();
};


// ============================================================================
/** handle the incident
 *  @see Incident
 *  @see IIncidentListener
 *  @see IIncidentSvc
 *  @param incident incident to be handled
 */
// ============================================================================
void CaloPhotonEstimatorTool::handle( const Incident& /* incident */  )
{
  MsgStream logbk( msgSvc() , name() );
  logbk<<MSG::DEBUG<<"Handle exception : reset pointers to TS"<<endreq;
};

// ============================================================================
double CaloPhotonEstimatorTool::likelihood(const CaloHypo* hypo )  const
{
  MsgStream logbk( msgSvc() , name() );

  // Get the relevant information - basic checks

  if( 0 == hypo )
    { return Exception( " *CaloHypo* points to NULL " );}

  if( 1 != hypo->clusters().size() ){
    logbk<<MSG::DEBUG<<"Number of clusters != 1"<<endreq;
    return -1.;
  }

  const SmartRef<CaloCluster> cluster=hypo->clusters().front();

  if ( 0 == cluster ){
    return Exception( " *CaloCluster* points to NULL " );
  }

  if( cluster->entries().empty() ){
    return Exception( " *CaloCluster* empty " );
  }

  CaloCluster::Entries::const_iterator iseed =
    ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                  cluster->entries().end  () ,
                                  CaloDigitStatus::SeedCell  ) ;

  if( iseed == cluster->entries().end() ){
    return Exception( " *SeedCell* not found ");
  }

  const CaloDigit* seed = iseed->digit();
  if( 0 == seed ){
    return Exception( " SeedCell *Digit* points to NULL! ");
  }

  //***
  // Evaluate Estimator Parameters : Energy, EPrs, Chi2 and Shower Shape ...

  const CaloPosition *pos = hypo->position() ;
  const  HepPoint3D position (pos->x(),pos->y(),pos->z());

  // Energy
  double energy=hypo->momentum()->momentum().e();
  double et=hypo->momentum()->momentum().perp();
  /// Chi2
  double chi2;
  const Table* table = m_match->direct();
  if( 0 == table     ) { return Error("Table* points to NULL!");} // RETURN

  const Range range = table->relations( cluster ) ;
  if( range.empty() )  { chi2=1.e+6; }            // bad match -> large value !
  else {
    chi2 = range.front().weight();
  }
  // Shower Shape
  double shape=pos->spread()(1,1)+pos->spread()(2,2);
  //Spd hit and Prs deposit
  double eSpd=0.;
  double ePrs=0.;

  // point in the detector
  HepPoint3D spdPoint,prsPoint;
  if (!m_extrapol){
    spdPoint.setX(pos->x());
    spdPoint.setY(pos->y());
    spdPoint.setZ( m_zSpd + m_shiftSpd );
    prsPoint.setX(pos->x());
    prsPoint.setY(pos->y());
    prsPoint.setZ(m_zPrs);
  }
  else {
    HepPoint3D position(pos->x(),pos->y(),pos->z());
    spdPoint  =  ( position - m_vertex );
    spdPoint *=  ( m_zSpd + m_shiftSpd - m_vertex.z() ) /
      ( position.z() - m_vertex.z() );
    spdPoint +=  m_vertex ;
    prsPoint  =  ( position - m_vertex );
    prsPoint *=  ( m_zPrs - m_vertex.z() ) /
      ( position.z() - m_vertex.z() );
    prsPoint +=  m_vertex ;
  }

  const CaloCellID cellSpd = m_detSpd->Cell( spdPoint );
  const CaloCellID cellPrs = m_detPrs->Cell( prsPoint );

	// Get CaloCell Deposits in the SPD and PRS
  if( !(CaloCellID() == cellSpd) ) { // valid cell!
	  for (SmartRefVector<CaloDigit>::const_iterator
           digit=hypo->digits().begin() ;
         digit != hypo->digits().end() ; digit++ ) {
	    if ( (*digit)->cellID() == cellSpd ) {
				eSpd=(*digit)->e();
	    }
	  }
	}

	if( !(CaloCellID() == cellPrs)  ) { // valid cell!
	  for (SmartRefVector<CaloDigit>::const_iterator
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

  logbk<<MSG::DEBUG<<"Photon Candidate :"<<endreq;
  logbk<<MSG::DEBUG<<" -E       ="<<energy<<endreq;
  logbk<<MSG::DEBUG<<" -Et      ="<<et<<endreq;
  logbk<<MSG::DEBUG<<" -Spd hit ="<<eSpd<<endreq;
  logbk<<MSG::DEBUG<<" -EPrs    ="<<ePrs<<endreq;
  logbk<<MSG::DEBUG<<" -Chi2    ="<<chi2<<endreq;
  logbk<<MSG::DEBUG<<" -ShShape ="<<shape<<endreq;
  logbk<<MSG::DEBUG<<"      => estimator="<<estimator<<endreq;

//  if (m_monitoring) {m_likelihood->fill(estimator, 1.);}

  return estimator;

};
// ============================================================================

double CaloPhotonEstimatorTool::operator() (const CaloHypo* hypo ) const{
  return likelihood(hypo);
};

// ============================================================================

const CaloHypotheses::Hypothesis& CaloPhotonEstimatorTool::hypothesis ( ) const
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
		// logbk<<MSG::DEBUG<<"Processing Histo ..."<<histoname<<endreq;

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
// ============================================================================
// The End
// ============================================================================

