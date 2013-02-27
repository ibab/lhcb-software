#include "HltBase/HltBase.h" 
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiHistoTool.h"

template<> 
HltBase<GaudiHistoAlg>::HltBase( const std::string& name,
				 ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{ hltBaseConstructor(); }

template <>
HltBase<GaudiHistoAlg>::HltBase ( const std::string& type   ,
                                  const std::string& name   ,
                                  const IInterface*  parent )
  : GaudiHistoAlg("ERROR",0)
{
  fatal() << " invalid tool constructor " << type << " name " << name
          << " interface" << parent << endmsg;
}

template<>
HltBase<GaudiHistoTool>::HltBase( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiHistoTool ( "ERROR", "ERROR", 0 )
{
  fatal() << " invalid algoritm constructor : name " << name
          << " svclocator " << pSvcLocator << endmsg;
}

template<>
HltBase<GaudiHistoTool>::HltBase ( const std::string& type   ,
                                   const std::string& name   ,
                                   const IInterface*  parent )
  : GaudiHistoTool(type,name,parent)
{ hltBaseConstructor(); }

