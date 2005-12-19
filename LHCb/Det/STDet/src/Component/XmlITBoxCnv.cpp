// $Id: XmlITBoxCnv.cpp,v 1.1 2005-12-19 15:18:25 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITBox.h"

typedef XmlUserDetElemCnv<DeITBox >        XmlITBoxCnv;
static CnvFactory<XmlITBoxCnv>           s_XmlITBoxCnvFactory ;
const ICnvFactory&  XmlITBoxCnvFactory = s_XmlITBoxCnvFactory ;
