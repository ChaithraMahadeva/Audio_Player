/*
 * CSoundFileDB.h
 *
 *  Created on: Jan 18, 2020
 *      Author: Admin
 */

#ifndef MYCODE_CSOUNDFILEDB_H_
#define MYCODE_CSOUNDFILEDB_H_

#include "CDatabase.h"

class CSoundFileDB: public CDatabase
{
	int m_sfid;
	char m_fPath[DB_TEXTLEN];
	char m_fName[DB_TEXTLEN];
	int m_fs;
	int m_noOfChs;


public:
	CSoundFileDB();
	virtual ~CSoundFileDB();

	int getSfid() const;
	const char* getFName() const;
	const char* getFPath() const;
	int getFs() const;
	int getNoOfChs() const;

	bool selectAllSoundFiles();
	int selectSamplingFrequency(int sfid);
	bool insertSoundFile(string fPath, string fName, int fsr, int noOfChs);
	bool selectALLDataWithID(int id);
	int getNumSounds();


};

#endif /* MYCODE_CSOUNDFILEDB_H_ */
