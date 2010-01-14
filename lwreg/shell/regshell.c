/*
 * Copyright Likewise Software    2004-2009
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
 *       regshell.c
 *
 * Abstract:
 *
 *        Registry
 *
 *        Registry Shell application
 *
 * Authors: Adam Bernstein (abernstein@likewise.com)
 */

#include "regshell.h"
#include <locale.h>
#include <pwd.h>
#include "histedit.h"

#define REGSHELL_ESC_CHAR '|'

static int gCaughtSignal;

typedef struct _EDITLINE_CLIENT_DATA
{
    int continuation;
    PREGSHELL_PARSE_STATE pParseState;

    /* File name completion Data */
    PSTR pszCompletePrevCmd;
    PSTR pszCompletePrevArg;
    PSTR *ppszCompleteMatches;
    DWORD dwCompleteMatchesLen;
    DWORD dwEnteredTextLen;
} EDITLINE_CLIENT_DATA, *PEDITLINE_CLIENT_DATA;

void
pfnRegShellSignal(int signal)
{
    gCaughtSignal = signal;
}



PSTR
RegShellGetRootKey(
    PREGSHELL_PARSE_STATE pParseState)
{
    return pParseState->pszFullRootKeyName ?
               pParseState->pszFullRootKeyName :
               pParseState->pszDefaultRootKeyName;
}


PSTR
RegShellGetDefaultKey(
    PREGSHELL_PARSE_STATE pParseState)
{
    return pParseState->pszFullKeyPath ?
               pParseState->pszFullKeyPath :
               pParseState->pszDefaultKey;
}


DWORD
RegShellListKeys(
    PREGSHELL_PARSE_STATE pParseState,
    PREGSHELL_CMD_ITEM rsItem)
{
    DWORD dwError = 0;
    DWORD dwSubKeyLen = 0;
    DWORD i = 0;
    PSTR pszSubKey = NULL;
    LW_WCHAR **ppSubKeys = NULL;

    BAIL_ON_INVALID_HANDLE(pParseState);
    BAIL_ON_INVALID_HANDLE(pParseState->hReg);
    dwError = RegShellUtilGetKeys(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  RegShellGetDefaultKey(pParseState),
                  rsItem->keyName,
                  &ppSubKeys,
                  &dwSubKeyLen);
    BAIL_ON_REG_ERROR(dwError);

    for (i=0; i<dwSubKeyLen; i++)
    {
        dwError = LwRtlCStringAllocateFromWC16String(&pszSubKey, ppSubKeys[i]);
        BAIL_ON_REG_ERROR(dwError);

#ifndef _LW_DEBUG
        printf("[%s]\n", pszSubKey);
#else
        printf("SubKey %d name is '%s'\n", i, pszSubKey);
#endif
        LWREG_SAFE_FREE_STRING(pszSubKey);
    }
cleanup:
    if (ppSubKeys)
    {
        for (i=0; i<dwSubKeyLen && ppSubKeys[i]; i++)
        {
            LWREG_SAFE_FREE_MEMORY(ppSubKeys[i]);
        }
    }
    LWREG_SAFE_FREE_MEMORY(ppSubKeys);
    return dwError;

error:
    goto cleanup;
}


DWORD
RegShellAddKey(
    PREGSHELL_PARSE_STATE pParseState,
    PREGSHELL_CMD_ITEM rsItem)
{
    DWORD dwError = 0;

    BAIL_ON_INVALID_HANDLE(pParseState);
    BAIL_ON_INVALID_HANDLE(pParseState->hReg);

    dwError = RegShellUtilAddKey(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  RegShellGetDefaultKey(pParseState),
                  rsItem->keyName,
                  TRUE);

cleanup:
    return dwError;
error:
    goto cleanup;
}


DWORD
RegShellDeleteKey(
    PREGSHELL_PARSE_STATE pParseState,
    PREGSHELL_CMD_ITEM rsItem)
{
    DWORD dwError = 0;

    BAIL_ON_INVALID_HANDLE(pParseState);
    BAIL_ON_INVALID_HANDLE(pParseState->hReg);

    dwError = RegShellUtilDeleteKey(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  RegShellGetDefaultKey(pParseState),
                  rsItem->keyName);

cleanup:
    return dwError;
error:
    goto cleanup;
}


DWORD
RegShellDeleteTree(
    PREGSHELL_PARSE_STATE pParseState,
    PREGSHELL_CMD_ITEM rsItem)
{
    DWORD dwError = 0;
    PSTR pszKeyName = NULL;

    BAIL_ON_INVALID_HANDLE(pParseState);
    BAIL_ON_INVALID_HANDLE(pParseState->hReg);

    pszKeyName = rsItem->keyName;
    if (pszKeyName && pszKeyName[0] == '\\')
    {
        pszKeyName++;
    }

    dwError = RegShellUtilDeleteTree(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  RegShellGetDefaultKey(pParseState),
                  pszKeyName);

cleanup:
    return dwError;
error:
    goto cleanup;
}


DWORD
RegShellDeleteValue(
    PREGSHELL_PARSE_STATE pParseState,
    PREGSHELL_CMD_ITEM rsItem)
{
    DWORD dwError = 0;

    BAIL_ON_INVALID_HANDLE(pParseState);
    BAIL_ON_INVALID_HANDLE(pParseState->hReg);

    dwError = RegShellUtilDeleteValue(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  RegShellGetDefaultKey(pParseState),
                  rsItem->keyName,
                  rsItem->valueName);
cleanup:
    return dwError;

error:
    goto cleanup;
}


DWORD
RegShellSetValue(
    PREGSHELL_PARSE_STATE pParseState,
    PREGSHELL_CMD_ITEM rsItem)
{

    LW_PVOID data = NULL;
    DWORD dataLen = 0;
    DWORD dwError = 0;
    DWORD type = 0;
    PSTR pszKeyName = NULL;
    BOOLEAN bFullPath = FALSE;

    pszKeyName = rsItem->keyName;
    if (pszKeyName && pszKeyName[0] == '\\')
    {
        pszKeyName++;
        bFullPath = TRUE;
    }

    dwError = RegShellUtilGetValue(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  !bFullPath ? RegShellGetDefaultKey(pParseState) : NULL,
                  pszKeyName,
                  rsItem->valueName,
                  &type,
                  NULL,
                  NULL);
    if (rsItem->command == REGSHELL_CMD_SET_VALUE)
    {
        BAIL_ON_REG_ERROR(dwError);
    }
    else
    {
        /* Don't allow addition of existing value */
        if (dwError == 0)
        {
            dwError = LWREG_ERROR_DUPLICATE_KEYVALUENAME;
            BAIL_ON_REG_ERROR(dwError);

        }
        type = rsItem->type;
    }

    switch (type)
    {
        case REG_MULTI_SZ:
            data = rsItem->args;
            break;
        case REG_SZ:
            data = rsItem->args[0];
            break;
        case REG_DWORD:
        case REG_BINARY:
            data = rsItem->binaryValue;
            dataLen = rsItem->binaryValueLen;
            break;
        default:
            break;
    }
    dwError = RegShellUtilSetValue(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  RegShellGetDefaultKey(pParseState),
                  rsItem->keyName,
                  rsItem->valueName,
                  type,
                  data,
                  dataLen);
cleanup:
    return dwError;

error:
    goto cleanup;
}

DWORD
RegShellDumpByteArray(
    PBYTE pByteArray,
    DWORD dwByteArrayLen)
{
    DWORD i = 0;
    DWORD dwError = 0;

    BAIL_ON_INVALID_POINTER(pByteArray);

    for (i=0; i<dwByteArrayLen; i++)
    {
        printf("%02x%s", pByteArray[i], (i+1)<dwByteArrayLen ? "," : "");
    }

cleanup:
    return dwError;

error:
    goto cleanup;
}

DWORD
RegShellImportFile(
    HANDLE hReg,
    PREGSHELL_CMD_ITEM rsItem,
    REGSHELL_UTIL_IMPORT_MODE eMode)
{
    HANDLE parseH = NULL;
    DWORD dwError = 0;
    REGSHELL_UTIL_IMPORT_CONTEXT importCtx = {0};

    dwError = RegParseOpen(rsItem->args[0], NULL, NULL, &parseH);
    BAIL_ON_REG_ERROR(dwError);

    importCtx.hReg = hReg;
    importCtx.eImportMode = eMode;

    dwError = RegParseInstallCallback(
                  parseH,
                  RegShellUtilImportCallback,
                  &importCtx,
                  NULL);
    BAIL_ON_REG_ERROR(dwError);

    dwError = RegParseRegistry(parseH);
    BAIL_ON_REG_ERROR(dwError);

    RegParseClose(parseH);

cleanup:
    return dwError;

error:
    goto cleanup;
}


DWORD
RegShellExportFile(
    HANDLE hReg,
    PREGSHELL_CMD_ITEM rsItem
    )
{
    DWORD dwError = 0;
    DWORD dwSubKeysCount = 0;
    char szRegFileName[PATH_MAX + 1];
    FILE* fp = NULL;
    HKEY hSubKey = NULL;
    HKEY hRootKey = NULL;
    PSTR pszFullPath = NULL;
    PSTR pszRootFullPath = NULL;
    PSTR pszDefaultKey = NULL;
    PSTR pszRootKey = NULL;
    PSTR pszTemp = NULL;


    strcpy(szRegFileName, rsItem->args[0]);

    RegStripWhitespace(szRegFileName, TRUE, TRUE);

    fp = fopen(szRegFileName, "w");
    if (fp == NULL) {
        dwError = errno;
        goto error;
    }

    if (rsItem->keyName && *rsItem->keyName)
    {
        dwError = LwRtlCStringDuplicate(
                      &pszRootKey,
                      rsItem->keyName);
        BAIL_ON_REG_ERROR(dwError);
        pszTemp = strchr(pszRootKey, '\\');
        if (pszTemp)
        {
            *pszTemp = '\0';
        }
        dwError = RegOpenKeyExA(hReg,
                                NULL,
                                pszRootKey,
                                0,
                                KEY_READ,
                                &hRootKey);
        if (dwError)
        {
            dwError = RegOpenKeyExA(hReg,
                                    NULL,
                                    HKEY_THIS_MACHINE,
                                    0,
                                    KEY_READ,
                                    &hRootKey);
            BAIL_ON_REG_ERROR(dwError);
            LWREG_SAFE_FREE_STRING(pszRootKey);
            dwError = LwRtlCStringDuplicate(
                          &pszRootKey,
                          HKEY_THIS_MACHINE);
            BAIL_ON_REG_ERROR(dwError);
            pszDefaultKey = strchr(rsItem->keyName, '\\');
            if (pszDefaultKey)
            {
                pszDefaultKey++;
            }
            else
            {
                pszDefaultKey = rsItem->keyName;
            }
        }
        else
        {
            pszDefaultKey = pszTemp + 1;
        }
        BAIL_ON_REG_ERROR(dwError);

        dwError = RegShellCanonicalizePath(pszDefaultKey,
                                           NULL,
                                           &pszFullPath,
                                           NULL,
                                           NULL);
        BAIL_ON_REG_ERROR(dwError);

        if (pszFullPath && strcmp(pszFullPath, "\\") != 0)
        {
            dwError = RegOpenKeyExA(
                          hReg,
                          hRootKey,
                          pszFullPath+1,
                          0,
                          KEY_READ,
                          &hSubKey);
            BAIL_ON_REG_ERROR(dwError);

        }
        else
        {
            hSubKey = hRootKey;
            hRootKey = NULL;
        }
    }

    dwError = LwRtlCStringAllocatePrintf(
                  &pszRootFullPath,
                  "%s%s",
                  pszRootKey,
                  pszFullPath);
    BAIL_ON_REG_ERROR(dwError);
    dwError = RegQueryInfoKeyA(
                  hReg,
                  hSubKey,
                  NULL,
                  NULL,
                  NULL,
                  &dwSubKeysCount,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL,
                  NULL);

    dwError = RegShellUtilExport(hReg,
                                 fp,
                                 hSubKey,
                                 pszRootFullPath,
                                 dwSubKeysCount);
    BAIL_ON_REG_ERROR(dwError);

cleanup:
    if (hSubKey)
    {
        RegCloseKey(hReg, hSubKey);
    }
    if (hRootKey)
    {
        RegCloseKey(hReg, hRootKey);
    }
    LWREG_SAFE_FREE_STRING(pszRootKey);
    LWREG_SAFE_FREE_STRING(pszFullPath);
    LWREG_SAFE_FREE_STRING(pszRootFullPath);

    if (fp)
    {
        fclose(fp);
    }
    return dwError;

error:
    goto cleanup;
}

DWORD
RegShellListValues(
    PREGSHELL_PARSE_STATE pParseState,
    PREGSHELL_CMD_ITEM rsItem,
    PDWORD pdwValuesListed)
{
    DWORD dwError = 0;
    DWORD dwValuesLen = 0;
    DWORD i = 0;
    DWORD dwMultiIndex = 0;
    DWORD dwValueNameLenMax = 0;
    DWORD dwValueNameLen = 0;
    DWORD dwValue = 0;
    DWORD dwValuesListed = 0;
    DWORD dwEscapedValueLen = 0;
    PSTR pszValueName = NULL;
    PSTR *ppszMultiStrArray = NULL;
    PSTR pszEscapedValue = NULL;
    PBYTE pData = NULL;
    PREGSHELL_UTIL_VALUE pValues = NULL;

    dwError = RegShellUtilGetValues(
                  pParseState->hReg,
                  RegShellGetRootKey(pParseState),
                  RegShellGetDefaultKey(pParseState),
                  rsItem->keyName,
                  &pValues,
                  &dwValuesLen);
    BAIL_ON_REG_ERROR(dwError);

    for (i=0; i<dwValuesLen; i++)
    {
        dwValueNameLen = wc16slen(pValues[i].pValueName);
        if (dwValueNameLen>dwValueNameLenMax)
        {
            dwValueNameLenMax = dwValueNameLen;
        }
    }
    dwValueNameLenMax++;

    for (i=0; i<dwValuesLen; i++)
    {
        if (dwError == 0)
        {
            LWREG_SAFE_FREE_STRING(pszValueName);

            dwError = LwRtlCStringAllocateFromWC16String(&pszValueName, pValues[i].pValueName);
            BAIL_ON_REG_ERROR(dwError);

#ifdef _LW_DEBUG
            printf("ListValues: value='%s\n", pszValueName);
            printf("ListValues: dataLen='%d'\n", pValues[i].dwDataLen);
#endif

            if (strcmp(pszValueName, "@") == 0 &&
                *((PSTR) pValues[i].pData) == '\0')
            {
                continue;
            }
            dwValuesListed++;
            printf("  \"%s\"%*s",
                   pszValueName,
                   (int) (strlen(pszValueName)-dwValueNameLenMax),
                   "");
            switch (pValues[i].type)
            {
                case REG_SZ:
                    dwError = RegShellUtilEscapeString(
                                  pValues[i].pData,
                                  &pszEscapedValue,
                                  &dwEscapedValueLen);
                    BAIL_ON_REG_ERROR(dwError);
                    printf("REG_SZ          \"%s\"\n",
                           pszEscapedValue);
                    LWREG_SAFE_FREE_MEMORY(pszEscapedValue);
                    break;

                case REG_DWORD:
                    memcpy(&dwValue, pValues[i].pData, sizeof(DWORD));
                    printf("REG_DWORD       0x%08x (%u)\n", dwValue, dwValue);
                    break;

                case REG_BINARY:
                    printf("REG_BINARY      ");
                    RegShellDumpByteArray(pValues[i].pData,
                                          pValues[i].dwDataLen);
                    printf("\n");
                    break;

                case REG_MULTI_SZ:
                    dwError = RegByteArrayToMultiStrsA(
                                  pValues[i].pData,
                                  pValues[i].dwDataLen,
                                  &ppszMultiStrArray);
                    BAIL_ON_REG_ERROR(dwError);
                    if (!ppszMultiStrArray[0])
                    {
                        /* Just print the type for a reg_multi_sz with no values */
                        printf("%*sREG_MULTI_SZ\n",
                               dwMultiIndex == 0 ? 0 :
                                   dwValueNameLenMax + 2,
                                   "");
                    }
                    else
                    {
                        for (dwMultiIndex=0;
                             ppszMultiStrArray[dwMultiIndex];
                             dwMultiIndex++)
                        {
                            dwError = RegShellUtilEscapeString(
                                          ppszMultiStrArray[dwMultiIndex],
                                          &pszEscapedValue,
                                          &dwEscapedValueLen);
                            BAIL_ON_REG_ERROR(dwError);
                            printf("%*sREG_MULTI_SZ[%d] \"%s\"\n",
                                   dwMultiIndex == 0 ? 0 :
                                       dwValueNameLenMax + 4,
                                       "",
                                   dwMultiIndex,
                                   pszEscapedValue);
                            LWREG_SAFE_FREE_MEMORY(pszEscapedValue);

                        }
                    }
                    RegFreeMultiStrsA(ppszMultiStrArray);
                    ppszMultiStrArray = NULL;
                    break;

                default:
                    printf("no handler for datatype %d\n", pValues[i].type);
            }
            LWREG_SAFE_FREE_MEMORY(pData);
        }
    }
cleanup:
    LWREG_SAFE_FREE_MEMORY(pszEscapedValue);
    RegFreeMultiStrsA(ppszMultiStrArray);
    RegShellUtilValueArrayFree(pValues, dwValuesLen);
    if (pdwValuesListed)
    {
        *pdwValuesListed = dwValuesListed;
    }
    LWREG_SAFE_FREE_STRING(pszValueName);
    return dwError;

error:
    goto cleanup;
}


DWORD
RegShellProcessCmd(
    PREGSHELL_PARSE_STATE pParseState,
    DWORD argc,
    PSTR *argv)
{

    DWORD dwError = 0;
    DWORD dwOpenRootKeyError = 0;
    DWORD dwValuesListed = 0;
    PREGSHELL_CMD_ITEM rsItem = NULL;
    PCSTR pszErrorPrefix = NULL;
    PSTR pszPwd = NULL;
    PSTR pszToken = NULL;
    PSTR pszKeyName = NULL;
    PSTR pszNewKeyName = NULL;
    PSTR pszNewDefaultKey = NULL;
    PSTR pszStrtokState = NULL;
    PSTR pszRootKeyName = NULL;
    PSTR pszFullKeyName = NULL;
    BOOLEAN bChdirOk = TRUE;
    HKEY hRootKey = NULL;
    CHAR szError[128] = {0};

    dwError = RegShellCmdParse(pParseState, argc, argv, &rsItem);
    if (dwError == 0)
    {
#ifdef _LW_DEBUG
        RegShellDumpCmdItem(rsItem);
#endif
        switch (rsItem->command)
        {
            case REGSHELL_CMD_LIST_KEYS:
                pszErrorPrefix = "list_keys: failed ";
                dwError = RegShellListKeys(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_LIST:
            case REGSHELL_CMD_DIRECTORY:
                pszErrorPrefix = "list: failed ";
                pszRootKeyName = RegShellGetRootKey(pParseState);
                pszFullKeyName = RegShellGetDefaultKey(pParseState);
                if (pszRootKeyName)
                {
                    printf("\n[%s%s%s%s%s]\n",
                        pszRootKeyName ? pszRootKeyName : "",
                        pszFullKeyName ? "\\" : "",
                        pszFullKeyName ?  pszFullKeyName : "",
                        rsItem->keyName ? "\\" : "",
                        rsItem->keyName ?  rsItem->keyName : "\\");
                }

                if (pszRootKeyName)
                {
                    dwError = RegShellListValues(
                                  pParseState,
                                  rsItem,
                                  &dwValuesListed);
                    BAIL_ON_REG_ERROR(dwError);
                }
                if (dwValuesListed > 0)
                {
                    printf("\n");
                }
                dwError = RegShellListKeys(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                printf("\n");
                break;

            case REGSHELL_CMD_ADD_KEY:
                pszErrorPrefix = "add_key: failed";
                dwError = RegShellAddKey(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_DELETE_KEY:
                pszErrorPrefix = "delete_key: failed ";
                dwError = RegShellDeleteKey(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_DELETE_VALUE:
                pszErrorPrefix = "delete_value: failed ";
                dwError = RegShellDeleteValue(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_DELETE_TREE:
                pszErrorPrefix = "delete_tree: failed ";
                dwError = RegShellDeleteTree(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_SET_VALUE:
                pszErrorPrefix = "set_value: failed ";
                dwError = RegShellSetValue(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_ADD_VALUE:
                pszErrorPrefix = "add_value: failed ";
                dwError = RegShellSetValue(pParseState, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_HELP:
                RegShellUsage(argv[0]);
                break;

            case REGSHELL_CMD_CHDIR:
                dwError = LwRtlCStringDuplicate(&pszNewKeyName, &argv[2][1]);
                BAIL_ON_REG_ERROR(dwError);
                pszNewKeyName [strlen(pszNewKeyName) - 1] = '\0';
                pszKeyName = pszNewKeyName;
                pszToken = strtok_r(pszKeyName, "\\/", &pszStrtokState);
                if (!pszToken)
                {
                    /*
                     * Handle special case where the only thing provided in
                     * the path is one or more \ characters (e.g [\\\]). In
                     * this case, strtok_r() return NULL when parsing
                     * a non-zero length pszKeyName string. This is essentially
                     * the cd \ case
                     */
                    if (pParseState->pszDefaultKey)
                    {
                        LWREG_SAFE_FREE_MEMORY(pParseState->pszDefaultKey);
                        dwError = 0;
                        goto cleanup;
                    }
                    LWREG_SAFE_FREE_MEMORY(pParseState->pszDefaultRootKeyName);
                }


                if (pParseState->pszDefaultKey)
                {
                    /*
                     * Another special case when the path begins with a / or \.
                     * Force the current working directory to root.
                     */
                    if (pszNewKeyName[0] != '/' && pszNewKeyName[0] != '\\')
                    {
                        dwError = LwRtlCStringDuplicate(
                                      &pszNewDefaultKey,
                                      pParseState->pszDefaultKey);
                        BAIL_ON_REG_ERROR(dwError);
                    }
                }


                while (pszToken)
                {
                    pszKeyName = NULL;
                    dwOpenRootKeyError = RegOpenKeyExA(
                                  pParseState->hReg,
                                  NULL,
                                  pszToken,
                                  0,
                                  KEY_READ,
                                  &hRootKey);
                    if (dwOpenRootKeyError == 0)
                    {
                        RegCloseKey(pParseState->hReg, hRootKey);
                        LWREG_SAFE_FREE_MEMORY(pParseState->pszDefaultRootKeyName);
                        LWREG_SAFE_FREE_STRING(pParseState->pszDefaultKey);
                        LWREG_SAFE_FREE_STRING(pszNewDefaultKey);
                        dwError = LwRtlCStringDuplicate(
                                      &pParseState->pszDefaultRootKeyName,
                                      pszToken);
                        BAIL_ON_REG_ERROR(dwError);
                    }
                    else if (strcmp(pszToken, "..") == 0)
                    {
                        if (pszNewDefaultKey)
                        {
                            pszPwd = strrchr(pszNewDefaultKey,
                                             '\\');
                            if (pszPwd)
                            {
                                    pszPwd[0] = '\0';
                            }
                            else
                            {
                                pszPwd = strrchr(pszNewDefaultKey,
                                                 '/');
                                if (pszPwd)
                                {
                                    pszPwd[0] = '\0';
                                }
                                else
                                {
                                    LWREG_SAFE_FREE_MEMORY(pszNewDefaultKey);
                                }
                            }
                        }
                    }
                    else if (strcmp(pszToken, ".") == 0)
                    {
                        /* This is a no-op */
                    }
                    else if (strcmp(pszToken, "...") == 0)
                    {
                        /* This is a broken path! */
                        dwError = LWREG_ERROR_INVALID_CONTEXT;
                        BAIL_ON_REG_ERROR(dwError);

                    }
                    else if (pszNewDefaultKey)
                    {
                        /* Append this token to current relative path */
                        dwError = RegAllocateMemory(
                                      sizeof(*pszPwd) * (strlen(pszToken) +
                                          strlen(pszNewDefaultKey)+3),
                                      (PVOID*)&pszPwd);
                        BAIL_ON_REG_ERROR(dwError);

                        strcpy(pszPwd, pszNewDefaultKey);
                        strcat(pszPwd, "\\");
                        strcat(pszPwd, pszToken);
                        dwError = RegShellIsValidKey(
                                      pParseState->hReg,
                                      pParseState->pszDefaultRootKeyName,
                                      pszPwd);
                        if (dwError)
                        {
                            LwRegGetErrorString(dwError,
                                                szError,
                                                sizeof(szError)-1);
                            printf("cd: key not valid '%s' [%s]\n",
                                   pszToken, szError);
                            LWREG_SAFE_FREE_MEMORY(pszPwd);
                            pszPwd = NULL;
                            bChdirOk = FALSE;
                            dwError = 0;
                            break;
                        }
                        else
                        {
                            LWREG_SAFE_FREE_MEMORY(pszNewDefaultKey);
                            pszNewDefaultKey = pszPwd;
                            pszPwd = NULL;
                        }
                    }
                    else
                    {
                        dwError = RegShellIsValidKey(
                                      pParseState->hReg,
                                      pParseState->pszDefaultRootKeyName,
                                      pszToken);
                        if (dwError)
                        {
                            LwRegGetErrorString(dwError,
                                                szError,
                                                sizeof(szError)-1);
                            printf("cd: key not valid '%s' [%s]\n",
                                   pszToken, szError);

                            bChdirOk = FALSE;
                            dwError = 0;
                            break;
                        }
                        else
                        {
                            LWREG_SAFE_FREE_MEMORY(pszNewDefaultKey);
                            dwError = LwRtlCStringDuplicate(
                                          &pszNewDefaultKey,
                                          pszToken);
                            BAIL_ON_REG_ERROR(dwError);
                        }
                    }
                    pszToken = strtok_r(pszKeyName, "\\/", &pszStrtokState);
                }
                if (bChdirOk)
                {
                    if (pParseState->pszDefaultKey)
                    {
                        LWREG_SAFE_FREE_MEMORY(pParseState->pszDefaultKey);
                    }
                    pParseState->pszDefaultKey = pszNewDefaultKey;
                    pszNewDefaultKey = NULL;
                }
                else
                {
                    LWREG_SAFE_FREE_MEMORY(pszNewDefaultKey);
                }
                break;

            case REGSHELL_CMD_PWD:
                if (RegShellGetRootKey(pParseState))
                {
                    printf("[%s\\%s]\n\n",
                            RegShellGetRootKey(pParseState),
                            RegShellGetDefaultKey(pParseState) ?
                                RegShellGetDefaultKey(pParseState) : "");
                }
                else
                {
                    printf("'%s'\n\n", "\\");
                }
                break;

            case REGSHELL_CMD_QUIT:
                exit(0);
                break;

            case REGSHELL_CMD_LIST_VALUES:
                pszErrorPrefix = "list_values: failed ";
                dwError = RegShellListValues(pParseState, rsItem, NULL);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_IMPORT:
                dwError = RegShellImportFile(
                              pParseState->hReg,
                              rsItem,
                              REGSHELL_UTIL_IMPORT_OVERWRITE);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_UPGRADE:
                dwError = RegShellImportFile(
                              pParseState->hReg,
                              rsItem,
                              REGSHELL_UTIL_IMPORT_UPGRADE);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_EXPORT:
                dwError = RegShellExportFile(pParseState->hReg, rsItem);
                BAIL_ON_REG_ERROR(dwError);
                break;

            case REGSHELL_CMD_SET_HIVE:
                if (argc < 3)
                {
                    dwError = LWREG_ERROR_INVALID_CONTEXT;
                    goto error;
                }

                LWREG_SAFE_FREE_STRING(pParseState->pszDefaultRootKeyName);
                dwError = LwRtlCStringDuplicate(&pParseState->pszDefaultRootKeyName, argv[2]);
                BAIL_ON_REG_ERROR(dwError);
                LWREG_SAFE_FREE_STRING(pParseState->pszDefaultKey);
                break;

            default:
                break;
        }
    }
    else
    {
        printf("%s: error parsing command, invalid syntax\n", argv[0]);
        RegShellUsage(argv[0]);
    }

cleanup:
    RegShellCmdParseFree(rsItem);
    LWREG_SAFE_FREE_STRING(pszNewKeyName);
    LWREG_SAFE_FREE_STRING(pszNewDefaultKey);

    return dwError;

error:
    goto cleanup;
}


DWORD
RegShellInitParseState(
    PREGSHELL_PARSE_STATE *ppParseState)
{
    DWORD dwError = 0;
    PREGSHELL_PARSE_STATE pParseState = NULL;

    BAIL_ON_INVALID_POINTER(ppParseState);

    dwError = RegAllocateMemory(sizeof(*pParseState), (PVOID*)&pParseState);
    BAIL_ON_REG_ERROR(dwError);

    dwError = RegOpenServer(&pParseState->hReg);
    BAIL_ON_REG_ERROR(dwError);

    dwError = RegLexOpen(&pParseState->lexHandle);
    BAIL_ON_REG_ERROR(dwError);

    dwError = RegIOBufferOpen(&pParseState->ioHandle);
    BAIL_ON_REG_ERROR(dwError);

    *ppParseState = pParseState;

cleanup:
    return dwError;

error:
    LWREG_SAFE_FREE_STRING(pParseState->pszDefaultRootKeyName);
    RegCloseServer(pParseState->hReg);
    RegLexClose(pParseState->lexHandle);
    RegIOClose(pParseState->ioHandle);
    LWREG_SAFE_FREE_MEMORY(pParseState);
    goto cleanup;
}


DWORD
RegShellCloseParseState(
    PREGSHELL_PARSE_STATE pParseState)
{
    DWORD dwError = 0;
    BAIL_ON_INVALID_POINTER(pParseState);

    LWREG_SAFE_FREE_STRING(pParseState->pszDefaultRootKeyName);
    LWREG_SAFE_FREE_STRING(pParseState->pszDefaultKey);
    LWREG_SAFE_FREE_STRING(pParseState->pszDefaultKeyCompletion);
    LWREG_SAFE_FREE_STRING(pParseState->pszFullRootKeyName);
    LWREG_SAFE_FREE_STRING(pParseState->pszFullKeyPath);
    RegIOClose(pParseState->ioHandle);
    RegLexClose(pParseState->lexHandle);
    RegCloseServer(pParseState->hReg);
    LwRtlMemoryFree(pParseState);

cleanup:
    return dwError;

error:
    goto cleanup;
}


DWORD
RegShellStrcmpLen(
    PSTR pszMatchStr,
    PSTR pszHaystackStr,
    DWORD dwMatchMaxLen,
    PDWORD pdwExtentLen)
{
    DWORD index = 0;
    DWORD dwMaxMatch = 0;
    DWORD dwError = 0;

    BAIL_ON_INVALID_POINTER(pszMatchStr);
    BAIL_ON_INVALID_POINTER(pszHaystackStr);
    BAIL_ON_INVALID_POINTER(pdwExtentLen);

    dwMaxMatch = *pdwExtentLen;
    for (index = 0;
         pszMatchStr[index] &&
         pszHaystackStr[index] &&
         pszMatchStr[index] == pszHaystackStr[index] &&
         index < dwMaxMatch;
         index++)
    {
        ;
    }
    if (dwMatchMaxLen)
    {
        if (pszHaystackStr[dwMatchMaxLen] == '\0')
        {
            *pdwExtentLen = index;
        }
        else
        {
            dwError = LWREG_ERROR_INVALID_CONTEXT;
        }
    }
    else
    {
        *pdwExtentLen = index;
    }

cleanup:
    return dwError;

error:
    goto cleanup;
}



DWORD
RegShellCompletionMatch(
    PSTR pszMatchStr,
    PWSTR *ppSubKeys,
    DWORD dwSubKeyLen,
    PSTR pszDefaultRootKeyName,
    PSTR pszDefaultKey,
    PSTR **pppMatchArgs,
    PDWORD pdwMatchArgsLen,
    PDWORD pdwMatchCommonIndex,
    PDWORD pdwMatchCommonLen)
{
    DWORD dwError = 0;
    DWORD i = 0;
    DWORD dwMatchArgsLen = 0;
    DWORD dwMinCommonLen = INT32_MAX;
    DWORD dwMaxCommonLen = 0;
    DWORD dwPrevMaxCommonLen = 0;
    DWORD dwMinCommonLenIndex = 0;
    DWORD dwMaxCommonLenIndex = 0;
    DWORD dwMatchMaxLen = 0;
    DWORD dwStrLen = 0;
    BOOLEAN bBackslashEnd = FALSE;
    PSTR pszSubKey = NULL;

    PSTR *ppMatchArgs = NULL;
    PSTR pszPtr = NULL;


    if (dwSubKeyLen > 0)
    {
        dwError = RegAllocateMemory(sizeof(*ppMatchArgs) * dwSubKeyLen, (PVOID*)&ppMatchArgs);
        BAIL_ON_REG_ERROR(dwError);
    }

    dwStrLen = strlen(pszMatchStr);
    if (dwStrLen>0 && pszMatchStr[dwStrLen-1] == '\\')
    {
        dwMatchMaxLen = dwStrLen-1;
        pszMatchStr[dwMatchMaxLen] = '\0';
    }
    for (i=0; i<dwSubKeyLen; i++)
    {
        dwError = LwRtlCStringAllocateFromWC16String(&pszSubKey, ppSubKeys[i]);
        BAIL_ON_REG_ERROR(dwError);

        pszPtr = NULL;
        if (pszMatchStr && *pszMatchStr)
        {
            pszPtr = strstr(pszSubKey, pszMatchStr);
        }
        if (pszPtr && pszPtr == pszSubKey &&
            (dwMatchMaxLen == 0 || pszPtr[dwMatchMaxLen] == '\0'))
        {
            dwError = LwRtlCStringDuplicate(
                          &ppMatchArgs[dwMatchArgsLen], pszSubKey);
            BAIL_ON_REG_ERROR(dwError);
            dwStrLen = strlen(pszSubKey);
            if (dwStrLen < dwMinCommonLen)
            {
                dwMinCommonLen = dwStrLen;
                dwMinCommonLenIndex = dwMatchArgsLen;
            }
            dwMatchArgsLen++;
        }
        LWREG_SAFE_FREE_STRING(pszSubKey);
    }

    /*
     * Use dwMinCommonLenIndex as needle to find the longest
     * common string among the strings that matched pszMatchStr
     */
    dwStrLen = INT32_MAX;
    if (ppMatchArgs && ppMatchArgs[dwMinCommonLenIndex])
    {
        if (!bBackslashEnd)
        {
            dwMaxCommonLen = strlen(ppMatchArgs[dwMinCommonLenIndex]);
        }
    }
    for (i=0; i<dwMatchArgsLen; i++)
    {
        dwError = RegShellStrcmpLen(
                      ppMatchArgs[dwMinCommonLenIndex],
                      ppMatchArgs[i],
                      dwMatchMaxLen,
                      &dwMaxCommonLen);
        if (dwError == 0 && dwMaxCommonLen != dwPrevMaxCommonLen)
        {
            dwMaxCommonLenIndex = i;
            dwPrevMaxCommonLen = dwMaxCommonLen;
        }

    }
    *pppMatchArgs = ppMatchArgs;
    *pdwMatchArgsLen = dwMatchArgsLen;
    *pdwMatchCommonIndex = dwMaxCommonLenIndex;
    *pdwMatchCommonLen = dwPrevMaxCommonLen;
cleanup:
    return dwError;

error:
    RegShellCmdlineParseFree( dwMatchArgsLen, ppMatchArgs);
    goto cleanup;
}


unsigned char
pfnRegShellCompleteCallback(
    EditLine *el,
    int ch)
{
    const LineInfo *lineInfoCtx = el_line(el);
    PEDITLINE_CLIENT_DATA cldata = NULL;
    BOOLEAN bExactMatch = FALSE;
    BOOLEAN bBackslashEnd = FALSE;
    BOOLEAN bAllocArgs = FALSE;
    DWORD dwError = 0;
    DWORD dwSubKeyLen = 0;
    DWORD i = 0;
    DWORD dwLineLen = 0;
    DWORD dwArgPtrLen = 0;
    DWORD dwMatchArgsLen = 0;
    DWORD dwMatchBestIndex = 0;
    DWORD dwMatchBestLen = 0;
    DWORD dwStrLen = 0;
    LW_WCHAR **ppSubKeys = NULL;
    PSTR pszCurrentCmd = NULL;
    PSTR pszPtr = NULL;
    PSTR pszArgPtr = NULL;
    PSTR pszSubKey = NULL;
    PSTR *ppMatchArgs = NULL;
    PSTR pszBestMatchValue = NULL;
    PSTR pszFullMatchStr = NULL;

    dwError =  CC_ERROR;
    el_get(el, EL_CLIENTDATA, (void *) &cldata);
    BAIL_ON_INVALID_HANDLE(cldata);
    BAIL_ON_INVALID_HANDLE(cldata->pParseState);
    BAIL_ON_INVALID_HANDLE(cldata->pParseState->hReg);

    if (cldata->pParseState->pszDefaultKey &&
        !cldata->pParseState->pszDefaultKeyCompletion)
    {
        dwError = LwRtlCStringDuplicate(
                      &cldata->pParseState->pszDefaultKeyCompletion,
                      cldata->pParseState->pszDefaultKey);
        BAIL_ON_REG_ERROR(dwError);
    }

    dwLineLen = lineInfoCtx->cursor - lineInfoCtx->buffer;

    dwError = RegAllocateMemory(sizeof(*pszCurrentCmd) * (dwLineLen+1), (PVOID*)&pszCurrentCmd);
    BAIL_ON_REG_ERROR(dwError);

    strncat(pszCurrentCmd, lineInfoCtx->buffer, dwLineLen);

    /* Find end of current command */
    for (pszPtr = pszCurrentCmd; *pszPtr && !isspace((int) *pszPtr); pszPtr++)
        ;

    /* Find the start of the current key */
    for (;*pszPtr && isspace((int) *pszPtr); pszPtr++)
    {
        *pszPtr = '\0';
    }
    if (pszPtr && *pszPtr)
    {
        dwArgPtrLen = strlen(pszPtr);
        dwLineLen = pszPtr - pszCurrentCmd;
        pszArgPtr = pszCurrentCmd + dwLineLen;

        if (pszArgPtr[strlen(pszArgPtr)-1] != '\\')
        {
            pszPtr = strrchr(pszArgPtr, '\\');
            if (pszPtr)
            {
                pszArgPtr = pszPtr + 1;
            }
        }
        else
        {
            /*
             * This is not quite right here.
             * When user enters "cd a\b\c" then tries tab
             * completion, this code breaks.
             */
            pszPtr = strchr(pszArgPtr, '\\');
            if (pszPtr && pszPtr != &pszArgPtr[strlen(pszArgPtr)-1])
            {
                pszArgPtr = pszPtr + 1;
            }
            bBackslashEnd = TRUE;
        }

        dwStrLen = strlen(pszArgPtr) + 3;
        if (cldata->pParseState->pszDefaultRootKeyName)
        {
            dwStrLen += strlen(cldata->pParseState->pszDefaultRootKeyName);
        }
        if (cldata->pParseState->pszDefaultKeyCompletion)
        {
            dwStrLen += strlen(cldata->pParseState->pszDefaultKeyCompletion);
        }

        dwError = RegAllocateMemory(sizeof(*pszFullMatchStr) * dwStrLen, (PVOID*)&pszFullMatchStr);
        BAIL_ON_REG_ERROR(dwError);

        if (cldata->pParseState->pszDefaultRootKeyName)
        {
            strcat(pszFullMatchStr,
                   cldata->pParseState->pszDefaultRootKeyName);
            strcat(pszFullMatchStr, "\\");
        }
        if (cldata->pParseState->pszDefaultKeyCompletion)
        {
            strcat(pszFullMatchStr,
                   cldata->pParseState->pszDefaultKeyCompletion);
            strcat(pszFullMatchStr, "\\");
        }

        if (cldata->pParseState->pszDefaultKeyCompletion)
        {
            pszPtr = strstr(pszArgPtr,
                            cldata->pParseState->pszDefaultKeyCompletion);

        }
        else {
            pszPtr = NULL;
        }
        if (!pszPtr ||
            (pszPtr && pszPtr != pszArgPtr))
        {
            strcat(pszFullMatchStr, pszArgPtr);
        }
        pszArgPtr = pszFullMatchStr;
    }
    else
    {
        pszArgPtr = "";
    }

    /*
     * Something was looked up already by tab completion. Look to see if
     * any input has changed since the last list was presented. If not,
     * just display the existing list again.
     */
    if (cldata->pszCompletePrevCmd && cldata->pszCompletePrevArg &&
        strcmp(cldata->pszCompletePrevCmd,  pszCurrentCmd) == 0 &&
        strcmp(cldata->pszCompletePrevArg, pszArgPtr) == 0)
    {
        ppMatchArgs = cldata->ppszCompleteMatches;
        dwMatchArgsLen = cldata->dwCompleteMatchesLen;
        printf("\n");
        for (i=0; i<dwMatchArgsLen; i++)
        {
            pszPtr = strrchr(ppMatchArgs[i], '\\');
            if (pszPtr)
            {
                pszPtr++;
            }
            else
            {
                pszPtr = ppMatchArgs[i];
            }
            printf("%s\t", pszPtr);
        }

        putchar('\a');
        printf("\n");
        el_set(el, EL_REFRESH);
    }
    else
    {
        dwError = RegShellUtilGetKeys(
                      cldata->pParseState->hReg,
                      RegShellGetRootKey(cldata->pParseState),
                      cldata->pParseState->pszDefaultKeyCompletion,
                      NULL,
                      &ppSubKeys,
                      &dwSubKeyLen);
        BAIL_ON_REG_ERROR(dwError);
        bAllocArgs = TRUE;

        dwError = RegShellCompletionMatch(
                      pszArgPtr,
                      ppSubKeys,
                      dwSubKeyLen,
                      cldata->pParseState->pszDefaultRootKeyName,
                      cldata->pParseState->pszDefaultKeyCompletion,
                      &ppMatchArgs,
                      &dwMatchArgsLen,
                      &dwMatchBestIndex,
                      &dwMatchBestLen);
        /*
         * Match is ambiguous. Display the longest matches that are all common.
         */
        if (dwMatchArgsLen > 1)
        {
            if (pszArgPtr && *pszArgPtr)
            {
                dwStrLen = strlen(pszArgPtr);
                dwError = LwRtlCStringDuplicate(
                              &pszBestMatchValue,
                              ppMatchArgs[dwMatchBestIndex]);
                BAIL_ON_REG_ERROR(dwError);

                pszBestMatchValue[dwMatchBestLen] = '\0';
                if (pszBestMatchValue[dwStrLen] &&
                    el_insertstr(el, &pszBestMatchValue[dwStrLen]) == -1)
                {
                    printf("Oops: 1 el_insertstr failed\n");
                }
                else
                {
                    putchar('\a');
                }
            }
            else
            {
                /*
                 * No input provided, display all keys at this level
                 */
                putchar('\a');
                for (i=0; i<dwMatchArgsLen; i++)
                {
                    printf("%s\t", ppMatchArgs[i]);
                }
                printf("\n");
            }

        }
        else if (dwMatchArgsLen == 1 ||
                 (dwSubKeyLen == 1 &&
                  (dwArgPtrLen-cldata->dwEnteredTextLen) == 0))
        {
            if (dwMatchArgsLen == 1)
            {
                dwStrLen = strlen(pszArgPtr);
                /*
                 * Completion is unique. add to command line.
                 */
                if (ppMatchArgs[i][dwStrLen] &&
                    el_insertstr(el, &ppMatchArgs[i][dwStrLen]) == -1)
                {
                    printf("Oops: 2 el_insertstr failed\n");
                }
                else if (!bBackslashEnd)
                {
                    /* No string to append */
                    if (el_insertstr(el, "\\") == -1)
                    {
                        printf("Oops: 3 el_insertstr failed\n");
                    }
                }
            }
            else
            {
                dwError = LwRtlCStringAllocateFromWC16String(&pszSubKey, ppSubKeys[0]);
                BAIL_ON_REG_ERROR(dwError);

                /*
                 * Completion is unique. add to command line.
                 */
                pszPtr = strrchr(pszSubKey, '\\');
                if (pszPtr)
                {
                    pszPtr++;

                }
                else
                {
                    pszPtr = pszSubKey;
                }

                if (el_insertstr(el, pszPtr) == -1)
                {
                    printf("Oops: 2 el_insertstr failed\n");
                }
                /* No string to append */
                if (el_insertstr(el, "\\") == -1)
                {
                    printf("Oops: 3 el_insertstr failed\n");
                }
            }

            if (ppMatchArgs[i])
            {
                pszPtr = strchr(ppMatchArgs[i], '\\');
                if (pszPtr)
                {
                    pszPtr++;
                }
                else
                {
                    pszPtr = ppMatchArgs[i];
                }
            }
            else
            {
                pszPtr = strchr(pszSubKey, '\\');
                if (pszPtr)
                {
                    pszPtr++;
                }
                else
                {
                    pszPtr = pszSubKey;
                }
            }

            LWREG_SAFE_FREE_STRING(cldata->pParseState->pszDefaultKeyCompletion);
            dwError = LwRtlCStringDuplicate(
                          &cldata->pParseState->pszDefaultKeyCompletion,
                          pszPtr);
            BAIL_ON_REG_ERROR(dwError);
            LWREG_SAFE_FREE_STRING(cldata->pszCompletePrevCmd);
            LWREG_SAFE_FREE_STRING(cldata->pszCompletePrevArg);
            for (i=0;
                 cldata->ppszCompleteMatches && i<cldata->dwCompleteMatchesLen;
                 i++)
            {
                LWREG_SAFE_FREE_STRING(cldata->ppszCompleteMatches[i]);
            }
            LWREG_SAFE_FREE_MEMORY(cldata->ppszCompleteMatches);
            cldata->dwCompleteMatchesLen = 0;
            cldata-> dwEnteredTextLen = strlen(pszPtr) + 1;
            bExactMatch = TRUE;
            dwError = CC_REFRESH;
        }
        else
        {
            printf("\a\n");
            for (i=0; i<dwSubKeyLen; i++)
            {
                dwError = LwRtlCStringAllocateFromWC16String(&pszSubKey, ppSubKeys[i]);
                BAIL_ON_REG_ERROR(dwError);

                pszPtr = strrchr(pszSubKey, '\\');
                if (pszPtr)
                {
                    pszPtr++;
                }
                else
                {
                    pszPtr = pszSubKey;
                }
                printf("%s\t", pszPtr);
                LWREG_SAFE_FREE_STRING(pszSubKey);
            }
            printf("\n");
            el_set(el, EL_REFRESH);
        }
    }


cleanup:
    for (i=0; ppSubKeys && i<dwSubKeyLen; i++)
    {
        LWREG_SAFE_FREE_MEMORY(ppSubKeys[i]);
    }
    LWREG_SAFE_FREE_MEMORY(ppSubKeys);

    if (!bExactMatch)
    {
        LWREG_SAFE_FREE_STRING(cldata->pszCompletePrevCmd);
        cldata->pszCompletePrevCmd = pszCurrentCmd;
        pszCurrentCmd = NULL;
        LWREG_SAFE_FREE_STRING(cldata->pszCompletePrevArg);
        cldata->pszCompletePrevArg = pszBestMatchValue;
        pszBestMatchValue = NULL;

        /* Free previous command line entered */
        if (bAllocArgs)
        {
            RegShellCmdlineParseFree(cldata->dwCompleteMatchesLen,
                                     cldata->ppszCompleteMatches);
        }
        cldata->ppszCompleteMatches = ppMatchArgs;
        cldata->dwCompleteMatchesLen = dwMatchArgsLen;
    }
    else {
        RegShellCmdlineParseFree(dwMatchArgsLen,
                                 ppMatchArgs);
    }

    LWREG_SAFE_FREE_MEMORY(pszFullMatchStr);
    LWREG_SAFE_FREE_STRING(pszSubKey);
    LWREG_SAFE_FREE_STRING(pszCurrentCmd);
    LWREG_SAFE_FREE_STRING(pszBestMatchValue);
    return dwError;

error:
    goto cleanup;
}


static char *
pfnRegShellPromptCallback(EditLine *el)
{
    static char promptBuf[1024] = "";
    EDITLINE_CLIENT_DATA *cldata = NULL;

    el_get(el, EL_CLIENTDATA, (void *) &cldata);
    snprintf(promptBuf, sizeof(promptBuf), "%s%s%s%s ",
             cldata->pParseState->pszDefaultRootKeyName ?
                 cldata->pParseState->pszDefaultRootKeyName : "",
             cldata->pParseState->pszDefaultKey ? "\\" : "",
             cldata->pParseState->pszDefaultKey ?
             cldata->pParseState->pszDefaultKey : "\\",
             cldata->continuation ? ">>>" : ">");
    return promptBuf;
}


DWORD
RegShellExecuteCmdLine(
    PREGSHELL_PARSE_STATE pParseState,
    PSTR pszCmdLine,
    DWORD dwCmdLineLen)
{
    DWORD dwError = 0;
    DWORD dwNewArgc = 0;
    PSTR *pszNewArgv = NULL;

    dwError = RegIOBufferSetData(
                  pParseState->ioHandle,
                  pszCmdLine,
                  dwCmdLineLen);
    BAIL_ON_REG_ERROR(dwError);

    dwError = RegShellCmdlineParseToArgv(
                  pParseState,
                  &dwNewArgc,
                  &pszNewArgv);
    if (dwError == 0)
    {
        dwError = RegShellProcessCmd(pParseState,
                                     dwNewArgc,
                                     pszNewArgv);
    }
    if (dwError)
    {
        RegPrintError("regshell", dwError);
        dwError = 0;
    }
    RegShellCmdlineParseFree(dwNewArgc, pszNewArgv);
    LWREG_SAFE_FREE_STRING(pParseState->pszFullRootKeyName);
    pParseState->pszFullKeyPath = NULL;

cleanup:
    return dwError;

error:
    goto cleanup;

}


void
RegShellHandleSignalEditLine(int *signal, void *ctx)
{
#ifdef SIGWINCH
    if (*signal == SIGWINCH)
    {
        el_set((EditLine *) ctx, EL_REFRESH);
    }
#endif
    *signal = 0;
}


DWORD
RegShellProcessInteractiveEditLine(
    FILE *readFP,
    PREGSHELL_PARSE_STATE pParseState,
    PSTR pszProgramName)
{
    History *hist = NULL;
    HistEvent ev;
    EDITLINE_CLIENT_DATA el_cdata = {0};
    int num = 0;
    int ncontinuation = 0;
    int rv = 0;

    const char *buf = NULL;
    EditLine *el = NULL;
    BOOLEAN bHistFirst = FALSE;
    DWORD dwError = 0;
    DWORD i = 0;
    DWORD dwCmdLineLen = 0;
    DWORD dwEventNum = 0;
    PSTR pszCmdLine = NULL;
    PSTR pszNewCmdLine = NULL;
    PSTR pszNumEnd = NULL;
    PSTR pszHistoryFileDir = NULL;
    PSTR pszHistoryFileName = NULL;
    const char *hist_str = NULL;
    struct passwd *userPwdEntry = NULL;

    hist = history_init();
    history(hist, &ev, H_SETSIZE, 100);

    el = el_init(pszProgramName, stdin, stdout, stderr);

    /* Make configurable in regshellrc file */
    el_set(el, EL_EDITOR, "emacs");

    /* Signal handling in editline seems not to function... */
    el_set(el, EL_SIGNAL, 0);

    /* Set escape character from \ to | */
    el_set(el, EL_ESC_CHAR, (int) REGSHELL_ESC_CHAR);

    /* Editline prompt function; display info from pParseState */
    el_set(el, EL_PROMPT, pfnRegShellPromptCallback);

    /* Set regshell context */
    el_cdata.pParseState = pParseState;
    el_set(el, EL_CLIENTDATA, (void *) &el_cdata);

    /* Setup history context, and load previous history file */
    el_set(el, EL_HIST, history, hist);

    /* Build fully qualified path for history file */
    userPwdEntry = getpwuid(getuid());
    if (userPwdEntry)
    {
        pszHistoryFileDir = userPwdEntry->pw_dir;
    }
    if (!pszHistoryFileDir)
    {
        pszHistoryFileDir = "/tmp";
    }

    dwError = RegAllocateMemory(
                  strlen(pszHistoryFileDir) + sizeof("/.regshell_history"),
                  (PVOID) &pszHistoryFileName);
    BAIL_ON_REG_ERROR(dwError);

    strcpy(pszHistoryFileName, pszHistoryFileDir);
    strcat(pszHistoryFileName, "/.regshell_history");

    /* Retrieve this from user's home directory */
    history(hist, &ev, H_LOAD, pszHistoryFileName);

    /*
     * Bind j, k in vi command mode to previous and next line, instead
     * of previous and next history.
     */
    el_set(el, EL_BIND, "-a", "k", "ed-prev-line", NULL);
    el_set(el, EL_BIND, "-a", "j", "ed-next-line", NULL);

    /*
     * Register complete function callback
     */
    el_set(el,
           EL_ADDFN,
           "ed-complete",
           "Complete argument",
           pfnRegShellCompleteCallback);
    el_set(el, EL_BIND, "^I", "ed-complete", NULL);

    /*
     * Source the user's defaults file.
     */
    el_source(el, NULL);

    while ((buf = el_gets(el, &num))!=NULL && num!=0)
    {
        if (gCaughtSignal > 0)
        {
            RegShellHandleSignalEditLine(&gCaughtSignal, (LW_PVOID) el);
        }
        if (num>1 && buf[num-2] == REGSHELL_ESC_CHAR)
        {
            ncontinuation = 1;
        }
        else
        {
            ncontinuation = 0;
        }

#if 1 /* This mess needs to be put into a completion cleanup function */
        LWREG_SAFE_FREE_STRING(el_cdata.pParseState->pszDefaultKeyCompletion);
        for (i=0; el_cdata.ppszCompleteMatches && i<el_cdata.dwCompleteMatchesLen; i++)
        {
            LWREG_SAFE_FREE_STRING(el_cdata.ppszCompleteMatches[i]);
        }
        LWREG_SAFE_FREE_MEMORY(el_cdata.ppszCompleteMatches);
        LWREG_SAFE_FREE_STRING(el_cdata.pszCompletePrevCmd);
        el_cdata.dwEnteredTextLen = 0;
#endif


        el_cdata.continuation = ncontinuation;

        dwError = RegAllocateMemory(
                      sizeof(*pszNewCmdLine) * (dwCmdLineLen + num + 1),
                      (PVOID*)&pszNewCmdLine);
        BAIL_ON_REG_ERROR(dwError);

        if (pszCmdLine)
        {
            strncat(pszNewCmdLine, pszCmdLine, dwCmdLineLen);
            LWREG_SAFE_FREE_STRING(pszCmdLine);
        }
        if (ncontinuation)
        {
            num -= 2;
        }
        strncat(&pszNewCmdLine[dwCmdLineLen], buf, num);
        dwCmdLineLen += num;
        pszCmdLine = pszNewCmdLine;
        pszNewCmdLine = NULL;
        if (ncontinuation)
        {
            ncontinuation = 0;
            continue;
        }


        /*
         * Process history command recall (!nnn | !command syntax)
         */
        if (pszCmdLine[0] =='!')
        {
            /* !nnn case */
            if (isdigit((int)pszCmdLine[1]))
            {
                dwEventNum = strtol(&pszCmdLine[1], &pszNumEnd, 0);
                rv = history(hist, &ev, H_NEXT_EVENT, dwEventNum);
                if (rv == -1)
                {
                    printf("regshell: %d: event not found\n", dwEventNum);
                }
            }
            else
            {
                /*
                 * Handle !! command recall. !! recalls previous command,
                 * !!stuff appends "stuff" to end of previous command.
                 */
                if (pszCmdLine[1] == '!')
                {
                    rv = history(hist, &ev, H_FIRST);
                    if (rv == -1)
                    {
                        printf("regshell: !!: event not found\n");
                    }
                    else
                    {
                        bHistFirst = TRUE;
                    }
                }
                else
                {
                    /*
                     * !command case. Searches history for last occurrence
                     * of "command" in history list, and expands command line
                     * with that command.
                     */
                    hist_str = &pszCmdLine[1];
                    rv = history(hist, &ev, H_PREV_STR, hist_str);
                    if (rv == -1)
                    {
                        printf("regshell: %s: event not found\n", hist_str);
                    }
                }
            }

            if (rv == 0)
            {
                dwCmdLineLen = 0;

                if (bHistFirst)
                {
                    dwCmdLineLen += strlen(&pszCmdLine[2]);
                }
                else if (pszNumEnd)
                {
                    dwCmdLineLen += strlen(pszNumEnd);
                }
                dwCmdLineLen += strlen(ev.str);
                dwCmdLineLen++;

                dwError = RegAllocateMemory(
                              sizeof(*pszNewCmdLine) * dwCmdLineLen,
                              (PVOID*)&pszNewCmdLine);
                BAIL_ON_REG_ERROR(dwError);

                strcpy(pszNewCmdLine, ev.str);
                dwCmdLineLen = strlen(pszNewCmdLine);
                if (pszNewCmdLine[dwCmdLineLen-1] == '\n')
                {
                    pszNewCmdLine[dwCmdLineLen-1] = '\0';
                }

                if (bHistFirst)
                {
                    strcat(pszNewCmdLine, &pszNewCmdLine[2]);
                }
                else if (pszNumEnd)
                {
                    strcat(pszNewCmdLine, pszNumEnd);
                }
                dwCmdLineLen = strlen(pszNewCmdLine);
                LWREG_SAFE_FREE_STRING(pszCmdLine);
                pszCmdLine = pszNewCmdLine;
                pszNewCmdLine = NULL;

                /*
                 * Display command from history list
                 */
                printf("%s\n", pszCmdLine);
            }
        }

        if (pszCmdLine && pszCmdLine[0] != '\n')
        {
            rv = history(hist, &ev, H_ENTER, pszCmdLine);
            if (strcmp(pszCmdLine, "history\n") == 0)
            {
                for (rv = history(hist, &ev, H_LAST);
                     rv != -1;
                     rv = history(hist, &ev, H_PREV))
                {
                    fprintf(stdout, "%4d %s", ev.num, ev.str);
                }
            }
            else
            {
                dwError = RegShellExecuteCmdLine(
                              pParseState,
                              pszCmdLine,
                              dwCmdLineLen);
            }
        }
        LWREG_SAFE_FREE_STRING(pszCmdLine);
        dwCmdLineLen = 0;
    }

    /* Save current regshell history */
    history(hist, &ev, H_SAVE, pszHistoryFileName);

cleanup:
    LWREG_SAFE_FREE_STRING(pszHistoryFileName);
    el_end(el);
    history_end(hist);
    return dwError;

error:
    goto cleanup;
}



DWORD
RegShellProcessInteractive(
    FILE *readFP,
    PREGSHELL_PARSE_STATE parseState)
{
    CHAR cmdLine[8192] = {0};
    PSTR pszCmdLine = NULL;
    DWORD dwError = 0;
    PSTR pszTmpStr = NULL;
    BOOLEAN bDoPrompt = TRUE;
    BOOLEAN bFoundComment = FALSE;

    /* Interactive shell */
    do
    {
        if (bDoPrompt)
        {
            printf("%s%s%s> ",
                   parseState->pszDefaultRootKeyName ?
                       parseState->pszDefaultRootKeyName : "\\",
                   parseState->pszDefaultKey ? "\\" : "",
                   parseState->pszDefaultKey ?
                   parseState->pszDefaultKey : "\\");
            fflush(stdout);
        }
        pszCmdLine = fgets(cmdLine, sizeof(cmdLine)-1, readFP);
        if (pszCmdLine)
        {
            if (strlen(cmdLine) == 1)
            {
                if (readFP != stdin)
                {
                    bDoPrompt = FALSE;
                }
                continue;
            }

            /* Ignore leading white space or # comment on lines */
            for (pszTmpStr=cmdLine;
                 (int)*pszTmpStr && isspace((int)*pszTmpStr);
                 pszTmpStr++)
            {
                ;
            }

            if (pszTmpStr && *pszTmpStr == '#')
            {
                printf("%s%s", bFoundComment ? "" : "\n", cmdLine);
                bDoPrompt = FALSE;
                bFoundComment = TRUE;
                continue;
            }

            if (cmdLine[strlen(cmdLine)-1] == '\n')
            {
               cmdLine[strlen(cmdLine)-1] = '\0';
            }
            if (readFP != stdin)
            {
                printf("%s\n", cmdLine);
            }
            bDoPrompt = TRUE;
            bFoundComment = FALSE;

            dwError = RegShellExecuteCmdLine(
                          parseState,
                          cmdLine,
                          strlen(cmdLine));
            BAIL_ON_REG_ERROR(dwError);
        }
    } while (!feof(readFP));
cleanup:
    return dwError;

error:
    if (dwError)
    {
        RegPrintError("regshell", dwError);
    }
    goto cleanup;
}


int main(int argc, char *argv[])
{
    DWORD dwError = 0;
    PSTR pszInFile = NULL;
    PREGSHELL_PARSE_STATE parseState = NULL;
    FILE *readFP = stdin;
    DWORD indx = 0;
    struct sigaction action;

    setlocale(LC_ALL, "");
    dwError = RegShellInitParseState(&parseState);
    BAIL_ON_REG_ERROR(dwError);


    memset(&action, 0, sizeof(action));
    action.sa_handler = pfnRegShellSignal;
    action.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &action, NULL) < 0)
    {
        dwError = RegMapErrnoToLwRegError(errno);
        BAIL_ON_REG_ERROR(dwError);
    }

#ifdef SIGWINCH
    if (sigaction(SIGWINCH, &action, NULL) < 0)
    {
        dwError = RegMapErrnoToLwRegError(errno);
        BAIL_ON_REG_ERROR(dwError);
    }
#endif

    indx = 1;
    while (argc>1 && argv[indx][0] == '-')
    {
        if (strcmp(argv[indx], "--file") == 0 ||
            strcmp(argv[indx], "-f") == 0)
        {
            argc--;
            indx++;
            if (argc>1)
            {
                pszInFile = argv[indx++];
                argc--;
            }
            if (pszInFile)
            {
                readFP = fopen(pszInFile, "r");
                if (!readFP)
                {
                    fprintf(stderr, "Error opening file '%s'\n", pszInFile);
                    dwError = RegMapErrnoToLwRegError(errno);
                    BAIL_ON_REG_ERROR(dwError);
                }
            }
            dwError = RegShellProcessInteractive(readFP, parseState);
            BAIL_ON_REG_ERROR(dwError);
            if (readFP != stdin)
            {
                fclose(readFP);
            }
            dwError = 0;
            goto cleanup;
        }
        else if (strcmp(argv[indx], "--help") == 0 ||
            strcmp(argv[indx], "-h") == 0)
        {
            RegShellUsage(argv[0]);
            dwError = 0;
            goto cleanup;
        }
        else if (argv[indx][0] == '-')
        {
            printf("ERROR: unknown option %s\n", argv[indx]);
            RegShellUsage(argv[0]);
            dwError = 0;
            goto cleanup;
        }
    }

    if (argc == 1)
    {
        dwError = RegShellProcessInteractiveEditLine(
                      readFP,
                      parseState,
                      argv[0]);
        BAIL_ON_REG_ERROR(dwError);
    }
    else
    {
        dwError = RegShellProcessCmd(parseState, argc, argv);
        BAIL_ON_REG_ERROR(dwError);
    }

cleanup:
    RegShellCloseParseState(parseState);
    return dwError;

error:
    if (dwError)
    {
        RegPrintError("regshell", dwError);
    }
    goto cleanup;
}
