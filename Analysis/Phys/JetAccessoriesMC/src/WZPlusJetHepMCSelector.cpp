// $Id: $

#ifndef COMPONENTS_WZPLUSJETHEPMCSELECTOR_H
#define COMPONENTS_WZPLUSJETHEPMCSELECTOR_H 1

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MCInterfaces/IHepMCParticleSelector.h"            // Interface
// ===========================================================================
// LoKiGen
// ============================================================================
#include "LoKi/GenParticleCuts.h"
#include "LoKi/GenExtract.h"
// ============================================================================


/** @class WZPlusJetHepMCSelector WZPlusJetHepMCSelector.h Components/WZPlusJetHepMCSelector.h
 *
 *
 *  @author Victor Coco
 *  @author Albert Bursche
 *  @date   2012-01-11
 */
class WZPlusJetHepMCSelector : public GaudiTool, 
                               virtual public IHepMCParticleSelector
{
public:
  /// Standard constructor
  WZPlusJetHepMCSelector( const std::string& type,
                          const std::string& name,
                          const IInterface* parent);

  virtual ~WZPlusJetHepMCSelector( ); ///< Destructor

  virtual bool accept( const HepMC::GenParticle* aParticle ) const;

};
#endif // COMPONENTS_ZPLUSJETHEPMCSELECTOR_H

//-----------------------------------------------------------------------------
// Implementation file for class : WZPlusJetHepMCSelector
//
// 2009-10-22 : Victor Coco
// 2012-01-11 : Albert Bursche
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( WZPlusJetHepMCSelector )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
WZPlusJetHepMCSelector::WZPlusJetHepMCSelector( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IHepMCParticleSelector>(this);


}
//=============================================================================
// Destructor
//=============================================================================
WZPlusJetHepMCSelector::~WZPlusJetHepMCSelector() {}

//=============================================================================
// the main function that tell you if youy accept the GenParticle or not
//=============================================================================

bool WZPlusJetHepMCSelector::accept(const HepMC::GenParticle* p ) const
{

  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  GCut IsA_Parton = GABSID == 5 || GABSID == 4 ||  GABSID == 3 ||  GABSID == 2 ||   GABSID == 1 || GID == 21;
  GCut IsA_WZ = GABSID == 23;
  if (!IsA_Parton( p ))return false;
  // the particle is a quark
  HepMC::GenVertex * thePV =  p -> production_vertex() ;
  if (thePV == NULL) return false;
  //  if (thePV->parent_event()->signal_process_id()!=30)return false;
  switch (thePV->parent_event()->signal_process_id())
  {
  case 1: //Z or gamma production
  case 2: //W production
  case 15://gZ production
  case 16://gW production
  case 30://qZ production
  case 31://qW production
    break;
  default:
    return false;
  }
  HepMC::GenVertex::particle_iterator p_in ;
  bool hasWZsister = false;

  for(p_in = thePV -> particles_begin( HepMC::parents); p_in != thePV -> particles_end(HepMC::parents); ++p_in){
    if (!IsA_Parton(*p_in))return false;

  }

  for(p_in = thePV -> particles_begin( HepMC::children); p_in != thePV -> particles_end(HepMC::children); ++p_in){
    if (!IsA_WZ(*p_in)) hasWZsister = true;
  }
  if (!hasWZsister)return false;
  HepMC::GenVertex::particle_iterator p_out2 ;
  //for(p_out2 = p-> production_vertex() -> particles_begin( HepMC::children);
  //    p_out2 != (p)-> production_vertex() ->particles_end(HepMC::children); ++p_out2){
  //  always()<<"Daugthers: "<<GABSID(*p_out2)<<endmsg;
  //}

  //     // it have a quark as mother
  //     HepMC::GenVertex::particle_iterator p_out ;
  //     for(p_out = (*p_in)-> production_vertex() -> particles_begin( HepMC::children);
  //         p_out != (*p_in)-> production_vertex() ->particles_end(HepMC::children); ++p_out){
  //       if (IsA_WZ(*p_out)){
  //         HepMC::GenVertex::particle_iterator p_out2 ;
  //         for(p_out2 = p-> production_vertex() -> particles_begin( HepMC::children);
  //             p_out2 != (p)-> production_vertex() ->particles_end(HepMC::children); ++p_out2){
  //           always()<<"Daugthers: "<<GABSID(*p_out2)<<endmsg;

  //         }

  //         return true;
  //       }

  //     }
  //   }
  return true;
}
