#include "DynamicMessageToDataTable.h"
#include <assert.h>


DynamicMessageToDataTable::DynamicMessageToDataTable()
{
}


DynamicMessageToDataTable::~DynamicMessageToDataTable()
{
}

bool DynamicMessageToDataTable::serialToDataTable(const pb::Message* message, TABLE& table, bool exportHead)
{
	assert(message);
	
	if (exportHead)
	{
		ROW title;
		ROW type;
		ROW comment;
		table.push_back(title);
		table.push_back(type);
		table.push_back(comment);
		if(!serialHead(message, table))
			return false;
	}


	return serialBody(message, table);
}

std::string& DynamicMessageToDataTable::Trim(std::string& text)
{
	if (!text.empty())
	{
		text.erase(0, text.find_first_not_of((" \n\r\t")));
		text.erase(text.find_last_not_of((" \n\r\t")) + 1);

		size_t pos = text.find_first_of(',');
		if (pos != std::string::npos)
		{
			text = text.substr(0, pos);
		}
	}
	return text;
}

int DynamicMessageToDataTable::GetDefaultListSize(const pb::FieldDescriptor * fd)
{
	int count = fd->options().unknown_fields().field_count();
	for (int ui = 0; ui < count; ++ui)
	{
		const auto& uf = fd->options().unknown_fields().field(ui);
		if (uf.number() == 61003)
		{
			pb::uint64 n = uf.varint();
			return (int)n;
		}
		else if (uf.number() == 61004)
		{
			pb::uint64 n = uf.varint();
			return (int)n;
		}
	}
	return 0;
}

bool DynamicMessageToDataTable::serialHead(const pb::Message* message, TABLE& table, const std::string& prefix, const std::string& prefixname)
{
	const pb::Descriptor* descriptor = message->GetDescriptor();
	assert(descriptor);
	size_t cnt = table.size();
	ROW& title = table[cnt-3];
	ROW& type = table[cnt-2];
	ROW& comment = table[cnt - 1];

	auto LocalTitle = [&prefixname](const pb::FieldDescriptor * fd, int idx)->std::string
	{
		std::string arrayIndex = "[" + std::to_string(idx) + "]";
		std::string pre = prefixname.empty() ? "" : prefixname + ".";
		if (fd->is_repeated())
		{	
			return pre + fd->name() + arrayIndex;
		}
		else
			return pre + fd->name();
	};

	for (int i = 0; i < descriptor->field_count(); i++)
	{
		const pb::FieldDescriptor * fd = descriptor->field(i);
		const pb::FieldDescriptor::Type tp = fd->type();
		if (!fd->is_repeated() && tp != pb::FieldDescriptor::Type::TYPE_MESSAGE) 
		{//basic type
			title.push_back(LocalTitle(fd, -1));
			type.push_back(fd->TypeName(tp));
			//comment
			std::string cdesc;
			pb::SourceLocation* location = new pb::SourceLocation;
			if (fd->GetSourceLocation(location))
				cdesc = Trim(location->trailing_comments);
			delete location;
			if (cdesc.empty())
				cdesc = fd->name();
			comment.push_back(prefix + cdesc);
		}
		else if (fd->is_repeated() && tp != pb::FieldDescriptor::Type::TYPE_MESSAGE) 
		{//basic type array
			int cc = GetDefaultListSize(fd);
			for (int idx = 0; idx < cc; ++idx)
			{
				title.push_back(LocalTitle(fd, idx));
				type.push_back(fd->TypeName(tp));
				//comment
				std::string cdesc;
				pb::SourceLocation* location = new pb::SourceLocation;
				if (fd->GetSourceLocation(location))
					cdesc = Trim(location->trailing_comments);
				delete location;
				if (cdesc.empty())
					cdesc = fd->name();
				comment.push_back(prefix + std::to_string(idx + 1) + cdesc);
			}
		}
		else if (!fd->is_repeated() && tp == pb::FieldDescriptor::Type::TYPE_MESSAGE) 
		{//sub message type
			const pb::Message &msg = message->GetReflection()->GetMessage(*message, fd);
			std::string pre;
			pb::SourceLocation* location = new pb::SourceLocation;
			if (fd->GetSourceLocation(location))
				pre = Trim(location->trailing_comments);
			if(pre.empty())
				pre = fd->name();
			delete location;

			if (!serialHead(&msg, table, pre, LocalTitle(fd, -1))) return false;
		}
		else
		{//sub message type array
			std::string pre;
			pb::SourceLocation* location = new pb::SourceLocation;
			if (fd->GetSourceLocation(location))
				pre = Trim(location->trailing_comments);
			if (pre.empty())
				pre = fd->name();
			delete location;

			pb::DynamicMessageFactory factory;
			const pb::Message* msg = factory.GetPrototype(fd->message_type());

			int cc = GetDefaultListSize(fd);
			for (int idx = 0; idx < cc; ++idx)
			{				
				if (!serialHead(msg, table, pre + std::to_string(idx + 1), LocalTitle(fd, idx))) return false;
			}
		}
	}

	return true;
}

bool DynamicMessageToDataTable::serialBody(const pb::Message* message, TABLE& table)
{
	const pb::Descriptor* descriptor = message->GetDescriptor();
	assert(descriptor);
	if (descriptor->field_count() == 0) return true;
	table.push_back(ROW());
	
	return serialBodyRecursively(message, table);
}

bool DynamicMessageToDataTable::serialBodyRecursively(const pb::Message* message, TABLE& table)
{
	const pb::Descriptor* descriptor = message->GetDescriptor();
	const pb::Reflection* reflection = message->GetReflection();
	assert(descriptor);
	assert(reflection);
	size_t cnt = table.size();
	ROW& row = table[cnt - 1];

	for (int i = 0; i < descriptor->field_count(); i++)
	{
		const pb::FieldDescriptor * fd = descriptor->field(i);
		const pb::FieldDescriptor::Type tp = fd->type();
		
		if (!fd->is_repeated())
		{
			std::string valueString;
			switch (tp)
			{
				// optional bool
			case pb::FieldDescriptor::TYPE_BOOL:
			{
				bool value = reflection->GetBool(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional int32 �䳤���뷽ʽ�����븺��ʱ������Ч
			case pb::FieldDescriptor::TYPE_INT32:
			{
				int32_t value = reflection->GetInt32(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional int64 �䳤���뷽ʽ�����븺��ʱ������Ч
			case pb::FieldDescriptor::TYPE_INT64:
			{
				int64_t value = reflection->GetInt64(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional uint32 �䳤���뷽ʽ
			case pb::FieldDescriptor::TYPE_UINT32:
			{
				uint32_t value = reflection->GetUInt32(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional uint64 �䳤���뷽ʽ
			case pb::FieldDescriptor::TYPE_UINT64:
			{
				uint64_t value = reflection->GetUInt64(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional float
			case pb::FieldDescriptor::TYPE_FLOAT:
			{
				float value = reflection->GetFloat(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional double
			case pb::FieldDescriptor::TYPE_DOUBLE:
			{
				double value = reflection->GetDouble(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional ö��ֵ
			case pb::FieldDescriptor::TYPE_ENUM:
			{
				int32_t value = reflection->GetEnumValue(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional bytes ����˳���ֽ�����
			case pb::FieldDescriptor::TYPE_BYTES:
			{
				std::string value = reflection->GetString(*message, fd);
				valueString = value;
				break;
			}
			// optional string ������UTF-8�������ASCII����
			case pb::FieldDescriptor::TYPE_STRING:
			{
				std::string value = reflection->GetString(*message, fd);
				valueString = value;
				break;
			}
			// optional embedded message ���ݹ鴦��
			case pb::FieldDescriptor::TYPE_MESSAGE:
			{
				break;
			}
			// optional fixed32 ����4�ֽڣ���ֵ��2^28��Ļ��������uint32��Ч
			case pb::FieldDescriptor::TYPE_FIXED32:
			{
				uint32_t value = reflection->GetUInt32(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional fixed64 ����8�ֽڣ���ֵ��2^56��Ļ��������uint64��Ч
			case pb::FieldDescriptor::TYPE_FIXED64:
			{
				uint64_t value = reflection->GetUInt64(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional sfixed32 ����4�ֽ�
			case pb::FieldDescriptor::TYPE_SFIXED32:
			{
				int32_t value = reflection->GetInt32(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional sfixed64 ����8�ֽ�
			case pb::FieldDescriptor::TYPE_SFIXED64:
			{
				int64_t value = reflection->GetInt64(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional sint32 Z�α䳤���뷽ʽ��Խ����0Խ��Ч
			case pb::FieldDescriptor::TYPE_SINT32:
			{
				int32_t value = reflection->GetInt32(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			// optional sint64 Z�α䳤���뷽ʽ��Խ����0Խ��Ч
			case pb::FieldDescriptor::TYPE_SINT64:
			{
				int64_t value = reflection->GetInt64(*message, fd);
				valueString = std::to_string(value);
				break;
			}
			default:
				break;
			}
			if (tp != pb::FieldDescriptor::TYPE_MESSAGE)
				row.push_back(valueString);
			else
			{
				if (!serialBodyRecursively(&reflection->GetMessage(*message, fd), table))
					return false;
			}
		}
		else
		{
			int size = reflection->FieldSize(*message, fd);
			for (int j = 0; j < size; ++j)
			{
				std::string valueString;
				switch (tp)
				{
				// repeated bool
				case pb::FieldDescriptor::TYPE_BOOL:
				{
					bool value = reflection->GetRepeatedBool(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated int32 �䳤���뷽ʽ�����븺��ʱ������Ч
				case pb::FieldDescriptor::TYPE_INT32:
				{
					int32_t value = reflection->GetRepeatedInt32(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated int64 �䳤���뷽ʽ�����븺��ʱ������Ч
				case pb::FieldDescriptor::TYPE_INT64:
				{
					int64_t value = reflection->GetRepeatedInt64(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated uint32 �䳤���뷽ʽ
				case pb::FieldDescriptor::TYPE_UINT32:
				{
					uint32_t value = reflection->GetRepeatedUInt32(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated uint64 �䳤���뷽ʽ
				case pb::FieldDescriptor::TYPE_UINT64:
				{
					uint64_t value = reflection->GetRepeatedUInt64(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated float
				case pb::FieldDescriptor::TYPE_FLOAT: {
					float value = reflection->GetRepeatedFloat(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated double
				case pb::FieldDescriptor::TYPE_DOUBLE:
				{
					double value = reflection->GetRepeatedDouble(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated ö��ֵ
				case pb::FieldDescriptor::TYPE_ENUM:
				{
					int32_t value = reflection->GetRepeatedEnumValue(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated bytes ����˳���ֽ�����
				case pb::FieldDescriptor::TYPE_BYTES:
				{
					std::string value = reflection->GetRepeatedString(*message, fd, j);
					valueString = value;
					break;
				}
				// repeated string ������UTF-8�������ASCII����
				case pb::FieldDescriptor::TYPE_STRING:
				{
					std::string value = reflection->GetRepeatedString(*message, fd, j);
					valueString = value;
					break;
				}
				// repeated embedded message ���ݹ鴦��
				case pb::FieldDescriptor::TYPE_MESSAGE:
				{
					break;
				}
				// repeated fixed32 ����4�ֽڣ���ֵ��2^28��Ļ��������uint32��Ч
				case pb::FieldDescriptor::TYPE_FIXED32:
				{
					uint32_t value = reflection->GetRepeatedUInt32(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated fixed64 ����8�ֽڣ���ֵ��2^56��Ļ��������uint64��Ч
				case pb::FieldDescriptor::TYPE_FIXED64:
				{
					uint64_t value = reflection->GetRepeatedUInt64(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated sfixed32 ����4�ֽ�
				case pb::FieldDescriptor::TYPE_SFIXED32:
				{
					int32_t value = reflection->GetRepeatedInt32(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated sfixed64 ����8�ֽ�
				case pb::FieldDescriptor::TYPE_SFIXED64:
				{
					int64_t value = reflection->GetRepeatedInt64(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated sint32 Z�α䳤���뷽ʽ��Խ����0Խ��Ч
				case pb::FieldDescriptor::TYPE_SINT32:
				{
					int32_t value = reflection->GetRepeatedInt32(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				// repeated sint64 Z�α䳤���뷽ʽ��Խ����0Խ��Ч
				case pb::FieldDescriptor::TYPE_SINT64:
				{
					int64_t value = reflection->GetRepeatedInt64(*message, fd, j);
					valueString = std::to_string(value);
					break;
				}
				default:
					break;
				}
				if (tp != pb::FieldDescriptor::TYPE_MESSAGE)
					row.push_back(valueString);
				else				
				{
					const pb::Descriptor* subMessageDescriptor = fd->message_type();
					if (!serialBodyRecursively(&reflection->GetRepeatedMessage(*message, fd, j), table))
						return false;
				}
			}
		}
	}
	return true;
}