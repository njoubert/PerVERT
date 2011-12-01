//
//
//  daemon.h
//
//  Created by Niels Joubert on 2011-01-21.
//  Copyright (c) 2011 Niels Joubert. All rights reserved.
//


#ifndef _UTILS_DAEMON_H_
#define _UTILS_DAEMON_H_

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cstdio>

namespace Utils {
	
static int daemonize(int exit_success, int exit_failure) {
	pid_t pid, sid;

	/* already a daemon */
	if ( getppid() == 1 ) return -1;

	/* Fork off the parent process */
	pid = fork();
	if (pid < 0) {
		exit(exit_failure);
	}
	/* If we got a good PID, then we can exit the parent process. */
	if (pid > 0) {
		exit(exit_success);
	}

	/* At this point we are executing as the child process */

	/* Change the file mode mask */
	umask(0);

	/* Create a new SID for the child process */
	sid = setsid();
	if (sid < 0) {
		exit(exit_failure);
	}

	/* Change the current working directory.  This prevents the current
	   directory from being locked; hence not being able to remove it. */
	if ((chdir("/")) < 0) {
		exit(exit_failure);
	}

	/* Redirect standard files to /dev/null */
	freopen( "/dev/null", "r", stdin);
	freopen( "/dev/null", "w", stdout);
	freopen( "/dev/null", "w", stderr);

	return pid;
}

} /* namespace Utils */

#endif /* _UTILS_DAEMON_H_ */
