#ifndef XMLDERICH1HPDPANELCNV_CPP
#define XMLDERICH1HPDPANELCNV_CPP 1

// Include files
#include "RichDet/DeRich1HPDPanel.h"
#include "DetDesc/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRich1HPDPanel>  XmlDeRich1HPDPanelCnv;
static CnvFactory<XmlDeRich1HPDPanelCnv> s_DeRich1HPDPanelFactory;
const ICnvFactory& XmlDeRich1HPDPanelCnvFactory = s_DeRich1HPDPanelFactory;

#endif // XMLDERich1HPDPANELCNV_CPP
