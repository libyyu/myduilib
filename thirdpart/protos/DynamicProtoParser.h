#ifndef __DYNAMICPROTOPARSER_H__
#define __DYNAMICPROTOPARSER_H__
#pragma once
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <functional>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
//#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/compiler/importer.h>

namespace pb = google::protobuf;

class DynamicProtoParser
{
public:
	using FileDescriptorHash = std::unordered_map<std::string, const pb::FileDescriptor*>;
	DynamicProtoParser();
	~DynamicProtoParser();
public:
public:
	void Reset();
	bool ImportFile(const std::string& szPath, const std::string& szProtoFile);
	const pb::FileDescriptor* GetFileDescriptorByFileName(const std::string& szPath, const std::string& szProtoFile);
	const pb::compiler::Importer* GetImpoter() { return m_pImporter; }
	const pb::DescriptorPool* GetPool() { return m_pImporter->pool(); };
	const FileDescriptorHash GetFileDescriptors() { return m_FileDescriptorHash; }
	const pb::FileDescriptor* GetMainFileDescriptor() { return m_pMainFileDescriptor; }
protected:
	friend class MyProtoImportErrorCollector;
	void OnImportError(const std::string& filename, int line, int column, const std::string& message);
private:
	pb::compiler::Importer*		m_pImporter;
	FileDescriptorHash			m_FileDescriptorHash;
	const pb::FileDescriptor*   m_pMainFileDescriptor;
	std::string                 m_package;
};

#endif//__DYNAMICPROTOPARSER_H__
