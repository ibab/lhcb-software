#include "Gaucho/MonH1F.h"

MonH1F::MonH1F(IMessageSvc* msgSvc, const std::string& source, int version):
  MonH1D(msgSvc, source, version)
{
  MsgStream msg(msgSvc, source + ":MonH1F");
  msg << MSG::WARNING << "Histograms with floats are not implemented in Gaudi. Using doubles (MonH1D) instead." <<  endreq;
}
