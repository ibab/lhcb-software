// $Id: GenVertex_Streamer.cpp,v 1.5 2008-10-31 16:40:15 cattanem Exp $
// see TClass::AdoptStreamer(new TClassStreamer(GenParticle_Streamer))

#include "TClassRef.h"

//Break the privacy of classes
#if !defined(private) && !defined(protected)
#define private public
#define protected public
#endif

#include "TBuffer.h"
#include "TVirtualStreamerInfo.h"

#include "HepMC/GenVertex.h"
#include "HepMC/GenEvent.h"

using namespace HepMC;





void GenVertex_Streamer(TBuffer &R__b, void *obj);


//RootStreamer(GenVertex,GenVertex_Streamer)

   void GenVertex_Streamer(TBuffer &R__b, void *obj) {
   //read an old version of GenVertex into a new GenVertex
   //old version had:
   // public:
   //   CLHEP::HepLorentzVector  m_position;    
   //   set<HepMC::GenParticle*> m_particles_in;    
   //   set<HepMC::GenParticle*> m_particles_out;    
   //   int                      m_id;      
   //   HepMC::WeightContainer   m_weights;          
   //   HepMC::GenEvent*         m_event;            
   //   int                      m_barcode;              
   //with CLHEP::HepLorentzVector
   // public:
   //   CLHEP::Hep3Vector pp;          
   //   double            ee;          
   // and hep3Vector being:
   // public:
   //   double    dx;          
   //   double    dy;          
   //   double    dz;          
   static TClassRef vert_cl("HepMC::GenVertex");
   static TClassRef vec_part_cl("vector<HepMC::GenParticle*>");
   static TClassRef weights_cl("HepMC::WeightContainer");
   static TClassRef hlv_cl("CLHEP::HepLorentzVector");
   static TClassRef h3v_cl("CLHEP::Hep3Vector");

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c, vert_cl);

      bool hepmc2 = true;
      
      if (R__v < 10) {
         // First let's discover the real version!
         TVirtualStreamerInfo *info = (TVirtualStreamerInfo*)vert_cl->GetStreamerInfos()->At( R__v );
         if( info ) {
           hepmc2 = !( info->GetCheckSum() == 0xf1246706 );
         }
      }

      if (hepmc2) {
         R__b.ReadClassBuffer(vert_cl, obj, R__v, R__s, R__c);
         return;
      }
      HepMC::GenVertex *vertex = (HepMC::GenVertex*)obj;
      
      //crap_cl->Streamer( vertex, R__b );
      //====process old versions
      //1- read CLHEP::HepLorentzVector versions
      {
         UInt_t R__s_tmp, R__c_tmp;
         //CLHEP::HepLorentzVector
         Version_t R__v_tmp = R__b.ReadVersion(&R__s_tmp, &R__c_tmp, hlv_cl);
         //CLHEP::Hep3Vector
         R__v_tmp = R__b.ReadVersion(&R__s_tmp, &R__c_tmp, h3v_cl);
         //2- read CLHEP::HepLorentzVector data
         Double_t dx,dy,dz,ee;
         R__b >> dx;
         R__b >> dy;
         R__b >> dz;
         R__b.CheckByteCount(R__s_tmp,R__c_tmp,h3v_cl);
         R__b >> ee;

         //3- copy old CLHEP vector into new HepMC vector
         vertex->set_position( FourVector( dx, dy, dz, ee ) );
      }
         
      //4- read the old set collections and convert them to vector
      
      // We can either use the public interface in which case we need
      // to read in all the particle in a temporary container and
      // then add then up using the method:
      //     add_particle_in
      //     add_particle_out
      // or we can use the #define private protected and use the data member directly!
      
      R__b.ReadClassBuffer(vec_part_cl, &vertex->m_particles_in, vec_part_cl->GetClassVersion(), 0, 0 );
      R__b.ReadClassBuffer(vec_part_cl, &vertex->m_particles_out, vec_part_cl->GetClassVersion(), 0, 0 );

      //4- read the other unchnaged members
      // See GenParticle_Streamer.cxx for alternative using the member functions.
      R__b >> vertex->m_id;
      weights_cl->Streamer( &(vertex->m_weights), R__b);      
      R__b >> vertex->m_event;
      R__b >> vertex->m_barcode;
      
      //====end of old versions

   } else {
      R__b.WriteClassBuffer(vert_cl,obj);
   }
}
