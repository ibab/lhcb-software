#ifndef XMLDERICHHPDPANELCNV_CPP
#define XMLDERICHHPDPANELCNV_CPP 1

// Include files
#include "RichDet/DeRichHPDPanel.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichHPDPanel>  XmlDeRichHPDPanelCnv;
static CnvFactory<XmlDeRichHPDPanelCnv> s_DeRichHPDPanelFactory;
const ICnvFactory& XmlDeRichHPDPanelCnvFactory = s_DeRichHPDPanelFactory;

#endif // XMLDERICHHPDPANELCNV_CPP
