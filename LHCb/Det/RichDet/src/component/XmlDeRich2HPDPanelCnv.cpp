#ifndef XMLDERICH2HPDPANELCNV_CPP
#define XMLDERICH2HPDPANELCNV_CPP 1

// Include files
#include "RichDet/DeRich2HPDPanel.h"
#include "DetDesc/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRich2HPDPanel>  XmlDeRich2HPDPanelCnv;
static CnvFactory<XmlDeRich2HPDPanelCnv> s_DeRich2HPDPanelFactory;
const ICnvFactory& XmlDeRich2HPDPanelCnvFactory = s_DeRich2HPDPanelFactory;

#endif // XMLDERich2HPDPANELCNV_CPP
