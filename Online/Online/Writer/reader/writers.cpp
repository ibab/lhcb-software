#include "Writer/writers.h"
#include "Writer/writernetobject.h"
#include "Writer/writernetfileobject.h"
#include <string>
#include <map>
#include <set>

using namespace std;

Writers::Writers() {
    this->writerCount = 0;
    this->allWriters = NULL; 
};

void Writers::addWriter(int writerPID) {
    if(this->allWriters == NULL) {
        this->allWriters = new AllWriters();
    }
    this->allWriters->insert( make_pair(writerPID, WriterNetObject(writerPID)) );
    this->writerCount++;
};

WriterNetObject* Writers::getWriter(int writerPID) {
    if(this->allWriters == NULL) {
        return NULL;
    }
    AllWritersIterator i = allWriters->find( writerPID );
    if(i != allWriters->end()) {
            return &i->second;
    }
    return NULL;
};

void Writers::deleteWriter(int writerPID) {
    if(this->allWriters == NULL) {
        return;
    }
    AllWritersIterator i = allWriters->find( writerPID );
    if(i != allWriters->end()) {
        allWriters->erase(i);
        if(this->writerCount >= 1) {
            this->writerCount--;
        }
    }
};

int Writers::addFile(int writerPID, const char* fn) {
    if(this->allWriters == NULL) {
        return 0;
    }
    AllWritersIterator i = allWriters->find( writerPID );
    if(i != allWriters->end()) {
        i->second.addFile(fn);
        return writerPID;
    }
    return 0;
};

void Writers::removeFile(int writerPID, const char* fn) {
    AllWritersIterator i = allWriters->find( writerPID );
    if(i != allWriters->end()) {
        i->second.removeFile(fn);
    }
};

void Writers::addKeyValuePair(int writerPID, const char* fn, const char* key, const char* value) {
    AllWritersIterator i = allWriters->find( writerPID );
    if(i != allWriters->end()) {
        i->second.addKeyValuePair(fn, key, value); // writer found, delegate
    }
}

void Writers::debug(FILE* fp) {
    if(this->allWriters == NULL || this->allWriters->size() == 0) {
        fprintf(fp, "\nNo writers are active\n");
        fflush(fp);
        return;
    }
    AllWritersIterator i = this->allWriters->begin();
    while(i != allWriters->end()) {
        fprintf(fp, "\nWriter: ");
        fprintf(fp, "%i", i->second.getPID() );
        i->second.debug(fp); // delegate
        i++;
    }
    fflush(fp);
}

Writers::~Writers() {
    delete this->allWriters;
};

