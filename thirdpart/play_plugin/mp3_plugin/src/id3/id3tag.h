/*
 * libid3tag - ID3 tag manipulation library
 * Copyright (C) 2000-2004 Underbit Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * If you would like to negotiate alternate licensing terms, you may do
 * so by contacting: Underbit Technologies, Inc. <info@underbit.com>
 *
 * $Id: id3tag.h,v 1.17 2004/01/23 23:22:46 rob Exp $
 */

# ifndef LIBID3TAG_ID3TAG_H
# define LIBID3TAG_ID3TAG_H

# ifdef __cplusplus
extern "C" {
# endif

//# define ID3_TAG_VERSION		0x0400
# define ID3_TAG_VERSION		0x0300

# define ID3_TAG_VERSION_MAJOR(x)	(((x) >> 8) & 0xff)
# define ID3_TAG_VERSION_MINOR(x)	(((x) >> 0) & 0xff)

typedef unsigned char id3_byte_t;
typedef unsigned long id3_length_t;

typedef unsigned long id3_ucs4_t;

typedef unsigned char id3_latin1_t;
typedef unsigned short id3_utf16_t;
typedef signed char id3_utf8_t;

struct id3_tag {
  unsigned int refcount;		//引用计数
  unsigned int version;		//版本
  int flags;							//标志
  int extendedflags;				//扩展标志
  int restrictions;
  int options;						//选项
  unsigned int nframes;		//帧数
  struct id3_frame **frames;	//帧列表
  id3_length_t paddedsize;	//填充数据长度
};

# define ID3_TAG_QUERYSIZE	10

/* ID3v1 field frames */

# define ID3_FRAME_TITLE	"TIT2"
# define ID3_FRAME_ARTIST	"TPE1"
# define ID3_FRAME_ALBUM	"TALB"
# define ID3_FRAME_TRACK	"TRCK"
# define ID3_FRAME_YEAR		"TDRC"
# define ID3_FRAME_GENRE	"TCON"
# define ID3_FRAME_COMMENT	"COMM"

/* special frames */

# define ID3_FRAME_OBSOLETE	"ZOBS"	/* with apologies to the French 为什么要向法国人致谦??*/

/* tag flags */

enum {
  ID3_TAG_FLAG_UNSYNCHRONISATION     = 0x80,			//非同步frame，对mp3文件无影响
  ID3_TAG_FLAG_EXTENDEDHEADER        = 0x40,				//是否有紧贴在10字节header结构后面的变长extend header结构
  ID3_TAG_FLAG_EXPERIMENTALINDICATOR = 0x20,
  ID3_TAG_FLAG_FOOTERPRESENT         = 0x10,

  ID3_TAG_FLAG_KNOWNFLAGS            = 0xf0
};

/* tag extended flags */

enum {
  ID3_TAG_EXTENDEDFLAG_TAGISANUPDATE   = 0x40,
  ID3_TAG_EXTENDEDFLAG_CRCDATAPRESENT  = 0x20,	//?使用crc校验
  ID3_TAG_EXTENDEDFLAG_TAGRESTRICTIONS = 0x10,		//?使用了tag约束

  ID3_TAG_EXTENDEDFLAG_KNOWNFLAGS      = 0x70
};

/* tag restrictions */
//tag的最大长度枚举约束
enum {
  ID3_TAG_RESTRICTION_TAGSIZE_MASK             = 0xc0,
  ID3_TAG_RESTRICTION_TAGSIZE_128_FRAMES_1_MB  = 0x00,
  ID3_TAG_RESTRICTION_TAGSIZE_64_FRAMES_128_KB = 0x40,
  ID3_TAG_RESTRICTION_TAGSIZE_32_FRAMES_40_KB  = 0x80,
  ID3_TAG_RESTRICTION_TAGSIZE_32_FRAMES_4_KB   = 0xc0
};
//文本编码枚举约束
enum {
  ID3_TAG_RESTRICTION_TEXTENCODING_MASK        = 0x20,
  ID3_TAG_RESTRICTION_TEXTENCODING_NONE        = 0x00,
  ID3_TAG_RESTRICTION_TEXTENCODING_LATIN1_UTF8 = 0x20
};

//text field的最大长度枚举约束
enum {
  ID3_TAG_RESTRICTION_TEXTSIZE_MASK            = 0x18,
  ID3_TAG_RESTRICTION_TEXTSIZE_NONE            = 0x00,
  ID3_TAG_RESTRICTION_TEXTSIZE_1024_CHARS      = 0x08,
  ID3_TAG_RESTRICTION_TEXTSIZE_128_CHARS       = 0x10,
  ID3_TAG_RESTRICTION_TEXTSIZE_30_CHARS        = 0x18
};
//图片类型枚举
enum {
  ID3_TAG_RESTRICTION_IMAGEENCODING_MASK       = 0x04,
  ID3_TAG_RESTRICTION_IMAGEENCODING_NONE       = 0x00,
  ID3_TAG_RESTRICTION_IMAGEENCODING_PNG_JPEG   = 0x04
};
//图片文件大小枚举
enum {
  ID3_TAG_RESTRICTION_IMAGESIZE_MASK           = 0x03,
  ID3_TAG_RESTRICTION_IMAGESIZE_NONE           = 0x00,
  ID3_TAG_RESTRICTION_IMAGESIZE_256_256        = 0x01,
  ID3_TAG_RESTRICTION_IMAGESIZE_64_64          = 0x02,
  ID3_TAG_RESTRICTION_IMAGESIZE_64_64_EXACT    = 0x03
};

/* library options */
//库操作

enum {
  ID3_TAG_OPTION_UNSYNCHRONISATION = 0x0001,	/* use unsynchronisation 非同步*/
  ID3_TAG_OPTION_COMPRESSION       = 0x0002,	/* use compression 使用压缩*/
  ID3_TAG_OPTION_CRC               = 0x0004,	/* use CRC使用循环冗余码校验 */

  ID3_TAG_OPTION_APPENDEDTAG       = 0x0010,	/* tag will be appended 附加tag */
  ID3_TAG_OPTION_FILEALTERED       = 0x0020,	/* audio data was altered 音频数据已经被改变*/

  ID3_TAG_OPTION_ID3V1             = 0x0100	/* render ID3v1/ID3v1.1 tag 提供  ID3v1/ID3v1.1 tag */
};

struct id3_frame {
  char id[5];								//帧标志ID, like "TIT2"
  char const *description;			//帧描述
  unsigned int refcount;				//引用计数
  int flags;									//标志
  int group_id;							//所属组ID(???)
  int encryption_method;				//加密模式
  id3_byte_t *encoded;				//压缩模式？？？
  id3_length_t encoded_length;		//压缩数据长度???
  id3_length_t decoded_length;	//声明一下解压后的长度
  unsigned int nfields;					//字段数量
  union id3_field *fields;				//字段列表
};

enum {
  /* frame status flags 帧状态标志*/
  ID3_FRAME_FLAG_TAGALTERPRESERVATION	= 0x4000,
  ID3_FRAME_FLAG_FILEALTERPRESERVATION	= 0x2000,
  ID3_FRAME_FLAG_READONLY		= 0x1000,

  ID3_FRAME_FLAG_STATUSFLAGS            = 0xff00,

  /* frame format flags 帧格式标志*/
  ID3_FRAME_FLAG_GROUPINGIDENTITY	= 0x0040,
  ID3_FRAME_FLAG_COMPRESSION		= 0x0008,
  ID3_FRAME_FLAG_ENCRYPTION		= 0x0004,
  ID3_FRAME_FLAG_UNSYNCHRONISATION	= 0x0002,
  ID3_FRAME_FLAG_DATALENGTHINDICATOR	= 0x0001,

  ID3_FRAME_FLAG_FORMATFLAGS            = 0x00ff,

  ID3_FRAME_FLAG_KNOWNFLAGS             = 0x704f
};

enum id3_field_type {
  ID3_FIELD_TYPE_TEXTENCODING,
  ID3_FIELD_TYPE_LATIN1,
  ID3_FIELD_TYPE_LATIN1FULL,
  ID3_FIELD_TYPE_LATIN1LIST,
  ID3_FIELD_TYPE_STRING,
  ID3_FIELD_TYPE_STRINGFULL,
  ID3_FIELD_TYPE_STRINGLIST,
  ID3_FIELD_TYPE_LANGUAGE,
  ID3_FIELD_TYPE_FRAMEID,
  ID3_FIELD_TYPE_DATE,
  ID3_FIELD_TYPE_INT8,
  ID3_FIELD_TYPE_INT16,
  ID3_FIELD_TYPE_INT24,
  ID3_FIELD_TYPE_INT32,
  ID3_FIELD_TYPE_INT32PLUS,
  ID3_FIELD_TYPE_BINARYDATA
};

enum id3_field_textencoding {
  ID3_FIELD_TEXTENCODING_ISO_8859_1 = 0x00,
  ID3_FIELD_TEXTENCODING_UTF_16     = 0x01,
  ID3_FIELD_TEXTENCODING_UTF_16BE   = 0x02,
  ID3_FIELD_TEXTENCODING_UTF_8      = 0x03
};

union id3_field {
  enum id3_field_type type;				//字段类型枚举值
  struct {
    enum id3_field_type type;			
    signed long value;						//整数字段
  } number;
  struct {
    enum id3_field_type type;
    id3_latin1_t *ptr;
  } latin1;
  struct {
    enum id3_field_type type;
    unsigned int nstrings;
    id3_latin1_t **strings;
  } latin1list;
  struct {
    enum id3_field_type type;
    id3_ucs4_t *ptr;
  } string;
  struct {
    enum id3_field_type type;
    unsigned int nstrings;
    id3_ucs4_t **strings;
  } stringlist;
  struct {
    enum id3_field_type type;
    char value[9];
  } immediate;
  struct {
    enum id3_field_type type;
    id3_byte_t *data;
    id3_length_t length;
  } binary;
};

/* file interface */

enum id3_file_mode {
  ID3_FILE_MODE_READONLY = 0,
  ID3_FILE_MODE_READWRITE
};
//打开mp3文件，在id3_file结构中包含了文件句柄，路径和id3_tag对象
struct id3_file *id3_file_open(char const *, enum id3_file_mode);
struct id3_file *id3_file_fdopen(int, enum id3_file_mode);
int id3_file_close(struct id3_file *);
//取得id3_tag对象
struct id3_tag *id3_file_tag(struct id3_file const *);
//更新文件
int id3_file_update(struct id3_file *);

/* tag interface */
//创建一个新的id3_tag
struct id3_tag *id3_tag_new(void);
//删除一个id3_tag
void id3_tag_delete(struct id3_tag *);
//取得id3_tag的版本
unsigned int id3_tag_version(struct id3_tag const *);
//设置option,第二个参数为mask,第三个参数为值
int id3_tag_options(struct id3_tag *, int, int);
//写明id3_tag的长度
void id3_tag_setlength(struct id3_tag *, id3_length_t);
//清除所有帧
void id3_tag_clearframes(struct id3_tag *);
//加一帧到id3_tag
int id3_tag_attachframe(struct id3_tag *, struct id3_frame *);
//从id3_tag中删除一帧
int id3_tag_detachframe(struct id3_tag *, struct id3_frame *);
//在id3_tag中查找指定帧
struct id3_frame *id3_tag_findframe(struct id3_tag const *,
				    char const *, unsigned int);
//在指定的数据块中是否包含id3tag信息
signed long id3_tag_query(id3_byte_t const *, id3_length_t);
//在指定的数据块中加载id3tag信息
struct id3_tag *id3_tag_parse(id3_byte_t const *, id3_length_t);
id3_length_t id3_tag_render(struct id3_tag const *, id3_byte_t *);

/* frame interface */
//用指定的帧名创建一帧
struct id3_frame *id3_frame_new(char const *);
//删除一帧
void id3_frame_delete(struct id3_frame *);
//从帧里用索引取得字段
union id3_field *id3_frame_field(struct id3_frame const *, unsigned int);

/* field interface */
//取得某个字段的类型枚举
enum id3_field_type id3_field_type(union id3_field const *);
//置整数字段值
int id3_field_setint(union id3_field *, signed long);
int id3_field_settextencoding(union id3_field *, enum id3_field_textencoding);
int id3_field_setstrings(union id3_field *, unsigned int, id3_ucs4_t **);
int id3_field_addstring(union id3_field *, id3_ucs4_t const *);
int id3_field_setlanguage(union id3_field *, char const *);
int id3_field_setlatin1(union id3_field *, id3_latin1_t const *);
int id3_field_setfulllatin1(union id3_field *, id3_latin1_t const *);
int id3_field_setstring(union id3_field *, id3_ucs4_t const *);
int id3_field_setfullstring(union id3_field *, id3_ucs4_t const *);
int id3_field_setframeid(union id3_field *, char const *);
int id3_field_setbinarydata(union id3_field *,
			    id3_byte_t const *, id3_length_t);

signed long id3_field_getint(union id3_field const *);
enum id3_field_textencoding id3_field_gettextencoding(union id3_field const *);
id3_latin1_t const *id3_field_getlatin1(union id3_field const *);
id3_latin1_t const *id3_field_getfulllatin1(union id3_field const *);
id3_ucs4_t const *id3_field_getstring(union id3_field const *);
id3_ucs4_t const *id3_field_getfullstring(union id3_field const *);
unsigned int id3_field_getnstrings(union id3_field const *);
id3_ucs4_t const *id3_field_getstrings(union id3_field const *,
				       unsigned int);
char const *id3_field_getframeid(union id3_field const *);
id3_byte_t const *id3_field_getbinarydata(union id3_field const *,
					  id3_length_t *);

/* genre interface */

id3_ucs4_t const *id3_genre_index(unsigned int);
id3_ucs4_t const *id3_genre_name(id3_ucs4_t const *);
int id3_genre_number(id3_ucs4_t const *);

/* ucs4 interface */

id3_latin1_t *id3_ucs4_latin1duplicate(id3_ucs4_t const *);
id3_utf16_t *id3_ucs4_utf16duplicate(id3_ucs4_t const *);
id3_utf8_t *id3_ucs4_utf8duplicate(id3_ucs4_t const *);

void id3_ucs4_putnumber(id3_ucs4_t *, unsigned long);
unsigned long id3_ucs4_getnumber(id3_ucs4_t const *);

/* latin1/utf16/utf8 interfaces */

id3_ucs4_t *id3_latin1_ucs4duplicate(id3_latin1_t const *);
id3_ucs4_t *id3_utf16_ucs4duplicate(id3_utf16_t const *);
id3_ucs4_t *id3_utf8_ucs4duplicate(id3_utf8_t const *);

/* version interface */

# define ID3_VERSION_MAJOR	0
# define ID3_VERSION_MINOR	15
# define ID3_VERSION_PATCH	1
# define ID3_VERSION_EXTRA	" (beta)"

# define ID3_VERSION_STRINGIZE(str)	#str
# define ID3_VERSION_STRING(num)	ID3_VERSION_STRINGIZE(num)

# define ID3_VERSION	ID3_VERSION_STRING(ID3_VERSION_MAJOR) "."  \
			ID3_VERSION_STRING(ID3_VERSION_MINOR) "."  \
			ID3_VERSION_STRING(ID3_VERSION_PATCH)  \
			ID3_VERSION_EXTRA

# define ID3_PUBLISHYEAR	"2000-2004"
# define ID3_AUTHOR		"Underbit Technologies, Inc."
# define ID3_EMAIL		"info@underbit.com"

extern char const id3_version[];
extern char const id3_copyright[];
extern char const id3_author[];
extern char const id3_build[];

# ifdef __cplusplus
}
# endif

# endif
