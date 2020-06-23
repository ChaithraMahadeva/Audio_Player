/*
 * CAudioPlayerController.h
 *
 *  Created on: 09.01.2020
 *      Author: Chaithra Mahadeva
 */

#ifndef SRC_CAUDIOPLAYERCONTROLLER_H_
#define SRC_CAUDIOPLAYERCONTROLLER_H_
#include "CFile.h"
#include "CSoundFile.h"

#include "CFilterDB.h"
#include "CFilter.h"
#include "CUserInterface.h"
#include "CSoundFileDB.h"
#include "CAudioStreamOut.h"

class CAudioPlayerController {
private:

	CUserInterface* m_ui;
	CFilterDB m_filterColl;
	CFilter* m_pFilter;
    CSoundFileDB m_soundColl;
    CAudioStreamOut m_audiostream;
	CSoundFile* m_pSFile;
	// add the other parts of the controller (see UML Class diagram)

public:
	CAudioPlayerController();
	~CAudioPlayerController();
	void run(CUserInterface* pui);

private:
	void init();
	void chooseSound();
	void chooseFilter();
	void play();
	void chooseAmplitudescale();
	void manageSoundCollection();
	void manageFilterCollection();
	// add the other methods of the controller (see UML class diagram)
};

#endif /* SRC_CAUDIOPLAYERCONTROLLER_H_ */
