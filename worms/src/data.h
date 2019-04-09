#ifndef _DATA_H_
#define _DATA_H_

#include <string>

class TData{
public:
	std::string m_name;
	int m_id;
	int m_sock;
	char m_avatar;

	TData();
	~TData(){	};

	void SetName(std::string);
	void SetId(int);
	void SetAvatar(char);
};

TData::TData(){
	this->m_name = "";
}

void TData::SetName(std::string _name){
	this->m_name = _name;
}

void TData::SetId(int _id){
	this->m_id = _id;
}

void TData::SetAvatar(char _avatar){
	this->m_avatar = _avatar;
}

#endif