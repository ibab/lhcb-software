// $Id: GenParticle_Streamer.cpp,v 1.5 2008-10-31 16:40:15 cattanem Exp $
// see TClass::AdoptStreamer(new TClassStreamer(GenParticle_Streamer))

#include "TClassRef.h"

//Break the privacy of classes
#if !defined(private) && !defined(protected)
#define private public
#define protected public
#endif

#include "TBuffer.h"
#include "TVirtualStreamerInfo.h"


#include "HepMC/GenParticle.h"
#include "HepMC/GenEvent.h"

using namespace HepMC;





void GenParticle_Streamer(TBuffer &R__b, void *obj);

//RootStreamer(GenParticle,GenParticle_Streamer)

void GenParticle_Streamer(TBuffer &R__b, void *obj) {
   //read an old version of GenParticle into a new GenParticle
   //old version had:
   // public:
   //   CLHEP::HepLorentzVector m_momentum;    
   //   int                     m_pdg_id;      
   //   int                     m_status;      
   //   HepMC::Flow             m_flow;        
   //   HepMC::Polarization     m_polarization;    
   //   HepMC::GenVertex*       m_production_vertex;    
   //   HepMC::GenVertex*       m_end_vertex;          
   //   int                     m_barcode;              
   //with CLHEP::HepLorentzVector
   // public:
   //   CLHEP::Hep3Vector pp;          
   //   double            ee;          
   // and hep3Vector being:
   // public:
   //   double    dx;          
   //   double    dy;          
   //   double    dz;          
   
   static TClassRef part_cl("HepMC::GenParticle");
   static TClassRef flow_cl("HepMC::Flow");
   static TClassRef pola_cl("HepMC::Polarization");
   static TClassRef hlv_cl("CLHEP::HepLorentzVector");
   static TClassRef h3v_cl("CLHEP::Hep3Vector");

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c, part_cl);

      bool hepmc2 = true;

      if (R__v < 10) {
         // First let's discover the real version!
         TVirtualStreamerInfo *info = (TVirtualStreamerInfo*)part_cl->GetStreamerInfos()->At( R__v );
         if( info ) {
           hepmc2 = !( info->GetCheckSum() == 0x50c9eb75 );
         }
      }

      if (hepmc2) {
         R__b.ReadClassBuffer(part_cl, obj, R__v, R__s, R__c);
         return;
      }
      HepMC::GenParticle *particle = (HepMC::GenParticle*)obj;
      
      //====process old versions
      //1- read CLHEP::HepLorentzVector versions
      {
         UInt_t R__s_tmp, R__c_tmp;
         //CLHEP::HepLorentzVector
         Version_t R__v_tmp = R__b.ReadVersion(&R__s_tmp, &R__c_tmp, hlv_cl);
         //CLHEP::Hep3Vector
         R__v_tmp = R__b.ReadVersion(&R__s_tmp, &R__c_tmp, h3v_cl);
      }
      //2- read CLHEP::HepLorentzVector data
      Double_t dx,dy,dz,ee;
      R__b >> dx;
      R__b >> dy;
      R__b >> dz;
      R__b >> ee;
      //3- copy old CLHEP vector into new HepMC vector
      particle->set_momentum( FourVector( dx, dy, dz, ee ) );
         
      //4- read the other unchanged members
      int m_pdg_id;
      R__b >> m_pdg_id;
      particle->set_pdg_id( m_pdg_id );

      int m_status;
      R__b >> m_status;
      particle->set_status(m_status);

      // Because of the pointer back to the parent we can not
      // use the 'static object / setter' technique, it leads to
      // the deletion of object we are using.
      //       static HepMC::Flow flow;
      //       flow_cl->Streamer(&flow,R__b);
      //       particle->set_flow( flow );
      flow_cl->Streamer(&particle->m_flow,R__b);
      
      //       static HepMC::Polarization polarization;
      //       pola_cl->Streamer(&polarization,R__b);
      //       particle->set_polarization( polarization );
      pola_cl->Streamer(&particle->m_polarization,R__b);

      //       HepMC::GenVertex *production_vertex = 0;
      //       HepMC::GenVertex *end_vertex = 0;
      //       R__b >> production_vertex;
      //       R__b >> end_vertex;
      // Humm ... those are protected ... but on linux we can still cheat by using #define protected public  ... well in addition, those do too much, so this is commented out
      //       particle->set_production_vertex_(production_vertex);
      //       particle->set_end_vertex_(end_vertex);

      R__b >> particle->m_production_vertex;
      R__b >> particle->m_end_vertex;
      
      int barcode;
      R__b >> barcode;
      // Humm ... this is protected ... but on linux we can still cheat by using #define protected public
      particle->set_barcode_(barcode);
      
      //====end of old versions

   } else {
      R__b.WriteClassBuffer(part_cl,obj);
   }
}
