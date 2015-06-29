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
#include <vector>
#include <math.h>

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
    TotalProjectCount
};

static string ProjectNames[] = {
    "GRIS",
    "sBMP4",
    "SonicBirth",
    "Linux"
};

static string time2string(chrono::time_point<chrono::system_clock> p_time)
{
    auto in_time_t = chrono::system_clock::to_time_t(p_time);
    
    stringstream ss;
    ss << std::put_time(localtime(&in_time_t), "%X");
    return ss.str();
}

static string time2date(chrono::time_point<chrono::system_clock> p_time)
{
    auto in_time_t = chrono::system_clock::to_time_t(p_time);
    
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
}

class Bmp4PunchClock {
    
    bool bIsPunchedIn;
    chrono::time_point<chrono::system_clock> now, punchInTime, punchOutTime;
    chrono::duration<double> elapsedTime;
    
    string strSelectedOption;
    vector<long> vAllTimes;
    long hours, minutes, seconds;
    
    ofstream fileOutputStream;
    
    bool mProjectJustSelected;
    
    void punchIn(){
        bIsPunchedIn = true;
        punchInTime = chrono::system_clock::now();
        
        string strPunchInTime = time2string(punchInTime);
        cout << "PUNCHED IN at " << strPunchInTime << "\n";
        
        fileOutputStream << strPunchInTime << " - ";
        mProjectJustSelected = false;
    }
    
    void punchOut(){
        bIsPunchedIn = false;
        
        punchOutTime = chrono::system_clock::now();
        cout << "PUNCHED OUT at " << time2string(punchOutTime) << "\n";
        
        string strPunchOutTime = time2string(punchOutTime);
        
        fileOutputStream << strPunchOutTime;
        
        elapsedTime = punchOutTime - punchInTime;
        
        //elapsedTime = chrono::seconds(3661);
        
        vAllTimes.push_back(chrono::duration_cast<chrono::seconds>(elapsedTime).count());
        
        calculateTime(elapsedTime);
        
        cout << "time elapsed since last punch-in: "  << hours << ":" << minutes << ":" << seconds << "\n";
        
        fileOutputStream << "," << hours << ":" << minutes << ":" << seconds << "\n";
    }
    
    void calculateTime(long elapsedTime){
        seconds = elapsedTime;
        hours  = floor(seconds  / 3600);
        seconds -= hours * 3600;
        minutes = floor(seconds / 60);
        seconds -= minutes * 60;
    }
    
    void calculateTime(chrono::duration<double> elapsedTime){
        calculateTime(chrono::duration_cast<chrono::seconds>(elapsedTime).count());
    }

public:
    
    int iSelectedProject;
    
    Bmp4PunchClock():
    bIsPunchedIn(false),
    iSelectedProject(-1) {
        
    }
    
    ~Bmp4PunchClock(){
        if (bIsPunchedIn){
            punchOut();
        }
        
        if (fileOutputStream.is_open()){
            fileOutputStream.close();
        }
    }
    
    void wrapup(){
        long totalTime = 0;
        for (auto it = vAllTimes.begin(); it != vAllTimes.end(); ++it){
            totalTime += *it;
        }
        
        calculateTime(totalTime);
        
        cout << "You worked a total of : "  << hours << ":" << minutes << ":" << seconds << " on project " << ProjectNames[iSelectedProject] << "\n";
        
        now = chrono::system_clock::now();
        
        fileOutputStream << "TOTAL,,"  << hours << ":" << minutes << ":" << seconds << "\n";
        
        fileOutputStream.close();
    }
    
    bool projectSelection(){
        
        cout << "************ BMP4 PUNCH CLOCK ************\n\n\n";
        cout << "Pick the project you want to work on!\n";
        for (int iCurProject = 0; iCurProject < TotalProjectCount; ++iCurProject){
            cout << "[" << iCurProject << "] " <<  ProjectNames[iCurProject] << "\n";
        }
        
        char aSelectedProject;
        do {
            cout << ">";

            cin >> aSelectedProject;
            if (aSelectedProject == 'q'){
                return false;
            } else if (isalpha(aSelectedProject)){
                continue;
            }
            iSelectedProject = aSelectedProject - '0';
            
        } while (iSelectedProject < 0 || iSelectedProject >= TotalProjectCount);
        
        cout << "OK, good luck with: " << ProjectNames[iSelectedProject] << "! ";

		string wholeFilePath = FILEPATH + ProjectNames[iSelectedProject] + ".csv";
		fileOutputStream = ofstream(wholeFilePath, ios::app);
		if (!fileOutputStream){
			cerr << "ERROR: Cannot open file: " << wholeFilePath << "\n";
			exit(1);
		}
        
        now = chrono::system_clock::now();
        fileOutputStream << "--------------------------------\n";
        fileOutputStream << ProjectNames[iSelectedProject] << " - " << time2date(now) << "\n";
        
        mProjectJustSelected = true;
        
        return true;
    }
    
    void waitForPunches(){
        // ------------------ PUNCH IN AND OUT ------------------
        cout << "Type <p> to punch in and out, or <q> to exit.\n\n";
        do {
            strSelectedOption = "";
            
            //if project was just selected, we skip this and punch in right away
            if (!mProjectJustSelected){
                do {
                    cout << ">";
                    cin >> strSelectedOption;
                    
                } while (strSelectedOption != "p" && strSelectedOption != "q");
            } else {
                mProjectJustSelected = false;
            }
            if (bIsPunchedIn){
                punchOut();
            }  else if (strSelectedOption != "q"){
                punchIn();
            }
            
        } while (strSelectedOption != "q");
        
    }
    

};

int main(int argc, const char * argv[]) {
    
    Bmp4PunchClock punchClock;
    
    if (punchClock.projectSelection()){
        punchClock.waitForPunches();
        punchClock.wrapup();
    }
    
    return 0;
}
