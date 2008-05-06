// $Id: OTBankVersion.h,v 1.4 2008-05-06 11:45:00 wouter Exp $
#ifndef EVENT_OTBANKVERSION_H 
#define EVENT_OTBANKVERSION_H 1

// Include files

/** @file OTBankVersion.h Event/OTBankVersion.h
 *  Version numbers of OT RawBuffer bank encoding
 *
 *  @author Marco Cattaneo
 *  @date   2004-10-26
 */
namespace OTBankVersion 
{
  enum versions { DC04=0, v1=1, v2=2, DC06=v2, v3=3, DEFAULT=v3, UNDEFINED };
}
#endif // EVENT_OTBANKVERSION_H
