// -------------------------------------------------------------------------
// File: Event/HepMCEvent.cpp
//
// AuthorList:
//
//         W. Pokorski: Initial coding Jully 02 
// -------------------------------------------------------------------------
 
#include "Event/HepMCEvent.h"
#include "GaudiKernel/ObjectFactory.h"
#include "HepMC/IO_Ascii.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>

// -------------------------------------------------------------------------
HepMCEvent::HepMCEvent(const std::string& Name,int ProcessId, int EventNum) :
  m_generatorName(Name) {
// -------------------------------------------------------------------------
  m_pGenEvt = new HepMC::GenEvent(ProcessId, EventNum);
}

// -------------------------------------------------------------------------
HepMCEvent::HepMCEvent(const HepMCEvent& evt):KeyedObject<int>() 
// -------------------------------------------------------------------------
{
  
  // deep copy
  *this = evt;
}

// -------------------------------------------------------------------------
HepMCEvent& HepMCEvent:: operator = (const HepMCEvent& evt)
// -------------------------------------------------------------------------
{
  if(this != &evt) {
     m_generatorName = evt.generatorName();
     if(m_pGenEvt !=0) delete m_pGenEvt;
     const HepMC::GenEvent* ptr = evt.pGenEvt();
     m_pGenEvt = new HepMC::GenEvent(ptr->signal_process_id(),
              ptr->event_number());
  }
  return (*this);
}

// -------------------------------------------------------------------------
StreamBuffer&
HepMCEvent::serialize( StreamBuffer& s ) const           // write out
// -------------------------------------------------------------------------
{
    KeyedObject<int>::serialize( s );
    //
 
    std::vector<long int> random_states = m_pGenEvt->random_states();

    s << m_generatorName
      << m_pGenEvt->event_number() << m_pGenEvt->event_scale()
      << m_pGenEvt->alphaQCD() << m_pGenEvt->alphaQED()
      << m_pGenEvt->signal_process_id()
      << ( m_pGenEvt->signal_process_vertex() ? 
           m_pGenEvt->signal_process_vertex()->barcode():0 )
      << m_pGenEvt->vertices_size()
      << m_pGenEvt->particles_size()
      << (int)random_states.size();

    for ( std::vector<long int>::iterator rs = random_states.begin(); 
	  rs != random_states.end(); ++rs ) {
      s << *rs;
    }

    s << (uint)m_pGenEvt->weights().size();
    for ( HepMC::WeightContainer::const_iterator w = m_pGenEvt->weights().begin(); 
	  w != m_pGenEvt->weights().end(); ++w ) {
      s << *w ;
    }

    //

    for (HepMC::GenEvent::vertex_const_iterator v=m_pGenEvt->vertices_begin();
         v!= m_pGenEvt->vertices_end();v++){
      write_vertex(*v,s);
      
    }

    for (HepMC::GenEvent::particle_const_iterator p=m_pGenEvt->particles_begin();
         p!= m_pGenEvt->particles_end();p++){
      write_particle(*p,s);
    }

    return( s );    
}


// -------------------------------------------------------------------------
StreamBuffer&
HepMCEvent::serialize( StreamBuffer& s )              // read in
// -------------------------------------------------------------------------
{
    KeyedObject<int>::serialize( s );

    //      

    s >> m_generatorName;

    int event_number = 0, signal_process_id = 0, signal_process_vertex = 0,
      num_vertices = 0, num_particles=0, random_states_size = 0,
      weights_size = 0;

    double eventScale=0, alpha_qcd=0, alpha_qed=0;

    s >> event_number >> eventScale >> alpha_qcd >> alpha_qed
      >> signal_process_id >> signal_process_vertex
      >> num_vertices >> num_particles >> random_states_size;

    std::vector<long int> random_states(random_states_size);
    for ( int i = 0; i < random_states_size; ++i ) {
      s >> random_states[i];
    }
    
    s >> weights_size;
    HepMC::WeightContainer weights(weights_size);
    for ( int i1 = 0; i1 < weights_size; ++i1 ) s >> weights[i1];

    //    

    
    // creating GenEvent and adding vertices to it
 
    if (m_pGenEvt == 0) {
      m_pGenEvt = new HepMC::GenEvent(signal_process_id, event_number);
    } else {
      m_pGenEvt->set_signal_process_id( signal_process_id );
      m_pGenEvt->set_event_number( event_number );
    }
    m_pGenEvt->weights() = weights;
    m_pGenEvt->set_random_states( random_states );


    int bar_prod;
    int bar_end;

    for ( int i2 = 1; i2 <= num_vertices; ++i2 ) {
      HepMC::GenVertex* v = read_vertex( s);
      m_pGenEvt->add_vertex( v );
    }

    //


    // adding particles to vertices


    for( int j = 1; j<=num_particles; ++j){
      

      HepMC::GenParticle* p = read_particle(&bar_end,&bar_prod,s);      

      if(bar_prod){
        HepMC::GenVertex* prod_Vtx = m_pGenEvt -> barcode_to_vertex(bar_prod);
        prod_Vtx -> add_particle_out(p);
       }

      if(bar_end)
      {      
       HepMC::GenVertex* end_Vtx = m_pGenEvt -> barcode_to_vertex(bar_end);
       end_Vtx -> add_particle_in(p);
      }
      
    }
    

    //


    // setting the pointer to the signal_process_vertex (if used) 

    if ( signal_process_vertex != 0 ) {
      m_pGenEvt->set_signal_process_vertex(
       m_pGenEvt->barcode_to_vertex(signal_process_vertex));
    }

    //


    return ( s );

}

// -------------------------------------------------------------------------
void HepMCEvent::write_vertex(  const HepMC::GenVertex* v, StreamBuffer &s ) const {
// -------------------------------------------------------------------------


  s << v->barcode();  
  s << v->id();
  s << v->position().x();
  s << v->position().y();
  s << v->position().z();
  s << v->position().t();


  for ( HepMC::WeightContainer::const_iterator w = v->weights().begin(); 
	w != v->weights().end(); ++w ) {
    s << *w;
  }

}

// -------------------------------------------------------------------------
void HepMCEvent::write_particle( const HepMC::GenParticle* p, StreamBuffer &s  ) const {
// -------------------------------------------------------------------------

  
  s << p->barcode();
  s << p->pdg_id();
  s << p->momentum().px();
  s << p->momentum().py();
  s << p->momentum().pz();
  s << p->momentum().e();
  s << p->status();
  s << p->polarization().theta();
  s << p->polarization().phi();

  s << (p->end_vertex() ? p-> end_vertex() -> barcode() : 0 );
  s << (p->production_vertex() ? p -> production_vertex() -> barcode() :0);
  

  s << p->flow().size();
  for ( std::map<int,int>::const_iterator i = p->flow().begin();
	i != p->flow().end(); ++i ) {
    s << (*i).first << (*i).second;
  }
  

}

// -------------------------------------------------------------------------
HepMC::GenVertex* HepMCEvent::read_vertex( StreamBuffer &s )
// -------------------------------------------------------------------------
{
  
  int identifier = 0;
  int id =0;

  int weights_size = 0;
  
  double x = 0., y = 0., z = 0., t = 0.; 

  s >> identifier >> id >> x >> y >> z >> t;

  HepMC::WeightContainer weights(weights_size);

  for ( int i = 0; i < weights_size; ++i ) s >> weights[i];

  HepMC::GenVertex* v = new HepMC::GenVertex( HepLorentzVector(x,y,z,t),
  			  id, weights);
  
  v->suggest_barcode(identifier);


  return v;

}

// -------------------------------------------------------------------------
HepMC::GenParticle* HepMCEvent::read_particle
( int* bar_end, int* bar_prod, StreamBuffer &s ){
// -------------------------------------------------------------------------

  double px = 0., py = 0., pz = 0., e = 0., theta = 0., phi = 0.;
  int id = 0, status = 0,  flow_size = 0;
  int bar_code=0;


  s >> bar_code >> id >> px >> py >> pz >> e >> status 
    >> theta >> phi ;

  
  s >> *bar_end >> *bar_prod ;
  s >> flow_size;

  

  // Flow

  HepMC::Flow flow;
  int code_index, code;
  for ( int i = 1; i <= flow_size; ++i ) {
    s >> code_index >> code;
    flow.set_icode( code_index,code);
  }

  //

  HepMC::GenParticle* p = new HepMC::GenParticle( HepLorentzVector(px,py,pz,e), 
			      id, status, flow, 
			      HepMC::Polarization(theta,phi) );

  p->suggest_barcode(bar_code);
  

  return p;

}
