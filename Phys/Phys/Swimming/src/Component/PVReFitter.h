#ifndef PVREFITTER_H 
#define PVREFITTER_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiTool.h>

// DaVinci
#include <Kernel/IPVReFitter.h>

// Event
#include <Event/RecVertex.h>

class IIncidentSvc;

namespace Swimming {
namespace Interfaces {
   class IService;
}

/** @class PVReFitter PVReFitter.h
 *  
 *  A tool to forward refitting a PV to another tool and moving the vertex afterwards.
 *
 *  @author Roel Aaij
 *  @date   2011-11-24
 */
class PVReFitter : public extends1<GaudiTool, IPVReFitter> {
public: 
   /// Standard constructor
   PVReFitter( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  
   virtual StatusCode initialize();

   /// refit PV
   virtual StatusCode reFit(LHCb::VertexBase* PV) const;

   /// remove track used for a (B) LHCb::Particle and refit PV
   virtual StatusCode remove(const LHCb::Particle* part,  
                             LHCb::VertexBase* PV) const;


   virtual ~PVReFitter( ); ///< Destructor

private:
      
   std::string m_refitterName;
   std::string m_serviceName;

   const IPVReFitter* m_refitter;
   Interfaces::IService* m_service;

};

}
#endif // PVREFITTER_H
