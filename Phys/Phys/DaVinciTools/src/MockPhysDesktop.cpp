// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "MockPhysDesktop.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MockPhysDesktop
//
// 2011-01-05 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MockPhysDesktop );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MockPhysDesktop::MockPhysDesktop( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : 
  GaudiTool ( type, name , parent )
{
  declareInterface<IPhysDesktop>(this);

}
//=============================================================================
// Destructor
//=============================================================================
MockPhysDesktop::~MockPhysDesktop() {}
//=============================================================================
void MockPhysDesktop::setOutputLocation(const std::string& )
{
  error() << "IPhysDesktop::setOutputLocation obsolete!" << endmsg;
  return;
  
}


StatusCode MockPhysDesktop::setInputLocations(const std::vector<std::string>& /*locations*/) 
{

  return Error("IPhysDesktop::setInputLocations obsolete!");
  
}


StatusCode MockPhysDesktop::setP2PVInputLocations(const std::vector<std::string>& /*locations*/) 
{
  return Error("IPhysDesktop::setP2PVInputLocationns obsolete!");
}

StatusCode MockPhysDesktop::getEventInput()
{
  return Error("IPhysDesktop::getEventInput obsolete!");
  
}


const LHCb::Particle::ConstVector& MockPhysDesktop::particles() const
{
  error() << "IPhysDeskrop::particles() obsolete. "\
    "Use this->particles() for Particle::Range or this->i_particles() for Particle::ConstVector&" << endmsg;
  return m_parts;
  
}


const LHCb::Vertex::ConstVector& MockPhysDesktop::secondaryVertices() const
{
  error() << "IPhysDeskrop::secondaryVertices() obsolete. "\
    "Access secondary vertices via particles." << endmsg;
  return m_verts;
}


const Particle2Vertex::LightTable& MockPhysDesktop::Particle2VertexRelations() const
{

  error() << "IPHysDesktop::Particle2VertexRelations obsolete."\
    " Use this->i_p2PVTable()." << endmsg;
  return m_table;
  
}


Particle2Vertex::LightTable& MockPhysDesktop::Particle2VertexRelations()
{

  error() << "IPHysDesktop::Particle2VertexRelations obsolete."\
    " Use this->i_p2PVTable()." << endmsg;
  return m_table;
  
}


Particle2Vertex::Map& MockPhysDesktop::Particle2VertexMap()
{

  error() << "IPHysDesktop::Particle2VertexRelations obsolete." << endmsg;
  return m_map;
  

}


const Particle2Vertex::Map& MockPhysDesktop::Particle2VertexMap() const
{
  error() << "IPHysDesktop::Particle2VertexRelations obsolete." << endmsg;
  return m_map;
}


const LHCb::Particle* MockPhysDesktop::keep( const LHCb::Particle* ) 
{

  error() << "IPhysDesktop::keep(Particle) obsolete."\
    " Use this->mark(Particle) instead." << endmsg;
  return 0;
  

}

const LHCb::Vertex* MockPhysDesktop::keep( const LHCb::Vertex* ) 
{

  Exception("keep(Vertex) not allowed.");
  
  return 0;
  
}


const LHCb::RecVertex* MockPhysDesktop::keep( const LHCb::RecVertex* ) 
{
  error() << "IPhysDesktop::keep(RecVertex) obsolete."\
    " Use this->mark(RecVertex) instead." << endmsg;
  return 0;
  
}


StatusCode MockPhysDesktop::saveDesktop() const{

  return Error("IPhysDesktop::saveDesktop() obsolete. Use this->saveParticles().");  

}
  
StatusCode MockPhysDesktop::saveTrees( const LHCb::Particle::ConstVector& ) const
{

  return Error("IPhysDesktop::saveTrees(const Particle::ConstVector&) obsolere."\
               " Use this->markTrees(Particle::ConstVector&)");
  
}

  
StatusCode MockPhysDesktop::cloneTrees( const LHCb::Particle::ConstVector& )
{

  return Error("IPhysDesktop::cloneTrees(const Particle::ConstVector&) obsolete."\
               " Call this->cloneAndMark(const Particle*) for each entry in Particle::ConstVector");
  
  
}

  
StatusCode MockPhysDesktop::saveTrees(int) const
{
  return Error("IPhysDesktop::saveTrees obsolete.");
}


StatusCode MockPhysDesktop::cleanDesktop()
{
  return Error("IPhysDesktop::cleanDesktop obsolete.");
}

  
const std::string& MockPhysDesktop::getOutputLocation() const
{
  error() << "IPhysDesktop::getOutputLocation obsolete. "\
    "Use this->outputLocation()" << endmsg;
  return m_location;
  
}
  

StatusCode MockPhysDesktop::writeEmptyContainerIfNeeded() 
{
  return Error("IPhysDesktop::writeEmptyContainerIfNeeded obsolete. "\
               "Use this->writeEmptyContainerIfneeded.");
  
}


const LHCb::VertexBase* MockPhysDesktop::relatedVertex(const LHCb::Particle*) const
{
  error() << "IPhysDesktop::relatedVertex obsolete. Use this->bestVertex." 
          << endmsg;
  return 0;
}

void MockPhysDesktop::relate(const LHCb::Particle*, 
                             const LHCb::VertexBase*)
{
  error() << "IPhysDesktop::relate obsolete. DVAlgorithm takes care of relating via bestVertex mechanism." << endmsg;
  return;
  
}


Particle2Vertex::Table::Range MockPhysDesktop::particle2Vertices(const LHCb::Particle* part ) const
{
  error() << "IPhysDesktop::particle2Vertices obsolete." << endmsg;
  return m_table.relations(0);
}

void MockPhysDesktop::setWriteP2PV(const bool& ) 
{
  error() << "IPhysDesktop::setWriteP2PV obsolete." << endmsg;
  return;
}


void MockPhysDesktop::setUsingP2PV(const bool& ) 
{
  error() << "IPhysDesktop::setUsingP2PV obsolete." << endmsg;
  return;
}

void MockPhysDesktop::overWriteRelations(Particle2Vertex::Table::Range::const_iterator,
                                         Particle2Vertex::Table::Range::const_iterator ) 
{
  error() << "IPhysDesktop::overWriteRelations is obsolete."<< endmsg;
  return;
  
}


