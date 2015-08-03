#ifndef XMLDERICHHPDPANELCNV_CPP
#define XMLDERICHHPDPANELCNV_CPP 1

// Include files
#include "RichDet/DeRichHPDPanel.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichHPDPanel>  XmlDeRichHPDPanelCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichHPDPanelCnv )

#endif // XMLDERICHHPDPANELCNV_CPP
