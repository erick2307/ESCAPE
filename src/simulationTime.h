// MIT License
// Copyright (c) 2025 
// Luis Angel Moya Huallpa, Julio Cesar Ramirez Paredes, Erick Mas, Shunichi Koshimura
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef simulationTime_h
#define simulationTime_h

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// general headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <sys/stat.h> 
#include <iostream>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// own headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "dictionary.h"
#include "chrono"

class simulationTime {
private:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // timeValue           |-->| SIMULATION TIME 
    // deltaT                |-->| SIMULATION TIME STEP  
    // endTime               |-->| END TIME OF THE SIMULATION
    // graphicPrintoutPeriod |-->| TIME PERIOD FOR PRINTING DATA
    // startNumberSimulation |-->| START OF THE SIMULATION NUMBER
    // iNumberSimulation     |-->| CURRENT SIMULATION NUMBER
    // endNumberSimulation   |-->| END OF THE SIMULATION NUMBER
    // randomChoiceRate      |-->| COEFFICIENT OF CHOICE BETWEEN RANDOM OR SARSA 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int timeValue;
    const int deltaT;
    const int endTime;
    int startNumberSimulation;
    int iNumberSimulation;
    int endNumberSimulation;
    const int graphicPrintoutPeriod;
    const int pedestrianCountPeriod;
    std::chrono::high_resolution_clock::time_point startTimeSimulation;
    std::chrono::high_resolution_clock::time_point endTimeSimulation;
    double randomChoiceRate;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // constructor
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    simulationTime();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static member
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static simulationTime* simulationTimeInstance;

public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static member
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static int deltaTime;
    static std::string filenameData;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setTimeValue(int timeValue);
    void setStartNumberSimulation(int startNumberSimulation);
    void setINumberSimulation(int iNumberSimulation);
    void setEndNumberSimulation(int endNumberSimulation);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // getters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    int getTimeValue() const;
    const int getEndTime() const;
    const int getDeltaT() const;
    const int getGraphicPrintoutPeriod() const;
    int getStartNumberSimulation() const;
    int getINumberSimulation() const;
    int getEndNumberSimulation() const;
    const int getPedestrianCountPeriod() const;
    double getRandomChoiceRate() const;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static getters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static simulationTime* get();
    static std::string getFilenameData();

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // methods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    simulationTime& operator++(int);
    void increaseTime();
    void increaseINumberSimulation();
    void initializeNumberSimulation();
    int extractINumberSimulation() const;
    void calculateRandomChoiceRate();
    const double calculateTemp(const double r) const;
    bool running() const;
    void showTime() const;
    void showSimulationResults();
    bool checkGraphicPrintoutPeriod() const; 
    bool checkPedestrianCountPeriod() const; 
    
};

#endif

