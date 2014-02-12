#include <ctime>
#include <string>
/*f
 *   LHCb namespace declaration
 */
namespace LHCb {

  namespace MooreTest {
    struct StatEntry   {
      timespec time;
      unsigned long evt_seen;
      unsigned long evt_prod;
    };
    struct UserStats   {
      StatEntry  start;
      StatEntry  last;
      StatEntry  now;
      UserStats& operator+=(const UserStats& c);
    };
    class ResultMonitor  {
    public:
      ResultMonitor();
      virtual ~ResultMonitor();
      virtual void begin() = 0;
      virtual void end() = 0;
      virtual void output(const char* buffer) = 0;
      virtual void outputHeader() = 0;
      virtual void outputClient(const std::string& name, const UserStats& us) = 0;    
    };
  }
}
