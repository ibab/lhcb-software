#include "ROMon/TaskSupervisor.h"
#include "XMLDocument.h"

namespace XML {
  class TaskSupervisorParser : public XMLDocument {
  public:
    typedef ROMon::Cluster Cluster;
    typedef ROMon::Cluster::Node Node;
    typedef ROMon::Inventory Inventory;
    typedef std::list<ROMon::Cluster> Clusters;
  public:
    TaskSupervisorParser();
    virtual ~TaskSupervisorParser();
    void getNodes(xercesc::DOMNode* fde, Cluster& cluster) const;
    void getClusterNodes(Cluster& cluster) const;
    void getClusters(Clusters& clusters) const;
    void getInventory(Inventory& inv) const;
  };
}

