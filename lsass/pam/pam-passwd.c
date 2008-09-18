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
 *        pam-passwd.c
 *
 * Abstract:
 * 
 *        Likewise Security and Authentication Subsystem (LSASS)
 * 
 *        Pluggable Authentication Module
 * 
 *        Password API
 *
 * Authors: Krishna Ganugapati (krishnag@likewisesoftware.com)
 *          Sriram Nambakam (snambakam@likewisesoftware.com)
 */
#include "pam-lsass.h"

#ifndef PAM_BAD_ITEM
#define PAM_BAD_ITEM PAM_SERVICE_ERR
#endif

int
pam_sm_chauthtok(
    pam_handle_t* pamh, 
    int           flags, 
    int           argc, 
    const char**  argv
    )
{
    DWORD dwError = 0;
    PPAMCONTEXT pPamContext = NULL;
    
    LsaPamReadConfigFile();
    
    LSA_LOG_PAM_DEBUG("pam_sm_chauthtok::begin");
    
    if (!(flags & PAM_UPDATE_AUTHTOK) &&
        !(flags & PAM_PRELIM_CHECK))
    {
        dwError = PAM_AUTHTOK_ERR;
        BAIL_ON_LSA_ERROR(dwError);
    }
    
    dwError = LsaPamGetContext(
                    pamh,
                    flags,
                    argc,
                    argv,
                    &pPamContext);
    BAIL_ON_LSA_ERROR(dwError);
    
    if (flags & PAM_PRELIM_CHECK)
    {  
        dwError = LsaPamCheckCurrentPassword(
                            pamh,
                            pPamContext);
        BAIL_ON_LSA_ERROR(dwError);
    }
    
    if (flags & PAM_UPDATE_AUTHTOK) {
        
        pPamContext->pamOptions.bUseFirstPass = TRUE;
        
        dwError = LsaPamUpdatePassword(
                        pamh,
                        pPamContext);
        BAIL_ON_LSA_ERROR(dwError);
    }

cleanup:

    LSA_LOG_PAM_DEBUG("pam_sm_chauthtok::end");

    return LsaPamMapErrorCode(dwError, pPamContext);
    
error:

    LSA_LOG_PAM_DEBUG("pam_sm_chauthtok failed [code: %d]", dwError);

    goto cleanup;
}

int
LsaPamCheckCurrentPassword(
    pam_handle_t* pamh,
    PPAMCONTEXT pPamContext
    )
{
    DWORD   dwError = 0;
    HANDLE  hLsaConnection = (HANDLE)NULL;
    BOOLEAN bCheckOldPassword = TRUE;
    PSTR    pszOldPassword = NULL;
    PSTR    pszLoginId = NULL;
    
    LSA_LOG_PAM_DEBUG("LsaPamCheckCurrentPassword::begin");
    
    dwError = LsaPamGetLoginId(
                    pamh,
                    pPamContext,
                    &pszLoginId);
    BAIL_ON_LSA_ERROR(dwError);
    
    dwError = LsaOpenServer(&hLsaConnection);
    BAIL_ON_LSA_ERROR(dwError);
    
    dwError = LsaPamMustCheckCurrentPassword(
                     hLsaConnection,
                     pszLoginId,
                     &bCheckOldPassword);
    BAIL_ON_LSA_ERROR(dwError);
    
    if (bCheckOldPassword)
    {         
        dwError = LsaPamGetOldPassword(
                          pamh,
                          pPamContext,
                          &pszOldPassword);
        BAIL_ON_LSA_ERROR(dwError);
               
        dwError = LsaAuthenticateUser(
                          hLsaConnection,
                          pszLoginId,
                          pszOldPassword);
        BAIL_ON_LSA_ERROR(dwError);
     }
    
cleanup:

    if (hLsaConnection != (HANDLE)NULL)
    {
        LsaCloseServer(hLsaConnection);
    }
    
    LSA_SAFE_FREE_STRING(pszLoginId);
    LSA_SAFE_CLEAR_FREE_STRING(pszOldPassword);
    
    LSA_LOG_PAM_DEBUG("LsaPamCheckCurrentPassword::end");

    return LsaPamMapErrorCode(dwError, pPamContext);

error:

    LSA_LOG_PAM_DEBUG("LsaPamCheckCurrentPassword failed [code: %d]", dwError);

    goto cleanup;
}

int
LsaPamMustCheckCurrentPassword(
    HANDLE   hLsaConnection,
    PCSTR    pszLoginId,
    PBOOLEAN pbCheckOldPassword)
{
    DWORD dwError = 0;
    PVOID pUserInfo = NULL;
    DWORD dwUserInfoLevel = 1;
    BOOLEAN bCheckOldPassword = FALSE;
    
    LSA_LOG_PAM_DEBUG("LsaPamMustCheckCurrentPassword::begin");
    
    dwError = LsaFindUserByName(
                     hLsaConnection,
                     pszLoginId,
                     dwUserInfoLevel,
                     &pUserInfo);
    BAIL_ON_LSA_ERROR(dwError);
    
    if (((PLSA_USER_INFO_1)pUserInfo)->bIsLocalUser)
    {
        // Local root user does not have to
        // provider a user's old password.
        bCheckOldPassword = (geteuid() != 0);
    }
    else
    {
        // TODO: Handle domain admins
        // For now, prompt for old password of AD Users
        bCheckOldPassword = TRUE;
    }
    
    *pbCheckOldPassword = bCheckOldPassword;
    
cleanup:

    if (pUserInfo) {
        LsaFreeUserInfo(dwUserInfoLevel, pUserInfo);
    }
    
    LSA_LOG_PAM_DEBUG("LsaPamCheckCurrentPassword::end");

    return dwError;
    
error:

    *pbCheckOldPassword = TRUE;
    
    LSA_LOG_PAM_DEBUG("LsaPamCheckCurrentPassword failed [code: %d]", dwError);

    goto cleanup;
}

int
LsaPamUpdatePassword(
    pam_handle_t* pamh,
    PPAMCONTEXT   pPamContext
    )
{
    DWORD  dwError = 0;
    PSTR   pszOldPassword = NULL;
    PSTR   pszPassword = NULL;
    PSTR   pszLoginId = NULL;
    HANDLE hLsaConnection = (HANDLE)NULL;
    BOOLEAN bCheckOldPassword = FALSE;
    
    LSA_LOG_PAM_DEBUG("LsaPamUpdatePassword::begin");
    
    dwError = LsaPamGetLoginId(
                    pamh,
                    pPamContext,
                    &pszLoginId);
    BAIL_ON_LSA_ERROR(dwError);
    
    dwError = LsaOpenServer(&hLsaConnection);
    BAIL_ON_LSA_ERROR(dwError);
    
    dwError = LsaPamMustCheckCurrentPassword(
                         hLsaConnection,
                         pszLoginId,
                         &bCheckOldPassword);
    BAIL_ON_LSA_ERROR(dwError);
    
    if (bCheckOldPassword)
    {
    	dwError = LsaPamGetOldPassword(
                   pamh,
                   pPamContext,
                   &pszOldPassword);
    	BAIL_ON_LSA_ERROR(dwError);
    }
    
    dwError = LsaPamGetNewPassword(
                   pamh, 
                   pPamContext,
                   &pszPassword);
    BAIL_ON_LSA_ERROR(dwError);
    
    dwError = LsaChangePassword(
                   hLsaConnection,
                   pszLoginId,
                   pszPassword,
                   pszOldPassword);
    BAIL_ON_LSA_ERROR(dwError);
    
cleanup:

    LSA_SAFE_CLEAR_FREE_STRING(pszPassword);
    LSA_SAFE_CLEAR_FREE_STRING(pszOldPassword);
    LSA_SAFE_FREE_STRING(pszLoginId);
    
    if (hLsaConnection != (HANDLE)NULL)
    {
        LsaCloseServer(hLsaConnection);
    }
    
    LSA_LOG_PAM_DEBUG("LsaPamUpdatePassword::end");
    
    return LsaPamMapErrorCode(dwError, pPamContext);
    
error:

    LSA_LOG_PAM_DEBUG("LsaPamUpdatePassword failed [code: %d]", dwError);

    goto cleanup;
}

int
LsaPamGetCurrentPassword(
    pam_handle_t* pamh,
    PPAMCONTEXT   pPamContext,
    PSTR*         ppszPassword 
    )
{
    DWORD dwError = 0;
    PSTR pszPassword = NULL;
    BOOLEAN bPrompt = TRUE;
    PPAMOPTIONS pPamOptions = &pPamContext->pamOptions;
    
    LSA_LOG_PAM_DEBUG("LsaPamGetCurrentPassword::begin");
    
    if (pPamOptions->bTryFirstPass ||
        pPamOptions->bUseFirstPass) {
        PCSTR pszItem = NULL;
            
        dwError = pam_get_item(
                        pamh,
                        PAM_AUTHTOK,
                        (PAM_GET_ITEM_TYPE)&pszItem);
        if (dwError != PAM_SUCCESS)
        {
            if (dwError == PAM_BAD_ITEM)
            {
                if (pPamOptions->bUseFirstPass)
                {
                    BAIL_ON_LSA_ERROR(dwError);
                } 
                else
                {
                    dwError = 0;
                }
            }
            else
            {
               BAIL_ON_LSA_ERROR(dwError);
            }
            
        }
        else if (!IsNullOrEmptyString(pszItem))
        {
            dwError = LsaAllocateString(pszItem, &pszPassword);
            BAIL_ON_LSA_ERROR(dwError);
            bPrompt = FALSE;
        }
        else if (pPamOptions->bUseFirstPass)
        {
            dwError = PAM_BAD_ITEM;
            BAIL_ON_LSA_ERROR(dwError);
        }
    }
    
    if (bPrompt) {
       
       dwError = LsaPamConverse(
                        pamh,
                        "Password:",
                        PAM_PROMPT_ECHO_OFF,
                        &pszPassword);
       BAIL_ON_LSA_ERROR(dwError);
       
       dwError = pam_set_item(
                       pamh,
                       PAM_AUTHTOK, 
                       (const void*) pszPassword);
       BAIL_ON_LSA_ERROR(dwError);

    }
    
    *ppszPassword = pszPassword;
    
cleanup:

    LSA_LOG_PAM_DEBUG("LsaPamGetCurrentPassword::end");
    
    return LsaPamMapErrorCode(dwError, pPamContext);
    
error:

    LSA_SAFE_CLEAR_FREE_STRING(pszPassword);
    
    *ppszPassword = NULL;
    
    LSA_LOG_PAM_DEBUG("LsaPamGetCurrentPassword failed [code: %d]", dwError);

    goto cleanup;
}

int
LsaPamGetOldPassword(
    pam_handle_t* pamh,
    PPAMCONTEXT pPamContext,
    PSTR* ppszPassword
    )
{
    int dwError = 0;
    PSTR pszPassword = NULL;
    BOOLEAN bPrompt = TRUE;
    PPAMOPTIONS pPamOptions = &pPamContext->pamOptions;
    
    LSA_LOG_PAM_DEBUG("LsaPamGetOldPassword::begin");
    
    if (pPamOptions->bTryFirstPass ||
        pPamOptions->bUseFirstPass)
    {
        PCSTR pszItem = NULL;

#if defined(__LWI_SOLARIS__) || defined(__LWI_HP_UX__)
        /* Solaris doesn't use PAM_OLDAUTHTOK, but we previously saved
           the password as PAM_LSASS_OLDAUTHTOK during pam_sm_authenticate,
           so we'll grab it from there */

        /* HP-UX likes to clear PAM_OLDAUTHTOK between the two phases
           of chauthtok, so we also grab our saved version in this case */
        dwError = pam_get_data(
            pamh,
            PAM_LSASS_OLDAUTHTOK,
            (PAM_GET_ITEM_TYPE)&pszItem);
#else
        dwError = pam_get_item(
            pamh,
            PAM_OLDAUTHTOK,
            (PAM_GET_ITEM_TYPE)&pszItem);
#endif
        if (dwError == PAM_BAD_ITEM ||
                IsNullOrEmptyString(pszItem))
        {
            if (pPamOptions->bUseFirstPass)
            {
                bPrompt = FALSE;
                dwError = LSA_ERROR_INVALID_PASSWORD;
                BAIL_ON_LSA_ERROR(dwError);
            }
            else if (pPamOptions->bTryFirstPass)
            {
                dwError = 0;
            }
        }
        else if (dwError != PAM_SUCCESS)
        {
           BAIL_ON_LSA_ERROR(dwError);
        }
        else
        {
            dwError = LsaAllocateString(pszItem, &pszPassword);
            BAIL_ON_LSA_ERROR(dwError);
            
            bPrompt = FALSE;
        }
    }
    
    if (bPrompt) {
           
       dwError = LsaPamConverse(
                            pamh,
                            "Current password:",
                            PAM_PROMPT_ECHO_OFF,
                            &pszPassword);
       BAIL_ON_LSA_ERROR(dwError);
           
       dwError = pam_set_item(
                       pamh,
                       PAM_OLDAUTHTOK,
                       (const void*) pszPassword);
       BAIL_ON_LSA_ERROR(dwError);       

#ifdef __LWI_HP_UX__
       /* HP-UX clears PAM_OLDAUTHTOK between the two phases of chauthtok, so
          save a copy of the old password where we can find it later */
       dwError = LsaPamSetDataString(pamh, PAM_LSASS_OLDAUTHTOK, pszPassword);
       BAIL_ON_LSA_ERROR(dwError);
#endif
    }
    
    *ppszPassword = pszPassword;
    
cleanup:

    LSA_LOG_PAM_DEBUG("LsaPamGetOldPassword::end");
    
    return LsaPamMapErrorCode(dwError, pPamContext);
    
error:

    LSA_SAFE_CLEAR_FREE_STRING(pszPassword);
    
    *ppszPassword = NULL;
    
    LSA_LOG_PAM_DEBUG("LsaPamGetOldPassword failed [code: %d]", dwError);

    goto cleanup;
}

int
LsaPamGetNewPassword(
    pam_handle_t* pamh,
    PPAMCONTEXT   pPamContext,
    PSTR*         ppszPassword
    )
{
    int dwError = PAM_SUCCESS;
    PSTR pszPassword_1 = NULL;
    PSTR pszPassword_2 = NULL;
    DWORD dwLen_1 = 0;
    DWORD dwLen_2 = 0;
    BOOLEAN bPrompt = TRUE;
    PPAMOPTIONS pPamOptions = &pPamContext->pamOptions;
    
    LSA_LOG_PAM_DEBUG("LsaPamGetNewPassword::begin");
    
    if (pPamOptions->bUseAuthTok) {
    
        PCSTR pszItem = NULL;
                    
        dwError = pam_get_item(
                    pamh,
                    PAM_AUTHTOK,
                    (PAM_GET_ITEM_TYPE)&pszItem);
        BAIL_ON_LSA_ERROR(dwError);
        
        if (!IsNullOrEmptyString(pszItem)) {
            dwError = LsaAllocateString(pszItem, &pszPassword_1);
            BAIL_ON_LSA_ERROR(dwError);
            
            bPrompt = FALSE;
        }
    }
    
    while (bPrompt)
    {
       dwError = LsaPamConverse(
                pamh,
                "New password:",
                PAM_PROMPT_ECHO_OFF,
                &pszPassword_1);
       BAIL_ON_LSA_ERROR(dwError);
    
       dwError = LsaPamConverse(
                pamh,
                "Re-enter password:",
                PAM_PROMPT_ECHO_OFF,
                &pszPassword_2);
       BAIL_ON_LSA_ERROR(dwError);
    
       // Will never get NULL on success.
       // It could be an empty string though.
       dwLen_1 = strlen(pszPassword_1);
       dwLen_2 = strlen(pszPassword_2);
    
       if ((dwLen_1 != dwLen_2) ||
           (strcmp(pszPassword_1, pszPassword_2) != 0))
       {    
           LsaPamConverse(pamh, "Passwords do not match", PAM_ERROR_MSG, NULL);
           dwError = PAM_SUCCESS;
           
           LSA_SAFE_CLEAR_FREE_STRING(pszPassword_1);
           LSA_SAFE_CLEAR_FREE_STRING(pszPassword_2);
          
       }
       else
       {    
           dwError = pam_set_item(
                       pamh,
                       PAM_AUTHTOK,
                       (const void*) pszPassword_1);
           BAIL_ON_LSA_ERROR(dwError);
           
           bPrompt = FALSE;   
       }
    }

    *ppszPassword = pszPassword_1;
    
cleanup:

    LSA_SAFE_CLEAR_FREE_STRING(pszPassword_2);
    
    LSA_LOG_PAM_DEBUG("LsaPamGetNewPassword::end");

    return LsaPamMapErrorCode(dwError, pPamContext);
    
error:

    LSA_SAFE_CLEAR_FREE_STRING(pszPassword_1);

    *ppszPassword = NULL;
    
    LSA_LOG_PAM_DEBUG("LsaPamGetNewPassword failed [code: %d]", dwError);

    goto cleanup;
}


/*
local variables:
mode: c
c-basic-offset: 4
indent-tabs-mode: nil
tab-width: 4
end:
*/
