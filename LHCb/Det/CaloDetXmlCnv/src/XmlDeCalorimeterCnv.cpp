// $Id: XmlDeCalorimeterCnv.cpp,v 1.1.1.1 2002-03-28 13:32:34 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// include 
// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
// DetDesc
#include "DetDesc/XmlUserDetElemCnv.h"
// CaloDet 
#include "CaloDet/DeCalorimeter.h"

// ============================================================================
/** @file XmlDeCalorimeterCnv.cpp
 * 
 * Instantiation of a static factory class used by clients to create
 * instances of Xml converter for class DeCalorimeter 
 * @see DeCalorimeter 
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

/// converter type 
typedef XmlUserDetElemCnv<DeCalorimeter>   XmlDeCalorimeterCnv;

static CnvFactory<XmlDeCalorimeterCnv>         s_Factory ;
const ICnvFactory&XmlDeCalorimeterCnvFactory = s_Factory ;

// ============================================================================
// The End 
// ============================================================================
