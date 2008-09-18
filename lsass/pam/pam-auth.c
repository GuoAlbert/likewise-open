/* Editor Settings: expandtabs and use 4 spaces for indentation
 * ex: set softtabstop=4 tabstop=8 expandtab shiftwidth=4: *
 * -*- mode: c, c-basic-offset: 4 -*- */

/*
 * Copyright Likewise Software    2004-2008
 * All rights reserved.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the license, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.  You should have received a copy
 * of the GNU Lesser General Public License along with this program.  If
 * not, see <http://www.gnu.org/licenses/>.
 *
 * LIKEWISE SOFTWARE MAKES THIS SOFTWARE AVAILABLE UNDER OTHER LICENSING
 * TERMS AS WELL.  IF YOU HAVE ENTERED INTO A SEPARATE LICENSE AGREEMENT
 * WITH LIKEWISE SOFTWARE, THEN YOU MAY ELECT TO USE THE SOFTWARE UNDER THE
 * TERMS OF THAT SOFTWARE LICENSE AGREEMENT INSTEAD OF THE TERMS OF THE GNU
 * LESSER GENERAL PUBLIC LICENSE, NOTWITHSTANDING THE ABOVE NOTICE.  IF YOU
 * HAVE QUESTIONS, OR WISH TO REQUEST A COPY OF THE ALTERNATE LICENSING
 * TERMS OFFERED BY LIKEWISE SOFTWARE, PLEASE CONTACT LIKEWISE SOFTWARE AT
 * license@likewisesoftware.com
 */

/*
 * Copyright (C) Likewise Software. All rights reserved.
 *
 * Module Name:
 *
 *        pam-auth.c
 *
 * Abstract:
 * 
 *        Likewise Security and Authentication Subsystem (LSASS)
 * 
 *        Authentication API
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#include "pam-lsass.h"

int
pam_sm_authenticate(
    pam_handle_t* pamh, 
    int           flags, 
    int           argc,
    const char**  argv
    )
{
    DWORD       dwError = 0;
    PPAMCONTEXT pPamContext = NULL;
    PSTR        pszPassword = NULL;
    HANDLE      hLsaConnection = (HANDLE)NULL;
    PSTR        pszLoginId = NULL;
    
    LsaPamReadConfigFile();

    LSA_LOG_PAM_DEBUG("pam_sm_authenticate::begin");

    dwError = LsaPamGetContext(
                    pamh, 
                    flags, 
                    argc, 
                    argv, 
                    &pPamContext);
    BAIL_ON_LSA_ERROR(dwError);

    /* If we are just overriding the default repository
       (Solaris), only do that */
    if (pPamContext->pamOptions.bSetDefaultRepository)
    {
#ifdef HAVE_STRUCT_PAM_REPOSITORY
        struct pam_repository *currentRepository = NULL;
        pam_get_item(pamh, PAM_REPOSITORY, (void**) (void*) &currentRepository);
        if(currentRepository == NULL)
        {
            struct pam_repository files = { "files", NULL, 0 };
            if(pam_set_item(pamh, PAM_REPOSITORY, &files) != PAM_SUCCESS)
            {
                dwError = LSA_ERROR_INTERNAL;
                BAIL_ON_LSA_ERROR(dwError);
            }
        }

        /* This gets mapped to PAM_IGNORE */
        dwError = LSA_ERROR_NOT_HANDLED;
#else
        dwError = LSA_ERROR_INTERNAL;
        BAIL_ON_LSA_ERROR(dwError);
#endif
    }
    /* Otherwise, proceed with usual authentication */
    else
    {
        dwError = LsaPamGetLoginId(
            pamh,
            pPamContext,
            &pszLoginId);
        BAIL_ON_LSA_ERROR(dwError);
        
        dwError = LsaPamGetCurrentPassword(
            pamh,
            pPamContext,
            &pszPassword);
        BAIL_ON_LSA_ERROR(dwError);
        
        dwError = LsaOpenServer(&hLsaConnection);
        BAIL_ON_LSA_ERROR(dwError);
        
        dwError = LsaAuthenticateUser(
            hLsaConnection,
            pszLoginId,
            pszPassword);
        if (dwError == LSA_ERROR_PASSWORD_EXPIRED)
        {
            // deal with this error in the
            // next call to pam_sm_acct_mgmt
            pPamContext->bPasswordExpired = TRUE;
            dwError = PAM_SUCCESS;
        }
        BAIL_ON_LSA_ERROR(dwError);
        
#if defined(__LWI_SOLARIS__)
        /* On Solaris, we must save the user's password in
           a custom location so that we can pull it out later
           for password changes */
        dwError = LsaPamSetDataString(
            pamh,
            PAM_LSASS_OLDAUTHTOK, 
            pszPassword);
        BAIL_ON_LSA_ERROR(dwError);
#endif

    }
    
cleanup:

    if (hLsaConnection != (HANDLE)NULL)
    {
        LsaCloseServer(hLsaConnection);
    }

    LSA_SAFE_CLEAR_FREE_STRING(pszPassword);
    LSA_SAFE_FREE_STRING(pszLoginId);

    LSA_LOG_PAM_DEBUG("pam_sm_authenticate::end");

    return LsaPamMapErrorCode(dwError, pPamContext);
    
error:

    LSA_LOG_PAM_DEBUG("pam_sm_authenticate error [code:%d]", dwError);

    goto cleanup;
}

int
pam_sm_setcred(
    pam_handle_t* pamh,
    int           flags,
    int           argc,
    const char*   argv[]
    )
{
    LSA_LOG_PAM_DEBUG("pam_sm_setcred::begin");

    LSA_LOG_PAM_DEBUG("pam_sm_setcred::end");
    
    return 0;
}

