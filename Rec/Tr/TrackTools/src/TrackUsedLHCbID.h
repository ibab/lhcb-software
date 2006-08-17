// $Id: TrackUsedLHCbID.h,v 1.1 2006-08-17 09:35:18 mneedham Exp $
#ifndef _TrackUsedLHCbID_H
#define _TrackUsedLHCbID_H

/** @class TrackUsedLHCbID TrackTools/TrackUsedLHCbID.h
 *
 * Implementation of TrackUsedLHCbID
 * check if an LHCbID is used
 *
 *  @author M.Needham
 *  @date   2/08/2006
 */

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "Kernel/IUsedLHCbID.h"

#include <string>
#include <vector>

#include "Kernel/LHCbID.h"

class ITrackSelector;

class TrackUsedLHCbID: public GaudiTool,
	                virtual IUsedLHCbID,
                        virtual public IIncidentListener  {

public:

  /** constructer */
  TrackUsedLHCbID(const std::string& type,
                            const std::string& name,
                            const IInterface* parent);  

  /** destructer */
  virtual ~TrackUsedLHCbID();

  /** intialize */
  StatusCode initialize();

  /** Test if the LHCbID is used 
  * @param id to be test tested
  *  @return true if used
  */  
  bool used (const LHCb::LHCbID id) const;

  /** Implement the handle method for the Incident service.
  *  This is used to nform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

private:

  void initEvent() const;
  mutable bool m_configured;

  typedef std::vector<std::string> TrackContainers;
  mutable TrackContainers m_inputs;

  typedef std::vector<LHCb::LHCbID> LHCbIDs;
  mutable LHCbIDs m_usedCont;
 
  // for track selection 
  std::vector<std::string> m_names;
  typedef std::vector<ITrackSelector*> Selectors;
  Selectors m_selectors;

  // for comparing LHCbIDs
  class lessByID{
   public:
     inline bool operator() ( const LHCb::LHCbID& obj1 , const LHCb::LHCbID& obj2 ) const { 
        return obj1.lhcbID() < obj2.lhcbID() ; 
     }
  };

};

#endif
