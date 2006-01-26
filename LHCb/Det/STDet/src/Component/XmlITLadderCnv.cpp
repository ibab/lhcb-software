// $Id: XmlITLadderCnv.cpp,v 1.1 2006-01-26 13:49:55 mneedham Exp $

// DetDesc
#include "DetDescCnv/XmlUserDetElemCnv.h"

// TTDet
#include "STDet/DeITLadder.h"

typedef XmlUserDetElemCnv<DeITLadder >        XmlITLadderCnv;
static CnvFactory<XmlITLadderCnv>           s_XmlITLadderCnvFactory ;
const ICnvFactory&  XmlITLadderCnvFactory = s_XmlITLadderCnvFactory ;
