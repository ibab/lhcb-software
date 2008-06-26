// $Id: CaloSingleGammaTool.cpp,v 1.3 2008-06-26 13:07:04 jpalac Exp $
// ===========================================================================
// Include files
// GaudiKernel
#include "GaudiKernel/ToolFactory.h" 
/// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// Event
#include "Event/CaloDigit.h"
#include "Event/CaloDigitStatus.h"
#include "Event/CaloHypo.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"
/// local
#include "CaloSingleGammaTool.h"

/** @file 
 *  @author Frederic Machefert
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloSingleGammaTool );

//
// Standard Constructor
CaloSingleGammaTool::CaloSingleGammaTool(const std::string& type, 
                                         const std::string& name, 
                                         const IInterface* parent) 
  : GaudiTool( type, name, parent )
  //    , m_incSvc    ( 0        )
    , m_planeEcal ( )
    , m_planeSpd  ( )
    , m_planePrs  ( )
    , m_nameOfECAL(DeCalorimeterLocation::Ecal)
    , m_nameOfSPD (DeCalorimeterLocation::Spd)
    , m_nameOfPRS (DeCalorimeterLocation::Prs)
    , m_vertex    (          )
    , m_extrapolation ( true )
{
  declareInterface<ICaloHypoLikelihood>( this );
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
  debug() << "==> Initialize" << endreq;
  /// initialize the base class
  StatusCode sc = GaudiTool::initialize ();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  // ECAL
  SmartDataPtr<DeCalorimeter> detecal (detSvc(),m_nameOfECAL);
  if(!detecal)error()<<"Unable to retrieve ECAL detector "  <<m_nameOfECAL<<endreq;
  if(!detecal){ return StatusCode::FAILURE ;}

  // SPD
  SmartDataPtr<DeCalorimeter> detspd (detSvc(),m_nameOfSPD);
  if(!detspd)error()<<"Unable to retrieve SPD detector "<<m_nameOfSPD<<endreq;
  if(!detspd){ return StatusCode::FAILURE ; }
  
  // PRS
  SmartDataPtr<DeCalorimeter> detprs (detSvc(),m_nameOfPRS);
  if(!detprs)error()<<"Unable to retrieve PRS detector "<<m_nameOfPRS<<endreq;
  if(!detprs){ return StatusCode::FAILURE ; }
  
  // Convert Detectors to DeCalorimeter
  m_detEcal = (DeCalorimeter*) detecal;
  m_detSpd = (DeCalorimeter*) detspd;
  m_detPrs = (DeCalorimeter*) detprs;

  // Locate plane
  m_planeEcal= m_detEcal->plane(CaloPlane::Middle);
  m_planeSpd = m_detSpd->plane(CaloPlane::Middle);
  m_planePrs = m_detPrs->plane(CaloPlane::Middle);

  return StatusCode::SUCCESS ;
};

// ============================================================================
/** standard finalization method 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloSingleGammaTool::finalize   ()
{  
  return GaudiTool::finalize ();
};


// ============================================================================

const LHCb::CaloHypo::Hypothesis& CaloSingleGammaTool::hypothesis ( ) const
{
  return m_hypothesis ;
};

// ============================================================================
double CaloSingleGammaTool::likelihood(const LHCb::CaloHypo* hypo )  const
{

  double lhood=0.;

  if( 0 == hypo ) 
    { return Exception( " const CaloHypo* points to NULL! " );}

  if( hypo->clusters().size() ==1 ) {
    
    const SmartRef<LHCb::CaloCluster> cluster=*(hypo->clusters()).begin();
    
    if( cluster->entries().empty() ) 
    { return Exception( " Corresponding CaloCluster is empty! " );}
    
    LHCb::CaloCluster::Entries::const_iterator iseed = 
      LHCb::ClusterFunctors::
      locateDigit( cluster->entries().begin() ,  
                   cluster->entries().end  () ,  
                   LHCb::CaloDigitStatus::SeedCell  ) ;
    if( cluster->entries().end() == iseed ) 
    { return Exception( " The Seed Cell is not found! ");}
    ///
    const LHCb::CaloDigit* seed = iseed->digit();
    if( 0 == seed ) 
    { return Exception( " The Seed Digit points to NULL! ");}
    //
    
    const LHCb::CaloPosition *pos = hypo->position() ;
    
    // fix by V.B.  Many thanks to G.Corti . 
    if( 0 == pos ) 
    {
      Warning(" likelihood(): CaloPosition* points to NULL");
      return lhood ;
    }
    
    const  Gaudi::XYZPoint position (pos->x(),pos->y(),pos->z());
    
    double eSpd=0.;
    double ePrs=0.;


    Gaudi::XYZPoint spdPoint,prsPoint;

    if(m_extrapolation) {
      const Gaudi::XYZVector direction = position - m_vertex;
      typedef Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector>  LineType ;
      const LineType line =  LineType(m_vertex ,direction );
      double mu=0;
            
      if(!Gaudi::Math::intersection<LineType,Gaudi::Plane3D,Gaudi::XYZPoint>
         ( line , m_planeSpd , spdPoint , mu) )
        warning() << " CAN NOT EXTRAPOLATE TO THE SPD PLANE " << endreq;

      if(!Gaudi::Math::intersection<LineType,Gaudi::Plane3D,Gaudi::XYZPoint>
         ( line , m_planePrs , prsPoint , mu))
        warning() << " CAN NOT EXTRAPOLATE TO THE Prs PLANE " << endreq;
    }
    else{
      spdPoint = m_planeSpd.ProjectOntoPlane( position );
      prsPoint = m_planePrs.ProjectOntoPlane( position );
    }
    
    

    const LHCb::CaloCellID cellSpd = m_detSpd->Cell( spdPoint );
    const LHCb::CaloCellID cellPrs = m_detPrs->Cell( prsPoint );
    if( !(LHCb::CaloCellID() == cellSpd) ){  // valid cell! 
      for (SmartRefVector<LHCb::CaloDigit>::const_iterator 
             digit=hypo->digits().begin() ;
           digit != hypo->digits().end() ; digit++ ) {
        if ( (*digit)->cellID() == cellSpd ) {
          eSpd=(*digit)->e();
        }
      }
    }
    
      
    if( !(LHCb::CaloCellID() == cellPrs) ){  // valid cell! 
      for (SmartRefVector<LHCb::CaloDigit>::const_iterator 
             digit=hypo->digits().begin() ;
           digit != hypo->digits().end() ; digit++ ) {
        if ( (*digit)->cellID() == cellPrs ) {
          ePrs=(*digit)->e();
        }
      }
    }
    
    if (eSpd<1.) lhood=ePrs ;
    if (eSpd>1.) lhood=-ePrs; 
    
  }
  
  return lhood;
  
};

double CaloSingleGammaTool::operator() (const LHCb::CaloHypo* hypo ) const 
{ 
  return likelihood(hypo);
};


