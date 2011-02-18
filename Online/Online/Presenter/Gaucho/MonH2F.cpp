#include "Gaucho/MonH2F.h"

MonH2F::MonH2F(IMessageSvc* msgSvc, const std::string& source, int version):
  MonH2D(msgSvc, source, version)
{
  MsgStream msg(msgSvc, source + ":MonH2F");
  msg << MSG::DEBUG << "Histograms with floats are not implemented in Gaudi. Using doubles (MonH2D) instead." <<  endreq;
}
