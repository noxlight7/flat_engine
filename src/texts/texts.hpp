#pragma once
#include <fstream>
using namespace std;

// Объект, имеющий идентификатор
class IDObject
{
protected:
	// Идентификатор объекта
	unsigned long m_id;
};

// Объект, имеющий имя
class NamedObject : IDObject
{
protected:
	// Имя объекта
	char* m_name;
};

// Объект, имеющий название
class TitledObject : NamedObject
{
protected:
	// Название объекта
	char* m_title;
};

class IDownloadable abstract
{
public:
	virtual void loadv(const FILE* f);		// Загружает объект из файла
	virtual void savev(const FILE* f);		// Сохраняет объект в файл
};