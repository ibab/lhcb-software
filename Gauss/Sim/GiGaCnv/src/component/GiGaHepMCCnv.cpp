// $Id: GiGaHepMCCnv.cpp,v 1.8 2002-12-07 14:36:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#define GIGACNV_GIGAHEPMCCNV_CPP 1 
// ============================================================================
/// STL 
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
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
#include "GiGa/GiGaUtil.h"
/// HepMC
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/GenParticle.h"
/// GenEvent
#include "Event/HepMCEvent.h"
/// Geant4 includes
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
/// Local
#include "GiGaHepMCCnv.h" 

/** @file 
 *  Converter from HepMC to G4
 *  @author W. Pokorski 
 *  @date 15.02.2002
 */

// ============================================================================

static const  CnvFactory<GiGaHepMCCnv>         s_Factory ;
const        ICnvFactory&GiGaHepMCCnvFactory = s_Factory ;

// ============================================================================

GiGaHepMCCnv::GiGaHepMCCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
  , m_leaf( "" , classID() )
{
  ///
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Kine ) ; 
  setConverterName              ( "GiGaHepMCCnv"            ) ;
  ///
}; 

// ============================================================================

GiGaHepMCCnv::~GiGaHepMCCnv(){}; 

// ============================================================================

const CLID&         GiGaHepMCCnv::classID     () 
{
  return HepMCEvents::classID(); 
}

// ============================================================================

const unsigned char GiGaHepMCCnv::storageType ()
{ 
  return GiGaKine_StorageType; 
}

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

StatusCode GiGaHepMCCnv::finalize() 
{
 return GiGaCnvBase::finalize() ; };


// ============================================================================

StatusCode GiGaHepMCCnv::createRep
( DataObject*      object  , 
  IOpaqueAddress*& address ) 
{
  
  ///
  address = 0 ; 
  ///
  if( 0 ==  object   ) { return Error(" DataObject* points to NULL"  ); }

  HepMCEvents* hepVect=dynamic_cast<HepMCEvents*>( object );
  if( 0 ==  hepVect ) { return Error(" DataObject*(of type '"       +
                                        GiGaUtil::ObjTypeName( object) +
                                    "*') is not 'HepMCEvents*'!") ; }
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

StatusCode GiGaHepMCCnv::updateRep
( DataObject*      object  , 
  IOpaqueAddress*  address ) 
{
  
  ///
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL" ) ; } 
  if( 0 ==  object   ) { return Error(" DataObject*     points to NULL" ) ; } 
  ///

  HepMCEvents* hepVect = dynamic_cast<HepMCEvents*>( object ) ;
  if( 0 ==  hepVect ) { return Error("DataObject*(of type '"           + 
                                      GiGaUtil::ObjTypeName( object )   + 
                                      "*') is not 'HepMCEvents*'!") ; }

  /// loop over all events 
  HepMCEvents::iterator it;

  for(it=hepVect->begin(); it!=hepVect->end(); it++) {

  /// loop over all vertices in GenEvent 

   for (HepMC::GenEvent::vertex_const_iterator 
          pVertex= (*it)->pGenEvt()->vertices_begin();
        pVertex!= (*it)->pGenEvt()->vertices_end();pVertex++){
      
      // loop over outgoing particles

      for (HepMC::GenVertex::particles_out_const_iterator pParticle = 
           (*pVertex)->particles_out_const_begin();
           (*pVertex)->particles_out_const_end() != pParticle ; ++pParticle )
        {
          // skip particles with status diffrent from 1 or from 888
          if(((*pParticle)->status()==1)||((*pParticle)->status()==888)){
            
            G4PrimaryParticle* Particle=GenPartG4Part(*pParticle);

            G4PrimaryVertex* OrigVertex = 
              new G4PrimaryVertex((*pParticle)->production_vertex()->position().x(),
                                  (*pParticle)->production_vertex()->position().y(),
                                  (*pParticle)->production_vertex()->position().z(),
                                  (*pParticle)->production_vertex()->position().t());


            OrigVertex->SetPrimary ( Particle );

            *gigaSvc() << OrigVertex ;            
          }
        }      

      { MsgStream log( msgSvc(),  name() ) ;
      log << MSG::VERBOSE << "UpdateRep::Add Vertex to GiGa" << endreq; }
   }   
  }

  return StatusCode::SUCCESS;
}; 

// ============================================================================


G4PrimaryParticle* GiGaHepMCCnv::GenPartG4Part(HepMC::GenParticle* particle)
{
  static const std::string 
    ErrMsg1("GiGaCnv::GenPartG4Part: GenParticle* points to NULL!");

  if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }

  G4PrimaryParticle* Particle = 
    new G4PrimaryParticle( particle->pdg_id() , 
                           particle->momentum().px()*GeV ,
                           particle->momentum().py()*GeV ,
                           particle->momentum().pz()*GeV );

            if (particle->end_vertex()) 
              {

                for (HepMC::GenVertex::particles_out_const_iterator 
                       outPart=(particle->end_vertex())->particles_out_const_begin();
                     outPart!=(particle->end_vertex())->particles_out_const_end();
                     ++outPart)
                  {
                    Particle->SetDaughter(GenPartG4Part(*outPart));
                  }
              
              }
  
  return Particle;  
};











































































