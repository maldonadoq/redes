#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <iostream>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <sqlite3.h>

using std::string;
using std::vector;
using std::cerr;
using std::cout;

vector<vector<string> > result_select;
vector<string> tmp;
bool header;

inline bool exists(const string& name){
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

// to select
static int callback(void* data, int argc, char** argv, char** azColName){
	int i;

	if(header){
		tmp.clear();
		tmp.resize(argc);

		for(i=0; i<argc; i++)
			tmp[i] = azColName[i];

		result_select.push_back(tmp);
		header = false;
	}

	for(i=0; i<argc; i++){
		if(argv[i]){
			tmp[i] = argv[i];
		}
		else{
			tmp[i] = "NULL";
		}
	}	
	result_select.push_back(tmp);
	return 0;
}

class TDatabase{
private:
	sqlite3* m_sqlite;
	string m_name;
public:	
	TDatabase(string);
	TDatabase();
	~TDatabase();

	/*
		CRUD
	*/
	bool execute(string); 
	bool execute(string, vector<vector<string> > &);
};

TDatabase::TDatabase(){

}

TDatabase::TDatabase(string _name){
	this->m_name = _name;
    
	bool de = exists(m_name);

    int rq = 0; 
    rq = sqlite3_open(m_name.c_str(), &m_sqlite);
  
    if(rq){
        cerr << "Error Create Database " << sqlite3_errmsg(m_sqlite) << "\n";
    }
    else{
    	if(de){
    		cout << "Database Exists!\n";
    	}
    	else{
    		cout << "Created Database Successfully!\n";
			string sql;
			sql = 	"CREATE TABLE NODE("
						"ID 		INTEGER PRIMARY KEY AUTOINCREMENT, "
						"NAME 		TEXT 	NOT NULL UNIQUE, "
						"ATTRIBUTE 	TEXT"
					");";
			execute(sql);

			sql = 	"CREATE TABLE RELATION("
						"ID 		INTEGER PRIMARY KEY AUTOINCREMENT, "
						"NAME1 		TEXT                NOT NULL, "
						"NAME2 		TEXT                NOT NULL"
					");";
			execute(sql);
    	}
    }
}

bool TDatabase::execute(string _sql, vector<vector<string> > &_res){
	result_select.clear();
	_res.clear();
	header = true;

	string data("callback function");
	int rq = sqlite3_exec(m_sqlite, _sql.c_str(), callback, (void*)data.c_str(), NULL);

	if(rq != SQLITE_OK){
		cerr << "Error select\n";
		return false;
	}

	_res = result_select;

	return true;
}

bool TDatabase::execute(string _sql){
	char* messaggeError; 
	int rq = sqlite3_exec(m_sqlite, _sql.c_str(), NULL, 0, &messaggeError); 

    if (rq != SQLITE_OK) { 
        cout << messaggeError << "\n";
        sqlite3_free(messaggeError); 
        return false;
    }

    return true;
}

TDatabase::~TDatabase(){
	sqlite3_close(m_sqlite);
}

#endif