// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "Momentum2FlightAngleFilterCriterion.h"
#include "DaVinciTools/IGeomDispCalculator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Momentum2FlightAngleFilterCriterion
//
// 27/07/2004 : Luis Fernandez
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<Momentum2FlightAngleFilterCriterion> s_factory;
const IToolFactory& Momentum2FlightAngleFilterCriterionFactory=s_factory;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Momentum2FlightAngleFilterCriterion
::Momentum2FlightAngleFilterCriterion(
                                             const std::string& type,
                                             const std::string& name,
                                             const IInterface* parent)
  : GaudiTool ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties

  //================================================================
  declareProperty( "CutBestPV", m_CutBestPV = true);
  declareProperty( "CosAngle", m_mincos = -2.);
  //================================================================

}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode Momentum2FlightAngleFilterCriterion::initialize() {

  StatusCode sc = GaudiTool::initialize();
  debug() << ">>>   Momentum2FlightAngleFilterCriterion::initialize() " 
          << endreq;
  sc = service( "EventDataSvc", m_EDS, true );

  if( sc.isFailure() ) {
    err() << " Unable to retrieve EventDataSvc " << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool("GeomDispCalculator", m_ipTool, this);
  if(sc.isFailure()){
    err() << " Unable to retrieve GeomDispCalculator tool" << endreq;
    return sc;
  }

  //================================================================
  debug() << ">>>   Cuts are:" << endreq;

  if(m_mincos>-2.){
    debug() << ">>>   Cosine angle: "
            << m_mincos << endreq;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool Momentum2FlightAngleFilterCriterion
::isSatisfied( const Particle* const & part ){

  SmartDataPtr<Vertices> PV(m_EDS,VertexLocation::Primary);

  verbose() << ">>>> Looping on " << PV->size() << " PVs" << endreq;
  VertexVector::const_iterator iv;
  int npv = 0;

  // PV w.r.t. which the composite has the smallest IPS
  Vertex *BestPV = 0;
  
  const Vertex* secv = part->endVertex();
  if (!secv){ 
    verbose() << "No decay vertex found" << endreq;
    return (false);
  }
  
  verbose() << "Found decay vertex z [mm] = " 
          << secv->position().z()<< endreq;

  double normIPSMin = -1.;
  
  bool happy  = true;

  // loop over PVs
  for (iv=PV->begin();iv!=PV->end();++iv) {
    Vertex* pv = *iv;
    if( pv->type()!=Vertex::Primary) continue;

    npv++;
    verbose() << "Primary vertex number = " << npv << endreq;
    verbose() << "Primary vertex z [mm] = " 
              << pv->position().z()<< endreq;

    //================================================================
    // Find the PV w.r.t. which the composite has the smallest IPS
    // Calculate the IPS of the composite candidate 
    if(m_CutBestPV){
      double ip, ipe;
      StatusCode scip = m_ipTool->calcImpactPar(*part,*pv,ip,ipe);
      if(scip.isFailure()) continue;
      double normIPS = ip/ipe;
      verbose() << "IPS = " << normIPS << " sigma" << endreq;
      if(normIPSMin<0||normIPS<normIPSMin) {
        normIPSMin=normIPS;
        BestPV=pv;
      }
    }
    //================================================================
    else{
      // w.r.t. all PVs      
      // Calculate the distance between the two vertices
      Hep3Vector dist = secv->position()-pv->position();
      // momentum of the particle
      Hep3Vector p(part->momentum().vect());
      
      if(p.mag()==0||dist.mag()==0){
        return (false);
      }
      
      double cosangle = p.dot(dist)/p.mag()/dist.mag();
      debug() << "Cosine of the angle between momentum and flight distance = " 
             << cosangle << endreq;
      
      if(m_mincos>-2. && cosangle<m_mincos){
        happy = false;
        break; // found at least one PV not satisfying one of the criteria
      }      
    }
    //================================================================
    
  } // loop over PVs
  
  //================================================================
  if(m_CutBestPV && BestPV){

    // Calculate the distance between the two vertices
    Hep3Vector dist = secv->position()-BestPV->position();
    // momentum of the particle
    Hep3Vector p(part->momentum().vect());

    if(p.mag()==0||dist.mag()==0){
      return (false);
    }

    double cosangle = p.dot(dist)/p.mag()/dist.mag();
    debug() << "Cosine of the angle between momentum and flight distance = " 
        << cosangle << endreq;

    if(m_mincos>-2. && cosangle<m_mincos){
      happy = false;
    }
  }
  //================================================================  

  debug() << "Happy: " << happy << endreq ;
  
  return (happy);
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool Momentum2FlightAngleFilterCriterion
::operator()( const Particle* const & part ) {
  return this->isSatisfied( part );
}

//=============================================================================
