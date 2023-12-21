#pragma once
#include <fstream>
using namespace std;

// ������, ������� �������������
class IDObject
{
protected:
	// ������������� �������
	unsigned long m_id;
};

// ������, ������� ���
class NamedObject : IDObject
{
protected:
	// ��� �������
	char* m_name;
};

// ������, ������� ��������
class TitledObject : NamedObject
{
protected:
	// �������� �������
	char* m_title;
};

class IDownloadable abstract
{
public:
	virtual void loadv(const FILE* f);		// ��������� ������ �� �����
	virtual void savev(const FILE* f);		// ��������� ������ � ����
};