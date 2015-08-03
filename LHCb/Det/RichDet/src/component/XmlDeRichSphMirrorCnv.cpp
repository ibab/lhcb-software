#ifndef XMLDERICHSPHMIRRORCNV_H 
#define XMLDERICHSPHMIRRORCNV_H 1

// Include files
#include "RichDet/DeRichSphMirror.h"
#include "DetDescCnv/XmlUserDetElemCnv.h"

typedef  XmlUserDetElemCnv<DeRichSphMirror>  XmlDeRichSphMirrorCnv;
DECLARE_CONVERTER_FACTORY( XmlDeRichSphMirrorCnv )

#endif // XMLDERICHSPHMIRRORCNV_H
