/**********************************************************************
*   Copyright (C) 1999-2002, International Business Machines
*   Corporation and others.  All Rights Reserved.
***********************************************************************/

/*--------------------------------------------------------------------------
*
* File locexp.c
*
* Modification History:
*
*   Date        Name        Description
*   7/14/1999    srl        Created
*   8/16/1999    srl        Display overhaul - locale status at bottom, etc.
*  10/14/1999    srl        Update for ICU1.3+, prepare for checkin!!
*   8/17/2000    srl        Update for 1.6
*  10/09/2000    srl        Put .gifs and .htmls, etc into resource bundles
*   7/14/2001    srl        Adding configurable Collation demo
*  18/10/2001    srl        Adding RBNF.  Glad to have RBNF available again!
*  30/10/2001    srl        Adding LocaleScript. updating RBNF.
*   2/20/2003    srl        Adding Robot Exclusion META tags, finally 
*                             renaming 'tmp' to something meaningful.
****************************************************************************
*/

#include "locexp.h"

void displayLocaleExplorer(LXContext *lx)
{
    UErrorCode status = U_ZERO_ERROR;
    char portStr[100];
    int n;
    
    /* set up the port string */
    {
        const char *port;
        port = getenv("SERVER_PORT");
        
        if(port && strcmp(port,"80"))
        {
            portStr[0] = ':';
            strncpy(portStr+1,port,7);
            portStr[7]=0;
        }
        else
        {
            portStr[0] = 0;
        }
    }

    /* -------------- */
    
    u_fprintf(lx->OUT,"<html>");
    
    u_fprintf(lx->OUT, "%s", "\r\n<!-- Locale Explorer (c) 1999-2003 IBM, Inc. and Others. \r\n All rights reserved. \r\n  http://oss.software.ibm.com/icu/  \r\n\r\n-->");
    
    u_fprintf(lx->OUT, "<head><title>");
    lx->backslashCtx.html = FALSE;
    printPath(lx, lx->curLocale, lx->curLocale, FALSE);
    
    if(strstr(lx->queryString, "EXPLORE"))
    {
        lx->inDemo = TRUE;
        u_fprintf(lx->OUT, " &gt; %U", FSWF("exploreTitle", "Explore"));
    }
    else
    {
        lx->inDemo = FALSE;
    }
    
    lx->backslashCtx.html =TRUE;
    u_fprintf(lx->OUT, "</title>\r\n");
    
    /* if(!lx->pathInfo || !(lx->pathInfo[0])) */
    {
        const char *host;
        host = getenv("LX_FORCE_HOST");  /* special variable - to force the hostname */
        if((host == NULL)||!host[0])
        {
            host = getenv("HTTP_HOST");  /* Apache httpd canonical host name */
        }
        if((host == NULL)||!host[0])
        {
            host = getenv("SERVER_NAME"); /* the server's idea of the host name */
        }
        if((host == NULL)||!host[0])
        {
            host = "";
        }
        u_fprintf(lx->OUT, "<base href=\"http://%s%s%s/%s/%s/\">\r\n",         host, portStr, 
            lx->scriptName, lx->cLocale, 
            lx->chosenEncoding); /* Ensure that all relative paths have the cgi name followed by a slash.  */
    }


    /* Robot Exclusion */
    if(strstr(lx->queryString,"PANICDEFAULT") ||  
       (lx->pathInfo && strstr(lx->pathInfo,"transliterated"))) {
      u_fprintf(lx->OUT, "<META NAME=\"robots\" Content=\"nofollow,noindex\">\r\n");
    } else if(!strncmp(lx->queryString, "locale_all", 10) || strstr(lx->queryString,"converter")){
      u_fprintf(lx->OUT, "<META NAME=\"robots\" CONTENT=\"nofollow\">\r\n");
    } else if(lx->pathInfo && *lx->pathInfo && lx->pathInfo[1] && !strstr(lx->pathInfo,"en_US")) {
      u_fprintf(lx->OUT, "<META NAME=\"robots\" Content=\"nofollow,noindex\">\r\n");
    } else if(lx->chosenEncoding && lx->chosenEncoding[0] && !strstr(lx->chosenEncoding, "utf-8")) {
      u_fprintf(lx->OUT, "<META NAME=\"robots\" Content=\"nofollow,noindex\">\r\n");
    } else if(strstr(lx->queryString, "_=")) {
      u_fprintf(lx->OUT, "<META NAME=\"robots\" CONTENT=\"nofollow\">\r\n");
    }
    if(lx->chosenEncoding && lx->chosenEncoding[0]) {
      u_fprintf(lx->OUT, "<meta http-equiv=\"content-type\" content=\"text/html; charset=%s\">\r\n", lx->chosenEncoding);
    }

    
    u_fprintf(lx->OUT, "%U", 
              FSWF ( /* NOEXTRACT */ "htmlHEAD",
                     "</HEAD>\r\n<BODY BGCOLOR=\"#FFFFFF\" > \r\n")
              );

    if(strstr(lx->queryString,"EXPLORE"))
    {
        printHelpImg(lx, "display", 
            FSWF("display_ALT", "Display Problems?"),
            FSWF("display_GIF", "displayproblems.gif"),
            FSWF("display_OPTIONS", "ALIGN=RIGHT"));
        
        u_fprintf(lx->OUT, "<font size=+1>");
        printPath(lx, lx->curLocale, lx->curLocale, FALSE);
        u_fprintf(lx->OUT, "</font><p>");
        
    }
    else
    {
        UBool hadExperimentalSubLocales = FALSE;

        if(lx->queryString && lx->queryString[0]  && !lx->curLocale && (lx->queryString[0] == '_'))
        {
            UChar dispName[1024];
            UErrorCode stat = U_ZERO_ERROR;
            dispName[0] = 0;
            uloc_getDisplayName(lx->curLocaleName, lx->cLocale, dispName, 1024, &stat);
            
            u_fprintf(lx->OUT, "<ul><b>%U [%U]</b></ul>\r\n",
                FSWF("warningInheritedLocale", "Note: You're viewing a non existent locale. The ICU will support this with inherited information. But the Locale Explorer is not designed to understand such locales. Inheritance information may be wrong!"), dispName);
        }
      
        if(isExperimentalLocale(lx->curLocaleName) && lx->queryString && lx->queryString[0] )
        {
            u_fprintf(lx->OUT, "<ul><b>%U</b></ul>\r\n",
                FSWF("warningExperimentalLocale", "Note: You're viewing an experimental locale. This locale is not part of the official ICU installation! <FONT COLOR=red>Please do not file bugs against this locale.</FONT>") );
        }
        
        u_fprintf(lx->OUT, "<table summary=\"%U\" width=100%%><tr><td align=left valign=top>", FSWF("title", "ICU LocaleExplorer"));
        
        u_fprintf(lx->OUT, "<font size=+1>");
        printPath(lx, lx->curLocale, lx->curLocale, TRUE);
        u_fprintf(lx->OUT, "</font>");
        
        u_fprintf(lx->OUT, "</td><td rowspan=2 align=right valign=top width=1>");
        
        printHelpImg(lx, "display", 
            FSWF("display_ALT", "Display Problems?"),
            FSWF("display_GIF", "displayproblems.gif"),
            FSWF("display_OPTIONS", "ALIGN=RIGHT"));
        
        u_fprintf(lx->OUT, "\r\n</TD></TR><TR><TD>");
        
        
        /* look for sublocs */
        if(lx->curLocale && lx->curLocale->nSubLocs)
        {
            u_fprintf(lx->OUT, "%U<BR><ul>", FSWF("sublocales", "Sublocales:"));
            mySort(lx->curLocale, &status, FALSE); /* Sort sub locales */
            
            for(n=0;n<lx->curLocale->nSubLocs;n++)
            {
                UBool wasExperimental = FALSE;

                if(n != 0)
                {
                    u_fprintf(lx->OUT, ", ");
                }

                if(lx->curLocale->subLocs[n].isVariant) u_fprintf(lx->OUT, " [");
                
                u_fprintf(lx->OUT, "<a href=\"?_=%s\">", 
                    lx->curLocale->subLocs[n].str);
                
                if(isExperimentalLocale(lx->curLocale->subLocs[n].str))
                {
                    u_fprintf(lx->OUT, "<i><font color=\"#9999FF\">");
                    hadExperimentalSubLocales = TRUE;
                    wasExperimental = TRUE;
                }
                u_fprintf_u(lx->OUT, lx->curLocale->subLocs[n].ustr);
                if(wasExperimental)
                {
                    u_fprintf(lx->OUT, "</font></i>");
                }
                u_fprintf(lx->OUT, "</a>");
                
                if(lx->curLocale->subLocs[n].isVariant) u_fprintf(lx->OUT, "] ");
            }
            u_fprintf(lx->OUT, "</ul>");
        }
        
        /* Look for cousins with the same leaf component */
        /* For now: ONLY do for xx_YY locales */
        if(lx->curLocale && lx->parLocale &&         /* have locale & parent found (i.e. installed) */
            (lx->parLocale->parent == lx-> locales) ) /* parent's parent is root */
        {
            int count =0;
            int i;
            const char *stub;
            char buf[500];
            /* safe 'cause all these strings come from getInstalledLocales' */
            stub = lx->curLocale->str + strlen(lx->parLocale->str);
            /* u_fprintf(lx->OUT,"<B>STUB is: %s</B>\n",stub); */
            
            /* OK, now find all children X of my grandparent,  where  (  X.parent.str + stub == X ) */
            for(i=0;i<lx->locales->nSubLocs;i++)
            {
                if(!strcmp(lx->locales->subLocs[i].str, lx->parLocale->str))
                {
                    continue; /* Don't search our parent (same language) */
                }
                
                strcpy(buf, lx->locales->subLocs[i].str);
                strcat(buf, stub);
                
                if(findLocaleNonRecursive(&(lx->locales->subLocs[i]), buf) != -1)
                {
                    UBool wasExperimental = FALSE;
                    
                    if((count++) > 0)
                    {
                        u_fprintf(lx->OUT, ", ");
                    }
                    else
                    { /* header */
                        u_fprintf_u(lx->OUT, 
                            FSWF("otherLanguageSameCountryLocales", "<b>%U</b> under other languages"),
                            lx->curLocale->ustr);
                        u_fprintf(lx->OUT, ": ");
                    }
                    
                    
                    u_fprintf(lx->OUT, "<a href=\"?_=%s\">", 
                        buf);
                    
                    if(isExperimentalLocale(buf))
                    {
                        u_fprintf(lx->OUT, "<i><font color=\"#9999FF\">");
                        hadExperimentalSubLocales = TRUE;
                        wasExperimental = TRUE;
                    }
                    
                    u_fprintf_u(lx->OUT, lx->locales->subLocs[i].ustr);
                    
                    if(wasExperimental)
                    {
                        u_fprintf(lx->OUT, "</font></i>");
                    }
                    u_fprintf(lx->OUT, "</a>");
                }
            }
            if(count > 0)
            {
                u_fprintf(lx->OUT, "<br>\r\n");
            }
        }
        
#if 0
        /* This code shows sibling locales */
        if(lx->curLocale && (lx->parLocale) && (lx->locales != lx->parLocale) && (lx->parLocale->nSubLocs > 1))
        {
            int count =0 ;
            /* It's not a language, and it has siblings. */
            
            /*	mySort(lx->parLocale, &status, FALSE);  */ /* Sorting your parent seems to be a bad idea! */
            
            for(n=0;n<lx->parLocale->nSubLocs;n++)
            {
                UBool wasExperimental = FALSE;
                
                u_fprintf(lx->OUT, " -%s- ", lx->parLocale->subLocs[n].str);
                
                if( (&(lx->parLocale->subLocs[n]) != lx->curLocale) /* && it's not a placeholder like de_ */ )
                {
                    if((count++) > 0)
                        u_fprintf(lx->OUT, ", ");
                    
                    u_fprintf(lx->OUT, "<a href=\"?_=%s\">", 
                        lx->parLocale->subLocs[n].str);
                    
                    if(isExperimentalLocale(lx->parLocale->subLocs[n].str))
                    {
                        u_fprintf(lx->OUT, "<i><font color=\"#9999FF\">");
                        hadExperimentalSubLocales = TRUE;
                        wasExperimental = TRUE;
                    }
                    u_fprintf_u(lx->OUT, lx->parLocale->subLocs[n].ustr);
                    if(wasExperimental)
                    {
                        u_fprintf(lx->OUT, "</font></i>");
                    }
                    u_fprintf(lx->OUT, "</a>");
                }
                else
                {
                    u_fprintf(lx->OUT, " { DUP } ", lx->parLocale->subLocs[n].str);
                }
            }
            
            u_fprintf(lx->OUT, "</ul>");
            
        }
#endif
        
        /* this notice covers sublocs and sibling locs */
        if(hadExperimentalSubLocales)
            u_fprintf(lx->OUT, "<br>%U", FSWF("locale_experimental", "Locales in <I>Italics</I> are experimental and not officially supported."));
        
        u_fprintf(lx->OUT, "</td></tr></table>\r\n");
    }
    
    
    if ( lx->queryString == NULL )
        lx->queryString = ""; /* for sanity */
    
    if( ( (!*lx->queryString)  /* && !lx->setLocale && !(lx->setEncoding)*/) 
        || strstr(lx->queryString, "PANICDEFAULT")) /* They're coming in cold. Give them the spiel.. */
    {
        u_fprintf(lx->OUT, "<ul>");
        u_fprintf_u(lx->OUT, 
            FSWF("introSpiel", "This demo illustrates the International Components for Unicode localization data.  The data covers %V different languages, further divided into %V regions and variants.  For each language, data such as days of the week, months, and their abbreviations are defined.  <p>ICU is an open-source project."),
            (double)(lx->locales->nSubLocs),
            (double)(uloc_countAvailable()-(lx->locales->nSubLocs)));
        u_fprintf(lx->OUT, "<p>\r\n");
#if 0
        u_fprintf(lx->OUT, "%U<p>\r\n",
            FSWF/**/(/**/"specialMessage_2000Oct30",/**/
            "<i>Note: Locale Explorer should be much faster, but.. there's an ongoing problem where (at least) Microsoft Internet Explorer users will be faced with a blank page or an error page.. if this occurs, please simply hit Reload and all should be corrected.</i>"));
#endif
        
        u_fprintf(lx->OUT, "</UL>");
    }
    
    
    /* Logic here: */
    if( /* !lx->setLocale || */  !strncmp(lx->queryString,"locale", 6))     /* ?locale  or not set: pick locale */
    {
        char *restored;
        
        restored = strchr(lx->queryString, '&');
        if(restored)
        {
            restored ++;
        }
        
        if(!restored)
        {
            restored = "converter"; /* what to go on to */
        }
        
        if(lx->setLocale)
        {
            u_fprintf(lx->OUT, "<h4>%U</h4>\r\n", FSWF("changeLocale", "Change the Locale used for Labels"));
        }
        else
        {
            u_fprintf(lx->OUT, "<h4>%U</h4>\r\n", FSWF("chooseLocale", "Choose Your Locale."));
        }
        
        u_fprintf(lx->OUT, "<table summary=\"%U\" WIDTH=\"70%\"><TR>", FSWF("chooseLocale_summary", "Choose Locale"));
        u_fprintf(lx->OUT, "<td colspan=2 align=right>");
        printHelpTag(lx, "chooseLocale", NULL);
        u_fprintf(lx->OUT, "</td></tr></table>\r\n");
        chooseLocale(lx, FALSE, (char*)lx->cLocale, restored, (UBool)!strncmp(lx->queryString,"locale_all", 10));
    }
    else if (!strncmp(lx->queryString,"converter", 9))  /* ?converter */
    {
        char *restored;
        
        restored = strchr(lx->queryString, '&');
        if(restored)
        {
            restored ++;
        }
        
        /*
        if(lx->setEncoding)
        u_fprintf(lx->OUT, ": %U</H2>\r\n", FSWF("changeEncoding", "Change Your Encoding"));
        else
        u_fprintf(lx->OUT, ": %U</H2>\r\n", FSWF("chooseEncoding", "Choose Your Encoding"));
        */
        u_fprintf(lx->OUT, "<hr>");
        
        if(lx->queryString[9] == '=')
        {
            /* choose from encodings that match a string */
            char *sample;
            char *end;
            UChar usample[256];
            
            sample = strdup(lx->queryString + 10);
            end    = strchr(sample, '&');
            
            if(end == NULL)
            {
                end = sample + strlen(sample);
            }
            
            unescapeAndDecodeQueryField(usample, 256, sample);
            
            *end = 0;
            u_fprintf(lx->OUT, "%U: %s<p>\r\n", FSWF("converter_matchesTo", "Looking for matches to these chars: "), sample);
            chooseConverterMatching(lx, restored, usample);
        }
        else
        {
            /* choose from all the converters */
            chooseConverter(lx, restored);
        }
    }
    else if (!strncmp(lx->queryString,"SETTZ=",6))
    {
        /* lx->newZone is initted early, need it for cookies :) */
        if(u_strlen(lx->newZone))
        {
            UErrorCode status = U_ZERO_ERROR;
            u_fprintf(lx->OUT, "Got zone=%U<P>\n", lx->newZone);
            u_fprintf(lx->OUT, "Time there =%U\n", date(lx->newZone,UDAT_FULL,lx->cLocale,&status));
        }
        
        u_fprintf(lx->OUT, "%U: <form><input name=\"SETTZ\" value=\"%U\"><input type=submit></form>\r\n", 
            FSWF("zone_set", "Set timezone to:"),
            lx->newZone);
        u_fprintf(lx->OUT, "<ul><i>%U</i></ul>\r\n", 
            FSWF("zone_warn","Note: only works if you have cookies turned on."));
        
        {
            const char *cook;
            cook = getenv("HTTP_COOKIE");
            if(cook)
            {
                u_fprintf(lx->OUT, "<u>%s</u>\r\n", cook);
            }
        }
        
    }
    else
    {
      /* show an entire locale */
      showOneLocale(lx);
    }
    
    printStatusTable(lx);
#if 0
    /* "find a better converter" */
    if(COLLECT_getChars()[0] != 0x0000)
    {
        UConverterFromUCallback oldCallback;
        UErrorCode status2 = U_ZERO_ERROR;
        
        oldCallback = ucnv_getFromUCallBack(((UConverter*)u_fgetConverter(lx->OUT)));
        
        u_fprintf(lx->OUT, "<table width=100%% border=1><tr><td>%U<br>", FSWF("encoding_Missing", "The following characters could not be displayed properly by the current encoding:"));
        
        ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), &UCNV_FROM_U_CALLBACK_ESCAPE, &status2);
        
        u_fprintf(lx->OUT, "%U", COLLECT_getChars());
        
        ucnv_setFromUCallBack((UConverter*)u_fgetConverter(lx->OUT), oldCallback, &status2);
        
        u_fprintf(lx->OUT, "<br><a href=\"?converter=");
        writeEscaped(lx, COLLECT_getChars());
        
        if(strncmp(lx->queryString, "converter",9)) /* TODO: FIXME */
            u_fprintf(lx->OUT,"&%s", lx->queryString);
        u_fprintf(lx->OUT, "\">");
        u_fprintf(lx->OUT, "%U</a>\r\n",
            FSWF("encoding_PickABetter", "Click here to search for a better encoding"));
        
        u_fprintf(lx->OUT, "</td></tr></table>\r\n");
    }
#endif
    
    u_fprintf_u(lx->OUT, FSWF( /* NOEXTRACT */ "htmlTAIL", "<!-- No HTML footer -->"));
    
    /* a last resort. will switch to English if they get lost.. */
    /* DO NOT localize the following */
    /* What this does:  
    - brings them to the 'choose your locale' pane in English, then
    - brings them to the 'choose your encoding' pane in their locale, then
    - lists the locales to browse
    */
    
#ifndef LXHOST
# define LXHOST ""
#endif
    
    if(!strcmp(lx->cLocale,"klingon"))
        u_fprintf(lx->OUT, "<p>Thank you for using the ICU LocaleExplorer, from %s compiled %s %s %s<p>\r\n", LXHOSTNAME, __DATE__, __TIME__, LXHOST);
    
    u_fprintf(lx->OUT, "</body></html>\r\n");
    
    u_fflush(lx->OUT);
    
    u_fclose(lx->OUT);
    
    fflush(stderr);
    
    if(lx->defaultRB)
        ures_close(lx->defaultRB);

    if(lx->curRB)
        ures_close(lx->curRB);
    
    return;
}

const UChar *defaultLanguageDisplayName(LXContext *lx)
{
    UErrorCode status = U_ZERO_ERROR;
    UChar *displayName = lx->displayName;
    
    if(displayName[0] == 0x0000)
    {
        uloc_getDisplayLanguage(lx->cLocale, lx->cLocale ,displayName, 1024, &status);
    }
    
    return displayName;
}

UBool didUserAskForKey(LXContext *lx, const char *key)
{
    const char *start, *limit, *tmp1, *tmp2;
    
    
    /* look to see if they asked for it */
    start = lx->queryString;
    while( (start = strstr(start, "SHOW")) )
    {
        start += 4;
        
        tmp1 = strchr(start,'='); /*look for end of that field */
        tmp2 = strchr(start,'&'); /*look for end of that field */
        
        limit = tmp1;
        if( (tmp1 && tmp2) && (tmp1 > tmp2) ) /* pin equalsign to same field */	
        {
            limit = tmp2;
        }
        else if(tmp1 == NULL) /* no equalsign */
        {
            limit = tmp2;
        }
        
        if(limit==NULL)
            limit = start + strlen(start);
        
        
        if(((size_t)(limit-start) == strlen(key)) && !strncmp(start, key, (limit-start)))
        {
            return TRUE;
        }
        if(limit==start)
        {
            return FALSE;
        }
        start=limit;
    }
    return FALSE;
}


void exploreFetchNextPattern(LXContext *lx, UChar *dstPattern, const char *patternText)
{
    /* make QS point to the first char of the field data */
    const char *qs = strchr(patternText, '=');
    if(qs == NULL) {
      *dstPattern = 0;
      return;
    }
    qs++;

/*  unescapeAndDecodeQueryField(dstPattern, 1000, qs); */
    unescapeAndDecodeQueryField_enc(dstPattern, 1000, qs, lx->chosenEncoding);
    u_replaceChar(dstPattern, 0x0020, 0x00A0);
}

