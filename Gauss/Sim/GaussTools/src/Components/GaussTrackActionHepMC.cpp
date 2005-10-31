/// STD & STL 
#include <functional>
#include <algorithm>
// G4
#include "G4TrackingManager.hh"
/// GaudiKernel
#include "GaudiKernel/PropertyMgr.h"
/// GiGa 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaUtil.h"
#include "GiGa/GiGaMACROs.h"
/// local 
#include "GaussTrackActionHepMC.h"
#include "GaussTools/GaussTrackInformation.h"

// ============================================================================
/** @file 
 *  
 *  Implementation file for class : GaussTrackActionHepMC
 *  @date 24/07/2001 
 *  @author Witek Pokorski
 */
// ============================================================================

// ============================================================================
/// Factory business 
// ============================================================================
IMPLEMENT_GiGaFactory( GaussTrackActionHepMC ) ;

// ============================================================================
/** standard constructor 
 *  @see GiGaTrackActionBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GaussTrackActionHepMC::GaussTrackActionHepMC
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : GiGaTrackActionBase( type , name , parent  ) 
{
  mcmgr = MCTruthManager::GetInstance();
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GaussTrackActionHepMC::~GaussTrackActionHepMC() {};
// ============================================================================

// ============================================================================
/** initialise the action object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GaussTrackActionHepMC::initialize () 
{
  StatusCode sc = GiGaTrackActionBase::initialize();
  if( sc.isFailure() ) 
  { return Error("Could not initialize the base class!", sc ); }
  //
  return Print("Iinitialized successfully" , 
               StatusCode::SUCCESS         , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** perform the pre-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GaussTrackActionHepMC::PreUserTrackingAction  ( const G4Track* track )
{
  // new track is being started
  // we record its initial momentum
  fourmomentum = HepLorentzVector(track->GetTotalEnergy(), track->GetMomentum());
};
// ============================================================================

// ============================================================================
/** perform the post-action
 *  @param track pointer to Geant4 track object 
 */ 
// ============================================================================
void GaussTrackActionHepMC::PostUserTrackingAction  ( const G4Track* track )
{
  //
  HepMC::GenEvent* genevt = mcmgr->GetCurrentEvent();
  
  // if track is to be stored, create new GenParticle, and its decay vertex
  // we check the flag (store or not store) in the GaussTrackInformation

  //  G4VUserTrackInformation* uinf = track->GetUserInformation(); 
  GaussTrackInformation* ginf = (GaussTrackInformation*) track->GetUserInformation();

  //  if(trackMgr()->GetStoreTrajectory())
  if(ginf->storeHepMC())
  {
    // we create the corresponding GenParticle
    HepMC::GenParticle* particle = 
      new HepMC::GenParticle(fourmomentum, track->GetDefinition()->GetPDGEncoding());
    
    int trid = track->GetTrackID();
    // we use the track ID to set the barcode
    particle->suggest_barcode(trid);
    
    // we create the GenVertex corresponding to the end point of the track
    HepMC::GenVertex* endvertex = 
      new HepMC::GenVertex(HepLorentzVector(track->GetGlobalTime(), track->GetPosition()));
    
    // barcode of the endvertex = - barcode of the track
    endvertex->suggest_barcode(-trid);
    endvertex->add_particle_in(particle);
    genevt->add_vertex(endvertex);

    int parentid = track->GetParentID();
    HepLorentzVector prodpos(track->GetGlobalTime() - track->GetLocalTime(),
                             track->GetVertexPosition());

    if(parentid) // not primary
    {
      // here we could try to improve speed by searching only through particles which 
      // belong to the given primary tree
      HepMC::GenParticle* mother = genevt->barcode_to_particle(track->GetParentID());
      //
      if(mother)
      {
        // we first check whether the mother's end vertex corresponds to the particle's
        // production vertex
        HepMC::GenVertex* motherendvtx = mother->end_vertex();
        HepLorentzVector motherendpos = motherendvtx->position();

        if( motherendpos.x() == prodpos.x() &&
            motherendpos.y() == prodpos.y() &&
            motherendpos.z() == prodpos.z() ) // if yes, we attach the particle
        {
          motherendvtx->add_particle_out(particle);
        }
        else // if not, we check whether the mother is biological or adopted
        {            
          if(ginf->noDirectParent()) // adopted
          {  
            bool found = false;
            // first check if any of the dummy particles has the end vertex at the right place
            for(HepMC::GenVertex::particles_out_const_iterator it=motherendvtx->particles_out_const_begin();
                it!=motherendvtx->particles_out_const_end(); it++)
            {
              if((*it)->pdg_id()==-999999)
              {
                HepLorentzVector dummypos = (*it)->end_vertex()->position();

                if( dummypos.x() == prodpos.x() &&
                    dummypos.y() == prodpos.y() &&
                    dummypos.z() == prodpos.z() ) 
                {
                  (*it)->end_vertex()->add_particle_out(particle);
                  found = true;
                  break;
                }
              }
            }
            // and if not, create a dummy particle connecting to the end vertex of the mother
            if(!found)
            {
              HepMC::GenVertex* childvtx = new HepMC::GenVertex(prodpos);
              childvtx->add_particle_out(particle);
              // the dummy vertex gets the barcode -20000000 minus the daughter particle barcode
              childvtx->suggest_barcode(-20000000-trid);
              genevt->add_vertex(childvtx);
              
              HepMC::GenParticle* dummypart = new HepMC::GenParticle(HepLorentzVector(),-999999);
              // the dummy particle gets the barcode 20000000 plus the daughter particle barcode
              dummypart->suggest_barcode(20000000+trid);
              childvtx->add_particle_in(dummypart);
              motherendvtx->add_particle_out(dummypart);
            }
          }
          else // biological
          {
            // we 'split' the mother into two particles and create a new vertex
            HepMC::GenVertex* childvtx = new HepMC::GenVertex(prodpos);
            childvtx->add_particle_out(particle);
            genevt->add_vertex(childvtx);
            // we first detach the mother from its original vertex 
            motherendvtx->remove_particle(mother);
            // and attach it to the new vertex
            childvtx->add_particle_in(mother);
            // now we create a new particle representing the mother after interaction
            // the barcode of the new particle is 100000000 + the original barcode 
            HepMC::GenParticle* mothertwo = new HepMC::GenParticle(*mother);
            mothertwo->suggest_barcode(100000000 + mother->barcode());
            // we also reset the barcodes of the vertices
            motherendvtx->suggest_barcode(-100000000 - mother->barcode());
            childvtx->suggest_barcode(- mother->barcode());
            // we attach it to the new vertex where interaction took place
            childvtx->add_particle_out(mothertwo);
            // and we attach it to the original endvertex
            motherendvtx->add_particle_in(mothertwo);
          }
        }
      }
      else 
        // mother GenParticle is not there for some reason...
        // if this happens, we need to revised the philosophy... 
        // a solution would be to create HepMC particles at the begining of each track
      {
        std::cout << "mother not there "<< track->GetDefinition()->GetParticleName() << std::endl;
        std::cout << "barcode " << track->GetParentID() << std::endl;
      }
    }
    else // primary
    {
      // 
      HepMC::GenVertex* primaryvtx = new HepMC::GenVertex(prodpos);

      primaryvtx->add_particle_out(particle);
      genevt->add_vertex(primaryvtx);
      // add id to the list of primaries
      mcmgr->GetPrimaryBarcodes().push_back(trid);
    }    
  }
};
// ============================================================================

