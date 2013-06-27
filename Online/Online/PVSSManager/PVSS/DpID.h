// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSManager/PVSS/DpID.h,v 1.4 2007-04-11 17:45:47 frankb Exp $
//  ====================================================================
//  DpID.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DPID_H
#define ONLINE_PVSS_DPID_H

#ifdef _WIN32
typedef __int8   int8_t;
typedef __int16  int16_t;
typedef __int32  int32_t;
typedef __int64  int64_t;

typedef unsigned __int8   uint8_t;
typedef unsigned __int16  uint16_t;
typedef unsigned __int32  uint32_t;
typedef unsigned __int64  uint64_t;
#else
#include <stdint.h>
#endif

// Forward declarations
class DpIdentifier;
class __ostream;

/*
 * PVSS namespace declaration
 */
namespace PVSS {

  /**@class DpID DpID.h PVSS/DpID.h
    *
    * Fake DpID with the same data content as the
    * "true" DpID class from PVSS.
    * Can be removed once PVSS works with VC71 and RTTI enabled.
    *
    * Note:
    * This class should not be instantiated by users. This is too
    * dangerous. It typically is received from PVSS and copied.
    *
    * @author   M.Frank
    * @version  1.0
    */
  class DpID  {
  protected:
    enum ConfigNo { ZERO, ONE , TWO };
    uint32_t         systemID;
    uint32_t         dpID;
    int16_t          elmentID;
    ConfigNo         configNo;
    int16_t          detailNo;
    int32_t          attrNo;
    int16_t          dpTypeID;

  public:

    unsigned long    system() const { return systemID;  }
    unsigned long    id() const     { return dpID;      }
    signed short int element() const{ return elmentID;  }
    ConfigNo         config() const { return configNo;  }
    signed short int detail() const { return detailNo;  }
    signed long      attr() const   { return attrNo;    }
    signed short     type() const   { return dpTypeID;  }

    /// Standard constructor
  #ifdef __GCCXML
    DpID()
    : systemID(0), dpID(0), elmentID(0), configNo(ZERO), 
      detailNo(0), attrNo(0), dpTypeID(0)
    {
    }
  #endif
    DpID(char /* d */) 
    : systemID(0), dpID(0), elmentID(0), configNo(ZERO), 
      detailNo(0), attrNo(0), dpTypeID(0)
    {
    }
    /// Copy constructor
    DpID(const DpID& c) 
    : systemID(c.systemID), dpID(c.dpID), elmentID(c.elmentID), configNo(c.configNo), 
      detailNo(c.detailNo), attrNo(c.attrNo), dpTypeID(c.dpTypeID) 
    {
    }
    /// Assignment operator
    DpID& operator=(const DpID& c) {
      systemID  = c.systemID;
      dpID      = c.dpID;
      elmentID  = c.elmentID;
      configNo  = c.configNo;
      detailNo  = c.detailNo;
      attrNo    = c.attrNo;
      dpTypeID  = c.dpTypeID; 
      return *this; 
    }
    /// Assignment operator
    DpID& operator=(const DpIdentifier& cc) {
      DpID* c = (DpID*)&cc;
      systemID  = c->systemID;
      dpID      = c->dpID;
      elmentID  = c->elmentID;
      configNo  = c->configNo;
      detailNo  = c->detailNo;
      attrNo    = c->attrNo;
      dpTypeID  = c->dpTypeID; 
      return *this; 
    }
    /// Standard destructor
    virtual ~DpID() {}
    /// Conversion to DpIdentifier
    operator DpIdentifier& ()  { 
      return *(DpIdentifier*)this;
    }
    /// Conversion to DpIdentifier (const)
    operator const DpIdentifier& ()  const { 
      return *(DpIdentifier*)this;
    }
    /// PVSS overload: Invalidate data members
    virtual void invalidateAll()  {
      systemID  = 0;
      dpID      = 0;
      elmentID  = 0;
      configNo  = ZERO;
      detailNo  = 0;
      detailNo  = 0;
      dpTypeID  = 0;
    }
  #ifndef __GCCXML
    /// PVSS overload: debug
    virtual void debug(__ostream &, int) const {}
  #endif
    /// Assignment operator
    bool operator==(const DpID& c) const {
      return systemID ==  c.systemID && 
            dpID     ==  c.dpID     &&
            elmentID ==  c.elmentID &&
            configNo ==  c.configNo &&
            detailNo ==  c.detailNo &&
            attrNo   ==  c.attrNo   &&
            dpTypeID ==  c.dpTypeID;
    }
    /// Operator less to be used for map insertion
    bool operator<(const DpID& c) const {
      if     (&c==this)return false;  // Check first the most relevant ones....
      else if(dpID<c.dpID)          return true;
      else if(dpID>c.dpID)          return false;
      else if(elmentID<c.elmentID)  return true;
      else if(elmentID>c.elmentID)  return false;
      else if(dpTypeID<c.dpTypeID)  return true;
      else if(dpTypeID>c.dpTypeID)  return false;

      else if(systemID<c.systemID)  return true;
      else if(systemID>c.systemID)  return false;
      else if(configNo<c.configNo)  return true;
      else if(configNo>c.configNo)  return false;
      else if(detailNo<c.detailNo)  return true;
      else if(detailNo>c.detailNo)  return false;
      else if(attrNo<c.attrNo)      return true;
      else if(attrNo>c.attrNo)      return false;
      else                          return false;
    }
  };
}      // End namespace PVSS
#endif //ONLINE_PVSS_DPID_H
