#pragma once

#include <vector>
#include <tchar.h>
//#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options/detail/convert.hpp>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>
class ISerialize
{
public:
#ifdef _UNICODE 
	typedef wchar_t              chartype;
	typedef std::wstring         stringtype;
	typedef const chartype* pcchar_type;
	typedef boost::property_tree::wiptree node_type;
	typedef node_type::assoc_iterator   node_itor_type;
	typedef std::wostringstream  sstream;
	typedef std::wifstream       ifstream;
	typedef std::wofstream       ofstream;
#else
	typedef char                 chartype;
	typedef std::string          stringtype;
	typedef const chartype* pcchar_type;
	typedef boost::property_tree::iptree node_type;
	typedef node_type::assoc_iterator   node_itor_type;
	typedef std::ostringstream  sstream;
	typedef std::ifstream       ifstream;
	typedef std::ofstream       ofstream;
#endif

	bool Load(ISerialize* pIserial);
	bool Load(const stringtype& s);
	bool Load(pcchar_type s);
	bool LoadFromFile(pcchar_type sFile);

	bool SaveToStream(sstream& ss);
	bool SaveToFile(pcchar_type sFile);



	virtual bool serial(node_type& node,bool isload) = 0;
	bool serial(node_type& node,bool isload, pcchar_type nodename);

	template<typename K>
	static bool serial_range(std::vector<K>& v,node_type& node,bool isload, pcchar_type nodename)
	{
		typedef typename std::vector<K>::iterator iter;
		if(isload)
		{
			v.clear();
			node_itor_type it = node.find(nodename);
			if(it != node.not_found())
			{
				BOOST_FOREACH(node_type::value_type& i,it->second)
				{
					node_type& item_node = i.second;
					K value;
					if(value.serial(item_node,isload))
						v.push_back(value);
				}
			}
		}
		else
		{
			node_type list_node;
			for (iter it = v.begin(),iend = v.end(); it != iend;++it)
			{
				node_type item_node;
				if(!it->serial(item_node,isload))
					return false;
				list_node.push_back(node_type::value_type(_T(""),item_node));
			}

			node.add_child(nodename,list_node);
		}

		return true;
	}

	template<typename K>
	static bool serial_range(std::vector<boost::shared_ptr<K> >& v,node_type& node,bool isload, pcchar_type nodename)
	{
		typedef typename boost::shared_ptr<K>  value_type;
		typedef typename std::vector<boost::shared_ptr<K> >::iterator iter;
		if(isload)
		{
			v.clear();
			node_itor_type it = node.find(nodename);
			if(it != node.not_found())
			{
				BOOST_FOREACH(node_type::value_type& i,it->second)
				{
					node_type& item_node = i.second;
					value_type value(new K());
					if(value->serial(item_node,isload))
						v.push_back(value);
				}
			}
		}
		else
		{
			node_type list_node;
			for (iter it = v.begin(),iend = v.end(); it != iend;++it)
			{
				node_type item_node;
				if(!(*it)->serial(item_node,isload))
					return false;
				list_node.push_back(node_type::value_type(_T(""),item_node));
			}

			node.add_child(nodename,list_node);
		}

		return true;
	}
};
