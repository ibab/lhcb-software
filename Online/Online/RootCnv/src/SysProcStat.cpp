/*
 * SysProcStat.cpp
 *
 *  Created on: Jul 6, 2012
 *      Author: Ivan Valencik
 */

#include "RootCnv/SysProcStat.h"

#include <cstdio>
#include <sys/time.h>
using namespace Gaudi;

SysProcStat::SysProcStat() {
	update();
}

SysProcStat::~SysProcStat() { }

/*
 * Update system performance values from /proc/self/stat virtaul file.
 */
int SysProcStat::update() {
	FILE* file = fopen("/proc/self/stat", "r");
	int par = fscanf(file,
			"%d %s %c %d %d"
			"%d %d %d %lu %lu"
			"%lu %lu %lu %lu %lu"
			"%ld %ld %ld %ld %ld"
			"%ld %lu %lu %ld %lu"
			"%lu %lu %lu %lu %lu"
			"%lu %lu %lu %lu %lu"
			"%lu %lu %d %d",
			&pid, comm, &state, &ppid, &pgrp,
			&session, &tty_nr, &tpgid, &flags, &minflt,
			&cminflt, &majflt, &cmajflt, &utime, &stime,
			&cutime, &cstime, &priority, &nice, &num_threads,
			&itrealvalue, &startime, &vsize, &rss, &rlim,
			&startcode, &endcode, &startstack, &kstkesp, &kstkeip,
			&signal, &blocked, &sigingore, &sigcatch, &wchan,
			&nswap, &cnswap, &exit_signal, &processor);
	fclose(file);

	if (par != 39)
		return -1;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

	return pid;
}
