#ifndef XMLDERICHMAPMTPANELCNV_CPP
#define XMLDERICHMAPMTPANELCNV_CPP 1

// Include files
#include "RichDet/DeRichMaPMTPanel.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichMaPMTPanel>  XmlDeRichMaPMTPanelCnv;
static CnvFactory<XmlDeRichMaPMTPanelCnv> s_DeRichMaPMTPanelFactory;
const ICnvFactory& XmlDeRichMaPMTPanelCnvFactory = s_DeRichMaPMTPanelFactory;

#endif // XMLDERichMAPMTPANELCNV_CPP
