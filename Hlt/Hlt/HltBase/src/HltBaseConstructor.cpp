#include <stdexcept>
#include "HltBase/HltBase.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiHistoTool.h"

template <>
HltBase<GaudiHistoAlg>::HltBase( std::string name, ISvcLocator* pSvcLocator )
    : GaudiHistoAlg( std::move( name ), pSvcLocator )
{
    hltBaseConstructor();
}

template <>
HltBase<GaudiHistoAlg>::HltBase( std::string type, std::string name,
                                 const IInterface* parent )
    : GaudiHistoAlg( "ERROR", nullptr )
{
    fatal() << " invalid tool constructor " << type << " name " << name
            << " interface" << parent << endmsg;
    throw std::logic_error{"Invalide tool constructor"};
}

template <>
HltBase<GaudiHistoTool>::HltBase( std::string name, ISvcLocator* pSvcLocator )
    : GaudiHistoTool( "ERROR", "ERROR", nullptr )
{
    fatal() << " invalid algoritm constructor : name " << name << " svclocator "
            << pSvcLocator << endmsg;
    throw std::logic_error{"Invalide tool constructor"};
}

template <>
HltBase<GaudiHistoTool>::HltBase( std::string type, std::string name,
                                  const IInterface* parent )
    : GaudiHistoTool( std::move( type ), std::move( name ), parent )
{
    hltBaseConstructor();
}

