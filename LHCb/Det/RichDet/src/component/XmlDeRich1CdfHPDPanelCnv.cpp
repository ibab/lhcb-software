#ifndef XMLDERICH1CDFHPDPANELCNV_CPP
#define XMLDERICH1CdfHPDPANELCNV_CPP 1

// Include files
#include "RichDet/DeRich1CdfHPDPanel.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRich1CdfHPDPanel>  XmlDeRich1CdfHPDPanelCnv;
static CnvFactory<XmlDeRich1CdfHPDPanelCnv> s_DeRich1CdfHPDPanelFactory;
const ICnvFactory& XmlDeRich1CdfHPDPanelCnvFactory = 
                                       s_DeRich1CdfHPDPanelFactory;

#endif // XMLDERICH1CDFHPDPANELCNV_CPP
