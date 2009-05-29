#include "CPP/Table.h"
#include <cstdlib>

class TableEntry {
  friend class Table;
  void* key;
  void* value;
  TableEntry* chain;
};

Table::Table (int n) {
  size = 32;
  while (size < n) {
    size <<= 1;
  }
  first = new TableEntry*[size];
  --size;
  last = &first[size];
  for (TableEntry** e = first; e <= last; e++) {
    *e = 0;
  }
}

Table::~Table () {
  delete first;
}

inline TableEntry* Table::Probe (void* i) {
  return first[(size_t)i & size];
}

inline TableEntry** Table::ProbeAddr (void* i) {
  return &first[(size_t)i & size];
}

void Table::Insert (void* k, void* v) {
  TableEntry* e = new TableEntry;
  e->key = k;
  e->value = v;
  TableEntry** a = ProbeAddr(k);
  e->chain = *a;
  *a = e;
}

bool Table::Find (void** v, void* k) {
  for (TableEntry* e = Probe(k); e != 0; e = e->chain) {
    if (e->key == k) {
      *v = e->value;
      return true;
    }
  }
  return false;
}

void Table::Remove (void* k) {
  TableEntry** a = ProbeAddr(k);
  TableEntry* e = *a, *prev;
  if (e != 0) {
    if (e->key == k) {
      *a = e->chain;
      delete e;
    } else {
      do {
        prev = e;
        e = e->chain;
      } while (e != 0 && e->key != k);
      if (e != 0) {
        prev->chain = e->chain;
        delete e;
      }
    }
  }
}
