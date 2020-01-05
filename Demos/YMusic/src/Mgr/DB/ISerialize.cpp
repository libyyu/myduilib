#include "stdafx.h"
#include "ISerialize.h"
#include <string>
#include <codecvt>

#include "YApp.h"


bool ISerialize::serial(node_type& node,bool isload, pcchar_type nodename)
{
	assert(!node.empty());
	assert(nodename);

	if(isload)
	{
		node_itor_type it = node.find(nodename);
		if(node.not_found() == it)
			return assert(false),false;
		return serial(it->second,isload);
	}
	else
	{
		node_type new_node;
		if(!serial(new_node,isload))
			return false;

		node.add_child(nodename,new_node);
	}

	return true;
}
bool ISerialize::Load(ISerialize* pIserial)
{
	assert(pIserial);
	if(pIserial)
	{
		node_type node;
		if(pIserial->serial(node,false))
			return this->serial(node,true);
	}
	return false;
}
bool ISerialize::Load(const stringtype& s)
{
#ifdef _UNICODE
	typedef std::wstring_convert<std::codecvt_utf8<wchar_t> > w2utf8CoderT;
	w2utf8CoderT coder;
	node_type node;
	try
	{
		std::wistringstream stream(coder.from_bytes(s));
		read_json(stream,node);
		return serial(node,true);
	}
	catch(std::exception& e)
	{
		sLogError(_T("LoadFromFile error(%s)"),e.what());
	}
	catch (...)
	{
	}
	assert(false);
	return false;
#else
	node_type node;
	try
	{
		std::istringstream stream(s);
		read_json(stream, node);
		return serial(node, true);
	}
	catch (std::exception&)
	{
		//sLogError(_T("LoadFromFile error(%s)"), e.what());
	}
	catch (...)
	{
	}
	assert(false);
	return false;
#endif
}
bool ISerialize::Load(pcchar_type s)
{
#ifdef _UNICODE
	std::locale oldLocale;
	std::locale utf8Locale(oldLocale,new boost::program_options::detail::utf8_codecvt_facet());
	std::wistringstream jsonIStream;
	jsonIStream.imbue(utf8Locale);
	jsonIStream.str(s);

	try
	{
		node_type node;
		read_json(jsonIStream,node);
		return serial(node,true);
	}
	catch(std::exception& e)
	{
		sLogError(_T("LoadFromFile error(%s)"),e.what());
	}
	catch (...)
	{
	}
	assert(false);
	return false;
#else
	std::locale oldLocale;
	std::locale utf8Locale(oldLocale, new boost::program_options::detail::utf8_codecvt_facet());
	std::istringstream jsonIStream;
	jsonIStream.imbue(utf8Locale);
	jsonIStream.str(s);

	try
	{
		node_type node;
		read_json(jsonIStream, node);
		return serial(node, true);
	}
	catch (std::exception&)
	{
//		sLogError("LoadFromFile error(%s)", e.what());
	}
	catch (...)
	{
	}
	assert(false);
	return false;
#endif
}
bool ISerialize::LoadFromFile(pcchar_type sFile)
{
	assert(sFile);
	std::locale oldLocale;
	std::locale utf8Locale(oldLocale,new boost::program_options::detail::utf8_codecvt_facet());
	ISerialize::ifstream strem(sFile);
	strem.imbue(utf8Locale);

	try
	{
		node_type node;
		read_json(strem,node);
		return serial(node,true);
	}
	catch(std::exception&)
	{
//		sLogError("LoadFromFile error(%s)",e.what());
	}
	catch (...)
	{
	}
	assert(false);
	return false;
}
bool ISerialize::SaveToStream(ISerialize::sstream& ss)
{
	std::locale oldLocale;
	std::locale utf8Locale(oldLocale,new boost::program_options::detail::utf8_codecvt_facet());

	node_type node;
	serial(node,false);

	ss.imbue(utf8Locale);
	try
	{
		write_json(ss,node);
		return true;
	}
	catch(std::exception&)
	{
//		sLogError("LoadFromFile error(%s)",e.what());
		assert(false);
	}
	catch (...)
	{
		assert(false);
	}
	
	return false;
}

bool ISerialize::SaveToFile(pcchar_type sFile)
{
	std::locale oldLocale;
	std::locale utf8Locale(oldLocale,new boost::program_options::detail::utf8_codecvt_facet());
	ISerialize::ofstream strem(sFile);
	strem.imbue(utf8Locale);

	try
	{
		node_type node;
		serial(node,false);
		write_json(strem,node);
		return true;
	}
	catch(std::exception&)
	{
//		sLogError("LoadFromFile error(%s)",e.what());
		assert(false);
	}
	catch (...)
	{
		assert(false);
	}
	return false;
}