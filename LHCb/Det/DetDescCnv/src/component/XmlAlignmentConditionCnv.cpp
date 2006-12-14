// $Id: XmlAlignmentConditionCnv.cpp,v 1.2 2006-12-14 13:14:09 ranjard Exp $
// Include files 
#ifndef XMLALIGNMENTCONDITION_H 
#define XMLALIGNMENTCONDITION_H 1

// Include files
#include "DetDesc/AlignmentCondition.h"
#include "DetDescCnv/XmlUserConditionCnv.h"
#include "GaudiKernel/CnvFactory.h"

typedef  XmlUserConditionCnv<AlignmentCondition>  XmlAlignmentConditionCnv;
DECLARE_CONVERTER_FACTORY(XmlAlignmentConditionCnv)

#endif // XMLALIGNMENTCONDITION_H
