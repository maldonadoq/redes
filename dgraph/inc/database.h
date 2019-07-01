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

inline bool exists(const string& name){
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}

// to select
static int callback(void* data, int argc, char** argv, char** azColName){
	int i;
	// fprintf(stderr, "%s: ", (const char*)data);
	// printf("\n");
	tmp.clear();
	for (i = 0; i < argc; i++) {
		// printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		if(argv[i]){
			tmp.push_back(argv[i]);
		}
		else{
			tmp.push_back("NULL");
		}
	}
	result_select.push_back(tmp);

	// printf("\n");
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

	void tcreate_table(string);				// Create Table

	/*
		CRUD
	*/
	bool tinsert(string);								// C
	bool tupdate(string);								// U
	bool tdelete(string);								// D 
	bool tselect(string, vector<vector<string> > &);	// R
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
    	}
    }

    string sql;
    sql = 	"CREATE TABLE NODE("
				"ID 		INTEGER PRIMARY KEY AUTOINCREMENT, "
				"NAME 		TEXT 	NOT NULL UNIQUE, "
				"ATTRIBUTE 	TEXT"
			");";
	tcreate_table(sql);

	sql = 	"CREATE TABLE RELATION("
				"ID 		INTEGER PRIMARY KEY AUTOINCREMENT, "
				"NAME1 		TEXT                NOT NULL, "
				"NAME2 		TEXT                NOT NULL"
			");";
	tcreate_table(sql);
}

void TDatabase::tcreate_table(string _sql){
	char* messaggeError; 
	int rq = sqlite3_exec(m_sqlite, _sql.c_str(), NULL, 0, &messaggeError); 
  
    if (rq != SQLITE_OK) { 
        cout << messaggeError << "\n";
        sqlite3_free(messaggeError); 
    } 
    else{
        cout << "Table created Successfully\n";
    }
}

bool TDatabase::tinsert(string _sql){
	char* messaggeError; 
	int rq = sqlite3_exec(m_sqlite, _sql.c_str(), NULL, 0, &messaggeError); 

    if (rq != SQLITE_OK) { 
        cout << messaggeError << "\n";
        sqlite3_free(messaggeError); 

        return false;
    }

    return true;
}

bool TDatabase::tselect(string _sql, vector<vector<string> > &_res){
	result_select.clear();
	_res.clear();

	string data("callback function");
	int rq = sqlite3_exec(m_sqlite, _sql.c_str(), callback, (void*)data.c_str(), NULL);

	if(rq != SQLITE_OK){
		cerr << "Error select\n";
		return false;
	}

	_res = result_select;

	return true;
}

bool TDatabase::tupdate(string _sql){
	char* messaggeError; 
	int rq = sqlite3_exec(m_sqlite, _sql.c_str(), NULL, 0, &messaggeError); 

    if (rq != SQLITE_OK) { 
        cout << messaggeError << "\n";
        sqlite3_free(messaggeError); 
        return false;
    } 

    return true;
}

bool TDatabase::tdelete(string _sql){
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