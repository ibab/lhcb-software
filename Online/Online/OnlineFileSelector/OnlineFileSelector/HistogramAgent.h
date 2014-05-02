// $Id: HistogramAgent.h,v 1.3 2003/04/15 07:51:15 mato Exp $
#ifndef GAUDIKERNEL_HISTOGRAMAGENT_H
#define GAUDIKERNEL_HISTOGRAMAGENT_H

#include "GaudiKernel/ClassID.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataSelector.h"
#include "GaudiKernel/IDataStoreAgent.h"

/** @class HistogramAgent HistogramAgent.h GaudiKernel/HistogramAgent.h

    HistogramAgent base in charge of collecting all the refereces to
    DataObjects in a transient store that passes some selection criteria. The
    default behaviour is to collect all entries.

    @author Markus Frank
*/
class HistogramAgent : virtual public IDataStoreAgent  {
protected:
  IDataSelector m_objects;
public:
  /// Default creator
  HistogramAgent() {
  }
  /// Destructor
  virtual ~HistogramAgent()  {
  }
  /// Return the set of selected DataObjects
  IDataSelector* selectedObjects()    {
    return &m_objects;
  }
  /// Analyses a given directory entry 
  virtual bool analyse(IRegistry* pRegistry, int )   {
    DataObject* obj = pRegistry->object();
    if ( 0 != obj )  {
      if ( obj->clID() != CLID_StatisticsFile )    {
        m_objects.push_back(obj);
        return true;
      }
    }
    return false;
  }
};
#endif // GAUDIKERNEL_HISTOGRAMAGENT_H
