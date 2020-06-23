/*
 * CAudioPlayerController.cpp
 *
 *  Created on: 09.01.2020
 *      Author: Wirth
 */
////////////////////////////////////////////////////////////////////////////////
// Header
#define USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>			// functions to scan files in folders (used in Lab04prep_DBAdminInsert)
using namespace std;

#include "CASDDException.h"

#include "CFile.h"
#include "CSoundFile.h"

#include "CFilterDB.h"
#include "CFilter.h"

#include "CUserInterface.h"
#include "CAudioPlayerController.h"

CAudioPlayerController::CAudioPlayerController() {
	m_pSFile = NULL;		// association with 1 or 0 CSoundFile-objects
	m_pFilter = NULL;		// association with 1 or 0 CFilter-objects
	m_ui = NULL;			// association with 1 or 0 CUserInterface-objects
}

CAudioPlayerController::~CAudioPlayerController() {
	if (m_pSFile)
		delete m_pSFile;
	if (m_pFilter)
		delete m_pFilter;
}

void CAudioPlayerController::run(CUserInterface *pui) {
	// if an exception has been thrown by init, the user is not able to use the player
	// therefore the program is terminated (unrecoverable error)
	try {
		m_ui = pui;	// set the current user interface (given by the parameter)
		init();	// initialize the components of the controller, if possible and necessary
	} catch (CASDDException &e) {
		string eMsg = "Error from: ";
		m_ui->showMessage(eMsg + e.getSrcAsString() + "" + e.getErrorText());
		return;
	}

	//////////////////////////////////////////////////
	// main menue of the player
	// todo: Add further menu items, the corresponding cases and method calls to the following code
	// note: the last item of the menu must be empty (see CUserInterfaceCmdIOW code)
	string mainMenue[] = { "select sound", "select filter","Play Sound","select Amplitude scale",
			"manage sound collection","manage filter collection", "terminate player", "" };
	while (1) {
		// if an exception will be thrown by one of the methods, the main menu will be shown
		// after an error message has been displayed. The user may decide, what to do (recoverable error)
		// For instance, if the user selects a filter without having selected a sound file before, an error
		// message tells the user to select a sound file. He/She may decide to do this and then select a filter
		// once more. In this case, the error condition is eliminated and the program may continue regularly.
		try {
			// display the menu and get the user's choice
			int selitem = m_ui->getListSelection(mainMenue);
			// process the user's choice by calling the appropriate CAudioPlayerControllerMethod
			switch (selitem) {
			case 0:
				chooseSound();
				break;
			case 1:
				chooseFilter();
				break;
			case 2:
				play();
				break;
			case 3:
				chooseAmplitudescale();
				break;
			case 4:
				manageSoundCollection();
				break;
			case 5:
				manageFilterCollection();
				break;
			default:
				return;
			}
		} catch (CASDDException &e) {
			string eMsg = "Error from: ";
			m_ui->showMessage(
					eMsg + e.getSrcAsString() + "" + e.getErrorText());
		}
	}
}

void CAudioPlayerController::init() {
	m_ui->init();
	// no printing - the controller is the only object which may initiate
	// printing via the view object (MVC design)
	m_filterColl.allowPrint(false);
	if (false == m_filterColl.open("AudioFilterCollectionDB", "root"))
		throw CASDDException(SRC_Database, -1, m_filterColl.getSQLErrorMsg());

	m_soundColl.allowPrint(false);
	if (false == m_soundColl.open("SoundcollectionDB", "root"))
		throw CASDDException(SRC_Database, -1, m_soundColl.getSQLErrorMsg());
	///////////////////
	// todo: initialize other attributes if necessary
}

void CAudioPlayerController::chooseSound() {
	int numsnd = m_soundColl.getNumSounds();
	m_soundColl.closeQuery();
	if(numsnd!=0)
	{   m_soundColl.selectAllSoundFiles();

	// prepare a string array for the user interface, that will contain  a menu with the selection of sounds
	// there is place for an additional entry for an empty string
	string *pFlt = new string[numsnd + 1];
	// prepare an integer array for the corresponding filter IDs to pass them to the user interface as well
	int *pFIDs = new int[numsnd + 1];

	for (int i = 0; i < numsnd; i++) {
		m_soundColl.fetch();
		// get a record of sound files
		// instead to print the sound file data,they will be inserted into the string array and the sound ID array
		pFIDs[i] = m_soundColl.getSfid();
		pFlt[i] = "Filename: " +  std::string(m_soundColl.getFName()) + ", File path: "
				+ std::string(m_soundColl.getFPath()) + ", fs: "
				+ to_string(m_soundColl.getFs()) + ", No of Channels: "
				+to_string(m_soundColl.getNoOfChs());
	}
	m_soundColl.closeQuery();

	// pass the arrays to the user interface and wait for the user's input
	// if the user provides a soundID which is not in pFIDs, the method returns
	// CUI_UNKNOWN
	int sid = m_ui->getListSelection(pFlt, pFIDs);

	// destroy the arrays
	delete[] pFlt;
	delete[] pFIDs;

	/////////////////////////////////////
	// create a sound according to the user's choice
	if (sid != CUI_UNKNOWN) {
		if (sid >= 0)// the user has chosen a filter from the filter collection
		{
			// get the sound's data
			m_soundColl.selectALLDataWithID(sid);

			string soundFilePath = m_soundColl.getFPath();
			string soundFileName = m_soundColl.getFName();
			string soundFileDir = soundFilePath + "\\" + soundFileName;

			if(m_pSFile)
				delete m_pSFile;

			m_pSFile = new CSoundFile(soundFileDir.c_str(), FILE_READ);
			m_pSFile->open();
		}
		m_soundColl.closeQuery();
	}
	}else{
		m_ui->showMessage(
				"Error from selectSound: Please select a valid sound!");
	}
}

void CAudioPlayerController::chooseFilter() {
	if (!m_pSFile) // a sound file must have been created by the chooseSoundFile method before
	{
		m_ui->showMessage(
				"Error from selectFilter: No sound file. Select sound file before filter!");
		return;
	}
	// get the sampling rate from the current sound file
	int fs = m_pSFile->getSampleRate();

	/////////////////////////////////////
	// list the appropriate filters for the sound
	int numflt = m_filterColl.selectNumFilters(fs); // get the number of appropriate filter files
	if (numflt)									// if there are filters that fit
	{
		// prepare a string array for the user interface, that will contain  a menu with the selection of filters
		// there is place for an additional entry for an unfiltered sound and an empty string
		string *pFlt = new string[numflt + 2];
		// prepare an integer array for the corresponding filter IDs to pass them to the user interface as well
		// there is place for -1 (unfiltered sound)
		int *pFIDs = new int[numflt + 1];

		// select the appropriate filters
		m_filterColl.selectFilters(fs);

		for (int i = 0; i < numflt; i++) {
			m_filterColl.fetch();	// get a record of filter data
			// instead to print the filter data, the will be inserted into the string array and the filter ID array
			pFIDs[i] = m_filterColl.getFilterID();
			pFlt[i] = m_filterColl.getFilterType() + "/"
					+ m_filterColl.getFilterSubType() + ", order="
					+ to_string(m_filterColl.getOrder()) + "/delay="
					+ to_string(m_filterColl.getDelay()) + "s]: "
					+ m_filterColl.getFilterInfo();
		}
		m_filterColl.closeQuery();

		// add the last menu entry for the choice of an unfiltered sound
		pFIDs[numflt] = -1;
		pFlt[numflt] = "-1 [unfiltered sound]";

		// pass the arrays to the user interface and wait for the user's input
		// if the user provides a filterID which is not in pFIDs, the method returns
		// CUI_UNKNOWN
		int fid = m_ui->getListSelection(pFlt, pFIDs);
		//m_ui->setUserInput(fid);

		// destroy the arrays
		delete[] pFlt;
		delete[] pFIDs;

		/////////////////////////////////////
		// create a filter according to the user's choice
		if (fid != CUI_UNKNOWN) {
			if (fid >= 0)// the user has chosen a filter from the filter collection
			{
				// get the filter's data
				if (true == m_filterColl.selectFilterData(fid)) {
					// if there was a filter object from a preceding choice of the user, delete this
					if (m_pFilter)
						delete m_pFilter;
					// create filter
					m_pFilter = new CFilter(m_filterColl.getACoeffs(),
							m_filterColl.getBCoeffs(), m_filterColl.getOrder(),
							m_filterColl.getDelay(), m_pSFile->getSampleRate(),
							m_pSFile->getNumChannels());
				} else {
					// wrong ID (may only accidently happen - logical error in the program?)
					m_ui->showMessage(
							"Error from selectFilter: No filter data available! Did not change filter. ");
				}
			} else	// the user has chosen not to filter the sound
			{
				if (m_pFilter)// if there was a filter object from a preceding choice of the user
				{
					delete m_pFilter;	// ... delete this
					m_pFilter = NULL;		// currently we have no filter
					m_ui->showMessage(
							"Message from selectFilter: Filter removed. ");
				}
			}
		} else
			m_ui->showMessage(
					"Error from selectFilter: Invalid filter selection! Play unfiltered sound. ");
	} else
		m_ui->showMessage(
				"Error from selectFilter: No filter available! Play unfiltered sound. ");
}

void CAudioPlayerController::play()
{
	if (!m_pSFile) // a sound file must have been created by the chooseSoundFile method before playing
	{
		m_ui->showMessage(
				"Error from play: No sound file. Select sound file before playing!");
		return;
	}

	//create buffers for filtered and unfiltered data
	float* sbufUnfiltered;
	float* sbufFiltered;
	m_pSFile->print();

	//getdata from the sound file
	int framesPerBlock=(int)(m_pSFile->getSampleRate()/8);
	int sblockSizeUnfiltered=m_pSFile->getNumChannels()*framesPerBlock;
	sbufUnfiltered = new float[sblockSizeUnfiltered];
	sbufFiltered = new float[sblockSizeUnfiltered];

	//open the audiostream with necessary parameters
	m_audiostream.open(m_pSFile->getNumChannels(), m_pSFile->getSampleRate(),8, framesPerBlock, sblockSizeUnfiltered);

	//Read the data from sound file
	int readSize = m_pSFile->read(sbufUnfiltered, framesPerBlock);

	//start the audiostream
	m_audiostream.start();

	bool iow = m_ui->iowcheck();
	switch (iow)
	{
	case true:
		cout << "Press button to start, pause and resume play back" << endl;
		break;
	case false:
		cout<<"Press enter to play";
		cin.ignore();
		break;
	}


	m_ui->wait4Key();

	bool bplay = true;
	//start playing
	while( framesPerBlock == readSize)
	{
		if(bplay == true)
		{
			if(m_pFilter)
			{
				//to play with filter
				m_pFilter->filter(sbufUnfiltered,sbufFiltered,framesPerBlock);
				m_audiostream.play(sbufFiltered,framesPerBlock);
				m_ui->showAmplitude(sbufFiltered,framesPerBlock);
				readSize = m_pSFile->read(sbufUnfiltered, framesPerBlock);
			}else{
				//to play without filter
				m_audiostream.play(sbufUnfiltered,framesPerBlock);
				m_ui->showAmplitude(sbufUnfiltered,framesPerBlock);
				readSize = m_pSFile->read(sbufUnfiltered, framesPerBlock);
			}
		}
		if(true==m_ui->wait4Key(false)){
			bplay = !bplay;
		}
	}

	m_audiostream.stop();
	m_audiostream.close();
	m_pSFile->close();

	//delete the buffers
	delete[] sbufFiltered;
	delete[] sbufUnfiltered;
}

void CAudioPlayerController::chooseAmplitudescale()
{
	//display options for the user to select
	string mainMenue[] = { "Linear scale", "Logarithmic scale", "" };

	//display option for the user to select
	int selscale = m_ui->getListSelection(mainMenue);

	switch (selscale) {
	case 0:
		m_ui->setAmplitudeScaling(SCALING_MODE_LIN);
		break;
	case 1:
		m_ui->setAmplitudeScaling(SCALING_MODE_LOG);
		break;
	default:
		return;
	}
}

void CAudioPlayerController::manageSoundCollection(){
	// user input for sound file path
	string sndfolder;
	m_ui->showMessage("Enter sound file path: ");
	sndfolder = m_ui->getUserInputPath();

	//////////////////////////////////////////
	// Code from Lab04prep_DBAdminInsert and Lab04prep_insertFilterTest
	// iterates through the folder that the user entered and inserts all
	// the filters it finds in the folder (reading txt files)
	dirent *entry;
	DIR *dp;
	string sndfile;

	dp = opendir(sndfolder.c_str());
	if (dp == NULL) {
		m_ui->showMessage("Could not open sound file folder.");
		return;
	}

	while((entry = readdir(dp)))
	{
		sndfile=entry->d_name;
		cout << sndfile << ":";
		if(sndfile.rfind(".wav")!=string::npos)		//.wav file?
		{
			if(m_pSFile)
				delete m_pSFile;

			m_pSFile = new CSoundFile((sndfolder+sndfile).c_str(),FILE_READ);
			m_pSFile->open();
			m_pSFile->print();
			if(!m_soundColl.insertSoundFile(sndfolder+sndfile,sndfile,m_pSFile->getSampleRate(),m_pSFile->getNumChannels()))
				m_ui->showMessage(m_soundColl.getSQLErrorMsg());

			m_pSFile->close();
		}
		else
			m_ui->showMessage(" irrelevant file of other type or directory");
	}
	closedir(dp);
}

void CAudioPlayerController::manageFilterCollection() {
	// user input for filter file path
	string fltfolder;
	m_ui->showMessage("Enter filter file path: ");
	fltfolder = m_ui->getUserInputPath();

	//////////////////////////////////////////
	// Code from Lab04prep_DBAdminInsert and Lab04prep_insertFilterTest
	// iterates through the folder that the user entered and inserts all
	// the filters it finds in the folder (reading txt files)
	dirent *entry;
	DIR *dp;
	string fltfile;

	dp = opendir(fltfolder.c_str());
	if (dp == NULL) {
		m_ui->showMessage("Could not open filter file folder.");
		return;
	}

	while ((entry = readdir(dp))) {
		fltfile = entry->d_name;
		m_ui->showMessage(
				"Filter file to insert into the database: " + fltfolder
				+ fltfile + ":");

		if (fltfile.rfind(".txt") != string::npos)	// txt file?
		{
			const int rbufsize = 100;// assume not more than 100 different sampling frequencies
			char readbuf[rbufsize];

			// get all sampling frequencies contained in the file
			int numFs = rbufsize;
			int fsbuf[rbufsize];
			CFilterFile::getFs(fltfolder + fltfile, fsbuf, numFs);

			for (int i = 0; i < numFs; i++)	// iterate through all found fs
			{
				CFilterFile ff(fsbuf[i], (fltfolder + fltfile).c_str(),
						FILE_READ); // create a file object for a certain fs
				ff.open();
				if (ff.read(readbuf, rbufsize))	// read information about the filter with the fs
				{
					// send information to the user interface to display it
					string fileinfo = "Inserting filter file: "
							+ ff.getFilterType() + "/" + ff.getFilterSubType()
							+ " filter [order=" + to_string(ff.getOrder())
							+ ", delay=" + to_string(ff.getDelay()) + "s, fs="
							+ to_string(fsbuf[i]) + "Hz] " + ff.getFilterInfo();
					m_ui->showMessage(fileinfo);

					// insert the filter into the filter collection database
					if (false
							== m_filterColl.insertFilter(ff.getFilterType(),
									ff.getFilterSubType(), fsbuf[i],
									ff.getOrder(), 1000. * ff.getDelay(),
									ff.getFilterInfo(), ff.getBCoeffs(),
									ff.getNumBCoeffs(), ff.getACoeffs(),
									ff.getNumACoeffs()))
						m_ui->showMessage(m_filterColl.getSQLErrorMsg()); // if error, let the user interface show the error message
				} else
					m_ui->showMessage(
							"No coefficients available for fs="
							+ to_string(fsbuf[i]) + "Hz");
				ff.close();
			}
		} else
			m_ui->showMessage(" irrelevant file of other type or directory");
	}
	closedir(dp);
}

