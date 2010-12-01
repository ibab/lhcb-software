// $Id: TupleToolNeutrinoReco.cpp,v 1.6 2010-04-12 12:34:58 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include <Kernel/DVAlgorithm.h>
#include <Kernel/GetDVAlgorithm.h>

// local
#include "TupleToolNeutrinoReco.h"

#include "Event/Particle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolNeutrinoReco
//
// 2010-11-24 : Rob Lambert
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolNeutrinoReco );
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolNeutrinoReco::TupleToolNeutrinoReco( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : TupleToolBase ( type, name , parent )
  , m_dva(NULL)
{
  declareInterface<IParticleTupleTool>(this);


}

//=============================================================================
StatusCode TupleToolNeutrinoReco::initialize()
{
  if( ! TupleToolBase::initialize() ) return StatusCode::FAILURE;

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  
  if (NULL==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);  
  
  return StatusCode::SUCCESS;

}


//=============================================================================

StatusCode TupleToolNeutrinoReco::fill( const LHCb::Particle*
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple )
{
  const std::string prefix=fullName(head);
  
  bool test = true;
  if( P )
  {
    const LHCb::VertexBase* aPV = m_dva->bestPV ( P );
    // Parallel and Perpendicular Momentum of the D_s\mu combination
    // pa for Parallel and pe for Perpendicular
    double Nu_Parl = MomentumParallel(aPV,P,&P->momentum());
    double Nu_Perp = MomentumPerpendicular(aPV,P,&P->momentum());
    
    double a=4*(Nu_Perp*Nu_Perp+P->measuredMass()*P->measuredMass());
    double X=4*Nu_Parl*Nu_Parl*(2*Nu_Perp*Nu_Perp+P->measuredMass()*P->measuredMass());
    double Y=8*Nu_Perp*Nu_Perp*Nu_Parl*Nu_Parl;
    
    // momentum components
    test &= tuple->column( prefix+"_Nu_a", a );
    test &= tuple->column( prefix+"_Nu_X", X );
    test &= tuple->column( prefix+"_Nu_Y", Y );
    test &= tuple->column( prefix+"_Nu_Parl", Nu_Parl );
    test &= tuple->column( prefix+"_Nu_Perp", Nu_Perp );
    
  }
  else 
  {
    return StatusCode::FAILURE;
  }
  return StatusCode(test);
}
double TupleToolNeutrinoReco::MomentumParallel(const LHCb::VertexBase* pv, 
                                               const LHCb::Particle* b, 
                                               const ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >* mv) 
{
  
  // fd means flight distance
  double fd_x = b->endVertex()->position().X()-pv->position().X();
  double fd_y = b->endVertex()->position().Y()-pv->position().Y();
  double fd_z = b->endVertex()->position().Z()-pv->position().Z();
  
  double fd = sqrt(fd_x*fd_x + fd_y*fd_y + fd_z*fd_z);
  
  fd_x=fd_x/fd;
  fd_y=fd_y/fd;
  fd_z=fd_z/fd;
  
  // return the dot product of the flight distance unit vector
  // with the momentum
  return mv->Px()*fd_x + mv->Py()*fd_y + mv->Pz()*fd_z;
}

double TupleToolNeutrinoReco::MomentumPerpendicular(const LHCb::VertexBase* pv, 
                                                    const LHCb::Particle* b, 
                                                    const ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >* mv) 
{
   double fd_x = b->endVertex()->position().X()-pv->position().X();
   double fd_y = b->endVertex()->position().Y()-pv->position().Y();
   double fd_z = b->endVertex()->position().Z()-pv->position().Z();

   double fd = sqrt(fd_x*fd_x + fd_y*fd_y + fd_z*fd_z);

   fd_x=fd_x/fd;
   fd_y=fd_y/fd;
   fd_z=fd_z/fd;

   double P_Pa = mv->Px()*fd_x + mv->Py()*fd_y + mv->Pz()*fd_z;

   double P_Pa_x = fd_x*P_Pa;
   double P_Pa_y = fd_y*P_Pa;
   double P_Pa_z = fd_z*P_Pa;

   double P_Pe_x = mv->Px()-P_Pa_x;
   double P_Pe_y = mv->Py()-P_Pa_y;
   double P_Pe_z = mv->Pz()-P_Pa_z;

   return sqrt(P_Pe_x*P_Pe_x + P_Pe_y*P_Pe_y + P_Pe_z*P_Pe_z);
}

