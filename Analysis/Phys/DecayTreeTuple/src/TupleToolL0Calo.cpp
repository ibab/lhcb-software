// $Id: TupleToolL0Calo.cpp, v1 2012-10-11$
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolL0Calo.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Kernel/IParticleTupleTool.h"
#include "Event/Particle.h"
#include "CaloInterfaces/IPart2Calo.h"

#include "TMath.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloTupleTool
//
// 2012-10-11 : Alexandra Martin Sanchez
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolL0Calo )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolL0Calo::TupleToolL0Calo( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : TupleToolBase ( type, name , parent )
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("WhichCalo", m_calo);
}

//=============================================================================

StatusCode TupleToolL0Calo::initialize( )
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  m_part2calo = tool<IPart2Calo>("Part2Calo","Part2Calo",this);
  return sc;
}

//=============================================================================
StatusCode TupleToolL0Calo::fill( const LHCb::Particle* mother,
                                  const LHCb::Particle* P,
                                  const std::string& head,
                                  Tuples::Tuple& tuple )
{

  bool test = true;
  if( !(P->isBasicParticle()) ) return StatusCode(test);

  const std::string prefix = fullName(head);

  double trackET, xProjection, yProjection;

  // Project the track on the selected calo
  if (m_calo == "HCAL"){
    m_part2calo->match( P, "/dd/Structure/LHCb/DownstreamRegion/Hcal" ); }
  else if (m_calo == "ECAL") {
    m_part2calo->match( P, "/dd/Structure/LHCb/DownstreamRegion/Ecal" ); }
  else {
    m_calo = "HCAL";
    Info("No calorimeter has been set. Projecting particle on HCAL by default.");
    m_part2calo->match( P, "/dd/Structure/LHCb/DownstreamRegion/Hcal" );
  }
  

  // Calculate a few observables
  trackET = TMath::Sqrt(TMath::Power(m_part2calo->caloState().x(),2)+TMath::Power(m_part2calo->caloState().y(),2))/TMath::Sqrt(TMath::Power(m_part2calo->caloState().x(),2)+TMath::Power(m_part2calo->caloState().y(),2)+TMath::Power(m_part2calo->caloState().z(),2))*TMath::Sqrt(TMath::Power(m_part2calo->caloState().p(),2)+TMath::Power(P->measuredMass(),2));

  xProjection = m_part2calo->caloState().x();
  yProjection = m_part2calo->caloState().y();


  // Fill the tuple

  if (m_calo == "HCAL") {
    test &= tuple->column( prefix+"_L0Calo_HCAL_trueET", trackET );
    test &= tuple->column( prefix+"_L0Calo_HCAL_xProjection", xProjection );
    test &= tuple->column( prefix+"_L0Calo_HCAL_yProjection", yProjection );
    test &= tuple->column( prefix+"_L0Calo_HCAL_region", isinside_HCAL( xProjection , yProjection ) );
  }
  else if (m_calo == "ECAL") {
    test &= tuple->column( prefix+"_L0Calo_ECAL_trueET", trackET );
    test &= tuple->column( prefix+"_L0Calo_ECAL_xProjection", xProjection );
    test &= tuple->column( prefix+"_L0Calo_ECAL_yProjection", yProjection );
    test &= tuple->column( prefix+"_L0Calo_ECAL_region", isinside_ECAL( xProjection , yProjection ) );
  }
  

  if( msgLevel( MSG::VERBOSE ) ) verbose() << "Returns " << test << endreq;
  return StatusCode(test);

}


int TupleToolL0Calo::isinside_HCAL(double x, 
                                   double y) 
{
  bool inside, inner, outer;
  inside = true;
  inner = false;
  outer = false;

  if (TMath::Abs(x) < HCAL_xMax_Outer && TMath::Abs(y) < HCAL_yMax_Outer) { // projection inside calo
    if (TMath::Abs(x) < HCAL_xMax_Inner && TMath::Abs(y) < HCAL_yMax_Inner) { // projection inside inner calo (else is outer calo)
      if (TMath::Abs(x) > 2*HCAL_CellSize_Inner) // projections outside the beampipe (in x)
        inner = true;
      else if (TMath::Abs(y) > 2*HCAL_CellSize_Inner) // projections outside the beampipe (not in x, so in y)
        inner = true;
      else
        inside = false;
    }
    else
      outer = true;
  }      
  else
    inside = false;
  
  if (!inside)
    return -1;
  else if (inner)
    return 1;
  else if (outer)
    return 0;
  else
    return -999;

}

int TupleToolL0Calo::isinside_ECAL(double x, 
                                   double y) 
{
  bool inside, inner, middle, outer;
  inside = true;
  inner = false;
  middle = false;
  outer = false;

  if (TMath::Abs(x) < ECAL_xMax_Outer && TMath::Abs(y) < ECAL_yMax_Outer) { // projection inside calo
    if (TMath::Abs(x) < ECAL_xMax_Middle && TMath::Abs(y) < ECAL_yMax_Middle) { // projection inside middle calo (else is outer calo)
      if (TMath::Abs(x) < ECAL_xMax_Inner && TMath::Abs(y) < ECAL_yMax_Inner) { // projection inside inner calo 
        if (TMath::Abs(x) > 2*ECAL_CellSize_Inner) // projections outside the beampipe (in x)
          inner = true;
        else if (TMath::Abs(y) > 2*ECAL_CellSize_Inner) // projections outside the beampipe (not in x, so in y)
          inner = true;
        else
          inside = false;
      }
      else
        middle = true;
    }      
    else
      outer = true;
  }
  else
    inside = false;
  
  if (!inside)
    return -1;
  else if (inner)
    return 2;
  else if (middle)
    return 1;
  else if (outer)
    return 0;
  else
    return -999;

}
