// $Id: CheckOverlap.cpp,v 1.1 2002-10-21 17:58:22 gcorti Exp $

// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "CheckOverlap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CheckOverlap: CheckOverlap.cpp
//
// 28/06/2002
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CheckOverlap>          s_factory ;
const        IToolFactory& CheckOverlapFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CheckOverlap::CheckOverlap( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : AlgTool( type, name , parent ) {
  // Declaring implemented interfaces
  declareInterface<ICheckOverlap>(this);
  
}

//===========================================================================
/// Check for duplicate use of a protoparticle to produce particles.
/// Continue a previous check using the contents of the vector of pointers
/// to protoparticles.(Most intended for internal use by the recursive calls).
/// Arguments: parts is a vector of pointer to particles. 
///            proto is a vector of pointers to protoparticles.
//  Real checking method. Scans the tree headed by parts. Add each
//  protoparticle to proto if it is a new one. Return true otherwise.
//  If called directly by the user, it will continue a previous check, 
//  not start a new one!
//===========================================================================
 bool CheckOverlap::foundOverlap( const ParticleVector & parts, 
                                 std::vector<ContainedObject* > & 
                                 proto ) {
  
  MsgStream log( msgSvc(), name() );
  
  ParticleVector::const_iterator iPart;
  std::vector<ContainedObject* >::iterator iProto;
  
  for (iPart=parts.begin(); iPart != parts.end(); ++iPart){
    
    log << MSG::VERBOSE << "Analising particle " 
        << (*iPart)->particleID().pid() 
        << " with origin " << (*iPart)->origin() << endreq;
    
    if((*iPart)->origin() != 0 ){
      // It its a simple particle made from protoparticle. Check.
      for(iProto=proto.begin(); iProto != proto.end(); iProto++){
        if ( (*iPart)->origin() == (*iProto) ) {
          log << MSG::DEBUG << "Found duplicate use of "
              << "protoparticle " << *iProto << endreq;
          ProtoParticle * myproto = dynamic_cast<ProtoParticle * >(*iProto);  
          ProtoParticle::PIDInfoVector::iterator iProtoID;
          for (iProtoID=(myproto)->pIDInfo().begin();
               iProtoID!=(myproto)->pIDInfo().end(); ++iProtoID){
            log << MSG::DEBUG << " protoID = " << (*iProtoID).first 
                << " protoCL = " << (*iProtoID).second << endreq;
          }
          
          
          return true;
        }
      }
      proto.push_back( (*iPart)->origin() );
    }
    else{
      // It is a composite particle. Analyse its products.
      ParticleVector m_parts = toStdVector((*iPart)->endVertex()->products());
      
      if (foundOverlap( m_parts, proto ) ) return true;
    }
    
  }
  return false;
}

// Auxiliary function to convert a SmartRefVector<T>& to a std::vector<T*>
template <class T> 
std::vector<T*> toStdVector( SmartRefVector<T>& refvector ) {
  std::vector<T*> tvector;
  for( SmartRefVector<T>::iterator ip = refvector.begin();ip !=
         refvector.end(); ++ip ) {
    tvector.push_back( *ip );
  }
  return tvector;
}
