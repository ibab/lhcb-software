#include "Writer/writernetobject.h"
#include "Writer/writernetfileobject.h"
#include <string>
#include <map>
#include <set>

using namespace std;

int WriterNetObject::getPID() {
    return this->writerPID;
};
        
WriterNetObject::WriterNetObject(int writerPID) {
    this->writerPID = writerPID;
    this->files = NULL;
    this->fileCount = 0;
};

WriterNetObject::WriterNetObject(const WriterNetObject& other) {
   this->writerPID = other.writerPID;
   this->fileCount = other.fileCount;
   if(other.files == NULL) {
       this->files = NULL;
   } else {
       this->files = new FilesContainer(*(other.files));
   }
}

void WriterNetObject::addFile(const char* fn) {
    if(this->files == NULL) {
        this->files = new FilesContainer();
    }
    this->files->insert( make_pair(string(fn), WriterNetFileObject(this->writerPID, fn)) );
    this->fileCount++;
};
        
/** Searches if the file belongs to this writer and if so
 *  removes it from the list. */
void WriterNetObject::removeFile(const char* fn) {
    if(this->files == NULL) {
        return;
    }
    FilesContainerIterator i = this->files->find( string(fn));
    if(i != this->files->end()) {
        this->files->erase(i);
    }
};

bool WriterNetObject::operator==(const WriterNetObject& other) const {
    return this->writerPID == other.writerPID;
}

bool WriterNetObject::operator!=(const WriterNetObject& other) const {
    return this->writerPID != other.writerPID;
}

bool WriterNetObject::operator<(const WriterNetObject& other) const {
    return this->writerPID < other.writerPID;
}

bool WriterNetObject::operator>(const WriterNetObject& other) const {
    return this->writerPID > other.writerPID;
}

void WriterNetObject::addKeyValuePair(const char* fn, const char* key, const char* value) {
    if(this->files == NULL) {
        return;
    }
    FilesContainerIterator i = this->files->find( string(fn) );
    if(i != this->files->end()) {
        i->second.addKeyValue(key, value); // WriterNetFileObject found, delegate
    }
}

void WriterNetObject::debug(FILE* fp) {
    if(this->files == NULL || this->files->size() == 0) {
        fprintf(fp, "\nNo files for this WriterNetObject\n");
        fflush(fp);
        return;
    }
    FilesContainerIterator i = this->files->begin();
    while(i != files->end()) {
        fprintf(fp, "\n\t");
        fprintf(fp, i->first.c_str());
        i->second.debug(fp); // delegate (print details for this file)
        i++;
    }
    fprintf(fp, "\n");
    fflush(fp);
}


/** Destructor: deletes all files belonging to this writer */
WriterNetObject::~WriterNetObject() {
    delete this->files;
};

