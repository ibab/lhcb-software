#ifndef Vertex_H
#define Vertex_H 1

#include "TVector3.h"
#include "Riostream.h"
#include "globals.h"

using namespace std;
class Vertex;
class Particle;
typedef std::vector<Vertex*> Vertices;
typedef std::vector<Particle*> Particles;

/** @class Vertex
 *
 *  Class representing a Primary, Secondary or Seed Vertex
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */

class Vertex {

 public:
  Vertex( ){
    m_outgoingParticles.clear();
    m_type=0;
    m_position=TVector3(0,0,0);
    m_chisquare=0;
    m_likelihood=0;
    m_motherpart=NULL;
  }
  ~Vertex( ){} ///< Destructor
 
  enum Type{ Primary  = 1,
	     Pileup   = 2,
	     Secondary= 3,
	     Seed     = 4,
	     none     = 0
  };

  int  type()          { return m_type ; }///< return type of vertex
  double chi2PerDoF() { return m_chisquare; } ///< chi2 per degree of freedom 
  double likelihood() { 
    if(m_type!=4) cout<<"calling likelihood for non-seed vertex.."<<endl;
    return m_likelihood; 
  }   ///< returns a likelihood value if the vertex is of type Seed

  int nrofdaughters() { return m_outgoingParticles.size(); }///< number of daughters

  bool isPrimary()   {return m_type==1? true:false; }///< is a Primary Vertex
  bool isPileup()    {return m_type==2? true:false; }///< is a Pileup Vertex
  bool isSecondary() {return m_type==3? true:false; }///< is a Secondary Vertex
  bool isSeed()      {return m_type==4? true:false; }///< is a Vertex from seeding algorithm

  TVector3 position() { return m_position ; }///< position in space wrt to PV

  Particle* motherParticle()  { return m_motherpart; } ///< mother particle  
  Particles outgoingParticles() { return m_outgoingParticles; } ///< container of daughter particles

  void addToOutgoingParticles(Particle* a) {m_outgoingParticles.push_back(a);}
  void clearOutgoingParticles() {m_outgoingParticles.clear();}

  void setType( int a ){ m_type = a; }
  void setChi2PerDoF(double a) {m_chisquare=a;}
  void setPosition( TVector3 a ){ m_position = a; }
  void setLikelihood(double a) {m_likelihood=a;}

  void setOutgoingParticles(Particles a) {m_outgoingParticles=a;}
  void setMotherParticle(Particle* a) {m_motherpart=a;}

  void Print() {
    if(DBGLEVEL<3) {
      cout<< "---Vertex type="<<m_type << " isPrimary="<<this->isPrimary()
	  << " nr.daughters="<< this->nrofdaughters()
	  << " chi2="<< m_chisquare<<endl;
      cout<< " position_x="<< m_position.x() 
	  << "   y="<< m_position.y() 
	  << "   z="<< m_position.z() 
	  <<endl;
    }
  } ///< dump vertex information

 private:

  TVector3 m_position;
  Particles m_outgoingParticles;
  int m_type;
  Particle* m_motherpart;
  double m_chisquare, m_likelihood;

};

#endif 
