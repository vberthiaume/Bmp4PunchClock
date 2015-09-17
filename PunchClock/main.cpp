//
//  main.cpp
//  PunchClock
//
//  Created by Vincent Berthiaume on 2015-03-24.
//  The MIT License (MIT)

//  Copyright (c) 2015 Vincent Berthiaume
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.
//


#include <iostream>
#include <fstream>
#include <chrono>  // chrono::system_clock
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string
#include <sstream>
#include <vector>
#include <math.h>
#include <dirent.h>

using namespace std;

#ifdef _WIN32
    static const string FILEPATH = "C:/Users/Vincent/Dropbox/PunchClockHours";
#endif

#ifdef __APPLE__
    static const string FILEPATH = "/Users/nicolai/Dropbox/PunchClockHours";
#endif

#ifdef __linux__
	static const string FILEPATH = "/home/vberthiaume/Dropbox/PunchClockHours";
#endif

enum Projects {
    GRIS = 0,
    sBMP4,
    SonicBirth,
    Linux,
    Sound,
    TotalProjectCount
};

static string ProjectNames[] = {
    "GRIS",
    "sBMP4",
    "SonicBirth",
    "Linux",
    "Sound"
};

static string time2string(chrono::time_point<chrono::system_clock> p_time){
    auto in_time_t = chrono::system_clock::to_time_t(p_time);
    stringstream ss;
    ss << std::put_time(localtime(&in_time_t), "%X");
    return ss.str();
}


static void string2HMS(const string &p_time, long &iS, long &iM, long &iH){
    std::stringstream ss(p_time);
    std::string item;
    char delim(':');
    std::getline(ss, item, delim);
    iH = stoi(item);
    std::getline(ss, item, delim);
    iM = stoi(item);
    std::getline(ss, item, delim);
    iS = stoi(item);
}

static chrono::duration<int> string2duration(const string &p_time){
    long iH, iM, iS;
    string2HMS(p_time, iS, iM, iH);
    return chrono::duration<int> (iS + 60*iM + 3600* iH);
}

static string time2date(chrono::time_point<chrono::system_clock> p_time){
    auto in_time_t = chrono::system_clock::to_time_t(p_time);
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

static void sec2SMH(long elapsedTime, long &p_lS, long &p_lM, long &p_lH){
    p_lS = elapsedTime;
    p_lH  = floor(p_lS  / 3600);
    p_lS -= p_lH * 3600;
    p_lM = floor(p_lS / 60);
    p_lS -= p_lM * 60;
}

static long SMH2Sec(const long &p_lS, const long &p_lM, const long &p_lH){
    long totalSeconds = p_lS;
    totalSeconds += 60 * p_lM;
    totalSeconds += 3600 * p_lH;
    return totalSeconds;
}

class Bmp4PunchClock {
    
    bool bIsPunchedIn;
    chrono::time_point<chrono::system_clock> m_oNow, m_oPunchInTime, m_oPunchOutTime;
    chrono::duration<double> m_oElapsedTime;
    string m_strSelectedOption;
    vector<long> m_vAllTimes;
    long m_lHours, m_lMinutes, m_lSeconds;
    ofstream m_oFileOutputStream;
    bool m_bProjectJustSelected;
    
    void punchIn(){
        bIsPunchedIn = true;
        m_oPunchInTime = chrono::system_clock::now();
        
        string strPunchInTime = time2string(m_oPunchInTime);
        cout << "PUNCHED IN at " << strPunchInTime << "\n";
        
        m_oFileOutputStream << strPunchInTime << " - ";
        m_oFileOutputStream.flush();
        m_bProjectJustSelected = false;
    }
    
    void punchOut(){
        bIsPunchedIn = false;
        
        m_oPunchOutTime = chrono::system_clock::now();
        cout << "PUNCHED OUT at " << time2string(m_oPunchOutTime);
        
        string strPunchOutTime = time2string(m_oPunchOutTime);
        
        m_oFileOutputStream << strPunchOutTime;
        
        m_oElapsedTime = m_oPunchOutTime - m_oPunchInTime;
        
        //elapsedTime = chrono::seconds(3661);
        
        m_vAllTimes.push_back(chrono::duration_cast<chrono::seconds>(m_oElapsedTime).count());
        
        calculateTime(m_oElapsedTime);
        
        //cout << "time elapsed since last punch-in: "  << hours << ":" << minutes << ":" << seconds << "\n";
        
        m_oFileOutputStream << "," << m_lHours << ":" << m_lMinutes << ":" << m_lSeconds << "\n";
        m_oFileOutputStream.flush();
        sumUp();
        
    }
    
    void sumUp(){
        long totalTime = 0;
        for (auto it = m_vAllTimes.begin(); it != m_vAllTimes.end(); ++it){
            totalTime += *it;
        }
        ::sec2SMH(totalTime, m_lSeconds, m_lMinutes, m_lHours);
        cout << "; " << m_lHours << ":" << m_lMinutes << ":" << m_lSeconds << "\n";
    }
    
    void calculateTime(chrono::duration<double> elapsedTime){
        ::sec2SMH(chrono::duration_cast<chrono::seconds>(elapsedTime).count(), m_lSeconds, m_lMinutes, m_lHours);
    }

    int iSelectedProject;
    bool m_bMoreOptions;

public:
    Bmp4PunchClock()
    :bIsPunchedIn(false)
    ,iSelectedProject(-1)
    ,m_bMoreOptions(false)
    {
    }
    
    ~Bmp4PunchClock(){
        if (bIsPunchedIn){
            punchOut();
        }
        
        if (m_oFileOutputStream.is_open()){
            m_oFileOutputStream.close();
        }
    }
    
    bool isAskingMoreOption(){
        return m_bMoreOptions;
    }
    
    void wrapup(){
        long totalTime = 0;
        for (auto it = m_vAllTimes.begin(); it != m_vAllTimes.end(); ++it){
            totalTime += *it;
        }
        
        ::sec2SMH(totalTime, m_lSeconds, m_lMinutes, m_lHours);
        cout << "You worked a total of : "  << m_lHours << ":" << m_lMinutes << ":" << m_lSeconds << " on project " << ProjectNames[iSelectedProject] << "\n";
        
//        now = chrono::system_clock::now();
        
        m_oFileOutputStream << "TOTAL,,"  << m_lHours << ":" << m_lMinutes << ":" << m_lSeconds << "\n";
        m_oFileOutputStream.close();
    }
    
    bool projectSelection(){
        cout << "************ BMP4 PUNCH CLOCK ************\n\n\n";
        cout << "Pick the project you want to work on, or type m for more options!\n";
        for (int iCurProject = 0; iCurProject < TotalProjectCount; ++iCurProject){
            cout << "[" << iCurProject << "] " <<  ProjectNames[iCurProject] << "\n";
        }
        
        char aSelectedProject;
        do {
            cout << ">";

            cin >> aSelectedProject;
            if (aSelectedProject == 'q'){
                return false;
            } else if (aSelectedProject == 'm'){
                m_bMoreOptions = true;
                return false;
            } else if (isalpha(aSelectedProject)){
                continue;
            }
            iSelectedProject = aSelectedProject - '0';
            
        } while (iSelectedProject < 0 || iSelectedProject >= TotalProjectCount);
        
        cout << "OK, good luck with: " << ProjectNames[iSelectedProject] << "! ";

		string wholeFilePath = FILEPATH + ProjectNames[iSelectedProject] + ".csv";
		m_oFileOutputStream = ofstream(wholeFilePath, ios::app);
		if (!m_oFileOutputStream){
			cerr << "ERROR: Cannot open file: " << wholeFilePath << "\n";
			exit(1);
		}
        
        m_oNow = chrono::system_clock::now();
        m_oFileOutputStream << "--------------------------------\n";
        m_oFileOutputStream << ProjectNames[iSelectedProject] << " - " << time2date(m_oNow) << "\n";
        m_oFileOutputStream.flush();
        m_bProjectJustSelected = true;
        
        return true;
    }
    
    void waitForPunches(){
        // ------------------ PUNCH IN AND OUT ------------------
        cout << "Type <p> to punch in and out, or <q> to exit.\n\n";
        do {
            m_strSelectedOption = "";
            
            //if project was just selected, we skip this and punch in right away
            if (!m_bProjectJustSelected){
                do {
                    cout << ">";
                    cin >> m_strSelectedOption;
                } while (m_strSelectedOption != "p" && m_strSelectedOption != "q");
            } else {
                m_bProjectJustSelected = false;
            }
            if (bIsPunchedIn){
                punchOut();
            }  else if (m_strSelectedOption != "q"){
                punchIn();
            }
        } while (m_strSelectedOption != "q");
    }
};
//----------------------------------------------------------------------------------------
static void sumTime(){
    //go through lines and search for
        //project name (in enum)
        //date
        //TOTAL,, time
    //store all this stuff in vectors or something
    //then go through the vectors, searching for
    
    //open FILEPATH directory
    DIR *dir;
    struct dirent *ent;
    string DIR_PATH("/Users/nicolai/Dropbox/");
    if ((dir = opendir (DIR_PATH.c_str())) == NULL) {
        cerr << "could not open directory";
        return;
    }
    //find all files containing PunchClockHours
    while ((ent = readdir (dir)) != NULL) {
        string fileName(ent->d_name);
        if (fileName.find("PunchClockHours") != string::npos){
            //see which project we have
            for (int iCurProject = 0; iCurProject < TotalProjectCount; ++iCurProject) {
                if (fileName.find(ProjectNames[iCurProject]) != string::npos){
                    ifstream file(DIR_PATH + fileName);
                    if (!file.is_open()){
                        return;
                    }
                    long lProjectTotalTimeInSec = 0;
                    
                    //go through lines, searching for current project name
                    string line;
                    while (getline(file, line)){
                        if(line.find(ProjectNames[iCurProject]) != string::npos){
                            size_t dashPos = line.find(" - ");
                            string date = line.substr (dashPos+3);
                            //cout << ProjectNames[iCurProject] << "\t" << date;
                            //search for total time for that date
                            while (getline(file, line)){
                                size_t timePos = line.find("TOTAL,,");
                                if(timePos != string::npos){
                                    
                                    //read line in format HH:MM:SS
                                    string time = line.substr (timePos+7);
                                    
                                    //string to s,m,h
                                    long s,m,h;
                                    string2HMS(time, s,m,h);
                                    
                                    //smh to total seconds
                                    lProjectTotalTimeInSec += SMH2Sec(s,m,h);
                                    break;
                                }
                            }
                        }
                    }
                    //convert time in seconds to readable time
                    long s,m,h;
                    sec2SMH(lProjectTotalTimeInSec, s,m,h);
                    
                    cout << ProjectNames[iCurProject] << "\t" << h << ":" << m << ":" << s << endl;

                    file.close();
                }
            }
        }
    }
    closedir (dir);
}
//----------------------------------------------------------------------------------------
int main(int argc, const char * argv[]) {
    Bmp4PunchClock punchClock;
    if (punchClock.projectSelection()){
        punchClock.waitForPunches();
        punchClock.wrapup();
    } else if (punchClock.isAskingMoreOption()){
        sumTime();
    }
    return 0;
}
