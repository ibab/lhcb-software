// $Id: GiGaHepMCCnv.cpp,v 1.13 2003-11-25 14:01:29 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.12  2003/10/31 12:40:05  witoldp
// fixed units in GiGaHepMCCnv
//
// Revision 1.11  2003/07/11 17:42:59  witoldp
// added collision converter
//
// Revision 1.10  2003/03/05 15:19:19  ranjard
// v11r2 - fixes for Win32
//
// Revision 1.9  2003/01/30 11:10:36  witoldp
// fixes to GiGaHepMCCnv
//
// Revision 1.8  2002/12/07 14:36:26  ibelyaev
//  see $GIGACNVROOT/doc/release.notes
// 
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
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"

/// Local
#include "GiGaHepMCCnv.h" 

class comp_bar
{
public:
  bool operator() (HepMC::GenParticle* parta, HepMC::GenParticle* partb)
  {
    return parta->barcode() < partb->barcode();
  }
};

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
  
  std::vector<HepMC::GenParticle*> outpart;
  outpart.clear();
  
  for(it=hepVect->begin(); it!=hepVect->end(); it++) 
    {
      /// loop over all vertices in GenEvent
      for (HepMC::GenEvent::vertex_const_iterator 
             pVertex= (*it)->pGenEvt()->vertices_begin();
           pVertex!= (*it)->pGenEvt()->vertices_end();pVertex++)
        {
          // loop over outgoing particles, check for the ones with 
          // status 1 of 888 and store them in the temporary vector
          
          for (HepMC::GenVertex::particles_out_const_iterator pParticle = 
                 (*pVertex)->particles_out_const_begin();
               (*pVertex)->particles_out_const_end() != pParticle ; ++pParticle)
            {              
              // skip particles with status diffrent from 1 or from 888

              if(((*pParticle)->status()==1)||
                 ((*pParticle)->status()==888)||((*pParticle)->status()==889))
                {
                  outpart.push_back(*pParticle);
                }
            }
        }
      
      // sort the vector, so we always put them in the same order into G4
      std::sort(outpart.begin(), outpart.end(), comp_bar());
      
      // here I am assuming that all the particles with status 1, 888 and 889, i.e. 
      // all particles produced by Pythia are coming from the same physical vertex
      // if that was not the case, one would need to implement a more sophisticated 
      // machinery to assign particles to different vertices on a case by case basis      
      
      G4PrimaryVertex* OrigVertex = 
        new G4PrimaryVertex
        ((*(outpart.begin()))->production_vertex()->position().x(),
         (*(outpart.begin()))->production_vertex()->position().y(),
         (*(outpart.begin()))->production_vertex()->position().z(),
         ((*(outpart.begin()))->production_vertex()->position().t())/c_light);
      
      for(std::vector<HepMC::GenParticle*>::iterator ioutpart=outpart.begin();
          outpart.end()!=ioutpart;ioutpart++)
        {
          G4PrimaryParticle* Particle=GenPartG4Part(*ioutpart);
          
          OrigVertex->SetPrimary ( Particle );
        }
      
      *gigaSvc() << OrigVertex ;
      
      outpart.clear();
    }  
  
  return StatusCode::SUCCESS;
}; 

// ============================================================================


G4PrimaryParticle* GiGaHepMCCnv::GenPartG4Part(HepMC::GenParticle* particle)
{
  static const std::string 
    ErrMsg1("GiGaCnv::GenPartG4Part: GenParticle* points to NULL!");

  if( 0 == particle ) { throw GiGaException( ErrMsg1 ) ; }

  std::vector<HepMC::GenParticle*> outp;
  
  G4PrimaryParticle*  Particle = 
    new G4PrimaryParticle( particle->pdg_id() , 
                           particle->momentum().px()*GeV ,
                           particle->momentum().py()*GeV ,
                           particle->momentum().pz()*GeV );  
  
  if (particle->end_vertex()) 
    {
      // assign decay time
      HepLorentzVector theLorentzV = (particle->end_vertex()->position()
                        - particle->production_vertex()->position());
      
      Hep3Vector theBoost = particle->momentum().boostVector() ;
      
      Particle -> 
        SetProperTime((theLorentzV.boost(-theBoost)).t()/c_light);
      
      //       std::cout << "assigning time " 
      //                 << (particle->end_vertex()->position().t())/c_light
      //                 << " to " <<  particle->pdg_id() << std::endl;
      
                        
      // if particle has daughters, carry on with the conversion
      for (HepMC::GenVertex::particles_out_const_iterator outPart = 
             (particle->end_vertex())->particles_out_const_begin();
           (particle->end_vertex())->particles_out_const_end() != outPart ; 
           ++outPart )
        {
                outp.push_back(*outPart);
        }

        // sort the vector, so we always put them in the same order into G4
        std::sort(outp.begin(), outp.end(), comp_bar());
        
        for(std::vector<HepMC::GenParticle*>::iterator ioutpart=outp.begin();
            outp.end()!=ioutpart;ioutpart++)
          { 
            Particle->SetDaughter(GenPartG4Part(*ioutpart));
          }
    }
  
  return Particle;  
};











































































