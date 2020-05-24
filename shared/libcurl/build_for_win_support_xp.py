import os
import io
import sys
import re

file = open(sys.argv[1], 'r+')
infos = file.readlines()
file.seek(0,0)

print("add command to support xp")

for line in infos:
    line_new = line

    if re.search('/I. /I ../lib /I../include /nologo /W4 /wd4127 /EHsc /DWIN32 /FD /c /DBUILDING_LIBCURL', line):
        print("MakefileBuild.vc: add /D_USING_V110_SDK71_ to CFLAGS")
        line_new = line.replace('/I. /I ../lib /I../include /nologo /W4 /wd4127 /EHsc /DWIN32 /FD /c /DBUILDING_LIBCURL', '/I. /I ../lib /I../include /nologo /W4 /wd4127 /EHsc /DWIN32 /FD /c /DBUILDING_LIBCURL /D_USING_V110_SDK71_')
    elif re.search('/I../lib /I../include /nologo /W4 /EHsc /DWIN32 /FD /c', line):
        print("MakefileBuild.vc: add /D_USING_V110_SDK71_ to CURL_CFLAGS")
        line_new = line.replace('/I../lib /I../include /nologo /W4 /EHsc /DWIN32 /FD /c', '/I../lib /I../include /nologo /W4 /EHsc /DWIN32 /FD /c /D_USING_V110_SDK71_')
    elif re.search('/subsystem:console', line):
        print('MakefileBuild.vc: add "5.01" to CURL_LFLAGS')
        line_new = line.replace('/subsystem:console', '/subsystem:console')
    else:
        line_new = line
    
    file.write(line_new)

file.close
