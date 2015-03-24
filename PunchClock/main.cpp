//
//  main.cpp
//  PunchClock
//
//  Created by Nicolai on 2015-03-24.
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
    GRIS = 0,
    sBMP4,
    SonicBirth,
    TotalProjectCount
};

static string ProjectNames[] = {
    "GRIS",
    "sBMP4",
    "SonicBirth"
};


string time2string(chrono::time_point<chrono::system_clock> p_time)
{
    auto in_time_t = chrono::system_clock::to_time_t(p_time);
    
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%X");
    return ss.str();
}

string time2date(chrono::time_point<chrono::system_clock> p_time)
{
    auto in_time_t = chrono::system_clock::to_time_t(p_time);
    
    stringstream ss;
    ss << put_time(localtime(&in_time_t), "%Y-%m-%d");
    return ss.str();
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

int main(int argc, const char * argv[]) {
    
    bool bIsPunchedIn = false;
    chrono::time_point<chrono::system_clock> now, punchInTime, punchOutTime;
    chrono::duration<double> elapsedTime;
    int iSelectedProject = -1;;
    string strSelectedOption;
    vector<long> vAllTimes;
    long hours, minutes, seconds;

    ofstream fileOutputStream = ofstream("PunchClockHours.txt", ios::app);
    if(!fileOutputStream) cerr << "error";
    
    
    // ------------------ PROJECT SELECTION ------------------
    cout << "************ BMP4 PUNCH CLOCK ************\n\n\n";
    cout << "Pick your project for today!\n";
    for (int iCurProject = 0; iCurProject < TotalProjectCount; ++iCurProject){
        cout << "[" << iCurProject << "] " <<  ProjectNames[iCurProject] << "\n";
    }

    do {
        cout << ">";
        cin >> iSelectedProject;
        
    } while (iSelectedProject < GRIS || iSelectedProject >= TotalProjectCount);
    
    cout << "OK, good luck with: " << ProjectNames[iSelectedProject] << "!\n";

    now = chrono::system_clock::now();
    fileOutputStream << "--------------------------------\n";
    fileOutputStream << ProjectNames[iSelectedProject] << " - " << time2date(now) << "\n";
    
    
    // ------------------ PUNCH IN AND OUT ------------------
    cout << "\ntype <p> to punch in and out, or <x> to exit.\n\n";
    do {
        strSelectedOption = "";
        do {
            cout << ">";
            cin >> strSelectedOption;
            
        } while (strSelectedOption != "p" && strSelectedOption != "x");
        
        //punching out
        if (bIsPunchedIn){
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
        
        //punching in
        else {
            bIsPunchedIn = true;
            punchInTime = chrono::system_clock::now();
            cout << "Punched in at " << time2string(punchInTime) << "\n";
        }
        
    } while (strSelectedOption != "x");
   
    
    // ------------------ EXITING ------------------
    long totalTime = 0;
    for (auto it = vAllTimes.begin(); it != vAllTimes.end(); ++it){
        totalTime += *it;
    }
    
    calculateTime(totalTime, seconds, minutes, hours);
    
    cout << "this chunk of work lasted: "  << hours << ":" << minutes << ":" << seconds << "\n";

    now = chrono::system_clock::now();
    
    fileOutputStream << "TOTAL "  << hours << ":" << minutes << ":" << seconds << "\n";

    fileOutputStream.close();
    
    return 0;
}
