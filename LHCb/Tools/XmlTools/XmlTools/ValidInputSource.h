// $Id: ValidInputSource.h,v 1.1 2007-02-02 08:10:49 marcocle Exp $
#ifndef VALIDINPUTSOURCE_H_
#define VALIDINPUTSOURCE_H_

#include <GaudiKernel/Time.h>
#include <GaudiKernel/IValidity.h>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/PlatformUtils.hpp>

#include <xercesc/framework/MemoryManager.hpp>

#include <xercesc/framework/MemBufInputSource.hpp>


/** @class ValidInputSource XmlTools/ValidInputSource.h
 *  
 *  Extend xercesc::InputSource adding a validity (IValidity interface).
 *
 *  @author Marco Clemencic
 *  @date   2006-02-01
 */
class ValidInputSource: public xercesc::MemBufInputSource, virtual public IValidity {

public:

  /// Constructor
  ValidInputSource ( const XMLByte *const           srcDocBytes,
                     const unsigned int             byteCount,
                     const XMLCh *const             bufId,
                     const bool                     adoptBuffer = false,
                     xercesc::MemoryManager *const  manager = xercesc::XMLPlatformUtils::fgMemoryManager );

  /// Constructor
  ValidInputSource ( std::string                    buffer,
                     const XMLCh *const             bufId,
                     xercesc::MemoryManager *const  manager = xercesc::XMLPlatformUtils::fgMemoryManager );


  /// Destructor
  virtual ~ValidInputSource();

  // -------- Implementation of IValidity --------
   
  /// is the Object valid? (it can be always invalid)
  virtual bool isValid() const; 

  /// is the Object valid for a given time?
  virtual bool isValid( const Gaudi::Time& time) const; 

  /// since what time the Objest is valid?
  virtual const Gaudi::Time& validSince() const; 

  /// till  what time the Object is Valid?
  virtual const Gaudi::Time& validTill() const;

  /// set the validity range of the Object
  virtual void setValidity( const Gaudi::Time& since, const Gaudi::Time& until); 

  /// set the validity time of the Object 
  virtual void setValiditySince( const Gaudi::Time& since); 

  /// set the validity time of the Object 
  virtual void setValidityTill( const Gaudi::Time& until);

private:
  
  /// Time of the start of validity
  Gaudi::Time m_since;
  
  /// Time of end of validity
  Gaudi::Time m_until;

  /// optional buffer, external to xercesc
  std::string m_buffer;
  
};

#endif /*VALIDINPUTSOURCE_H_*/
