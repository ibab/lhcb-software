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
#include "Event/GenCollision.h"
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
  SmartDataPtr<HardInfos>   hards(evtSvc(),HardInfoLocation::Default);
  
  if (!hevnts)
    {
      return Error("Cannot find HepMCEvents!");
    }
  else
  {
    
    if (!hards) {
      return Error("Cannot find HardInfos!");
    }
    else
    {
      {
      for(HepMCEvents::iterator hiter=hevnts->begin();
          hevnts->end()!=hiter;hiter++)
        {
          HepMC::GenEvent* evt = (*hiter)->pGenEvt();
          
          // instanciate new collision
          GenCollision* collision=new GenCollision;  
          
          collision->setProcessType(evt->signal_process_id());
          
          if(evt->event_scale()>0.0)
            {
              collision->setIsSignal(true);
            }
          else
            {
              collision->setIsSignal(false);
            }
          // start looping over particles to find one with 
          // status 1 or 888 to get the coordinates of the production
          // vertex
          
          HepPoint3D primvtx;

          HepMC::GenEvent::particle_iterator pParticle=evt->particles_begin();
          bool test=false;
          
          do
            {  
              if(((*pParticle)->status()==1)||((*pParticle)->status()==888))
                {
                  primvtx=((*pParticle)->production_vertex()
                           ->position()).v();

                  test=true;
                }
              
              ++pParticle;
            }         
          while(evt->particles_end()!=pParticle && !test);
              
          collision->setPrimVtxPosition(primvtx);
          collisions->insert(collision);

          // now loop on the HardInfo to find corresponding HepMC event
          {
            for( HardInfos::iterator itHard = hards->begin();
               hards->end() != itHard; ++itHard ) {
              if( (*itHard)->event() == (*hiter) ) {
                collision->setHardInfo(*itHard);
              }
            }
          }
        }
      }
    }
  }
  
  //
  return StatusCode::SUCCESS;
};

