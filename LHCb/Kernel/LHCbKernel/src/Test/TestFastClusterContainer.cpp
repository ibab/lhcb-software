// $Id: TestFastClusterContainer.cpp,v 1.1 2006-01-17 14:29:45 cattanem Exp $

#include "Kernel/FastClusterContainer.h"
static int s_cnt[2] = {0,0};
struct Cluster  {
  unsigned m_chanID     : 24;
  unsigned m_size       :  1;
  unsigned m_threshold  :  1;
  unsigned m_center     :  3;
  unsigned m_pad        :  3;
  Cluster() {
    *(int*)this = 0;
    s_cnt[0]++;
  }
  ~Cluster() {        // Note: non virtual !
    s_cnt[1]++;
  }
  int chanID() const    { return m_chanID;    }
  int size() const      { return m_size;      }
  int threshold() const { return m_threshold; }
  unsigned center() const    { return m_center;    }
  void set(int i)  {
    m_chanID = i&0x00FFFFFF;
    m_size = i%2;
    m_threshold = (i+1)%2;
    m_center = i%8;
  }
};

static void __set(int* p, int i)  {
  // fast filling:
  // channel ID          size         threshold         cluster center
  //*p = (i&0x00FFFFFF) + ((i%2)<<24) + (((i+1)%2)<<25) + ((i%8)<<26);
  // _or_ with identical assembly code:
  *p = (i&0x00FFFFFF) + ((i%2)*0x01000000) + (((i+1)%2)*0x02000000) +
((i%8)*0x04000000);
}

void testCont()  {
  s_cnt[0] = s_cnt[1] = 0;
  {
    typedef FastClusterContainer<Cluster,int > ClusterCont;
    ClusterCont cnt;
    cnt.resize(128);
    int i, n;
    int* p = (int*)&(*cnt.begin());
    ClusterCont::iterator j;
    for(i=0, n=cnt.size(), j=cnt.begin(); i < n; ++i, ++p, ++j)  {
      ClusterCont::value_type& v = *j;
      v.set(i);
      __set(p, i);
      printf(" [%08X, %d %d %d] Channel ID:  %08X   size:%d
threshold:%d  center:%d  \n",
        *p, (i%2), (i+1)%2, (i%8), v.chanID(), v.size(), v.threshold(),
v.center());
    }
    {
      for(i=0, n=cnt.size(), j=cnt.begin(); i < n; ++i, ++p, ++j)  {
        ClusterCont::value_type& v = *j;
        printf("Channel ID:  %08X   size:%d   threshold:%d  center:%d
\n",
          v.chanID(), v.size(), v.threshold(), v.center());
      }
    }
  }
  printf("Clusters constructed:%d destructed:%d
diff:%d\n",s_cnt[0],s_cnt[1],s_cnt[0]-s_cnt[1]);
}
