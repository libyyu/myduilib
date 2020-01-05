#include "ape_tag_wrapper.h"
#include <cassert>
#include "IO.h"
#include "All.h"
#include "ape_tag.h"
#include <boost/shared_array.hpp>

namespace em_mp3_tag
{

ape_tag_factory::ape_tag_factory()
{
}

ape_tag_factory::~ape_tag_factory()
{
}

mp3_tag_interface* ape_tag_factory::create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end)
{
	assert(p_file != NULL);
	if (n_end - n_begin < APE_TAG_FOOTER_BYTES)
		return NULL;
	APE_TAG_FOOTER tag_footer;
	assert(sizeof(tag_footer) == APE_TAG_FOOTER_BYTES);
	int n_result = 0;
	n_result = p_file->seek(n_end - APE_TAG_FOOTER_BYTES, FILE_BEGIN);
	if (n_result != 0)
		return NULL;
	int n_offset = p_file->get_position();
	unsigned int n_bytes_read = 0;
	n_result = p_file->read(&tag_footer, APE_TAG_FOOTER_BYTES, &n_bytes_read);
	if (n_result != 0 || n_bytes_read != APE_TAG_FOOTER_BYTES)
		return NULL;
	if (!tag_footer.GetIsValid(true))
		return NULL;
	
	int n_field_bytes = tag_footer.GetFieldBytes();
	int n_tag_bytes = tag_footer.GetTotalTagBytes();
	
	n_result = p_file->seek(n_end - n_tag_bytes, FILE_BEGIN);
	if (n_result != 0)
		return NULL;

	boost::shared_array<unsigned char> sp_buf(new unsigned char[n_tag_bytes]);
	n_bytes_read = 0;
	n_result = p_file->read(sp_buf.get(), n_tag_bytes, &n_bytes_read);
	if (n_result != 0 || n_bytes_read != n_tag_bytes)
		return NULL;
	CAPETag* p_ape_tag = new CAPETag();
	bool b_result = p_ape_tag->init(sp_buf, n_end - n_tag_bytes, n_tag_bytes, &tag_footer);
	if (!b_result)			//ÄÚ²¿½âÎö×Ö¶ÎÊ§°Ü
	{
		delete p_ape_tag;
		p_ape_tag = NULL;
	}
	return p_ape_tag;
}

}			//end namespace em_mp3_tag
