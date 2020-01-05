#pragma once

#include "tag_base.h"
#include "IO.h"
#include <string>

namespace em_mp3_tag
{

class ape_tag_factory : public mp3_tag_factory
{
public:
	ape_tag_factory();
	~ape_tag_factory();
	virtual mp3_tag_interface* create_tag(io_base* p_file, unsigned int n_begin, unsigned int n_end);
};

}		//end namespace em_mp3_tag
