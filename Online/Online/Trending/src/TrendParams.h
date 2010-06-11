// $Id: TrendParams.h,v 1.1.1.1 2010-06-11 09:58:56 ocallot Exp $
#ifndef TRENDPARAMS_H
#define TRENDPARAMS_H 1

#define MAX_ENTRY   1000      // number of entries in a directory record
#define DATA_SIZE  20000      // max size in word of a data record
#define TREND_AREA "TRENDDIR" // Area where the trend are written

//== Structure for the tag block header

struct Header {
  unsigned int  size;
  short         type;
  short         version;
  long          nextAddress;
};

//== structure of the data record

struct DataRecord {
  unsigned int  size;
  short         type;
  short         version;
  union {
    int   i;
    float f;  
  }             data[DATA_SIZE];
};

//== structure of the directory record

struct DirectoryRecord {
  unsigned int   size;
  short          type;
  short          version;
  long           nextAddress;
  int            nbEntries;
  struct DirEntry {
    int firstTime;
    int fileOffset; 
  }              entry[MAX_ENTRY];
};

enum recordType { TYPE_TAG, TYPE_DIR, TYPE_DATA };

#endif // TRENDPARAMS_H
