#ifndef XMLDETUTORIALMASTERCNV_H
#define XMLDETUTORIALMASTERCNV_H 1


// local

#include "GaudiKernel/CnvFactory.h"

#include "DeTutorialMaster.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeTutorialMaster>  XmlDeTutorialMasterCnv;
DECLARE_CONVERTER_FACTORY( XmlDeTutorialMasterCnv )

#endif //XMLDETUTORIALMASTERCNV_H
