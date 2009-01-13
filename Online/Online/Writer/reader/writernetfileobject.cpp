#include "Writer/writernetfileobject.h"
#include <string>
#include <map>
#include <set>

using namespace std;

WriterNetFileObject::WriterNetFileObject(int processID, const char* fn) {
   this->writerPID = processID;
   this->fileName = new string(fn);
   this->keyValuePairs = new map<string, string>();
   this->keyValuePairsCount = 0;
};

WriterNetFileObject::WriterNetFileObject(const WriterNetFileObject& otherWriterNetFileObject) {
    this->writerPID = otherWriterNetFileObject.writerPID;
    this->fileName = new string(*otherWriterNetFileObject.fileName);
    this->keyValuePairs = new map<string, string>(*otherWriterNetFileObject.keyValuePairs);
    this->keyValuePairsCount = otherWriterNetFileObject.keyValuePairsCount;
}

bool WriterNetFileObject::operator==(const WriterNetFileObject& other) const {
    return (this->writerPID == other.writerPID) && (this->fileName == other.fileName);
}

bool WriterNetFileObject::operator!=(const WriterNetFileObject& other) const {
    return (this->writerPID != other.writerPID) || (this->fileName != other.fileName);
}

bool WriterNetFileObject::operator<(const WriterNetFileObject& other) const {
    return (this->fileName < other.fileName);
}

bool WriterNetFileObject::operator>(const WriterNetFileObject& other) const {
    return (this->fileName > other.fileName);
}

void WriterNetFileObject::addKeyValue(const char* key, const char* value) {
    // check if the key already exists
    string key_s = string(key);
    string value_s = string(value);
    map<string, string>::iterator it;
    it = this->keyValuePairs->find(key_s);
    if(it != this->keyValuePairs->end()) {
        (*this->keyValuePairs)[key_s] = value_s;
    }
    // key does not exist, insert
    this->keyValuePairs->insert( pair<string, string>(string(key), string(value)) );
    this->keyValuePairsCount++;
};

void WriterNetFileObject::debug(FILE* fp) {
    if(this->keyValuePairs == NULL) {
        fprintf(fp, "There are no key/value pairs for this file\n");
        fflush(fp);
        return;
    }
    fprintf(fp, "\n\t\t%i key/value pairs", this->keyValuePairs->size());
    KeyValuePairIterator i = this->keyValuePairs->begin();
    while(i != this->keyValuePairs->end()) {
        fprintf(fp, "\n\t\t");
        fprintf(fp, i->first.c_str());
        fprintf(fp, "=");
        fprintf(fp, i->second.c_str());
        i++;
    }
    fprintf(fp, "\n");
    fflush(fp);
}

WriterNetFileObject::~WriterNetFileObject() {
    delete this->fileName;
    delete this->keyValuePairs;
}

