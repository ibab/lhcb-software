// $Id: CaloSShape.cpp,v 1.1.1.1 2002-11-13 20:46:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
// Kernel
#include "Kernel/CaloHypotheses.h"
// CaloEvent/Event 
#include "Event/CaloHypo.h"
#include "Event/CaloDataFunctor.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloSShape.h"

// ============================================================================
/** @file
 * 
 * Implementation file for class : CaloSShape
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 21/03/2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloSShapeFactory 
 *  mandatory declaration of the Tool Factory
 */
// ============================================================================
static const ToolFactory<CaloSShape>         s_Factory ;
const       IToolFactory&CaloSShapeFactory = s_Factory ;

// ============================================================================
/** destructor (virtual) 
 */
// ============================================================================
CaloSShape::~CaloSShape() {};
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param  type actual type of the tool 
 *  @param  name the name of the instance
 *  @param  parent  pointer to parent object 
 */
// ============================================================================
CaloSShape::CaloSShape
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : CaloTool ( type , name , parent ) 
  , m_areaCorrectionsTypeNames ()
{
  declareInterface<ICaloHypoTool>( this ); 
  declareProperty ( "AreaCorrections" , m_areaCorrectionsTypeNames );
};
// ============================================================================


// ============================================================================
/** initialization of the tool 
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode 
CaloSShape::initialize  ()
{
  // initialize the base class
  StatusCode sc = CaloTool::initialize();
  //   locate the detector information
  const DeCalorimeter* calo = get( detSvc() , detName() , calo ) ;
  setDet( calo );
  /// locate corrections
  /// locate all area corrections 
  for( Names::const_iterator Type = m_areaCorrectionsTypeNames.begin() ;
       m_areaCorrectionsTypeNames.end() != Type  ; ++Type )
    {
      ICaloCorrection* corr = tool( *Type , corr ) ;
      m_areaCorrections.push_back( corr );
    };
  ///
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** finalization of the tool
 *  @see CaloTool
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode 
CaloSShape::finalize   ()
{
  /// release all used tools 
  std::for_each( m_areaCorrections.begin () , 
                 m_areaCorrections.end   () , 
                 std::mem_fun(&IInterface::release) );
  /// clear containers 
  m_areaCorrectionsTypeNames .clear();
  m_areaCorrections          .clear();  
  /// finalize the base class 
  return CaloTool::finalize () ;
};
// ============================================================================

// ============================================================================
/** The main processing method 
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
CaloSShape::process    ( CaloHypo* hypo  ) const { return (*this)( hypo ); }
// ============================================================================

// ============================================================================
/** The main processing method (functor interface)
 *  @param  hypo  pointer to CaloHypo object to be processed
 *  @return status code 
 */  
// ============================================================================
StatusCode 
CaloSShape::operator() ( CaloHypo* hypo  ) const 
{
  // avoid long names 
  using namespace CaloDataFunctor;
  
  // select "good" hypotheses 
  if( 0 == hypo ) 
    { return Error("CaloHypo* points to NULL!" , 
                   StatusCode( 200 ) ) ; }
  if( CaloHypotheses::Photon != hypo->hypothesis() ) 
    { return Error("Only for 'Photon' hypotesis is allowed!" , 
                   StatusCode( 201 ) ) ; }
  // only 1 cluster 
  if( 1 != hypo->clusters().size() ) 
    { return Error("Number of clusters is not equal to 1 !", 
                   StatusCode( 202 ) ) ; }
  //
  if( 0 != hypo->momentum() )
    { 
      Warning("CaloHypo::Momentum information is invalidated!");
      delete hypo->momentum();
      hypo->setMomentum( 0 ) ;
    }
  if( 0 != hypo->position() )
    { 
      Warning("CaloHypo::Position information is invalidated!");
      delete hypo->position();
      hypo->setPosition( 0 ) ;
    }
  
  // get the cluster 
  CaloCluster* cluster = hypo->clusters().front();
  if( 0 == cluster ) 
    { return Error("Cluster is invalid!", StatusCode( 203 ) ) ; }
  
  // get seed cell
  const CaloCluster::Entries& entries = cluster->entries();
  CaloCluster::Entries::const_iterator seed = 
    clusterLocateDigit( entries.begin ()          , 
                        entries.end   ()          , 
                        CaloDigitStatus::SeedCell );
  if( entries.end() == seed ) 
    { return Error("'Seed' is not found!" , StatusCode( 204 ) ) ; }
  const CaloDigit* digit = seed->digit() ;
  if( 0 == digit            )
    { return Error("'Seed' is invalid!"   , StatusCode( 205 ) ) ; }
  // get cluster position
  const CaloCluster::Position& clusPos = cluster->position();
  // get seed position 
  const HepPoint3D&            seedPos = det()->cellCenter( digit->cellID() ) ;
  // get the area index  
  const unsigned int area = digit->cellID().area() ;
  if( area >= m_areaCorrections.size() ) 
    { return Error("Wrong area index!", 206 );}
  // dispatch 
  CaloPosition* cp = (*(m_areaCorrections[area]))( clusPos , seedPos );
  if( 0 == cp ) { return Error("Position is not corrected!" , 207 ); }
  // update position
  hypo->setPosition( cp );
  //
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
