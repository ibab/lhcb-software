// $Id: STLayerSelector.h,v 1.0 2012-05-02 15:31:15 elsasser Exp $
#ifndef STLAYERSELECTOR_H 
#define STLAYERSELECTOR_H 1

// Include files
// from STL
#include <string>
#include <vector>
#include <map>

// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"

// from Kernel
#include "Kernel/ISTChannelIDSelector.h"


/** @class STLayerSelector STLayerSelector.h
 *  
 *  Algorithm to remove clusters in excluded layers
 * 
 *  @author Ch. Elsasser
 *  @date   2012-05-02
 */


class STLayerSelector : 
  public virtual ISTChannelIDSelector, 
  public GaudiTool
{
  friend class ToolFactory<STLayerSelector>;

public:

  virtual bool select (const LHCb::STChannelID& id ) const;
  virtual bool operator() ( const LHCb::STChannelID& id ) const;
  virtual StatusCode initialize();    ///< Algorithm initialization

protected:

  STLayerSelector ( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);
  virtual ~STLayerSelector();
  

private:

  STLayerSelector ();
  STLayerSelector ( const STLayerSelector & );
  STLayerSelector& operator= ( const STLayerSelector& );

private:

  std::string m_detType;
  std::vector<std::string> m_ignoredLayers;

  std::map<std::string, unsigned int> m_layerMap;


};





#endif // STLAYERSELECTOR_H 
