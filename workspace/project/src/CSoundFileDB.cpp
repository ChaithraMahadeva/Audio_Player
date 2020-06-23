/*
 * CSoundFileDB.cpp
 *
 *  Created on: Jan 18, 2020
 *      Author: Chaithra Mahadeva
 */

#include "CSoundFileDB.h"
#include "iostream"
using namespace std;
#include <codecvt>
#include <string>
using namespace std;

CSoundFileDB::CSoundFileDB()
{
	// TODO Auto-generated constructor stub
	m_sfid = 0;
	m_fs = 0;
	m_noOfChs = 0;
	m_fPath[0]=0;
	m_fName[0]=0;

}

const char* CSoundFileDB::getFName() const
{
	return m_fName;
}

const char* CSoundFileDB::getFPath() const
{
	return m_fPath;
}

int CSoundFileDB::getFs() const
{
	return m_fs;
}

int CSoundFileDB::getNoOfChs() const
{
	return m_noOfChs;
}

int CSoundFileDB::getSfid() const
{
	return m_sfid;
}

CSoundFileDB::~CSoundFileDB()
{
	// TODO Auto-generated destructor stub
}

bool CSoundFileDB::selectAllSoundFiles()
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	sSQL="SELECT * FROM soundfiles";
	if( _executeSQLStmt(sSQL, "selectAllSoundFiles ", true) )
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &m_sfid, 0, &m_info );
		SQLBindCol( m_hStmt, 2, SQL_C_CHAR, m_fPath, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 3, SQL_C_CHAR,m_fName, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &m_fs, 0, &m_info );
		SQLBindCol( m_hStmt, 5, SQL_C_LONG, &m_noOfChs, 0, &m_info );

		return true;
	}
	else
		return false;
}

int CSoundFileDB::selectSamplingFrequency(int sfid)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	int fs=-1;

	sSQL= "SELECT * FROM soundfiles WHERE sfid=" + to_string(sfid) + "";
	if( true == _executeSQLStmt(sSQL, "selectSamplingFrequency for a given sound ID sfid", true))
	{
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &fs, 0, &m_info );
		fetch();
		closeQuery();
	}
	return fs;
}


bool CSoundFileDB::insertSoundFile(string fPath, string fName, int fs, int noOfChs)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;


	string sSQL;

	sSQL= "INSERT INTO soundfiles (fPath,fName,fs,noOfChls) VALUES (\'" + (fPath) + "\',\'" + fName + "\'," +to_string(fs)+"," + to_string(noOfChs) + ");";
	_executeSQLStmt(sSQL, "insertSoundFile: path,name", false);

	closeQuery();
	return true;
}

bool CSoundFileDB::selectALLDataWithID(int sfid)
{
	if(m_eState!=DB_S_CONNECTED)
		return false;

	string sSQL;
	sSQL= "SELECT * FROM soundfiles WHERE sfid=" + to_string(sfid) + "";
	if( _executeSQLStmt(sSQL, "selectAllSoundFiles ", true) )
	{
		SQLBindCol( m_hStmt, 1, SQL_C_LONG, &m_sfid, 0, &m_info );
		SQLBindCol( m_hStmt, 2, SQL_C_CHAR, m_fPath, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 3, SQL_C_CHAR,m_fName, DB_TEXTLEN, &m_info );
		SQLBindCol( m_hStmt, 4, SQL_C_LONG, &m_fs, 0, &m_info );
		SQLBindCol( m_hStmt, 5, SQL_C_LONG, &m_noOfChs, 0, &m_info );

		bool success = fetch();
		if(success)
		return true;
	}

	return false;
}

int CSoundFileDB::getNumSounds()
{
	if(m_eState!=DB_S_CONNECTED)
			return false;

		string sSQL;
		int count=0;
		sSQL= "SELECT count(*) FROM soundfiles";
		if( _executeSQLStmt(sSQL, "select count of sounds ", true) )
		{
			SQLBindCol( m_hStmt, 1, SQL_C_LONG, &count, 0, &m_info );
			bool success = fetch();
			if(success)
			return count;
		}
		return 0;
}
