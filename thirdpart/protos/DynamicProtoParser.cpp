#include "DynamicProtoParser.h"

class MyProtoImportErrorCollector : public pb::compiler::MultiFileErrorCollector
{
	DynamicProtoParser* _pThis;
	virtual void AddError(const std::string& filename, int line, int column, const std::string& message) override
	{
		_pThis->OnImportError(filename, line, column, message);
	}
public:
	MyProtoImportErrorCollector(DynamicProtoParser* pThis) :_pThis(pThis) {}
};

DynamicProtoParser::DynamicProtoParser()
	: m_pImporter(nullptr)
	, m_pMainFileDescriptor(nullptr)
{

}
DynamicProtoParser::~DynamicProtoParser()
{
	Reset();
}

void DynamicProtoParser::Reset()
{
	if (m_pImporter) delete m_pImporter;
	m_pImporter = nullptr;
	m_FileDescriptorHash.clear();
	m_pMainFileDescriptor = nullptr;
	m_package = "";
}

/*
���벢����proto�ļ��� �ɹ�����true
*/
bool
DynamicProtoParser::ImportFile(const std::string& szPath, const std::string& szProtoFile)
{
	std::string szFile = szPath + "/" + szProtoFile;
	std::replace(szFile.begin(), szFile.end(), '\\', '/');
	if (m_FileDescriptorHash.count(szFile) > 0)
		return true;

	if (m_pImporter)
	{
		delete m_pImporter;
		m_pImporter = nullptr;
	}

	pb::compiler::DiskSourceTree sourceTree;
	sourceTree.MapPath("", szPath);
	MyProtoImportErrorCollector errorCollector(this);
	m_pImporter = new pb::compiler::Importer(&sourceTree, &errorCollector);
	const pb::FileDescriptor* pMainDescriptor = m_pImporter->Import(szProtoFile);
	if (!pMainDescriptor)
		return false;
	m_pMainFileDescriptor = pMainDescriptor;
	m_FileDescriptorHash.insert(std::pair<std::string, const pb::FileDescriptor*>(szFile, pMainDescriptor));

	//dependent
	for (int iDependent = 0; iDependent < pMainDescriptor->dependency_count(); ++iDependent)
	{
		const pb::FileDescriptor* pDependentDescriptor = pMainDescriptor->dependency(iDependent);
		std::string pbFileName = pDependentDescriptor->name();
		szFile = szPath + "/" + pbFileName;
		std::replace(szFile.begin(), szFile.end(), '\\', '/');
		if (m_FileDescriptorHash.count(szFile) == 0)
			m_FileDescriptorHash.insert(std::pair<std::string, const pb::FileDescriptor*>(szFile, pDependentDescriptor));
	}

	return true;
}

/*
�����ļ���ȡ������ϢFileDescriptor*
*/
const pb::FileDescriptor*
DynamicProtoParser::GetFileDescriptorByFileName(const std::string& szPath, const std::string& szProtoFile)
{
	std::string szFile = szPath + "/" + szProtoFile;
	std::replace(szFile.begin(), szFile.end(), '\\', '/');
	if (m_FileDescriptorHash.count(szFile) > 0)
		return m_FileDescriptorHash[szFile];

	return nullptr;
}

/*
����ص�����
*/
void
DynamicProtoParser::OnImportError(const std::string& filename, int line, int column, const std::string& message)
{
	std::cerr << filename << "\tline:" << line << "\tcolum:" << column << "\t" << message << std::endl;
}