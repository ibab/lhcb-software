// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetDescExample/src/XmlDeCalorimeterCnv.cpp,v 1.1 2001-06-28 09:50:08 sponce Exp $
#ifndef XMLDECALORIMETERCNV_H 
#define XMLDECALORIMETERCNV_H 1

// Include files
#include "DeCalorimeter.h"
#include "DetDesc/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeCalorimeter>  XmlDeCalorimeterCnv;
static CnvFactory<XmlDeCalorimeterCnv> DeCalorimeterFactory;
const ICnvFactory& XmlDeCalorimeterCnvFactory = DeCalorimeterFactory;

#endif // XMLDECALORIMETERCNV_H
