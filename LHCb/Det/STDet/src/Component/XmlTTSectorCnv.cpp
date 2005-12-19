// $Id: XmlTTSectorCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeTTSector.h"

typedef XmlUserDetElemCnv<DeTTSector >        XmlTTSectorCnv;
static CnvFactory<XmlTTSectorCnv>           s_XmlTTSectorCnvFactory ;
const ICnvFactory&  XmlTTSectorCnvFactory = s_XmlTTSectorCnvFactory ;
