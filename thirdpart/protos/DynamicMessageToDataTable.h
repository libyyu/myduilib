#ifndef __DYNAMICMESSAGETODATATABLE_H__
#define __DYNAMICMESSAGETODATATABLE_H__
#pragma once
#include <vector>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

namespace pb = google::protobuf;

class DynamicMessageToDataTable
{
public:
	typedef std::vector<std::string> ROW;
	typedef std::vector<std::string> COL;
	typedef std::vector<std::vector<std::string> > TABLE;

	DynamicMessageToDataTable();
	~DynamicMessageToDataTable();
	static std::string& Trim(std::string& text);
public:
	bool serialToDataTable(const pb::Message* message, TABLE& table, bool exportHead = true);
protected:
	bool serialHead(const pb::Message* message, TABLE& table, const std::string& prefixcomment = "", const std::string& prefixname = "");
	bool serialBody(const pb::Message* message, TABLE& table);
	bool serialBodyRecursively(const pb::Message* message, TABLE& table);
protected:
	int GetDefaultListSize(const pb::FieldDescriptor * fd);
};

#endif//__DYNAMICMESSAGETODATATABLE_H__
