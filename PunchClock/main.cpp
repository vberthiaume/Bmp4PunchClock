//
//  main.cpp
//  PunchClock
//
//  Created by Vincent Berthiaume on 2015-03-24.
//  Copyright (c) 2015 BMP4. All rights reserved.
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

enum Projects {
    Project0 = 0,
    Project1,
    Project2,
    TotalProjectCount
};

static string ProjectNames[] = {
    "Project0",
    "Project1",
    "Project2"
};

static string time2string(chrono::time_point<chrono::system_clock> p_time)
{
    auto in_time_t = chrono::system_clock::to_time_t(p_time);
    
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%X");
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
    
    void punchIn(){
        bIsPunchedIn = false;
        
        punchOutTime = chrono::system_clock::now();
        cout << "Punched out at " << time2string(punchOutTime) << "\n";
        elapsedTime = punchOutTime - punchInTime;
        
        //elapsedTime = chrono::seconds(3661);
        
        vAllTimes.push_back(chrono::duration_cast<chrono::seconds>(elapsedTime).count());
        
        calculateTime(elapsedTime, seconds, minutes, hours );
        
        cout << "time elapsed since last punch-in: "  << hours << ":" << minutes << ":" << seconds << "\n";
        
        fileOutputStream << hours << ":" << minutes << ":" << seconds << "\n";
    }
    
    void punchOut(){
        bIsPunchedIn = true;
        punchInTime = chrono::system_clock::now();
        cout << "Punched in at " << time2string(punchInTime) << "\n";
    }

public:
    
    int iSelectedProject;
    
    Bmp4PunchClock():
    bIsPunchedIn(false),
    iSelectedProject(-1) {
        fileOutputStream = ofstream("PunchClockHours.txt", ios::app);
        if(!fileOutputStream) cerr << "error";
    }
    
    ~Bmp4PunchClock(){
        
        if (bIsPunchedIn){
            punchOut();
        }
        
        long totalTime = 0;
        for (auto it = vAllTimes.begin(); it != vAllTimes.end(); ++it){
            totalTime += *it;
        }
        
        calculateTime(totalTime, seconds, minutes, hours);
        
        cout << "You worked a total of : "  << hours << ":" << minutes << ":" << seconds << " on project " << ProjectNames[iSelectedProject] << "\n";
        
        now = chrono::system_clock::now();
        
        fileOutputStream << "TOTAL "  << hours << ":" << minutes << ":" << seconds << "\n";
        
        fileOutputStream.close();
    }
    
    void projectSelection(){
        cout << "************ BMP4 PUNCH CLOCK ************\n\n\n";
        cout << "Pick the project you want to work on!\n";
        for (int iCurProject = 0; iCurProject < TotalProjectCount; ++iCurProject){
            cout << "[" << iCurProject << "] " <<  ProjectNames[iCurProject] << "\n";
        }
        
        do {
            cout << ">";
            cin >> iSelectedProject;
            
        } while (iSelectedProject < 0 || iSelectedProject >= TotalProjectCount);
        
        cout << "OK, good luck with: " << ProjectNames[iSelectedProject] << "! ";
        
        now = chrono::system_clock::now();
        fileOutputStream << "--------------------------------\n";
        fileOutputStream << ProjectNames[iSelectedProject] << " - " << time2date(now) << "\n";

    }
    
    void waitForPunches(){
        // ------------------ PUNCH IN AND OUT ------------------
        cout << "Type <p> to punch in and out, or <x> to exit.\n\n";
        do {
            strSelectedOption = "";
            do {
                cout << ">";
                cin >> strSelectedOption;
                
            } while (strSelectedOption != "p" && strSelectedOption != "x");
            
            //punching out
            if (bIsPunchedIn){
                punchIn();
            }
            
            //punching in
            else {
                punchOut();
            }
            
        } while (strSelectedOption != "x");
        
    }
    
    void calculateTime(long elapsedTime, long& workSeconds, long& workMinutes, long& workHours){
        workSeconds = elapsedTime;
        workHours  = floor(workSeconds  / 3600);
        workSeconds -= workHours * 3600;
        workMinutes = floor(workSeconds / 60);
        workSeconds -= workMinutes * 60;
    }
    
    void calculateTime(chrono::duration<double> elapsedTime, long& workSeconds, long& workMinutes, long& workHours){
        calculateTime(chrono::duration_cast<chrono::seconds>(elapsedTime).count(), workSeconds, workMinutes, workHours);
    }
};

int main(int argc, const char * argv[]) {
    
    Bmp4PunchClock punchClock;
    
    punchClock.projectSelection();
    
    punchClock.waitForPunches();
    
    return 0;
}
