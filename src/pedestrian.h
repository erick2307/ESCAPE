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

#ifndef pedestrian_h
#define pedestrian_h

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// general headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <bits/types/FILE.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <math.h>
                                                \
#include <memory>
#include <random>
#include <iomanip>
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// own headers
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "io.h"
#include "sarsa.h"
// #include "subLink.h"
#include "stateMatrix.h"
#include "subLink.h"
#include "vector2D.h"
#include "velocity.h"
#include "simulationTime.h"

class node;
class link;
enum pedestrianStatus { passive, evacuating, evacuated, dead };

class pedestrian {
public:
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // idPedestrian        |-->| ID OF THE INTERSECTION
    // age                |-->| X COORDINATE OF THE NODE 
    // gender              |-->| Y COORDINATE OF THE NODE
    // hhType              |-->| 
    // hhId                |-->| 
    // position            |-->| POSITION OF THE PERSON
    // originNode        |-->| STARTING INTERSECTION WHEN STARTING SIMULATION  
    // initialTime       |-->| START TIME FOR THE PERSON TO START WALKING 
    // position            |-->| POSITION OF THE PERSON 
    // startNode          |-->| INTERSECTION OF A STREET 
    // endNode           |-->| OTHER INTERSECTION OF THE SAME STREET
    // previousStartNode  |-->| INITIAL INTERSECTION OF THE PREVIOUS STREET
    // currentLink          |-->| STREET WHERE THE PERSON CURRENTLY IS 
    // previousLink          |-->| PREVIOUS STREET THROUGH WHICH IT PASSED
    // pedestrianDirection |-->| DIRECTION OF THE PERSON
    // velocity           |-->| SPEED OF THE PERSON
    // evacuated            |-->| THE PERSON WHO REACHED AN EVACUATION POINT
    // totalReturn             |-->| COULD BE LIKE THE TOTAL GAIN 
    // nextIntersectionTime |-->| TIME OF NEXT ARRIVAL AT A NODE

    // stateMatrixCurrent  |-->| POINTER TO STATEMATRIX BEING EXPERIENCED 
    // stateMatrixPrevious |-->| POINTER TO PREVIOUSLY EXPERIENCED STATEMATRIX
    // QCurrent            |-->| POINTER TO QCURRENT 
    // QPrevious           |-->| POINTER TO QPREVIOUS 
    // linkCurrent         |-->| POINTER TO CURRENT STREET
    // linkPrevious         |-->| POINTER TO PREVIOUS STREET 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
private:
    const int idPedestrian;
    const int age;
    const int gender;
    const int hhType;
    const int hhId;
    const node* originNode;
    const int initialTime;
    vector2D position;
    node* startNodePtr;
    node* endNodePtr;
    vector2D pedestrianDirection;
    velocity pedestrianVelocity;
    pedestrianStatus pedestrianState;
    int reward;
    int previousIntersectionTime;
    bool intersection;
    stateMatrix* stateMatrixCurrentPtr;
    Q* QCurrentPtr;
    Q* QPreviousPtr;
    link* linkCurrentPtr;
    int idEndSublink;

public:

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static member
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static int counter;
    static const int meanRayleigh;
    const static int surviveReward;
    const static int deadReward;
    const static int stepReward;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // constructor
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    pedestrian(const int age, const int gender, const int hhType, const int hhId, node* originNode);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // setters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void setPosition(vector2D position);
    void setStartNode(node* startNode);
    void setEndNode(node* endNode);
    void setPreviousStartNode(node* previousStartNode);
    void setCurrentLink(link* currentLink);
    void setPedestrianDirection(vector2D pedestrianDirection);
    void setPedestrianVelocity(double pedestrianVelocity);
    void setPedestrianState(pedestrianStatus pedestrianState);
    void setReward(int reward);
    void setIntersection(bool intersection);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // getters
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    const int getIdPedestrian() const;
    const int getAge() const;
    const int getGender() const;
    const int getHHType() const;
    const int getHHId() const;
    const node* getOriginNode() const;
    const int getInitialTime() const;
    vector2D getPosition() const;
    node* getStartNode() const;
    node* getEndNode() const;
    vector2D getPedestrianDirection() const;
    velocity &getPedestrianVelocity();
    pedestrianStatus& getPedestrianState();
    int getReward() const;
    int getPreviousIntersectionTime() const;
    bool getIntersection() const;
    stateMatrix* getStateMatrixCurrent() const;
    double* getQCurrent() const;
    double* getQPrevious() const;
    link* getLinkCurrent() const;

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // methods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    bool operator==(const pedestrian& pedestrian2) const;
    void modelPedestrian();
    void walk();
    link* generalLinkChoice() const;
    link* randomLinkChoice() const;
    link* sarsaLinkChoice() const;
    bool checkNodeFull() const;
    bool checkEndLink() const;
    int calculateIdEndSublink() const;
    double calculateIdSublink();
    int calculateReward() const;
    int calculateTravelTime() const;
    void countPedestrianInSublink();
    void calculateDensityInSublink();
    void twoConsecutiveStreetsChoice();
    void calculatePedestrianDirection();
    vector2D calculateDirectionSign();
    int calculateNumberSign(double number);
    void reset();
    void showMovementPedestrian() const;
    void showPedestrian() const;
    void printPedestrianPosition(fileIO* file) const;
    void printPedestrianVelocity(fileIO* file) const;
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // static metods
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    static const double calculateScaleRayleigh();
    static double calculateRayleighDistribution(const double sigma);
    static void plotPedestrians(fileIO* const file);
    
    // static double calculateOptimalChoiceRate();
};
#endif
