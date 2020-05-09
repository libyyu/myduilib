#include "itag.h"
#include <stdio.h>

int main()
{
	int err = 0;
	ITag* ptag = plugin_load_tag("/Volumes/SHARED/WorkSpace/duilib/1-07 生日快乐 (Live).mp3", true, ITag::ReadAudioStyle::Fast, &err);
	const wchar_t* author = ptag->artist();
    const wchar_t* name = ptag->filename();
    wprintf(L"err = %d, author=%s, %s\n", err, author, name);
	return 0;
}
