// $Id: $

#ifndef COMPONENTS_ZPLUSJETHEPMCSELECTOR_H
#define COMPONENTS_ZPLUSJETHEPMCSELECTOR_H 1

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


/** @class ZPlusJetHepMCSelector ZPlusJetHepMCSelector.h Components/ZPlusJetHepMCSelector.h
 *
 *
 *  @author Victor Coco
 *  @date   2009-10-22
 */
class ZPlusJetHepMCSelector : public GaudiTool, virtual public IHepMCParticleSelector {
public:
  /// Standard constructor
  ZPlusJetHepMCSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  virtual ~ZPlusJetHepMCSelector( ); ///< Destructor

  virtual bool accept( const HepMC::GenParticle* aParticle ) const;

};
#endif // COMPONENTS_ZPLUSJETHEPMCSELECTOR_H

//-----------------------------------------------------------------------------
// Implementation file for class : ZPlusJetHepMCSelector
//
// 2009-10-22 : Victor Coco
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ZPlusJetHepMCSelector )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ZPlusJetHepMCSelector::ZPlusJetHepMCSelector( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IHepMCParticleSelector>(this);


}
//=============================================================================
// Destructor
//=============================================================================
ZPlusJetHepMCSelector::~ZPlusJetHepMCSelector() {}

//=============================================================================
// the main function that tell you if youy accept the GenParticle or not
//=============================================================================

bool ZPlusJetHepMCSelector::accept(const HepMC::GenParticle* p ) const
{

  // avoid long names
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  using namespace LoKi::Cuts  ;
  GCut IsA_Q = GABSID == 5 || GABSID == 4 ||  GABSID == 3 ||  GABSID == 2 ||   GABSID == 1;
  GCut IsA_Z = GABSID == 23;
  if (!IsA_Q( p ))return false;
  // the particle is a quark
  HepMC::GenVertex * thePV =  p -> production_vertex() ;
  if (thePV == NULL) return false;
  if (thePV->parent_event()->signal_process_id()!=30)return false;
  HepMC::GenVertex::particle_iterator p_in ;
  bool hasZsister = false;

  for(p_in = thePV -> particles_begin( HepMC::parents); p_in != thePV -> particles_end(HepMC::parents); ++p_in){
    if (!IsA_Q(*p_in))return false;

  }

  for(p_in = thePV -> particles_begin( HepMC::children); p_in != thePV -> particles_end(HepMC::children); ++p_in){
    if (!IsA_Z(*p_in)) hasZsister = true;
  }
  if (!hasZsister)return false;
  HepMC::GenVertex::particle_iterator p_out2 ;
  //for(p_out2 = p-> production_vertex() -> particles_begin( HepMC::children);
  //    p_out2 != (p)-> production_vertex() ->particles_end(HepMC::children); ++p_out2){
  //  always()<<"Daugthers: "<<GABSID(*p_out2)<<endmsg;
  //}

  //     // it have a quark as mother
  //     HepMC::GenVertex::particle_iterator p_out ;
  //     for(p_out = (*p_in)-> production_vertex() -> particles_begin( HepMC::children);
  //         p_out != (*p_in)-> production_vertex() ->particles_end(HepMC::children); ++p_out){
  //       if (IsA_Z(*p_out)){
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
