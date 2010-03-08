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
/// local
#include "CaloPhotonEstimatorTool.h"


// ============================================================================
/** @file
 *
 *  Implementation file for class CaloPhotonEstimatorTool
 *  Photon Selection Tool
 *  (LHCb 2004-03)
 *
 *  @author Frederic Machefert frederic.machefert@in2p3.fr
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
  
  , m_tracking    (true)

  , m_table()
  , m_tableLocation ( LHCb::CaloIdLocation::ClusterMatch )

  , m_vertex      (   )
  
  , m_binEPrs        (   )
  , m_binChi2        (   )
  , m_binSeed        (   )

  , m_signalEPrsData_0     (   )
  , m_signalEPrsSpdData_0  (   )
  , m_backgrEPrsData_0     (   )
  , m_backgrEPrsSpdData_0  (   )

  , m_signalChi2Data_0     (   )
  , m_signalChi2SpdData_0  (   )
  , m_backgrChi2Data_0     (   )
  , m_backgrChi2SpdData_0  (   )
  
  , m_signalSeedData_0    (   )
  , m_signalSeedSpdData_0 (   )
  , m_backgrSeedData_0    (   )
  , m_backgrSeedSpdData_0 (   )

  , m_signalEPrsData_1     (   )
  , m_signalEPrsSpdData_1  (   )
  , m_backgrEPrsData_1     (   )
  , m_backgrEPrsSpdData_1  (   )

  , m_signalChi2Data_1     (   )
  , m_signalChi2SpdData_1  (   )
  , m_backgrChi2Data_1     (   )
  , m_backgrChi2SpdData_1  (   )
  
  , m_signalSeedData_1    (   )
  , m_signalSeedSpdData_1 (   )
  , m_backgrSeedData_1    (   )
  , m_backgrSeedSpdData_1 (   )

  , m_signalEPrsData_2     (   )
  , m_signalEPrsSpdData_2  (   )
  , m_backgrEPrsData_2     (   )
  , m_backgrEPrsSpdData_2  (   )

  , m_signalChi2Data_2     (   )
  , m_signalChi2SpdData_2  (   )
  , m_backgrChi2Data_2     (   )
  , m_backgrChi2SpdData_2  (   )
  
  , m_signalSeedData_2    (   )
  , m_signalSeedSpdData_2 (   )
  , m_backgrSeedData_2    (   )
  , m_backgrSeedSpdData_2 (   )

  , m_extrapol           (true)
{
  declareInterface<ICaloHypoLikelihood>( this );

  declareProperty("DetEcal",m_nameOfECAL);
  declareProperty("DetPrs",m_nameOfPRS);
  declareProperty("DetSpd",m_nameOfSPD);
  
  declareProperty("EPrs"          ,m_binEPrs);
  declareProperty("Chi2"          ,m_binChi2);
  declareProperty("Seed"          ,m_binSeed);
 
  declareProperty( "Tracking"      , m_tracking ) ;
  
  declareProperty( "TableLocation" , m_tableLocation ) ;
  
  declareProperty("SigEPrs_noSpd_0_Data"  ,m_signalEPrsData_0);
  declareProperty("SigEPrs_Spd_0_Data"     ,m_signalEPrsSpdData_0);
  declareProperty("BkgEPrs_noSpd_0_Data"  ,m_backgrEPrsData_0);
  declareProperty("BkgEPrs_Spd_0_Data"      ,m_backgrEPrsSpdData_0);
  declareProperty("SigChi2_noSpd_0_Data"  ,m_signalChi2Data_0);
  declareProperty("SigChi2_Spd_0_Data"     ,m_signalChi2SpdData_0);
  declareProperty("BkgChi2_noSpd_0_Data"  ,m_backgrChi2Data_0);
  declareProperty("BkgChi2_Spd_0_Data"      ,m_backgrChi2SpdData_0);
  declareProperty("SigSeed_noSpd_0_Data"  ,m_signalSeedData_0);
  declareProperty("SigSeed_Spd_0_Data"      ,m_signalSeedSpdData_0);
  declareProperty("BkgSeed_noSpd_0_Data"  ,m_backgrSeedData_0);
  declareProperty("BkgSeed_Spd_0_Data"      ,m_backgrSeedSpdData_0);

  declareProperty("SigEPrs_noSpd_1_Data"  ,m_signalEPrsData_1);
  declareProperty("SigEPrs_Spd_1_Data"     ,m_signalEPrsSpdData_1);
  declareProperty("BkgEPrs_noSpd_1_Data"  ,m_backgrEPrsData_1);
  declareProperty("BkgEPrs_Spd_1_Data"      ,m_backgrEPrsSpdData_1);
  declareProperty("SigChi2_noSpd_1_Data"  ,m_signalChi2Data_1);
  declareProperty("SigChi2_Spd_1_Data"     ,m_signalChi2SpdData_1);
  declareProperty("BkgChi2_noSpd_1_Data"  ,m_backgrChi2Data_1);
  declareProperty("BkgChi2_Spd_1_Data"      ,m_backgrChi2SpdData_1);
  declareProperty("SigSeed_noSpd_1_Data"  ,m_signalSeedData_1);
  declareProperty("SigSeed_Spd_1_Data"      ,m_signalSeedSpdData_1);
  declareProperty("BkgSeed_noSpd_1_Data"  ,m_backgrSeedData_1);
  declareProperty("BkgSeed_Spd_1_Data"      ,m_backgrSeedSpdData_1);

  declareProperty("SigEPrs_noSpd_2_Data"  ,m_signalEPrsData_2);
  declareProperty("SigEPrs_Spd_2_Data"     ,m_signalEPrsSpdData_2);
  declareProperty("BkgEPrs_noSpd_2_Data"  ,m_backgrEPrsData_2);
  declareProperty("BkgEPrs_Spd_2_Data"      ,m_backgrEPrsSpdData_2);
  declareProperty("SigChi2_noSpd_2_Data"  ,m_signalChi2Data_2);
  declareProperty("SigChi2_Spd_2_Data"     ,m_signalChi2SpdData_2);
  declareProperty("BkgChi2_noSpd_2_Data"  ,m_backgrChi2Data_2);
  declareProperty("BkgChi2_Spd_2_Data"      ,m_backgrChi2SpdData_2);
  declareProperty("SigSeed_noSpd_2_Data"  ,m_signalSeedData_2);
  declareProperty("SigSeed_Spd_2_Data"      ,m_signalSeedSpdData_2);
  declareProperty("BkgSeed_noSpd_2_Data"  ,m_backgrSeedData_2);
  declareProperty("BkgSeed_Spd_2_Data"      ,m_backgrSeedSpdData_2);
  
  declareProperty("Extrapolation"    ,m_extrapol);

  if("HLT"==context())m_tableLocation = LHCb::CaloIdLocation::ClusterMatchHlt;  

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
  m_detSpd  = (DeCalorimeter*) detspd;
  m_detPrs  = (DeCalorimeter*) detprs;

  // Locate plane
  m_planeEcal = m_detEcal->plane(CaloPlane::ShowerMax);
  m_planeSpd  = m_detSpd ->plane(CaloPlane::Middle);
  m_planePrs  = m_detPrs ->plane(CaloPlane::Middle);

  // IP
  m_vertex=Gaudi::XYZPoint(0.,0.,0.);

  // Check data consistency
  if (m_binEPrs.size()!=6)
  { return Error("Wrong Binning Parameter (EPrs)"); }
  if (m_tracking) {
    if (m_binChi2.size()!=6)
    { return Error("Wrong Binning Parameter (Chi2Tk)"); }
  }
  if (m_binSeed.size()!=6)
  { return Error("Wrong Binning Parameter (Seed)"); }


// Build useful vector
  int index;
  double width;
  width=(m_binEPrs[ 2 ]-m_binEPrs[ 1 ])/double(m_binEPrs[ 0 ]);
  for ( index=0; index<=int (m_binEPrs[ 0 ]); ++index ) {
    m_ePrsXBin.push_back(double(index)*width+m_binEPrs[ 1 ]);
  }
  if (m_tracking){
    width=(m_binChi2[ 2 ]-m_binChi2[ 1 ])/double(m_binChi2[ 0 ]);
    for ( index=0; index<=int (m_binChi2[ 0 ]); ++index ) {
      m_chi2XBin.push_back(double(index)*width+m_binChi2[ 1 ]);
    }
  }

  width=(m_binSeed[ 2 ]-m_binSeed[ 1 ])/double(m_binSeed[ 0 ]);
  for ( index=0; index<=int (m_binSeed[ 0 ]); ++index ) {
    m_seedXBin.push_back(double(index)*width+m_binSeed[ 1 ]);
  }
  width=(m_binEPrs[ 5 ]-m_binEPrs[ 4 ])/double(m_binEPrs[ 3 ]);
  for ( index=0; index<=int (m_binEPrs[ 3 ]); ++index ) {
    m_ePrsYBin.push_back(double(index)*width+m_binEPrs[ 4 ]);
  }

  if (m_tracking) {
    width=(m_binChi2[ 5 ]-m_binChi2[ 4 ])/double(m_binChi2[ 3 ]);
    for ( index=0; index<=int (m_binChi2[ 3 ]); ++index ) {
      m_chi2YBin.push_back(double(index)*width+m_binChi2[ 4 ]);
    }
  }

  width=(m_binSeed[ 5 ]-m_binSeed[ 4 ])/double(m_binSeed[ 3 ]);
  for ( index=0; index<=int (m_binSeed[ 3 ]); ++index ) {
    m_seedYBin.push_back(double(index)*width+m_binSeed[ 4 ]);
  }

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
  if( 0 == hypo ){
    Warning("CaloHypo points to NULL",StatusCode::SUCCESS ).ignore();
    counter("Null hypo")+=1;
    return -999999.;
  }

  double energy, et, eSeed, eSpd, ePrs;
  unsigned int area;
  evalParam(hypo, energy, et, eSeed, eSpd, ePrs, area);
  
  double chi2 = -999.;
  if (m_tracking) {
    if( 1 != hypo->clusters().size() ){
      debug() <<"Number of clusters != 1"<<endmsg;
      return -1.;
    }
    const SmartRef<LHCb::CaloCluster> cluster=hypo->clusters().front();
    chi2 = evalChi2(cluster);
  } 
    
  return evalLikelihood(energy, et, eSeed, eSpd, ePrs, chi2, area);
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
double CaloPhotonEstimatorTool::likelihood(const LHCb::ProtoParticle* pp )  const
{
  // Get the relevant information - basic checks 
  if( 0 == pp ){
    Warning("ProtoP points to NULL",StatusCode::SUCCESS ).ignore();
    counter("Null proto")+=1;
    return -999999.;
  }


  SmartRef<LHCb::CaloHypo> hypo;
  SmartRefVector<LHCb::CaloHypo>::const_iterator hypolist;
  for (hypolist=pp->calo().begin(); hypolist!=pp->calo().end(); hypolist++){
    if((*hypolist)->hypothesis()==LHCb::CaloHypo::Photon) { hypo = (*hypolist) ; }
  }
  
  double energy, et, eSeed, eSpd, ePrs;
  unsigned int area;
  evalParam(hypo, energy, et, eSeed, eSpd, ePrs, area);
  
  double chi2 = -999.;
  if (m_tracking) chi2 = evalChi2(pp);
  
  return evalLikelihood(energy, et, eSeed, eSpd, ePrs, chi2, area);
}

//***********************************************************
double CaloPhotonEstimatorTool::evalChi2(const LHCb::CaloCluster* cluster) const {
  double chi2=-999.;
  m_table = get<LHCb::Calo2Track::IClusTrTable>( m_tableLocation ) ;
  const LHCb::Calo2Track::IClusTrTable::Range range = m_table -> relations ( cluster ) ;
  if( range.empty() )  { 
    chi2=1.e+6; 
  }            // bad match -> large value !
  else {
    chi2 = range.front().weight();
  }
  return chi2;
}

//***********************************************************
double CaloPhotonEstimatorTool::evalChi2(const LHCb::ProtoParticle* pp) const {
  return (pp->info(LHCb::ProtoParticle::CaloTrMatch , -999.));
}	
  
//***********************************************************
void CaloPhotonEstimatorTool::evalParam(const LHCb::CaloHypo* hypo,
					  double       &energy,
					  double       &et, 
					  double       &eSeed,
					  double       &eSpd,
					  double       &ePrs,
					  unsigned int &area) const {
  if( 1 != hypo->clusters().size() ){
    debug() <<"Number of clusters != 1"<<endmsg;
    return ;
  }

  const SmartRef<LHCb::CaloCluster> cluster=hypo->clusters().front();


  if( cluster->entries().empty() ){
    Warning("Empty entries",StatusCode::SUCCESS ).ignore();
    counter("Empty entries")+=1;
    return;
  }

  LHCb::CaloCluster::Entries::const_iterator iseed =
    LHCb::ClusterFunctors::locateDigit( cluster->entries().begin() ,
                                        cluster->entries().end  () ,
                                        LHCb::CaloDigitStatus::SeedCell  ) ;


  if( iseed == cluster->entries().end() ){
    Warning("Empty Entries",StatusCode::SUCCESS ).ignore();
    counter("Empty entries")+=1;
    return;
  }

  const LHCb::CaloDigit* seed = iseed->digit();
  if( 0 == seed ){
    Warning("Seed point to NULL",StatusCode::SUCCESS ).ignore();
    counter("NULL seed")+=1;
    return;
  }
  
  //***
  // Evaluate Estimator Parameters : Energy, EPrs, Chi2 and Eseed ...

  const LHCb::CaloPosition *pos = hypo->position() ;
  const Gaudi::XYZPoint position (pos->x(),pos->y(),pos->z());

  // Energy
  energy  = pos->e();
  Gaudi::XYZVector hypoVect = position-m_vertex;
  et=energy*hypoVect.Rho()/hypoVect.R();
  
  // Seed energy
  eSeed = energy>0.?(seed->e())/energy:-1.;
  
  // area
  area = seed->cellID().area();

  //Spd hit and Prs deposit
  eSpd=0.;
  ePrs=0.;

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
        warning() << " CAN NOT EXTRAPOLATE TO THE PRS PLANE " << endmsg;
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
  return; 
}

//***********************************************************
double CaloPhotonEstimatorTool::evalLikelihood(double energy,
					       double et,
					       double eSeed,
					       double eSpd,
					       double ePrs,
					       double chi2,
					       unsigned int area) const {

  // Corresponding Energy bin
  double transfE=log(1.35914*energy)-6.21461;
  unsigned int ibin;

  // Initialization
  double estimator;
  double signal, backgr;
  signal=1.; 	backgr=1.;

  // SPD Hit
  if (eSpd>1) {
    if (0==area){
      debug()<<"Spy Lh(Photon) - Area 0/Spd ( Etransf="<<transfE<<")"<<endmsg;
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      debug()<<"EPrs bin="<<ibin<<endmsg;
      signal *= m_signalEPrsSpdData_0[ ibin ];
      backgr *= m_backgrEPrsSpdData_0[ ibin ];
      debug()<<"Partial Sig Pdf="<<m_signalEPrsSpdData_0[ ibin ]<<endmsg;
      debug()<<"Partial Bkg Pdf="<<m_backgrEPrsSpdData_0[ ibin ]<<endmsg;
      if (m_tracking) {
	ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
	debug()<<"Chi2 bin="<<ibin<<endmsg;      
	signal *= m_signalChi2SpdData_0[ ibin ];
	backgr *= m_backgrChi2SpdData_0[ ibin ];
	debug()<<"Partial Sig Pdf="<<m_signalChi2SpdData_0[ ibin ]<<endmsg;
	debug()<<"Partial Bkg Pdf="<<m_backgrChi2SpdData_0[ ibin ]<<endmsg;
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      debug()<<"Seed bin="<<ibin<<endmsg;      
      signal *= m_signalSeedSpdData_0[ ibin ];
      backgr *= m_backgrSeedSpdData_0[ ibin ];
      debug()<<"Partial Sig Pdf="<<m_signalSeedSpdData_0[ ibin ]<<endmsg;
      debug()<<"Partial Bkg Pdf="<<m_backgrSeedSpdData_0[ ibin ]<<endmsg;
    }
    if (1==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsSpdData_1[ ibin ];
      backgr *= m_backgrEPrsSpdData_1[ ibin ];
      if (m_tracking){
	ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
	signal *= m_signalChi2SpdData_1[ ibin ];
	backgr *= m_backgrChi2SpdData_1[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedSpdData_1[ ibin ];
      backgr *= m_backgrSeedSpdData_1[ ibin ];
    }
    if (2==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsSpdData_2[ ibin ];
      backgr *= m_backgrEPrsSpdData_2[ ibin ];
      if (m_tracking){
	ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
	signal *= m_signalChi2SpdData_2[ ibin ];
	backgr *= m_backgrChi2SpdData_2[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedSpdData_2[ ibin ];
      backgr *= m_backgrSeedSpdData_2[ ibin ];
    }
  }

  // No SPD Hit
  else {
    if (0==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsData_0[ ibin ];
      backgr *= m_backgrEPrsData_0[ ibin ];
      if (m_tracking){
	ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
	signal *= m_signalChi2Data_0[ ibin ];
	backgr *= m_backgrChi2Data_0[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedData_0[ ibin ];
      backgr *= m_backgrSeedData_0[ ibin ];
    }
    if (1==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsData_1[ ibin ];
      backgr *= m_backgrEPrsData_1[ ibin ];
      if (m_tracking){
	ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
	signal *= m_signalChi2Data_1[ ibin ];
	backgr *= m_backgrChi2Data_1[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedData_1[ ibin ];
      backgr *= m_backgrSeedData_1[ ibin ];
    }
    if (2==area){
      ibin=bin(transfE,m_ePrsYBin)+(m_ePrsYBin.size()+1)*bin(ePrs,m_ePrsXBin);
      signal *= m_signalEPrsData_2[ ibin ];
      backgr *= m_backgrEPrsData_2[ ibin ];
      if (m_tracking){
	ibin=bin(transfE,m_chi2YBin)+(m_chi2YBin.size()+1)*bin(chi2,m_chi2XBin);
	signal *= m_signalChi2Data_2[ ibin ];
	backgr *= m_backgrChi2Data_2[ ibin ];
      }
      ibin=bin(transfE,m_seedYBin)+(m_seedYBin.size()+1)*bin(eSeed,m_seedXBin);
      signal *= m_signalSeedData_2[ ibin ];
      backgr *= m_backgrSeedData_2[ ibin ];
    }
  }

  estimator=(signal+backgr>0.)?signal/(signal+backgr):-1.;

  debug() <<"Photon Candidate :"<<endmsg;
  debug() <<" -E       ="<<energy<<endmsg;
  debug() <<" -Et      ="<<et<<endmsg;
  debug() <<" -Spd hit ="<<eSpd<<endmsg;
  debug() <<" -EPrs    ="<<ePrs<<endmsg;
  if (m_tracking) debug() <<" -Chi2    ="<<chi2<<endmsg;
  debug() <<" -ESeed   ="<<eSeed<<endmsg;
  debug() <<"      => estimator="<<estimator<<endmsg;

//  if (m_monitoring) {m_likelihood->fill(estimator, 1.);}

  return estimator;

};
// ============================================================================

// ============================================================================

double CaloPhotonEstimatorTool::operator() (const LHCb::ProtoParticle* pp ) const{
  return likelihood(pp);
};

