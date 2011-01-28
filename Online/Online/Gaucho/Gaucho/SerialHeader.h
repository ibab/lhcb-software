/*
 * SerialHeader.h
 *
 *  Created on: Aug 26, 2010
 *      Author: beat
 */

#ifndef SERIALHEADER_H_
#define SERIALHEADER_H_

#define SERHEADER_Compress 1<<0
#define SERHEADER_Version 1

class SerialHeader
{
public:
  int flags;
  int version;
  int comp_version;
  long long ser_tim;
  long long run_number;
  int buffersize;
  SerialHeader & operator = (const SerialHeader &t){memcpy(this,&t, sizeof(SerialHeader));return *this;};
};

#endif /* SERIALHEADER_H_ */
