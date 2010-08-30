#ifndef __TABLE
#define __TABLE 1

class TableEntry;

/** @class Table  Table.h CPP/Table.h
 *
 */
class Table {
public:
  /// Initializing constructor
  explicit Table(int);
  /// Default destructor
  virtual ~Table();
  void Insert(void*, void*);
  bool  Find(void**, void*);
  void Remove(void*);
private:
  int size;
  TableEntry** first;
  TableEntry** last;
  
  TableEntry* Probe(void*);
  TableEntry** ProbeAddr(void*);
};

#endif
