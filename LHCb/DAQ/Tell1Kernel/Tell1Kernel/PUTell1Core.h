#ifndef PUTELL1CORE_H 
#define PUTELL1CORE_H 1

// words stored in a beetle map:
// hex value [sensor][beetle]
//         0 <= beetle <= 15
//         sensor = 0 --> C1
//         sensor = 1 --> A1
//         sensor = 2 --> C2
//         sensor = 3 --> A2
// with boolean tag:
// 	    false when the word is flipped

namespace PuTell1
{
  class dataObject {
    public:
      unsigned int dataWord;
      bool isOrdered;
  };

  enum tableLimits{
    SENSOR=4,
    BEETLE=16
  };
  
  enum sensor{
    C1=0, // sensor Id = 129; Tell1 = 22
    A1=1, // sensor Id = 128; Tell1 = 21
    C2=2, // sensor Id = 131; Tell1 = 26
    A2=3, // sensor Id = 132; Tell1 = 414
    NUM_SENSORS = 4
  };
  
  enum beetle{
    NUM_BEETLES = 16,
    STRIPS_PER_BEETLE = 128,
    STRIPS_PER_CHANNEL = 4 //every PU channel is 4 strips
  };
  
  enum PuBank{
    HEADERS_PER_SECTION = 2,
    WORDS_PER_SECTION = 34,
    NUMBITS = 32
  };
  
  enum PuNZSBank{
    NUM_PP = 4, // preProcessor FPGA's
    NUM_PPLINKS = 6, // each PP processes 6 optical links
    WORDS_PER_LINK = 36, // each link can receive 35 words; 1 is added by the Tell1 (only 0s)
    WORDS_PER_PP = 221, // 36 words per optical link * 6 Links
    WORD_WITH_FORMATINFO = 1, 
    FORMAT_BIT = 31,
    BEGIN_OF_DATAWORDS = 3
  };
  
  // Data
  typedef dataObject DataTable[SENSOR][BEETLE]; 
};
#endif // PUTELL1CORE_H
