//  ===========================================================================
#define GIGACNV_COLLISIONCNV_CPP 1 
// ============================================================================
// GaudiKernel
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"
// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
// GiGa
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/GiGaException.h" 
#include "GiGa/GiGaUtil.h"
// LHCbEvent 
#include "Event/MCParticle.h" 
#include "Event/MCVertex.h" 
#include "Event/HepMCEvent.h"
// Local
//
#include "GiGaCollisionCnv.h" 

// ============================================================================
/// factories 
// ============================================================================
static const  CnvFactory<GiGaCollisionCnv>         s_Factory ;
const        ICnvFactory&GiGaCollisionCnvFactory = s_Factory ;

// ============================================================================
/** standard constructor
 *  @param Loc pointer to service locator 
 */ 
// ============================================================================
GiGaCollisionCnv::GiGaCollisionCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf( "" , classID() )
{
  //
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Kine ) ; 
  setConverterName              ( "GiGaCollCnv"              ) ;
  //
  declareObject( GiGaLeaf( CollisionLocation::Default , objType() ) ); 
  //
}; 

// ============================================================================
// destructor 
// ============================================================================
GiGaCollisionCnv::~GiGaCollisionCnv()
{
}; 

// ============================================================================
// Class ID 
// ============================================================================
const CLID&         GiGaCollisionCnv::classID     () 
{ return Collision::classID(); }

// ============================================================================
// StorageType 
// ============================================================================
const unsigned char GiGaCollisionCnv::storageType ()
 { return GiGaKine_StorageType; }

// ============================================================================
/** initialize the converter
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCollisionCnv::initialize()
{
  StatusCode sc = GiGaCnvBase::initialize();
  if( sc.isFailure() ) { return Error("Could not initialize base class!",sc); }
  ///
  if( 0 == gigaSvc() ) { return Error("IGiGaSvc* points to NULL!"       ) ; }
  if( 0 == cnvSvc () ) { return Error("IConversionSvc* points to NULL!" ) ; }  
  if( 0 == kineSvc() ) { return Error("IGiGaKineSnvSvc* points to NULL!") ; }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalize  the converter
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCollisionCnv::finalize() 
{ return GiGaCnvBase::finalize() ; };

// ============================================================================
/** create the Object
 *  @param  address address of the object 
 *  @param  object  reference to created object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaCollisionCnv::createObj
( IOpaqueAddress*  address , 
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new Collisions();

  StatusCode sc = updateObj( address , object );
  if( sc.isFailure() ) 
    { 
      if( 0 != object ) { delete object ; object =  0 ; }
      return Error(" Could not create the object" , sc );
    }; 
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** update the Object
 *  @param  address address of the object 
 *  @param  object  reference to updated object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaCollisionCnv::updateObj
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{
  ///
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL" ) ; }
  if( 0 ==  object   ) { return Error(" DataObject* points to NULL"     ) ; }   
  Collisions* collisions = dynamic_cast<Collisions*> ( object ); 
  if( 0 ==  collisions ) { return Error(" DataObject*(of type '"          + 
                                      GiGaUtil::ObjTypeName( object )   + 
                                      "*') is not 'Collisions*'!"         ) ; }  
  // clear the container before update 
  collisions->clear(); 
  //
  SmartDataPtr<HepMCEvents> hevnts(evtSvc(),"/Event/Gen/HepMCEvents");
  SmartDataPtr<MCParticles> mcparts(evtSvc(),MCParticleLocation::Default);
  SmartDataPtr<MCVertices> mcvts(evtSvc(), MCVertexLocation::Default);
  
  
  if (!hevnts)
    {
      return Error("Cannot find HepMCEvents!");
    }
  else
    {
      for(HepMCEvents::iterator hiter=hevnts->begin();
          hevnts->end()!=hiter;hiter++)
        {
          HepMC::GenEvent* evt = (*hiter)->pGenEvt();
          
          // instanciate new collision
          Collision* collision=new Collision;  

          collision->setProcessType(evt->signal_process_id());
          
          if(evt->event_scale()>0.0)
            {
              collision->setIsSignal(true);
            }
          else
            {
              collision->setIsSignal(false);
            }
          
          for (HepMC::GenEvent::vertex_const_iterator 
                 pVertex= evt->vertices_begin();
               pVertex!= evt->vertices_end();pVertex++)
            {
              // start looping over outgoing particles to find one with 
              // status 1 or 888 to get the coordinates of the production
              // vertex

              HepPoint3D primvtx;

              for (HepMC::GenVertex::particles_out_const_iterator pParticle = 
                     (*pVertex)->particles_out_const_begin();
                   (*pVertex)->particles_out_const_end()!=pParticle;++pParticle)
                {                  
                  if(((*pParticle)->status()==1)||((*pParticle)->status()==888))
                    {
                      primvtx=((*pParticle)->production_vertex()
                               ->position()).v();
                      
                      continue;
                    } 
                  collision->setPrimVtxPosition(primvtx);
                }
              for(MCVertices::iterator viter=mcvts->begin();mcvts->end()!=viter;
                  viter++)
                {

                  cout << (*viter)->position() << "   " << primvtx << endl;
                  
                  // find the MCVertex (primary) corresponding to this collision
                  if((*viter)->position()==primvtx)
                    {
                      // set the pointer to the collision in the vertex and 
                      // in all the daughters
                      PointToCollision(*viter, collision);
                    }
                }
            }              
          collisions->insert(collision);
        }
    }
  //
  return StatusCode::SUCCESS;
};
  
StatusCode GiGaCollisionCnv::PointToCollision(MCVertex* vtx,Collision* collision)
{
  //
  vtx->setCollision(collision);
  typedef SmartRefVector<MCParticle>::iterator itp;
  typedef SmartRefVector<MCVertex>::iterator itv;

  // propagate it to all the daughters (and vertices)
  for(itp dauit=(vtx->products()).begin();
      (vtx->products()).end()!=dauit; dauit++)
    {
      (*dauit)->setCollision(collision);
      
      for(itv endvtx=((*dauit)->endVertices()).begin();
          ((*dauit)->endVertices()).end()!=endvtx;endvtx++)
        {
          PointToCollision(*endvtx, collision);
        }
    }
  return StatusCode::SUCCESS;
};
