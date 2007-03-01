// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/PVSSInterface/PVSS/DpIdentifier.h,v 1.2 2007-03-01 20:08:55 frankb Exp $
//  ====================================================================
//  DpIdentifier.h
//  --------------------------------------------------------------------
//
//  Author    : Markus Frank
//
//  ====================================================================
#ifndef ONLINE_PVSS_DPIDENTIFIER_H
#define ONLINE_PVSS_DPIDENTIFIER_H

/**@class DpIdentifier DpIdentifier.h PVSS/DpIdentifier.h
  *
  * Fake DpIdentifier with the same data content as the
  * "true" DpIdentifier class from PVSS.
  * Can be removed once PVSS works with VC71 and RTTI enabled.
  *
  * Note:
  * This class should not be instantiated by users. This is too
  * dangerous. It typically is received from PVSS and copied.
  *
  * @author   M.Frank
  * @version  1.0
  */
class DpIdentifier  {
protected:
  enum ConfigNo { ZERO, ONE , TWO };
  unsigned long    systemID;
  unsigned long    dpID;
  signed short int elmentID;
  ConfigNo         configNo;
  signed short int detailNo;
  signed long      attrNo;
  signed short     dpTypeID;
public:
  unsigned long system() const { return systemID; }
  unsigned long id() const { return dpID; }
  signed short int elemnt() const { return elmentID; }
  unsigned short type() const { return dpTypeID; }

  /// Standard constructor
#ifdef __GCCXML
  DpIdentifier()
  : systemID(0), dpID(0), elmentID(0), configNo(ZERO), 
    detailNo(0), attrNo(0), dpTypeID(0)
  {
  }
#endif
  DpIdentifier(char d) 
  : systemID(0), dpID(0), elmentID(0), configNo(ZERO), 
    detailNo(0), attrNo(0), dpTypeID(0)
  {
  }
  /// Copy constructor
  DpIdentifier(const DpIdentifier& c) 
  : systemID(c.systemID), dpID(c.dpID), elmentID(c.elmentID), configNo(c.configNo), 
    detailNo(c.detailNo), attrNo(c.attrNo), dpTypeID(c.dpTypeID) 
  {
  }
  /// Assignment operator
  DpIdentifier& operator=(const DpIdentifier& c) {
    systemID  = c.systemID;
    dpID      = c.dpID;
    elmentID  = c.elmentID;
    configNo  = c.configNo;
    detailNo  = c.detailNo;
    attrNo    = c.attrNo;
    dpTypeID  = c.dpTypeID; 
    return *this; 
  }
  /// Standard destructor
  virtual ~DpIdentifier() {}
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
  virtual void debug(ostream &, int) const {}
#endif
  /// Assignment operator
  bool operator==(const DpIdentifier& c) const {
    return systemID ==  c.systemID && 
           dpID     ==  c.dpID     &&
           elmentID ==  c.elmentID &&
           configNo ==  c.configNo &&
           detailNo ==  c.detailNo &&
           attrNo   ==  c.attrNo   &&
           dpTypeID ==  c.dpTypeID;
  }
  /// Operator less to be used for map insertion
  bool operator<(const DpIdentifier& c) const {
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
#endif //ONLINE_PVSS_DPIDENTIFIER_H
