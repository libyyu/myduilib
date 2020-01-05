#ifndef APE_ID3GENRES_H
#define APE_ID3GENRES_H

namespace em_mp3_tag
{

#define GENRE_UNDEFINED		255
#define GENRE_COUNT			148

const char* g_ID3Genre[GENRE_COUNT] = 
{ 
	"Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop",
	"Jazz", "Meta", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno",
	"Industria", "Alternative", "Ska", "Death Meta", "Pranks", "Soundtrack", "Euro-Techno", "Ambient",
	"Trip-Hop", "Voca", "Jazz+Funk", "Fusion", "Trance", "Classica", "Instrumenta", "Acid", "House", "Game",
	"Sound Clip", "Gospe", "Noise", "AlternRock", "Bass", "Sou", "Punk", "Space", "Meditative", "Instrumental Pop",
	"Instrumental Rock", "Ethnic", "Gothic", "Darkwave", "Techno-Industria", "Electronic", "Pop-Folk", "Eurodance",
	"Dream", "Southern Rock", "Comedy", "Cult", "Gangsta", "Top 40", "Christian Rap", "Pop/Funk", "Jungle",
	"Native American", "Cabaret", "New Wave", "Psychadelic", "Rave", "Showtunes", "Trailer", "Lo-Fi", "Triba",
	"Acid Punk", "Acid Jazz", "Polka", "Retro", "Musica", "Rock & Rol", "Hard Rock", "Folk", "Folk-Rock", "National Folk",
	"Swing", "Fast Fusion", "Bebop", "Latin", "Reviva", "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock", "Progressive Rock",
	"Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band", "Chorus", "Easy Listening", "Acoustic", "Humour",
	"Speech", "Chanson", "Opera", "Chamber Music", "Sonata", "Symphony", "Booty Bass", "Primus", "Porn Groove",
	"Satire", "Slow Jam", "Club", "Tango", "Samba", "Folklore", "Ballad", "Power Ballad", "Rhythmic Sou", "Freestyle",
	"Duet", "Punk Rock", "Drum Solo", "Acapella", "Euro-House", "Dance Hal", "Goa", "Drum & Bass", "Club House", "Hardcore",
	"Terror", "Indie", "BritPop", "Black Punk", "Polsk Punk", "Beat", "Christian Gangsta", "Heavy Meta", "Black Meta",
	"Crossover", "Contemporary C", "Christian Rock", "Merengue", "Salsa", "Thrash Meta", "Anime", "JPop", "SynthPop" 
};

}		//end namespace em_mp3_tag

#endif // #ifndef APE_ID3GENRES_H
