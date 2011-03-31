@rem
@rem @brief WiX automatic build script
@rem @date $Date: 2008-02-09 20:04:03 $
@rem @author Norkai Ando <n-ando@aist.go.jp>
@rem
@rem Copyright (C) 2008-2010
@rem     Noriaki Ando
@rem     Task-intelligence Research Group,
@rem     Intelligent Systems Research Institute,
@rem     National Institute of
@rem         Advanced Industrial Science and Technology (AIST), Japan
@rem     All rights reserved.
@rem
@rem $Id: autowix.cmd.in 1761 2010-01-23 16:59:35Z n-ando $
@rem

@rem ------------------------------------------------------------
@rem Variable Settings
@rem   usually only %TARGET% might be changed
@rem ------------------------------------------------------------
set PATH=%WIX%\bin;%PATH%
set VERSION=0.0.1
set TARGET=OpenCV-RTC
set TARGET_WXS=%TARGET%.wxs
set TARGET_WIXOBJ=%TARGET%.wixobj
set TARGET_FULL=%TARGET%-%VERSION%

@rem ------------------------------------------------------------
@rem Supported languages
@rem   supported languages have to be specified
@rem ------------------------------------------------------------
@set LANGUAGES=(ja-jp de-de es-es fr-fr hu-hu it-it)
set LANGUAGES=(ja-jp en-us)

echo off
@rem ------------------------------------------------------------
@rem Checking WiX
@rem ------------------------------------------------------------
if "x%WIX%" == "x" (
   echo "Windows Installer XML (WiX) is not installed"
   echo "Please download WiX 3.5 or later from http://wix.sourceforge.net/"
   goto END
)


@rem ------------------------------------------------------------
@rem Import Language-Country, Language codes, Codepages
@rem from langs.txt
@rem http://www.tramontana.co.hu/wix/lesson2.php#2.4
@rem ------------------------------------------------------------
for /F "tokens=1,2,3,4 delims=, " %%i in (langs.txt) do (
    set LC[%%j]=%%j
    set LANG[%%j]=%%k
    set CODE[%%j]=%%l
)

@rem ============================================================
@rem compile wxs file and link msi
@rem ============================================================
candle.exe %TARGET_WXS% -dlanguage=1033 -dcodepage=1252
light.exe -ext WixUIExtension -cultures:en-us ^
     	       -out %TARGET_FULL%.msi %TARGET_WIXOBJ%

set IDS=1033
setlocal ENABLEDELAYEDEXPANSION

for %%i in %LANGUAGES% do (

    @rem ------------------------------------------------------------
    @rem language ID list
    @rem
    set IDS=!IDS!,!LANG[%%i]!

    @rem ------------------------------------------------------------
    @rem compile wxs file and link msi
    @rem
    candle.exe %TARGET_WXS% -dlanguage=!LANG[%%i]! -dcodepage=!CODE[%%i]!
    light.exe -ext WixUIExtension -cultures:!LC[%%i]! ^
    	      -out %TARGET_FULL%_!LC[%%i]!.msi %TARGET_WIXOBJ%

    @rem ------------------------------------------------------------
    @rem creating transformation files
    @rem
    torch.exe -p -t language %TARGET_FULL%.msi %TARGET_FULL%_!LC[%%i]!.msi ^
    	      -out !LC[%%i]!.mst

    @rem ------------------------------------------------------------
    @rem embed transformation files
    @rem
    cscript wisubstg.vbs %TARGET_FULL%.msi !LC[%%i]!.mst !LANG[%%i]!

)

@rem ------------------------------------------------------------
@rem here mst embedded msi can be selected languages by 
@rem > msiexec /i SampleMulti.msi TRANSFORMS=":fr-fr.mst"
@rem

@rem ------------------------------------------------------------
@rem Update the summary information stream to list all
@rem supported languages of this package
@rem ------------------------------------------------------------
cscript WiLangId.vbs %TARGET_FULL%.msi Package %IDS%

:END





@rem ------------------------------------------------------------
@rem References
@rem
@rem WiX Tutorial Lesson 9 "Transforms"
@rem http://www.tramontana.co.hu/wix/lesson9.php
@rem
@rem WiX Tutorial Lesson 2 "User Interface"
@rem http://www.tramontana.co.hu/wix/lesson2.php#2.4
@rem
@rem Multi-Language MSI Packages without Setup.exe Launcher
@rem http://www.installsite.org/pages/en/msi/articles/embeddedlang/index.htm
@rem
@rem vb scripts
@rem http://www.myitforum.com/articles/6/view.asp?id=1070
@rem
@rem ------------------------------------------------------------

