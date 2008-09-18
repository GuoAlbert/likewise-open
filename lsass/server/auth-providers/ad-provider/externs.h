/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.  You should have received a copy of the GNU General
 * Public License along with this program.  If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        externs.h
 *
 * Abstract:
 *
 *        Likewise Security and Authentication Subsystem (LSASS)
 *
 *        Active Directory Authentication Provider
 *
 *        External Variables
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#ifndef __EXTERNS_H__
#define __EXTERNS_H__

extern pthread_rwlock_t gADGlobalDataLock;

#define ENTER_AD_GLOBAL_DATA_RW_READER_LOCK(bInLock)       \
        if (!bInLock) {                                    \
           pthread_rwlock_rdlock(&gADGlobalDataLock);      \
           bInLock = TRUE;                                 \
        }

#define LEAVE_AD_GLOBAL_DATA_RW_READER_LOCK(bInLock)       \
        if (bInLock) {                                     \
           pthread_rwlock_unlock(&gADGlobalDataLock);      \
           bInLock = FALSE;                                \
        }

#define ENTER_AD_GLOBAL_DATA_RW_WRITER_LOCK(bInLock)       \
        if (!bInLock) {                                    \
           pthread_rwlock_wrlock(&gADGlobalDataLock);      \
           bInLock = TRUE;                                 \
        }

#define LEAVE_AD_GLOBAL_DATA_RW_WRITER_LOCK(bInLock)       \
        if (bInLock) {                                     \
           pthread_rwlock_unlock(&gADGlobalDataLock);      \
           bInLock = FALSE;                                \
        }

extern PSTR    gpszADProviderName;

extern PSTR    gpszConfigFilePath;

extern BOOLEAN gbCreateK5Login;

extern BOOLEAN gbShutdownProvider;

extern AD_PROVIDER_DATA  gADProviderData;
extern PAD_PROVIDER_DATA  gpADProviderData;

extern PSTR gpszUnprovisionedModeShell;
extern PSTR gpszUnprovisionedModeHomedirTemplate;

extern const CHAR gcSeparatorDefault;
extern CHAR gcSeparator;

extern const DWORD gdwCacheReaperTimeoutSecsMinimum;
extern const DWORD gdwCacheReaperTimeoutSecsDefault;
extern const DWORD gdwCacheReaperTimeoutSecsMaximum;
extern DWORD gdwCacheReaperTimeoutSecs;

extern pthread_t       gCacheReaperThread;
extern pthread_mutex_t gCacheReaperThreadLock;
extern pthread_cond_t  gCacheReaperThreadCondition;
extern pthread_t*      gpCacheReaperThread;

extern const DWORD gdwMachinePasswordSyncPwdLifetimeMinimum;
extern const DWORD gdwMachinePasswordSyncPwdLifetimeDefault;
extern const DWORD gdwMachinePasswordSyncPwdLifetimeMaximum;
extern DWORD gdwMachinePasswordSyncPwdLifetime;

extern const DWORD gdwCacheEntryExpirySecsMinimum;
extern const DWORD gdwCacheEntryExpirySecsDefault;
extern const DWORD gdwCacheEntryExpirySecsMaximum;
extern DWORD gdwCacheEntryExpirySecs;

extern DWORD gdwMachinePasswordSyncThreadWaitSecs;

extern pthread_t       gMachinePasswordSyncThread;
extern pthread_mutex_t gMachinePasswordSyncThreadLock;
extern pthread_cond_t  gMachinePasswordSyncThreadCondition;
extern pthread_t*      gpMachinePasswordSyncThread;

extern DWORD gdwClockDriftSecs;

extern HANDLE ghPasswordStore;

extern BOOLEAN gbLDAPSignAndSeal;

extern LSA_PROVIDER_FUNCTION_TABLE gADProviderAPITable;

extern DWORD gdwMachineTGTExpiry;

extern double gdwMachineTGTExpiryGraceSeconds;

extern CACHE_CONNECTION_HANDLE gpCacheConnection;

extern PLSA_HASH_TABLE gpAllowedGroups;

extern BOOLEAN gbAssumeDefaultDomain;

extern BOOLEAN gbSyncSystemTime;

// please put all new globals in the following structure:
extern PLSA_AD_PROVIDER_STATE gpLsaAdProviderState;

#endif /* __EXTERNS_H__ */

