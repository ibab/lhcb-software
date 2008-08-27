#include "ROMon/TaskSupervisor.h"
#include "XMLDocument.h"
namespace XML {
  class TaskSupervisorParser : public XMLDocument {
  public:
    typedef Cluster::Node Node;
    typedef std::list<Cluster> Clusters;
  public:
    TaskSupervisorParser();
    virtual ~TaskSupervisorParser();
    void getNodes(xercesc::DOMNode* fde, Cluster& cluster) const;
    void getClusterNodes(Cluster& cluster) const;
    void getClusters(Clusters& clusters) const;
  };
}

