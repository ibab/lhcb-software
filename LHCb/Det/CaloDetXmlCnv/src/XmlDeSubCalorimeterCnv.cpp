// $Id: XmlDeSubCalorimeterCnv.cpp,v 1.2 2003-06-16 13:36:27 sponce Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2002/03/28 13:32:34  ibelyaev
// Det/CaloDetXmlCnv: new package
//
// ============================================================================
// GaudiKernel
#include  "GaudiKernel/CnvFactory.h"
// DetDesc 
#include  "DetDescCnv/XmlUserDetElemCnv.h"
// CaloDet 
#include  "CaloDet/DeSubCalorimeter.h"

// ============================================================================
/** @file XmlDeSubCalorimeterCnv.cpp
 * 
 * Instantiation of a static factory class used by clients to create
 * instances of Xml converter for class DeSubCalorimeter 
 * @see DeSubCalorimeter 
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

/// converter type 
typedef  XmlUserDetElemCnv<DeSubCalorimeter>  XmlDeSubCalorimeterCnv;

static CnvFactory<XmlDeSubCalorimeterCnv>         s_Factory ;
const ICnvFactory&XmlDeSubCalorimeterCnvFactory = s_Factory ;

// ============================================================================
// The End 
// ============================================================================












