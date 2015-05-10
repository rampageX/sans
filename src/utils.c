/*
 * utils.c - Some util functions
 *
 * Copyright (C) 2014 - 2015, Xiaoxiao <i@xiaoxiao.im>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef __MINGW32__
#  include "win.h"
#else
#  include <arpa/inet.h>
#  include <errno.h>
#  include <fcntl.h>
#  include <grp.h>
#  include <pwd.h>
#  include <netdb.h>
#  include <string.h>
#  include <sys/socket.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#include "utils.h"

#define UNUSED(x) do {(void)(x);} while (0)

uint16_t rand_uint16(void)
{
	static int flag = 0;
	if (flag == 0)
	{
		srand((unsigned)time(NULL));
		flag = 1;
	}
	return (uint16_t)(rand() & 0xffff);
}

int setnonblock(int fd)
{
#ifdef __MINGW32__
	unsigned long mode = 0;
	if (ioctlsocket(fd, FIONBIO, &mode) != NO_ERROR)
	{
		return -1;
	}
	return 0;
#else
	int flags;
	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
	{
		return -1;
	}
	if (-1 == fcntl(fd, F_SETFL, flags | O_NONBLOCK))
	{
		return -1;
	}
	return 0;
#endif
}

int settimeout(int fd)
{
	struct timeval timeout = { .tv_sec = 10, .tv_usec = 0};
	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(struct timeval)) != 0)
	{
		return -1;
	}
	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval)) != 0)
	{
		return -1;
	}
	return 0;
}

int setreuseaddr(int fd)
{
	int reuseaddr = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(int)) != 0)
	{
		return -1;
	}
	return 0;
}

int setkeepalive(int fd)
{
	int keepalive = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(int)) != 0)
	{
		return -1;
	}
	return 0;
}

#ifdef SO_NOSIGPIPE
int setnosigpipe(int fd)
{
	int nosigpipe = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe)) != 0)
	{
		return -1;
	}
	return 0;
}
#endif

int getsockerror(int fd)
{
	int error = 0;
	socklen_t len = sizeof(int);
	if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) != 0)
	{
		return -1;
	}
	return error;
}

int runas(const char *user)
{
#ifdef __MINGW32__
	UNUSED(user);
#else
	struct passwd *pw_ent = NULL;

	pw_ent = getpwnam(user);

	if (pw_ent != NULL)
	{
		if (setegid(pw_ent->pw_gid) != 0)
		{
			return -1;
		}
		if (seteuid(pw_ent->pw_uid) != 0)
		{
			return -1;
		}
	}
#endif
	return 0;
}

int daemonize(const char *pidfile, const char *logfile)
{
#ifdef __MINGW32__
	UNUSED(pidfile);
	UNUSED(logfile);
#else
	pid_t pid;

	pid = fork();
	if (pid < 0)
	{
		fprintf(stderr, "fork: %s\n", strerror(errno));
		return -1;
	}

	if (pid > 0)
	{
		FILE *fp = fopen(pidfile, "w");
		if (fp == NULL)
		{
			fprintf(stderr, "can not open pidfile: %s\n", pidfile);
		}
		else
		{
			fprintf(fp, "%d", pid);
			fclose(fp);
		}
		exit(EXIT_SUCCESS);
	}

	umask(0);

	if (setsid() < 0)
	{
		fprintf(stderr, "setsid: %s\n", strerror(errno));
		return -1;
	}

	fclose(stdin);
	FILE *fp;
	fp = freopen(logfile, "w", stdout);
	if (fp == NULL)
	{
		fprintf(stderr, "freopen: %s\n", strerror(errno));
		return -1;
	}
	fp = freopen(logfile, "w", stderr);
	if (fp == NULL)
	{
		fprintf(stderr, "freopen: %s\n", strerror(errno));
		return -1;
	}
#endif
	return 0;
}
