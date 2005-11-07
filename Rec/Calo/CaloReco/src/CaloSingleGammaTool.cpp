// $Id: CaloSingleGammaTool.cpp,v 1.6 2005-11-07 12:12:43 odescham Exp $ 
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2004/02/17 12:08:10  ibelyaev
//  update for new CaloKernel and CaloInterfaces
// 
// ===========================================================================
// Include files
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/IIncidentSvc.h"
// =========================================================================== 
// CaloKernel
// ===========================================================================
// DetDesc 
// ===========================================================================
#include "DetDesc/IGeometryInfo.h"
// =========================================================================== 
/// CaloDet 
// ===========================================================================
#include "CaloDet/DeCalorimeter.h"
// ===========================================================================
// CaloEvent
// ===========================================================================
#include "Kernel/CaloHypotheses.h" 
#include "Event/CaloDigit.h"
#include "Event/CaloDigitStatus.h"
#include "Event/CaloHypo.h"
// ===========================================================================
// CaloUtils
// ===========================================================================
#include "CaloUtils/ClusterFunctors.h"
// ===========================================================================
/// local
// ===========================================================================
#include "CaloSingleGammaTool.h"
// ===========================================================================

/** @file 
 *  @author Frederic Machefert
 */

// Instantiation of a static factory class used by clients to create
// instances of this tool

static const ToolFactory<CaloSingleGammaTool> s_factory;
const IToolFactory& CaloSingleGammaToolFactory = s_factory;

//
// Standard Constructor
CaloSingleGammaTool::CaloSingleGammaTool(const std::string& type, 
                           const std::string& name, 
                           const IInterface* parent) 
  : CaloTool( type, name, parent )
  , m_incSvc    ( 0        )
  , m_z         ( -100.*km )
  , m_zSpd      ( -100.*km )
  , m_zPrs      ( -100.*km )
  , m_nameOfECAL("/dd/Structure/LHCb/Ecal")
  , m_nameOfSPD("/dd/Structure/LHCb/Spd")
  , m_nameOfPRS("/dd/Structure/LHCb/Prs")
  , m_vertex    (          )
  , m_extrapolation ( true )
{
  declareInterface<ICaloHypoLikelihood>( this );
  declareInterface<IIncidentListener> (this);  
  declareProperty("Extrapolation",m_extrapolation);
  declareProperty("DetEcal",m_nameOfECAL);
  declareProperty("DetSpd",m_nameOfSPD);
  declareProperty("DetPrs",m_nameOfPRS);
  }
// ============================================================================
/** destructor, virtual and protected 
 */
// ============================================================================
CaloSingleGammaTool::~CaloSingleGammaTool(){}

// ============================================================================
/** standard initialization method 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSingleGammaTool::initialize ()
{
  MsgStream logbk( msgSvc() , name() );
  logbk << MSG::DEBUG << "==> Initialize" << endreq;

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

  const IGeometryInfo* geoinf = m_detEcal->geometry() ;
  if( 0 == geoinf ) { return Error("IGeotryInfo is not available!"); }
  // center of the detector in mother reference frame 
  HepPoint3D center = geoinf->toGlobal( HepPoint3D() );
  m_z = center.z() + m_detEcal->zShowerMax ();

  const IGeometryInfo* geoinfSpd = m_detSpd->geometry() ;
  if( 0 == geoinfSpd ) { return Error("IGeotryInfo is not available!"); }
  // center of the detector in mother reference frame 
  HepPoint3D centerSpd = geoinfSpd->toGlobal( HepPoint3D() );
  m_zSpd = centerSpd.z();

  m_shiftSpd=30.5;
  logbk<<MSG::INFO<<"Spd scintillateur z position : shift="<<m_shiftSpd<<endreq;

  const IGeometryInfo* geoinfPrs = m_detPrs->geometry() ;
  if( 0 == geoinfPrs ) { return Error("IGeotryInfo is not available!"); }
  // center of the detector in mother reference frame 
  HepPoint3D centerPrs = geoinfPrs->toGlobal( HepPoint3D() );
  m_zPrs = centerPrs.z();

  // locate incident service 
  m_incSvc = svc<IIncidentSvc>( "IncidentSvc" , true );
  if( 0 == m_incSvc  ) { return Error("Could not locate IncidentSvc!"     );}
  m_incSvc->addRef() ;
  m_incSvc->addListener( this , IncidentType::EndEvent   , 10 );
  
  ///
  return StatusCode::SUCCESS ;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSingleGammaTool::finalize   ()
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
void CaloSingleGammaTool::handle( const Incident& /* incident */  ) 
{ 
  MsgStream logbk( msgSvc() , name() );
  logbk<<MSG::DEBUG<<"Handle exception : reset pointers to TS"<<endreq;
};

// ============================================================================

const CaloHypotheses::Hypothesis& CaloSingleGammaTool::hypothesis ( ) const
{
  return m_hypothesis ;
};

// ============================================================================
double CaloSingleGammaTool::likelihood(const CaloHypo* hypo )  const
{
  MsgStream logbk( msgSvc() , name() );

  double lhood=0.;

  if( 0 == hypo ) 
    { return Exception( " const CaloHypo* points to NULL! " );}

  if( hypo->clusters().size() ==1 ) {

    const SmartRef<CaloCluster> cluster=*(hypo->clusters()).begin();
    
    if( cluster->entries().empty() ) 
      { return Exception( " Corresponding CaloCluster is empty! " );}
    
    CaloCluster::Entries::const_iterator iseed = 
      ClusterFunctors::
      locateDigit( cluster->entries().begin() ,  
		   cluster->entries().end  () ,  
		   CaloDigitStatus::SeedCell  ) ;
    if( cluster->entries().end() == iseed ) 
      { return Exception( " The Seed Cell is not found! ");}
    ///
    const CaloDigit* seed = iseed->digit();
    if( 0 == seed ) 
      { return Exception( " The Seed Digit points to NULL! ");}
    //
    
    const CaloPosition *pos = hypo->position() ;
    
    // fix by V.B.  Many thanks to G.Corti . 
    if( 0 == pos ) 
      {
        Warning(" likelyhood(): CaloPosition* points to NULL");
        return lhood ;
      }
    
    const  HepPoint3D position (pos->x(),pos->y(),pos->z());

    /**
       logbk<<MSG::DEBUG<<"cluster position: "
       <<pos->x()<<" "
       <<pos->y()<<" "
       <<pos->z()<<endreq;
       logbk<<"m_Ecalz:"<<m_z<<endreq;
    */
    
    double eSpd=0.;
    double ePrs=0.;

    if(m_extrapolation) {
      // point in the detector
      
      HepPoint3D newPoint; 
      
      newPoint  =  ( position - m_vertex );
      newPoint *=  ( m_zSpd + m_shiftSpd - m_vertex.z() ) /
        ( position.z() - m_vertex.z() );
      newPoint +=  m_vertex ;
      /**
         logbk<<MSG::DEBUG<<"Spd point: "
         <<newPoint.x()<<" "
         <<newPoint.y()<<" "
         <<newPoint.z()<<endreq;
      */
      const CaloCellID cellSpd = m_detSpd->Cell( newPoint );
      logbk<<MSG::DEBUG<<cellSpd<<endreq;
      
      newPoint  = ( position - m_vertex );
      newPoint *=  ( m_zPrs - m_vertex.z() ) /
	( position.z() - m_vertex.z() );
      newPoint +=  m_vertex ;
      /**
         logbk<<MSG::DEBUG<<"Prs point: "
         <<newPoint.x()<<" "
         <<newPoint.y()<<" "
         <<newPoint.z()<<endreq;
      */
      const CaloCellID cellPrs = m_detPrs->Cell( newPoint );
      logbk<<MSG::DEBUG<<cellPrs<<endreq;  
      
      
      if( !(CaloCellID() == cellSpd) )  // valid cell! 
	{          
	  for (SmartRefVector<CaloDigit>::const_iterator
		 digit=hypo->digits().begin() ;
	       digit != hypo->digits().end() ; digit++ ) {
	    if ( (*digit)->cellID() == cellSpd ) {
	      eSpd=(*digit)->e();
        //   logbk<<MSG::DEBUG<<"SPD digit energy:"<<eSpd<<endreq;
	    }
	  }
	}
      
      if( !(CaloCellID() == cellPrs) )  // valid cell! 
	{    
	  for (SmartRefVector<CaloDigit>::const_iterator 
		 digit=hypo->digits().begin() ;
	       digit != hypo->digits().end() ; digit++ ) {
	    if ( (*digit)->cellID() == cellPrs ) {
	      ePrs=(*digit)->e();
        //  logbk<<MSG::DEBUG<<"Prs digit energy:"<<ePrs<<endreq;
	    }
	  }
	}
    }
    
    else 
      {
	// point in the detector   
	HepPoint3D newPoint;  
	
	newPoint.setX(pos->x());
	newPoint.setY(pos->y());
	newPoint.setZ( m_zSpd + m_shiftSpd );

  /**
     logbk<<MSG::DEBUG<<"Spd point: "
     <<newPoint.x()<<" "
     <<newPoint.y()<<" "
     <<newPoint.z()<<endreq;
  */

	const CaloCellID cellSpd = m_detSpd->Cell( newPoint );
	logbk<<MSG::DEBUG<<cellSpd<<endreq;
	
	newPoint.setX(pos->x());
	newPoint.setY(pos->y());
	newPoint.setZ(m_zPrs);

  /** 
      logbk<<MSG::DEBUG<<"Prs point: "
      <<newPoint.x()<<" "
      <<newPoint.y()<<" "
      <<newPoint.z()<<endreq;
  */

	const CaloCellID cellPrs = m_detPrs->Cell( newPoint );

  //	logbk<<MSG::DEBUG<<cellPrs<<endreq;  

	if( !(CaloCellID() == cellSpd) )  // valid cell! 
	  {          
	    for (SmartRefVector<CaloDigit>::const_iterator 
		   digit=hypo->digits().begin() ;
		 digit != hypo->digits().end() ; digit++ ) {
	      if ( (*digit)->cellID() == cellSpd ) {
          eSpd=(*digit)->e();
          //		logbk<<MSG::DEBUG<<"SPD digit energy:"<<eSpd<<endreq;
	      }
	    }
	  }
	
	if( !(CaloCellID() == cellPrs) )  // valid cell! 
	  {    
	    for (SmartRefVector<CaloDigit>::const_iterator 
		   digit=hypo->digits().begin() ;
		 digit != hypo->digits().end() ; digit++ ) {
	      if ( (*digit)->cellID() == cellPrs ) {
		ePrs=(*digit)->e();
		// logbk<<MSG::DEBUG<<"Prs digit energy:"<<ePrs<<endreq;
	      }
	    }
	  }
      }
    
    /**
       logbk<<MSG::DEBUG<<"cluster E="<<cluster->e()
       <<" E[SPD]="<<eSpd
       <<" E[Prs]="<<ePrs
       <<endreq;
    */

    if (eSpd<1.) lhood=ePrs ;
    if (eSpd>1.) lhood=-ePrs; 

  }

  /**
     logbk<<MSG::DEBUG<<"PhotonId Likelyhood="
     <<lhood<<endreq;
  */

  return lhood;
  
};

double CaloSingleGammaTool::operator() (const CaloHypo* hypo ) const 
{ 
  return likelihood(hypo);
};

// ============================================================================
// The End 
// ============================================================================

