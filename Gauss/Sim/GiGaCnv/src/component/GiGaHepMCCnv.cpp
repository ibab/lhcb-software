// 
//  Converter from HepMC to G4
//
//  initial coding by W. Pokorski 15.02.2002 (based on GiGaMCVertexCnv by I. Belyaev)
//
// 
//  ===========================================================================
#define GIGACNV_GIGAHEPMCCNV_CPP 1 
// ============================================================================
/// STL 
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
/// GaudiKernel
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
/// GiGa
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/GiGaException.h" 
#include "GiGa/GiGaTrajectoryPoint.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
/// GiGaCnv
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGaCnv/GiGaCnvUtils.h"
/// HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
/// GeneratorObjects
#include "GeneratorObjects/McEventCollection.h"

/// Geant4 includes
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4TrajectoryContainer.hh"
/// Local
#include "GiGaCnvFunctors.h"
#include "GenPart2GenPart.h"
///
#include "GiGaHepMCCnv.h" 

// ============================================================================
/// factories 
// ============================================================================
static const  CnvFactory<GiGaHepMCCnv>         s_Factory ;
const        ICnvFactory&GiGaHepMCCnvFactory = s_Factory ;

// ============================================================================
/** standard constructor
 *  @param Loc pointer to service locator 
 */ 
// ============================================================================
GiGaHepMCCnv::GiGaHepMCCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf( "" , classID() )
{

  ///
  setNameOfGiGaConversionService( "GiGaKineCnvSvc"  ); 
  setConverterName              ( "GiGaHepMCCnv"      );
  ///
}; 

// ============================================================================
// destructor 
// ============================================================================
GiGaHepMCCnv::~GiGaHepMCCnv(){}; 

// ============================================================================
// Class ID 
// ============================================================================
const CLID&         GiGaHepMCCnv::classID     () 
{
  return McEventCollection::classID();
}

// ============================================================================
// StorageType 
// ============================================================================
const unsigned char GiGaHepMCCnv::storageType ()
{ 
  return GiGaKine_StorageType; 
}

// ============================================================================
/** initialize the converter
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaHepMCCnv::initialize()
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
StatusCode GiGaHepMCCnv::finalize() 
{
 return GiGaCnvBase::finalize() ; };



// ============================================================================
/** create the representation of the object 
 *  @param  object  reference to the object 
 *  @param  address address of the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaHepMCCnv::createRep
( DataObject*      object  , 
  IOpaqueAddress*& address ) 
{
  
  ///
  address = 0 ; 
  ///
  if( 0 ==  object   ) { return Error(" DataObject* points to NULL"  ); }

  McEventCollection* hepColl=dynamic_cast<McEventCollection*>( object );
  if( 0 ==  hepColl ) { return Error(" DataObject*(of type '"       +
                                        GiGaUtil::ObjTypeName( object) +
                                    "*') is not 'McEventCollection*'!") ; }
  /// create IOpaqueAddress
  if( 0 == addressCreator() ) 
    { return Error("CreateRep::AddressCreator is not available"); } 
  StatusCode status = 
    addressCreator()->createAddress( repSvcType  () , 
                                     classID     () , 
                                    m_leaf.par  () , 
                                    m_leaf.ipar () , 
                                    address        );   
  if( status.isFailure() ) 
    { return Error("CreateRep::Error from Address Creator",status) ; }
  if( 0 == address       ) 
    { return Error("CreateRe::Invalid address is created") ; } 

  ///

  return updateRep( object , address ) ; 

  /// 

}; 

// ============================================================================
/** update the representation of the object 
 *  @param  object  reference to the object 
 *  @param  address address of the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaHepMCCnv::updateRep
( DataObject*      object  , 
  IOpaqueAddress*  address ) 
{
  
  ///
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL" ) ; } 
  if( 0 ==  object   ) { return Error(" DataObject*     points to NULL" ) ; } 
  ///

  McEventCollection* hepColl = dynamic_cast<McEventCollection*>( object ) ;
  if( 0 ==  hepColl ) { return Error("DataObject*(of type '"           + 
                                      GiGaUtil::ObjTypeName( object )   + 
                                      "*') is not 'McEventCollection*'!") ; }  

  //create a particle converter

  GenPart2GenPart Cnv( ppSvc() );

  /// loop over all events in McEventCollection
  McEventCollection::iterator it;


  int nVertex=0;
  int nOutvtx=0;
  int nPart=0;


  for(it=hepColl->begin(); it!=hepColl->end(); it++) {
    

  // create a primary vertex at (0,0,0,0)

    G4PrimaryVertex* OrigVertex = 
      new G4PrimaryVertex(0,0,0,0);
    G4PrimaryVertex* Vertex;    

  /// loop over all vertices in GenEvent 

   for (HepMC::GenEvent::vertex_const_iterator pVertex= (*it)->pGenEvt()->vertices_begin();
         pVertex!= (*it)->pGenEvt()->vertices_end();pVertex++){

     nVertex++;
     
     bool outvert=false;
     bool origvtx=((*pVertex)->position().x()==0 &&
                 (*pVertex)->position().y()==0 &&
                 (*pVertex)->position().z()==0 &&
                 (*pVertex)->position().t()==0);
      
      // loop over outgoing particles and find the ones without end vertices

      for (HepMC::GenVertex::particles_out_const_iterator pParticle = 
           (*pVertex)->particles_out_const_begin();
           (*pVertex)->particles_out_const_end() != pParticle ; ++pParticle )
        {
          // skip particles with end vertices
          if (!((*pParticle)->end_vertex())) {          

            nPart++;
            
          /// if a particle is without end vertex then perform the conversion
            
          G4PrimaryParticle* Particle = Cnv( *pParticle );

          // if this not yet done, create PrimaryVertex to which it will belong
   
            if(!outvert && !origvtx){
              outvert=true;
              nOutvtx++;              
              Vertex = 
                new G4PrimaryVertex( (*pVertex)->position().x()*mm ,
                                     (*pVertex)->position().y()*mm ,
                                     (*pVertex)->position().z()*mm ,
                                     (*pVertex)->position().t()*mm ) ;
            }

            if(!origvtx){            
              Vertex->SetPrimary( Particle );           
            }
            else{
              
              OrigVertex->SetPrimary ( Particle );
            }
            
          }
          
        }
            
      if (outvert && !origvtx){        
        *gigaSvc() << Vertex ;
      }
      

      { MsgStream log( msgSvc(),  name() ) ;
      log << MSG::VERBOSE << "UpdateRep::Add Vertex to GiGa" << endreq; }
   }
   
   *gigaSvc() << OrigVertex ;
   
  }
  { MsgStream log( msgSvc(),  name() ) ;
       log << MSG::INFO << 
         " Number of vertices examined "<< nVertex << endreq;
       log << MSG::INFO <<
         " Number of vertices converted (- origin) "<< nOutvtx << endreq;
       log << MSG::INFO << 
         " Number of particles converted "<< nPart << endreq;
  }
  

  return StatusCode::SUCCESS; 
  ///
}; 

// ============================================================================











































































