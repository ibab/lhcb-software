// $Id: XmlAlignmentConditionCnv.cpp,v 1.1 2005-06-03 10:02:11 jpalac Exp $
// Include files 
#ifndef XMLALIGNMENTCONDITION_H 
#define XMLALIGNMENTCONDITION_H 1

// Include files
#include "DetDesc/AlignmentCondition.h"
#include "DetDescCnv/XmlUserConditionCnv.h"

typedef  XmlUserConditionCnv<AlignmentCondition>  XmlAlignmentConditionCnv;
static CnvFactory<XmlAlignmentConditionCnv> AlignmentConditionFactory;
const ICnvFactory& XmlAlignmentConditionCnvFactory = AlignmentConditionFactory;

#endif // XMLALIGNMENTCONDITION_H
