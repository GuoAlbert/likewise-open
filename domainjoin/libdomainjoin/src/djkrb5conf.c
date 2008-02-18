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

#include "domainjoin.h"
#include "ctarray.h"
#include "ctstrutils.h"
#include "djstr.h"
#include "djauthinfo.h"

#define GCE(x) GOTO_CLEANUP_ON_CENTERROR((x))

/*Krb5 configuration files follow a modified ini file format. In it there are several stanzas:
[logging]

[realms]

Each stanza can contain name = value pairs. There can be multiple names in the same stanza:
[stanza]
name1 = value
name2 = value
name2 = value

Stanzas can also contain compound elements which contain more compound elements or name value pairs:
[stanza]
name1 = {
subelement1 = value
subelement2 = value
}

Comments are started with a # or a ;. They must be on lines by themselves. So there cannot be a comment on a name value line.
[stanza]
#comment
;comment
name = value ;not a comment

This file format is parsed as nested Krb5Entrys. The top most entry is the root node. It contains stanzas as subelements. The stanzas contain name value pairs and compound elements.

Comment lines can also be included in any of the entry types. Comments have no children, no name, no beginSeparator, and no value. The comment mark and value are stored in the leadingWhiteSpace variable.

Name value pairs can be distinguished from compound elements because name value pairs are leaf entries (have no children).

Stanzas can have no children, so they must be distinguished by their level in the hierarchy. Stanzas are always at the second level.

A parsed name value pair can be written out in the following form (spaces are added for readability and should not be written):
<leadingWhiteSpace> <name> <beginSeparator (is =)> <value>

A parsed compound element can be written out as:
<leadingWhiteSpace> <name> <beginSeparator (is = {)> <newline>
<subelements>
<leadingWhiteSpace> <value (is })>
*/

typedef struct _Krb5Entry
{
    struct _Krb5Entry *parent;
    char *leadingWhiteSpace;
    CTParseToken name;
    CTParseToken beginSeparator;
    DynamicArray subelements;
    CTParseToken value;
} Krb5Entry;

typedef BOOLEAN (*NodeClassifier)(const Krb5Entry *entry);

static BOOLEAN IsStanzaEntry(const Krb5Entry *entry)
{
    //Stanzas have a name, but no beginning separator or value
    return entry->name.value != NULL &&
        entry->beginSeparator.value == NULL &&
        entry->value.value == NULL;
}

static BOOLEAN IsGroupEntry(const Krb5Entry *entry)
{
    return CTStrEndsWith(entry->beginSeparator.value, "{");
}

static BOOLEAN IsValueEntry(const Krb5Entry *entry)
{
    return entry->beginSeparator.value != NULL &&
        !strcmp(entry->beginSeparator.value, "=");
}

static BOOLEAN IsCommentEntry(const Krb5Entry *entry)
{
    return entry->name.value == NULL &&
        entry->beginSeparator.value == NULL &&
        entry->value.value == NULL &&
        entry->subelements.size == 0;
}

static BOOLEAN IsRootEntry(const Krb5Entry *entry)
{
    return entry->parent == NULL;
}

static Krb5Entry * GetChild(Krb5Entry *entry, size_t child)
{
    if(child >= entry->subelements.size)
        return NULL;
    return ((Krb5Entry **)entry->subelements.data)[child];
}

static const Krb5Entry * GetChildConst(const Krb5Entry *entry, size_t child)
{
    if(child >= entry->subelements.size)
        return NULL;
    return ((const Krb5Entry **)entry->subelements.data)[child];
}

static CENTERROR InsertChildNode(Krb5Entry *parent, size_t index, Krb5Entry *child)
{
    child->parent = parent;
    return CTArrayInsert(&parent->subelements, index, sizeof(child),
            &child, 1);
}

static CENTERROR AddChildNode(Krb5Entry *parent, Krb5Entry *child)
{
    //If the new node is a comment, add it at the bottom, otherwise add it at
    //the bottom before the existing comments (mostly blank lines).
    size_t index = parent->subelements.size;
    const char *parentName = parent->name.value;
    const char *childName = child->name.value;
    if(parentName == NULL)
        parentName = "(null)";
    if(childName == NULL)
        childName = "(null)";
    DJ_LOG_VERBOSE("Adding child '%s' to '%s'", childName, parentName);
    if(!IsCommentEntry(child))
    {
        while(index > 0)
        {
            Krb5Entry *child = GetChild(parent, index - 1);
            if(!IsCommentEntry(child))
                break;
            index--;
        }
    }
    return InsertChildNode(parent, index, child);
}

static void FreeKrb5Entry(Krb5Entry **entry);

static void DeleteAllChildren(Krb5Entry *entry)
{
    size_t i;
    for(i = 0; i < entry->subelements.size; i++)
    {
        Krb5Entry *child = GetChild(entry, i);
        FreeKrb5Entry(&child);
    }
    CTArrayFree(&entry->subelements);
}

static void FreeKrb5EntryContents(Krb5Entry *entry)
{
    DeleteAllChildren(entry);
    CT_SAFE_FREE_STRING(entry->leadingWhiteSpace);
    CTFreeParseTokenContents(&entry->name);
    CTFreeParseTokenContents(&entry->beginSeparator);
    CTFreeParseTokenContents(&entry->value);
}

static void FreeKrb5Entry(Krb5Entry **entry)
{
    if(*entry != NULL)
    {
        FreeKrb5EntryContents(*entry);
        CT_SAFE_FREE_MEMORY(*entry);
    }
}

static CENTERROR CopyEntry(const Krb5Entry *source, Krb5Entry **copy)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *created = NULL;
    Krb5Entry *copiedChild = NULL;
    size_t i;
    GCE(ceError = CTAllocateMemory(sizeof(*created), (void **)&created));
    GCE(ceError = CTDupOrNullStr(source->leadingWhiteSpace, &created->leadingWhiteSpace));
    GCE(ceError = CTCopyTokenContents(&created->name, &source->name));
    GCE(ceError = CTCopyTokenContents(&created->beginSeparator, &source->beginSeparator));
    GCE(ceError = CTCopyTokenContents(&created->value, &source->value));
    for(i = 0; i < source->subelements.size; i++)
    {
        const Krb5Entry *sourceChild = GetChildConst(source, i);
        GCE(ceError = CopyEntry(sourceChild, &copiedChild));
        GCE(ceError = InsertChildNode(created, i, copiedChild));
        copiedChild = NULL;
    }
    *copy = created;
    created = NULL;

cleanup:
    FreeKrb5Entry(&created);
    FreeKrb5Entry(&copiedChild);
    return ceError;
}

static CENTERROR ReadKrb5File(const char *rootPrefix, const char *filename, Krb5Entry *conf);

static CENTERROR WriteKrb5Configuration(const char *rootPrefix, const char *filename, Krb5Entry *conf, BOOLEAN *modified);

static Krb5Entry * GetRootNode(Krb5Entry *child)
{
    while(child->parent != NULL)
    {
        child = child->parent;
    }
    return child;
}

static int GetEntryDepth(const Krb5Entry *entry)
{
    int depth = -1;
    while(entry != NULL)
    {
        entry = entry->parent;
        depth++;
    }
    return depth;
}

static CENTERROR ParseLine(Krb5Entry **parent, const char *linestr, const char **endptr)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    const char *pos = linestr;
    const char *token_start = NULL;
    const char *oldpos;
    Krb5Entry *line = NULL;
    BOOLEAN expectChildren = FALSE;

    GCE(ceError = CTAllocateMemory(sizeof(*line), (void **)&line));

    /* Find the leading whitespace in the line */
    token_start = pos;
    while(isblank(*pos)) pos++;
    if(*pos == '#' || *pos == ';')
    {
        //This is a comment line. The whole line is leading white space
        while(*pos != '\0' && *pos != '\n' && *pos != '\r') pos++;
    }
    GCE(ceError = CTStrndup(token_start, pos - token_start, &line->leadingWhiteSpace));

    if(*pos == '\0' || *pos == '\n' || *pos == '\r')
    {
        DJ_LOG_VERBOSE("Found krb5 comment '%s'", linestr);
        //This is a comment line
    }
    else if(*pos == '}')
    {
        DJ_LOG_VERBOSE("Found krb5 compound end '%s'", linestr);
        //This is the end of a compound statement
        if(!IsGroupEntry(*parent))
        {
            DJ_LOG_ERROR("Expecting line '%s' to end a compound statement, but no compound statement appears before it",
                    linestr);
            GCE(ceError = CENTERROR_DOMAINJOIN_INVALID_FORMAT);
        }
        GCE(ceError = CTReadToken(&pos, &(*parent)->value, "", "\r\n", " \t"));
        *parent = (*parent)->parent;
        FreeKrb5Entry(&line);
        goto cleanup;
    }
    else if(*pos == '[')
    {
        size_t len;
        DJ_LOG_VERBOSE("Found krb5 stanza '%s'", linestr);
        //This is a stanza
        *parent = GetRootNode(*parent);
        //Trim [
        pos++;
        GCE(ceError = CTReadToken(&pos, &line->name, "", "\r\n", " \t"));
        //Trim ]
        len = strlen(line->name.value);
        if(line->name.value[len - 1] == ']')
            line->name.value[len - 1] = 0;
        else
        {
            DJ_LOG_ERROR("Expecting krb5 stanza name '%s' to end with ]",
                    line->name.value);
            GCE(ceError = CENTERROR_DOMAINJOIN_INVALID_FORMAT);
        }
        //Add future lines under this stanza
        expectChildren = TRUE;
    }
    else
    {
        //This is either a name value pair, or a compound element
        GCE(ceError = CTReadToken(&pos, &line->name, " \t", "=\r\n", ""));
        if(*pos != '=')
        {
            DJ_LOG_ERROR("Expecting krb5 name value or compound statement '%s' to have a = at position %d",
                    linestr, pos - linestr);
            GCE(ceError = CENTERROR_DOMAINJOIN_INVALID_FORMAT);
        }
        oldpos = pos;
        GCE(ceError = CTReadToken(&pos, &line->beginSeparator, " \t", "\r\n", ""));
        if(*pos == '{')
        {
            DJ_LOG_VERBOSE("Found krb5 compound statement '%s'", linestr);
            //Oops, looks like this was really a compound statement, so we want to store the = and the { in the beginSeparator.
            CTFreeParseTokenContents(&line->beginSeparator);
            pos = oldpos;
            GCE(ceError = CTReadToken(&pos, &line->beginSeparator, "", "\r\n", " \t"));
            if(!CTStrEndsWith(line->beginSeparator.value, "{"))
            {
                DJ_LOG_ERROR("Expecting krb5 compound statement line '%s' to end with a {",
                        linestr);
                GCE(ceError = CENTERROR_DOMAINJOIN_INVALID_FORMAT);
            }
            //Add future lines under this statement
            expectChildren = TRUE;
        }
        else
        {
            DJ_LOG_VERBOSE("Found krb5 name value pair '%s'", linestr);
            //This is name value statement
            GCE(ceError = CTReadToken(&pos, &line->value, "", "\r\n", " \t"));
        }
    }

    GCE(ceError = AddChildNode(*parent, line));
    if(expectChildren)
    {
        *parent = line;
    }

cleanup:
    if(endptr != NULL)
        *endptr = pos;

    if(!CENTERROR_IS_OK(ceError))
        FreeKrb5Entry(&line);
    return ceError;
}

static CENTERROR ReadKrb5File(const char *rootPrefix, const char *filename, Krb5Entry *conf)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    FILE *file = NULL;
    PSTR buffer = NULL;
    char *fullPath = NULL;
    BOOLEAN endOfFile = FALSE;
    BOOLEAN exists;
    Krb5Entry *currentEntry = conf;

    memset(currentEntry, 0, sizeof(*currentEntry));

    GCE(ceError = CTAllocateStringPrintf(
            &fullPath, "%s%s", rootPrefix, filename));
    DJ_LOG_INFO("Reading krb5 file %s", fullPath);
    GCE(ceError = CTCheckFileOrLinkExists(fullPath, &exists));
    if(!exists)
    {
        DJ_LOG_INFO("File %s does not exist", fullPath);
        ceError = CENTERROR_INVALID_FILENAME;
        goto cleanup;
    }

    GCE(ceError = CTOpenFile(fullPath, "r", &file));
    while(TRUE)
    {
        GCE(ceError = CTReadNextLine(file, &buffer, &endOfFile));
        if(endOfFile)
            break;
        GCE(ceError = ParseLine(&currentEntry, buffer, NULL));
    }
    GCE(ceError = CTCloseFile(file));
    file = NULL;

cleanup:
    CT_SAFE_FREE_STRING(buffer);
    if(file != NULL)
        CTCloseFile(file);
    CT_SAFE_FREE_STRING(fullPath);
    if(!CENTERROR_IS_OK(ceError))
        FreeKrb5EntryContents(conf);
    return ceError;
}

static ssize_t FindNodeIndex(Krb5Entry *parent, size_t startIndex, const char *name)
{
    size_t i;
    for(i = startIndex; i < parent->subelements.size; i++)
    {
        Krb5Entry *child = GetChild(parent, i);
        if(child->name.value != NULL && !strcmp(child->name.value, name))
            return i;
    }
    return -1;
}

static Krb5Entry *FindEntryOfNameAndType(Krb5Entry *parent, int desiredDepth, const char *name, NodeClassifier type)
{
    int parentDepth = GetEntryDepth(parent);
    size_t i;
    if(parentDepth + 1 == desiredDepth)
    {
        for(i = 0; i < parent->subelements.size; i++)
        {
            Krb5Entry *child = GetChild(parent, i);
            if(child->name.value != NULL &&
                    !strcmp(child->name.value, name) && type(child))
            {
                return child;
            }
        }
    }
    else if(parentDepth < desiredDepth)
    {
        for(i = 0; i < parent->subelements.size; i++)
        {
            Krb5Entry *child = GetChild(parent, i);
            Krb5Entry *result = FindEntryOfNameAndType(child, desiredDepth, name, type);
            if(result != NULL)
                return result;
        }
    }
    return NULL;
}

static Krb5Entry *FindEntryOfType(Krb5Entry *parent, int desiredDepth, NodeClassifier type)
{
    int parentDepth = GetEntryDepth(parent);
    size_t i;
    if(parentDepth + 1 == desiredDepth)
    {
        for(i = 0; i < parent->subelements.size; i++)
        {
            Krb5Entry *child = GetChild(parent, i);
            if(type(child))
                return child;
        }
    }
    else if(parentDepth < desiredDepth)
    {
        for(i = 0; i < parent->subelements.size; i++)
        {
            Krb5Entry *child = GetChild(parent, i);
            Krb5Entry *result = FindEntryOfType(child, desiredDepth, type);
            if(result != NULL)
                return result;
        }
    }
    return NULL;
}

static CENTERROR CreateValueNode(Krb5Entry *conf, int depth, const char *name, const char *value, Krb5Entry **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *existing;
    Krb5Entry *created = NULL;

    *result = NULL;

    //Try to find a similar node to copy. This will keep the spacing consistent.
    existing = FindEntryOfNameAndType(conf, depth, name, IsValueEntry);
    if(existing == NULL)
        existing = FindEntryOfType(conf, depth, IsValueEntry);

    if(existing == NULL)
    {
        //Couldn't find a template. Got to create it from scratch
        GCE(ceError = CTAllocateMemory(sizeof(*created), (void **)&created));
        //Use one space to indent everything past the stanza level
        GCE(ceError = CTAllocateMemory(depth, (void **)&created->leadingWhiteSpace));
        memset(created->leadingWhiteSpace, ' ', depth);
        created->leadingWhiteSpace[depth] = 0;
        GCE(ceError = CTStrdup(" ", &created->name.trailingSeparator));
        GCE(ceError = CTStrdup("=", &created->beginSeparator.value));
        GCE(ceError = CTStrdup(" ", &created->beginSeparator.trailingSeparator));
    }
    else
    {
        GCE(ceError = CopyEntry(existing, &created));
    }
    CT_SAFE_FREE_STRING(created->name.value);
    GCE(ceError = CTStrdup(name, &created->name.value));
    CT_SAFE_FREE_STRING(created->value.value);
    GCE(ceError = CTStrdup(value, &created->value.value));

cleanup:
    if(CENTERROR_IS_OK(ceError))
        *result = created;
    else
        FreeKrb5Entry(&created);
    return ceError;
}

static CENTERROR CreateGroupNode(Krb5Entry *conf, int depth, const char *name, Krb5Entry **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *existing;
    Krb5Entry *created = NULL;

    *result = NULL;

    //Try to find a similar node to copy. This will keep the spacing consistent.
    existing = FindEntryOfNameAndType(conf, depth, name, IsGroupEntry);
    if(existing == NULL)
        existing = FindEntryOfType(conf, depth, IsGroupEntry);

    if(existing == NULL)
    {
        //Couldn't find a template. Got to create it from scratch
        GCE(ceError = CTAllocateMemory(sizeof(*created), (void **)&created));
        //Use one space to indent everything past the stanza level
        GCE(ceError = CTAllocateMemory(depth, (void **)&created->leadingWhiteSpace));
        memset(created->leadingWhiteSpace, ' ', depth);
        created->leadingWhiteSpace[depth] = 0;
        GCE(ceError = CTStrdup(" ", &created->name.trailingSeparator));
        GCE(ceError = CTStrdup("= {", &created->beginSeparator.value));
        GCE(ceError = CTStrdup("}", &created->value.value));
    }
    else
    {
        GCE(ceError = CopyEntry(existing, &created));
    }
    CT_SAFE_FREE_STRING(created->name.value);
    GCE(ceError = CTStrdup(name, &created->name.value));
    DeleteAllChildren(created);

cleanup:
    if(CENTERROR_IS_OK(ceError))
        *result = created;
    else
        FreeKrb5Entry(&created);
    return ceError;
}

static CENTERROR CreateStanzaNode(Krb5Entry *conf, const char *name, Krb5Entry **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *existing;
    Krb5Entry *created = NULL;

    *result = NULL;

    //Try to find a similar node to copy. This will keep the spacing consistent.
    existing = FindEntryOfNameAndType(conf, 1, name, IsStanzaEntry);
    if(existing == NULL)
        existing = FindEntryOfType(conf, 1, IsStanzaEntry);

    if(existing == NULL)
    {
        //Couldn't find a template. Got to create it from scratch
        GCE(ceError = CTAllocateMemory(sizeof(*created), (void **)&created));
    }
    else
    {
        GCE(ceError = CopyEntry(existing, &created));
    }
    CT_SAFE_FREE_STRING(created->name.value);
    GCE(ceError = CTStrdup(name, &created->name.value));
    DeleteAllChildren(created);

cleanup:
    if(CENTERROR_IS_OK(ceError))
        *result = created;
    else
        FreeKrb5Entry(&created);
    return ceError;
}

static Krb5Entry *GetFirstNode(Krb5Entry *parent, const char *name)
{
    ssize_t index = FindNodeIndex(parent, 0, name);
    if(index != -1)
    {
        Krb5Entry *child = GetChild(parent, index);
        return child;
    }
    return NULL;
}

static CENTERROR EnsureGroupNode(Krb5Entry *parent, const char *name, Krb5Entry **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *created = NULL;
    *result = GetFirstNode(parent, name);
    if(*result == NULL)
    {
        //Got to create it
        GCE(ceError = CreateGroupNode(GetRootNode(parent),
                    GetEntryDepth(parent) + 1,
                    name, &created));
        GCE(ceError = AddChildNode(parent, created));
        *result = created;
        created = NULL;
    }

cleanup:
    FreeKrb5Entry(&created);
    return ceError;
}

static CENTERROR EnsureStanzaNode(Krb5Entry *conf, const char *name, Krb5Entry **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *created = NULL;
    *result = GetFirstNode(conf, name);
    if(*result == NULL)
    {
        //Got to create it
        DJ_LOG_INFO("Creating krb5 stanza '%s'", name);
        GCE(ceError = CreateStanzaNode(conf,
                    name, &created));
        GCE(ceError = AddChildNode(conf, created));
        *result = created;
        created = NULL;
    }

cleanup:
    FreeKrb5Entry(&created);
    return ceError;
}

//Deletes all children that have the specified name
CENTERROR DeleteChildNode(Krb5Entry *parent, const char *name, size_t *removed)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    if(removed)
        *removed = 0;
    while(TRUE)
    {
        ssize_t index = FindNodeIndex(parent, 0, name);
        if(index == -1)
            break;

        GCE(ceError = CTArrayRemove(&parent->subelements, index,
                        sizeof(Krb5Entry *), 1));
        if(removed)
            (*removed)++;
    }

cleanup:
    return ceError;
}

static CENTERROR SetNodeValue(Krb5Entry *parent, const char *name, const char *value)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *created = NULL;
    ssize_t existingIndex = FindNodeIndex(parent, 0, name);
    DJ_LOG_VERBOSE("Settting krb5 name value '%s' to '%s' ", name, value);
    GCE(ceError = CreateValueNode(parent, GetEntryDepth(parent) + 1, name,
                value, &created));
    GCE(ceError = DeleteChildNode(parent, name, NULL));
    if(existingIndex != -1)
        GCE(ceError = InsertChildNode(parent, existingIndex, created));
    else
        GCE(ceError = AddChildNode(parent, created));
    created = NULL;
cleanup:
    FreeKrb5Entry(&created);
    return ceError;
}

const char *GetFirstNodeValue(Krb5Entry *parent, const char *name)
{
    Krb5Entry *entry = GetFirstNode(parent, name);
    if(entry == NULL)
        return NULL;
    return entry->value.value;
}

static CENTERROR SetChildNode(Krb5Entry *parent, Krb5Entry *child)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    ssize_t existingIndex = FindNodeIndex(parent, 0, child->name.value);
    if(existingIndex != -1)
    {
        GCE(ceError = DeleteChildNode(parent, child->name.value, NULL));
        GCE(ceError = InsertChildNode(parent, existingIndex, child));
    }
    else
        GCE(ceError = AddChildNode(parent, child));

cleanup:
    return ceError;
}

static CENTERROR
AddEncTypes(Krb5Entry *parent, const char *elementName, const char **add, size_t addCount)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    char *setEncTypes = NULL;
    const char *currentEncTypes = GetFirstNodeValue(parent, elementName);
    size_t i;

    if(currentEncTypes != NULL)
    {
        GCE(ceError = CTStrdup(currentEncTypes, &setEncTypes));
    }
    for(i = 0; i < addCount; i++)
    {
        if(setEncTypes == NULL)
        {
            GCE(ceError = CTStrdup(add[i], &setEncTypes));
        }
        else if(strstr(setEncTypes, add[i]) == NULL)
        {
            char *newString;
            GCE(ceError=CTAllocateStringPrintf(&newString, "%s %s", setEncTypes, add[i]));
            CT_SAFE_FREE_STRING(setEncTypes);
            setEncTypes = newString;
        }
    }
    GCE(ceError = SetNodeValue( parent, elementName, setEncTypes ));

cleanup:
    CT_SAFE_FREE_STRING(setEncTypes);
    return ceError;
}

typedef struct
{
    PSTR shortName;
    PSTR longName;
} DomainMapping;

static CENTERROR GetEscapedDomainName(const char *input, char **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    size_t i;
    DynamicArray array;
    *result = NULL;
    memset(&array, 0, sizeof(array));
    GCE(ceError = CTSetCapacity(&array, sizeof(char), strlen(input) + 3));
    for(i = 0; input[i]; i++)
    {
        if(input[i] == '.')
        {
            GCE(ceError = CTArrayAppend(&array, sizeof(char), "\\", 1));
        }
        GCE(ceError = CTArrayAppend(&array, sizeof(char), &input[i], 1));
    }
    GCE(ceError = CTArrayAppend(&array, sizeof(char), "\0", 1));
    *result = array.data;
    array.data = NULL;

cleanup:
    CTArrayFree(&array);
    return ceError;
}

static CENTERROR GetAuthToLocalRule(DomainMapping *mapping, char **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    char *escapedDomain = NULL;
    char *shortUpper = NULL;
    *result = NULL;
    GCE(ceError = GetEscapedDomainName(mapping->longName, &escapedDomain));
    CTStrToUpper(escapedDomain);
    GCE(ceError = CTStrdup(mapping->shortName, &shortUpper));
    CTStrToUpper(shortUpper);

    GCE(ceError = CTAllocateStringPrintf(result,
                "RULE:[1:$0\\$1](^%s\\\\.*)s/^%s/%s/",
                escapedDomain, escapedDomain, shortUpper));
cleanup:
    CT_SAFE_FREE_STRING(escapedDomain);
    CT_SAFE_FREE_STRING(shortUpper);
    return ceError;
}

static CENTERROR GetMappingsValueString(DomainMapping *mapping, char **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    *result = NULL;
    GCE(ceError = CTAllocateStringPrintf(result,
                "%s\\\\(.*) $1@%s",
                mapping->shortName, mapping->longName));
    CTStrToUpper(*result);
cleanup:
    return ceError;
}

static CENTERROR GetReverseMappingsValueString(DomainMapping *mapping, char **result)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    char *escapedDomain = NULL;
    *result = NULL;
    GCE(ceError = GetEscapedDomainName(mapping->longName, &escapedDomain));
    GCE(ceError = CTAllocateStringPrintf(result,
                "(.*)@%s %s\\$1",
                escapedDomain, mapping->shortName));
    CTStrToUpper(*result);
cleanup:
    CT_SAFE_FREE_STRING(escapedDomain);
    return ceError;
}

static void FreeDomainMappings(DynamicArray *mappings)
{
    size_t i;
    for(i = 0; i < mappings->size; i++)
    {
        DomainMapping *current = ((DomainMapping *)mappings->data) + i;
        CT_SAFE_FREE_STRING(current->shortName);
        CT_SAFE_FREE_STRING(current->longName);
    }
    CTArrayFree(mappings);
}

static ssize_t
FindMapping(DynamicArray *mappings,
    PSTR shortName)
{
    size_t i;
    for(i = 0; i < mappings->size; i++)
    {
        DomainMapping *current = ((DomainMapping *)mappings->data) + i;
        if(!strcmp(current->shortName, shortName))
        {
            return i;
        }
    }
    return -1;
}

static CENTERROR
GatherDomainMappings(
    DynamicArray *mappings,
    PCSTR pszShortDomainName,
    PCSTR pszDomainName)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    DomainMapping add;
    char *commandOutput = NULL;
    memset(mappings, 0, sizeof(*mappings));
    memset(&add, 0, sizeof(add));

    GCE(ceError = CTStrdup(pszDomainName, &add.longName));
    GCE(ceError = CTStrdup(pszShortDomainName, &add.shortName));
    GCE(ceError = CTArrayAppend(mappings, sizeof(add), &add, 1));
    memset(&add, 0, sizeof(add));

    //This command requires that the computer is joined to the domain, but
    //the auth daemon does not need to be running
    ceError = CTCaptureOutputWithStderr(
        PREFIXDIR "/bin/lwinet ads trusts",
        TRUE,
        &commandOutput);
    if(ceError == CENTERROR_SUCCESS)
    {
        char *linesaveptr;
        char *line;
        line = strtok_r(commandOutput, "\r\n", &linesaveptr);
        while(line != NULL)
        {
            //The output is in the form:
            //<trustnum> <short name> <num> <hexnum> <hexnum> <hexnum> <SID> <GUID> <long name>
            //e.g.:
            //3 CORP                0 0x1d 0x02 0x00 S-1-5-21-418081286-1191099226-2202501032  {19df18bd-f2f4-453a-82fd-63b02c896ae1} corp.centeris.com
            char *tokensaveptr;
            char *shortName;
            char *longName;
            CT_SAFE_FREE_STRING(add.shortName);
            CT_SAFE_FREE_STRING(add.longName);
            //trustnum
            strtok_r(line, " \t", &tokensaveptr);
            shortName = strtok_r(NULL, " \t", &tokensaveptr);
            if(shortName != NULL)
                GCE(ceError = CTStrdup(shortName, &add.shortName));
            //<num>
            strtok_r(NULL, " \t", &tokensaveptr);
            //<hexnum>
            strtok_r(NULL, " \t", &tokensaveptr);
            //<hexnum>
            strtok_r(NULL, " \t", &tokensaveptr);
            //<hexnum>
            strtok_r(NULL, " \t", &tokensaveptr);
            //<SID>
            strtok_r(NULL, " \t", &tokensaveptr);
            //<GUID>
            strtok_r(NULL, " \t", &tokensaveptr);
            longName = strtok_r(NULL, " \t", &tokensaveptr);
            if(longName != NULL)
                GCE(ceError = CTStrdup(longName, &add.longName));
            CTStripWhitespace(add.shortName);
            CTStripWhitespace(add.longName);

            if(!IsNullOrEmptyString(add.shortName) && !IsNullOrEmptyString(add.longName))
            {
                DJ_LOG_INFO("Lwinet found trust '%s' -> '%s'",
                        add.shortName,
                        add.longName);
                if( FindMapping(mappings, add.shortName) == -1)
                {
                    GCE(ceError = CTArrayAppend(mappings,
                                sizeof(add), &add, 1));
                    memset(&add, 0, sizeof(add));
                }
            }

            line = strtok_r(NULL, "\r\n", &linesaveptr);
        }
    }
    else if(ceError == CENTERROR_COMMAND_FAILED)
    {
         DJ_LOG_INFO("Failed to run lwinet ads trusts. This is expected if not yet joined to the domain");
         ceError = CENTERROR_SUCCESS;
    }
    else
        GCE(ceError);

    CT_SAFE_FREE_STRING(commandOutput);
    //This command requires that the auth daemon is running
    ceError = CTCaptureOutputWithStderr(
        PREFIXDIR "/bin/lwiinfo --details -m",
        TRUE,
        &commandOutput);
    if(ceError == CENTERROR_SUCCESS)
    {
        char *linesaveptr;
        char *line;
        line = strtok_r(commandOutput, "\r\n", &linesaveptr);
        while(line != NULL)
        {
            //The output is in the form:
            //<short name>, <long name>, <sid>
            //e.g.:
            //CORP, corp.centeris.com, S-1-5-21-418081286-1191099226-2202501032
            char *tokensaveptr;
            char *shortName;
            char *longName;
            CT_SAFE_FREE_STRING(add.shortName);
            CT_SAFE_FREE_STRING(add.longName);
            shortName = strtok_r(line, ",", &tokensaveptr);
            if(shortName != NULL)
                GCE(ceError = CTStrdup(shortName, &add.shortName));
            longName = strtok_r(NULL, ",", &tokensaveptr);
            if(longName != NULL)
                GCE(ceError = CTStrdup(longName, &add.longName));
            CTStripWhitespace(add.shortName);
            CTStripWhitespace(add.longName);

            if(!IsNullOrEmptyString(add.shortName) && !IsNullOrEmptyString(add.longName))
            {
                DJ_LOG_INFO("Lwiinfo found trust '%s' -> '%s'",
                        add.shortName,
                        add.longName);
                if( FindMapping(mappings, add.shortName) == -1)
                {
                    GCE(ceError = CTArrayAppend(mappings,
                                sizeof(add), &add, 1));
                    memset(&add, 0, sizeof(add));
                }
            }

            line = strtok_r(NULL, "\r\n", &linesaveptr);
        }
    }
    else if(ceError == CENTERROR_COMMAND_FAILED)
    {
         DJ_LOG_INFO("Failed to run lwiinfo --details -m. This is expected if the auth daemon is not running");
         ceError = CENTERROR_SUCCESS;
    }
    else
        GCE(ceError);

cleanup:
    CT_SAFE_FREE_STRING(commandOutput);
    if(!CENTERROR_IS_OK(ceError))
        FreeDomainMappings(mappings);
    CT_SAFE_FREE_STRING(add.shortName);
    CT_SAFE_FREE_STRING(add.longName);
    return ceError;
}

static CENTERROR
Krb5LeaveDomain(Krb5Entry *conf)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *libdefaults;
    GCE(ceError = EnsureStanzaNode(conf, "libdefaults", &libdefaults));
    GCE(ceError = DeleteChildNode(libdefaults, "default_realm", NULL));

cleanup:
    return ceError;
}

static CENTERROR
Krb5JoinDomain(Krb5Entry *conf,
    PCSTR pszDomainName,
    PCSTR pszShortDomainName)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry *libdefaults;
    Krb5Entry *realms;
    Krb5Entry *appdefaults;
    Krb5Entry *pamGroup;
    Krb5Entry *httpdGroup;
    Krb5Entry *domainGroup = NULL;
    Krb5Entry *addNode = NULL;
    char *domainUpper = NULL;
    char *autoShortDomain = NULL;
    DynamicArray trusts;
    char *mappingString = NULL;
    size_t i;
    const char *wantEncTypes[] = {"DES-CBC-CRC",
        "DES-CBC-MD5",
        "RC4-HMAC",
    };
    memset(&trusts, 0, sizeof(trusts));

    if(IsNullOrEmptyString(pszDomainName))
    {
        DJ_LOG_ERROR("Please specify the long domain name");
        GCE(ceError = CENTERROR_INVALID_PARAMETER);
    }
    if(IsNullOrEmptyString(pszShortDomainName))
    {
        char *dotPosition = NULL;
        CTStrdup(pszDomainName, &autoShortDomain);
        dotPosition = strchr(autoShortDomain, '.');
        if(dotPosition)
            *dotPosition = 0;
        pszShortDomainName = autoShortDomain;

        DJ_LOG_WARNING("Short domain name not specified. Defaulting to '%s'", pszShortDomainName);
    }

    GCE(ceError = GatherDomainMappings( &trusts,
                pszShortDomainName, pszDomainName));

    GCE(ceError = EnsureStanzaNode(conf, "libdefaults", &libdefaults));
    GCE(ceError = CTStrdup(pszDomainName, &mappingString));
    CTStrToUpper(mappingString);
    GCE(ceError = SetNodeValue( libdefaults, "default_realm",
                 mappingString ));
    GCE(ceError = AddEncTypes(libdefaults, "default_tgs_enctypes",
                wantEncTypes,
                sizeof(wantEncTypes)/sizeof(wantEncTypes[0])));
    GCE(ceError = AddEncTypes(libdefaults, "default_tkt_enctypes",
                wantEncTypes,
                sizeof(wantEncTypes)/sizeof(wantEncTypes[0])));
    GCE(ceError = SetNodeValue( libdefaults, "preferred_enctypes",
                "DES-CBC-CRC DES-CBC-MD5 RC4-HMAC" ));
    GCE(ceError = SetNodeValue( libdefaults, "dns_lookup_kdc", "true" ));

    GCE(ceError = EnsureStanzaNode(conf, "realms", &realms));
    GCE(ceError = CTStrdup(pszDomainName, &domainUpper));
    CTStrToUpper(domainUpper);
    GCE(ceError = CreateGroupNode(conf, 2, domainUpper, &domainGroup));
    for(i = 0; i < trusts.size; i++)
    {
        DomainMapping *current = ((DomainMapping *)trusts.data) + i;
        CT_SAFE_FREE_STRING(mappingString);
        GCE(ceError = GetAuthToLocalRule(current, &mappingString));
        GCE(ceError = CreateValueNode(conf, 3, "auth_to_local", mappingString, &addNode));
        GCE(ceError = AddChildNode(domainGroup, addNode));
        addNode = NULL;
    }
    GCE(ceError = CreateValueNode(conf, 3, "auth_to_local", "DEFAULT", &addNode));
    GCE(ceError = AddChildNode(domainGroup, addNode));
    addNode = NULL;

    //Replaces old auth to local rules
    GCE(ceError = SetChildNode(realms, domainGroup));
    domainGroup = NULL;

    GCE(ceError = EnsureStanzaNode(conf, "appdefaults", &appdefaults));
    GCE(ceError = EnsureGroupNode(appdefaults, "pam", &pamGroup));
    CT_SAFE_FREE_STRING(mappingString);
    //The first trust has the primary short domain name and long domain name
    GCE(ceError = GetMappingsValueString( (DomainMapping *)trusts.data, &mappingString ));
    GCE(ceError = SetNodeValue( pamGroup, "mappings", mappingString ));
    GCE(ceError = SetNodeValue( pamGroup, "forwardable", "true" ));
    GCE(ceError = SetNodeValue( pamGroup, "validate", "true" ));

    GCE(ceError = EnsureGroupNode(appdefaults, "httpd", &httpdGroup));
    CT_SAFE_FREE_STRING(mappingString);
    GCE(ceError = GetMappingsValueString( (DomainMapping *)trusts.data, &mappingString ));
    GCE(ceError = SetNodeValue( httpdGroup, "mappings", mappingString ));
    CT_SAFE_FREE_STRING(mappingString);
    //The first trust has the primary short domain name and long domain name
    GCE(ceError = GetReverseMappingsValueString( (DomainMapping *)trusts.data, &mappingString ));
    GCE(ceError = SetNodeValue( httpdGroup, "reverse_mappings", mappingString ));

cleanup:
    CT_SAFE_FREE_STRING(mappingString);
    CT_SAFE_FREE_STRING(autoShortDomain);
    FreeDomainMappings(&trusts);
    FreeKrb5Entry(&domainGroup);
    FreeKrb5Entry(&addNode);
    return ceError;
}

static CENTERROR ReadKrb5Configuration(const char *rootPrefix, Krb5Entry *conf, BOOLEAN *modified)
{
    char *fullPath = NULL;
    char *altPath = NULL;
    char *altDir = NULL;
    FILE *file = NULL;
    CENTERROR ceError = CENTERROR_SUCCESS;
    BOOLEAN _modified = FALSE;
    BOOLEAN exists;
    BOOLEAN solarisTemplateFile;
    if(rootPrefix == NULL)
        rootPrefix = "";

    GCE(ceError = CTAllocateStringPrintf(
            &fullPath, "%s%s", rootPrefix, "/etc/krb5.conf"));
    GCE(ceError = CTCheckFileOrLinkExists(fullPath, &exists));
    if(!exists)
    {
        CT_SAFE_FREE_STRING(altPath);
        GCE(ceError = CTAllocateStringPrintf(
                &altPath, "%s%s", rootPrefix, "/etc/krb5/krb5.conf"));
        GCE(ceError = CTCheckFileOrLinkExists(altPath, &exists));
        if(exists)
        {
            DJ_LOG_INFO("Symlinking system /etc/krb5/krb5.conf to /etc/krb5.conf");
            GCE(ceError = CTCreateSymLink(altPath, fullPath));
            _modified = TRUE;
        }
        else
        {
            DJ_LOG_INFO("Creating blank krb5.conf");
            ceError = CTOpenFile(fullPath, "w", &file);
            if(!CENTERROR_IS_OK(ceError))
            {
                DJ_LOG_ERROR("Unable to open '%s' for writing", fullPath);
                GCE(ceError);
            }
            GCE(ceError = CTCloseFile(file));
            file = NULL;
            _modified = TRUE;

            GCE(ceError = CTAllocateStringPrintf(
                    &altDir, "%s%s", rootPrefix, "/etc/krb5"));
            GCE(ceError = CTCheckDirectoryExists(altDir, &exists));
            if(exists)
            {
                DJ_LOG_INFO("Symlinking /etc/krb5.conf to /etc/krb5/krb5.conf");
                GCE(ceError = CTCreateSymLink(fullPath, altPath));
                _modified = TRUE;
            }
        }
    }

    GCE(ceError = CTCheckFileHoldsPattern(fullPath, "^[[:space:]]*___slave_kdcs___[[:space:]]*$", &solarisTemplateFile));

    if(solarisTemplateFile)
    {
        DJ_LOG_WARNING("The system krb5.conf is the default template file (which is syntactically incorrect). It will be replaced.");
    }
    else
    {
        GCE(ceError = ReadKrb5File(rootPrefix, "/etc/krb5.conf", conf));
    }

    if(*modified)
        *modified = _modified;

cleanup:
    if(file != NULL)
        CTCloseFile(file);
    CT_SAFE_FREE_STRING(fullPath);
    CT_SAFE_FREE_STRING(altDir);
    CT_SAFE_FREE_STRING(altPath);
    return ceError;
}

CENTERROR
DJModifyKrb5Conf(
    const char *testPrefix,
    BOOLEAN enable,
    PCSTR pszDomainName,
    PCSTR pszShortDomainName,
    BOOLEAN *modified
    )
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    Krb5Entry conf;
    BOOLEAN readModified;
    memset(&conf, 0, sizeof(conf));

    DJ_LOG_INFO("Starting krb5.conf configuration (%s)", enable? "enabling" : "disabling");

    if(testPrefix == NULL)
        testPrefix = "";

    GCE(ceError = ReadKrb5Configuration(testPrefix, &conf, &readModified));
    if(enable)
    {
        GCE(ceError = Krb5JoinDomain(&conf, pszDomainName, pszShortDomainName));
    }
    else
    {
        GCE(ceError = Krb5LeaveDomain(&conf));
    }
    GCE(ceError = WriteKrb5Configuration(testPrefix, "/etc/krb5.conf", &conf, modified));
    if(readModified && modified != NULL)
        *modified = TRUE;
    DJ_LOG_INFO("Finishing krb5.conf configuration");

cleanup:
    FreeKrb5EntryContents(&conf);
    return ceError;
}

static CENTERROR WriteEntry(FILE *file, Krb5Entry *lineObj)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    size_t i;
    if(lineObj->leadingWhiteSpace != NULL)
    {
        GCE(ceError = CTFilePrintf(file, "%s", lineObj->leadingWhiteSpace));
    }
    if(IsStanzaEntry(lineObj))
    {
        GCE(ceError = CTFilePrintf(file, "[%s]%s",
                lineObj->name.value,
                lineObj->name.trailingSeparator == NULL ?
                "" : lineObj->name.trailingSeparator));
    }
    else
    {
        GCE(ceError = CTWriteToken(file, &lineObj->name));
    }
    GCE(ceError = CTWriteToken(file, &lineObj->beginSeparator));
    if(IsStanzaEntry(lineObj) || IsGroupEntry(lineObj))
    {
        GCE(ceError = CTFilePrintf(file, "\n"));
    }
    for(i = 0; i < lineObj->subelements.size; i++)
    {
        GCE(ceError = WriteEntry(file, GetChild(lineObj, i)));
    }
    if(lineObj->value.value != NULL && lineObj->value.value[0] == '}')
    {
        //The leading whitespace for the ending brace isn't stored. We'll
        //assume it's the same as the line's leading white space. Even if it's
        //different we'll just end up fixing the tabbing for the user.
        GCE(ceError = CTFilePrintf(file, "%s", lineObj->leadingWhiteSpace));
    }
    GCE(ceError = CTWriteToken(file, &lineObj->value));
    if(!IsRootEntry(lineObj) && !IsStanzaEntry(lineObj))
        GCE(ceError = CTFilePrintf(file, "\n"));

cleanup:
    return ceError;
}

static CENTERROR WriteKrb5Configuration(const char *rootPrefix, const char *filename, Krb5Entry *conf, BOOLEAN *modified)
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    FILE *file = NULL;
    char *tempName = NULL;
    char *finalName = NULL;
    char *symtarget = NULL;
    BOOLEAN same;
    BOOLEAN islink;

    GCE(ceError = CTAllocateStringPrintf(&tempName, "%s%s.new", rootPrefix, filename));
    GCE(ceError = CTAllocateStringPrintf(&finalName, "%s%s", rootPrefix, filename));

    DJ_LOG_INFO("Writing krb5 file %s", finalName);

    ceError = CTOpenFile(tempName, "w", &file);
    if(!CENTERROR_IS_OK(ceError))
    {
        DJ_LOG_ERROR("Unable to open '%s' for writing", tempName);
        GCE(ceError);
    }

    GCE(ceError = WriteEntry(file, conf));

    GCE(ceError = CTCloseFile(file));
    file = NULL;

    GCE(ceError = CTFileContentsSame(tempName, finalName, &same));
    if(modified != NULL)
        *modified = !same;
    if(same)
    {
        DJ_LOG_INFO("File %s unmodified", finalName);
        GCE(ceError = CTRemoveFile(tempName));
    }
    else
    {
        DJ_LOG_INFO("File %s modified", finalName);
        GCE(ceError = CTCheckLinkExists(finalName, &islink));
        if(islink)
        {
            GCE(ceError = CTGetSymLinkTarget(finalName, &symtarget));
            DJ_LOG_INFO("Overwriting symlink target '%s' instead of link name '%s'", symtarget, finalName);
            CT_SAFE_FREE_STRING(finalName);
            finalName = symtarget;
            symtarget = NULL;
        }
        GCE(ceError = CTCloneFilePerms(finalName, tempName));
        GCE(ceError = CTBackupFile(finalName));
        GCE(ceError = CTMoveFile(tempName, finalName));
    }

cleanup:
    if(file != NULL)
        CTCloseFile(file);
    CT_SAFE_FREE_STRING(tempName);
    CT_SAFE_FREE_STRING(finalName);
    CT_SAFE_FREE_STRING(symtarget);
    return ceError;
}

CENTERROR
DJCopyKrb5ToRootDir(
        const char *srcPrefix,
        const char *destPrefix
        )
{
    CENTERROR ceError = CENTERROR_SUCCESS;
    PSTR srcPath = NULL;
    PSTR destPath = NULL;
    BOOLEAN exists;

    if(srcPrefix == NULL)
        srcPrefix = "";
    if(destPrefix == NULL)
        destPrefix = "";

    CT_SAFE_FREE_STRING(srcPath);
    GCE(ceError = CTAllocateStringPrintf(&srcPath, "%s/etc", srcPrefix));
    GCE(ceError = CTCheckDirectoryExists(srcPath, &exists));
    if(exists)
    {
        CT_SAFE_FREE_STRING(destPath);
        GCE(ceError = CTAllocateStringPrintf(&destPath, "%s/etc", destPrefix));
        GCE(ceError = CTCreateDirectory(destPath, 0700));
    }

    CT_SAFE_FREE_STRING(srcPath);
    GCE(ceError = CTAllocateStringPrintf(&srcPath, "%s/etc/krb5", srcPrefix));
    GCE(ceError = CTCheckDirectoryExists(srcPath, &exists));
    if(exists)
    {
        CT_SAFE_FREE_STRING(destPath);
        GCE(ceError = CTAllocateStringPrintf(&destPath, "%s/etc/krb5", destPrefix));
        GCE(ceError = CTCreateDirectory(destPath, 0700));
    }

    CT_SAFE_FREE_STRING(srcPath);
    GCE(ceError = CTAllocateStringPrintf(&srcPath, "%s/etc/krb5/krb5.conf", srcPrefix));
    GCE(ceError = CTCheckFileOrLinkExists(srcPath, &exists));
    if(exists)
    {
        CT_SAFE_FREE_STRING(destPath);
        GCE(ceError = CTAllocateStringPrintf(&destPath, "%s/etc/krb5/krb5.conf", destPrefix));
        GCE(ceError = CTCopyFileWithOriginalPerms(srcPath, destPath));
    }

    CT_SAFE_FREE_STRING(srcPath);
    GCE(ceError = CTAllocateStringPrintf(&srcPath, "%s/etc/krb5.conf", srcPrefix));
    GCE(ceError = CTCheckFileOrLinkExists(srcPath, &exists));
    if(exists)
    {
        CT_SAFE_FREE_STRING(destPath);
        GCE(ceError = CTAllocateStringPrintf(&destPath, "%s/etc", destPrefix));
        GCE(ceError = CTCreateDirectory(destPath, 0700));
        CT_SAFE_FREE_STRING(destPath);
        GCE(ceError = CTAllocateStringPrintf(&destPath, "%s/etc/krb5.conf", destPrefix));
        GCE(ceError = CTCopyFileWithOriginalPerms(srcPath, destPath));
    }

cleanup:
    CT_SAFE_FREE_STRING(srcPath);
    CT_SAFE_FREE_STRING(destPath);
    return ceError;
}

static QueryResult QueryKrb5(const JoinProcessOptions *options, LWException **exc)
{
    QueryResult result = FullyConfigured;
    BOOLEAN modified;
    PSTR tempDir = NULL;
    PSTR mappingString = NULL;
    PSTR shortName = NULL;
    Krb5Entry conf;
    Krb5Entry *libdefaults;
    Krb5Entry *default_realm;

    memset(&conf, 0, sizeof(conf));
    LW_CLEANUP_CTERR(exc, CTCreateTempDirectory(&tempDir));
    LW_CLEANUP_CTERR(exc, DJCopyKrb5ToRootDir(NULL, tempDir));
    LW_CLEANUP_CTERR(exc, ReadKrb5Configuration(tempDir, &conf, &modified));
    if(modified)
    {
        if(options->joiningDomain)
        {
            result = NotConfigured;
        }
        goto cleanup;
    }

    libdefaults = GetFirstNode(&conf, "libdefaults");
    if(libdefaults == NULL)
    {
        if(options->joiningDomain)
            result = NotConfigured;
        goto cleanup;
    }
    default_realm = GetFirstNode(libdefaults, "default_realm");
    if(options->joiningDomain)
    {
        LW_CLEANUP_CTERR(exc, CTStrdup(options->domainName, &mappingString));
        CTStrToUpper(mappingString);
        if(default_realm == NULL ||
            default_realm->value.value == NULL ||
            strcmp(default_realm->value.value, mappingString))
        {
            result = NotConfigured;
            goto cleanup;
        }
    }
    else
    {
        if(default_realm != NULL)
        {
            result = NotConfigured;
            goto cleanup;
        }
    }

    if(!options->joiningDomain)
        shortName = NULL;
    else if(options->shortDomainName != NULL)
        LW_CLEANUP_CTERR(exc, CTStrdup(options->shortDomainName, &shortName));
    else
    {
        //Ignore failures from this command
        DJGuessShortDomainName(options->domainName, &shortName);
    }
    LW_CLEANUP_CTERR(exc, DJModifyKrb5Conf(tempDir, options->joiningDomain,
        options->domainName,
        shortName, &modified));
    if(modified)
    {
        result = SufficientlyConfigured;
        goto cleanup;
    }

cleanup:
    if(tempDir != NULL)
    {
        CTRemoveDirectory(tempDir);
        CT_SAFE_FREE_STRING(tempDir);
    }
    CT_SAFE_FREE_STRING(mappingString);
    CT_SAFE_FREE_STRING(shortName);
    FreeKrb5EntryContents(&conf);
    return result;
}

static void DoKrb5(JoinProcessOptions *options, LWException **exc)
{
    LW_CLEANUP_CTERR(exc, DJModifyKrb5Conf(NULL, options->joiningDomain,
        options->domainName,
        options->shortDomainName, NULL));
cleanup:
    ;
}

static PSTR GetKrb5Description(const JoinProcessOptions *options, LWException **exc)
{
    PSTR tempDir = NULL;
    PSTR origPath = NULL;
    PSTR finalPath = NULL;
    PSTR ret = NULL;
    PSTR diff = NULL;
    PSTR shortName = NULL;
    BOOLEAN exists;
    BOOLEAN modified;

    LW_CLEANUP_CTERR(exc, CTCreateTempDirectory(&tempDir));
    LW_CLEANUP_CTERR(exc, DJCopyKrb5ToRootDir(NULL, tempDir));

    if(options->shortDomainName != NULL)
        LW_CLEANUP_CTERR(exc, CTStrdup(options->shortDomainName, &shortName));
    else
    {
        //Ignore failures from this command
        DJGuessShortDomainName(options->domainName, &shortName);
    }
    LW_CLEANUP_CTERR(exc, DJModifyKrb5Conf(tempDir, options->joiningDomain,
        options->domainName,
        shortName, &modified));

    LW_CLEANUP_CTERR(exc, CTAllocateStringPrintf(
            &finalPath, "%s%s", tempDir, "/etc/krb5.conf"));

    if(modified)
    {
        LW_CLEANUP_CTERR(exc, CTAllocateStringPrintf(
                &origPath, "%s%s", tempDir, "/etc/krb5.conf.lwidentity.orig"));
        LW_CLEANUP_CTERR(exc, CTCheckFileOrLinkExists(origPath, &exists));
        if(!exists)
        {
            LW_CLEANUP_CTERR(exc, CTStrdup("/dev/null", &origPath));
        }
    }
    else
    {
        LW_CLEANUP_CTERR(exc, CTStrdup(finalPath, &origPath));
    }

    LW_CLEANUP_CTERR(exc, CTGetFileDiff(origPath, finalPath, &diff, FALSE));
    CTStripTrailingWhitespace(diff);

    if(strlen(diff) < 1)
    {
        LW_CLEANUP_CTERR(exc, CTAllocateStringPrintf(
            &ret, "Fully configured"));
    }
    else
    {
        LW_CLEANUP_CTERR(exc, CTAllocateStringPrintf( &ret,
"The krb5.conf must have the default realm set when joined to a domain. Other optional settings may be set if krb5.conf is configured automatically. Here is a list of the changes that would be performed automatically:\n%s", diff));
    }

cleanup:
    if(tempDir != NULL)
    {
        CTRemoveDirectory(tempDir);
        CT_SAFE_FREE_STRING(tempDir);
    }
    CT_SAFE_FREE_STRING(origPath);
    CT_SAFE_FREE_STRING(finalPath);
    CT_SAFE_FREE_STRING(diff);
    CT_SAFE_FREE_STRING(shortName);
    return ret;
}

const JoinModule DJKrb5Module = { TRUE, "krb5", "configure krb5.conf", QueryKrb5, DoKrb5, GetKrb5Description };
