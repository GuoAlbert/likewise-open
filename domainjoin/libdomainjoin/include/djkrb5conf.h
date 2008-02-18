/*
 * Copyright (C) Centeris Corporation 2004-2007
 * Copyright (C) Likewise Software    2007-2008
 * All rights reserved.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as 
 * published by the Free Software Foundation; either version 2.1 of 
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>.
 */

/* ex: set tabstop=4 expandtab shiftwidth=4: */
#ifndef __DJKRB5CONF_H__
#define __DJKRB5CONF_H__

#include "djmodule.h"

CENTERROR
DJModifyKrb5Conf(
    const char *testPrefix,
    BOOLEAN enable,
    PCSTR pszDomainName,
    PCSTR pszShortDomainName,
    BOOLEAN *modified
    );

CENTERROR
DJCopyKrb5ToRootDir(
        const char *srcPrefix,
        const char *destPrefix
        );

extern const JoinModule DJKrb5Module;

#endif // __DJKRB5CONF_H__
