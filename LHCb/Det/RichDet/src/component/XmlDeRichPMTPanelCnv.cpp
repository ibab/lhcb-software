#ifndef XMLDERICHPMTPANELCNV_CPP
#define XMLDERICHPMTPANELCNV_CPP 1

// Include files
#include "RichDet/DeRichPMTPanel.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichPMTPanel>  XmlDeRichPMTPanelCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichPMTPanelCnv )

#endif // XMLDERICHPMTPANELCNV_CPP
