/*
 * utils.h - Some util functions
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

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

extern uint16_t rand_uint16(void);
extern int setnonblock(int fd);
extern int settimeout(int fd);
extern int setreuseaddr(int fd);
extern int setkeepalive(int fd);
#ifdef SO_NOSIGPIPE
extern int setnosigpipe(int fd);
#endif
extern int getsockerror(int fd);
extern int runas(const char *user);
extern int daemonize(const char *pidfile, const char *logfile);

#endif // UTILS_H
