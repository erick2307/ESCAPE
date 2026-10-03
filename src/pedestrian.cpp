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

#include "pedestrian.h"

#include "io.h"
#include "node.h"
#include "link.h"
#include "nodeEvacuation.h"
#include "sarsa.h"
#include "stateMatrix.h"
#include "stateMatrixs.h"
#include "simulationTime.h"
#include "vector2D.h"
#include "velocity.h"
#include <bits/types/FILE.h>
#include <iostream>
#include <vector>
#include "pedestrians.h"
#include <limits> // For std::numeric_limits

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int pedestrian::counter = 1;
const int pedestrian::meanRayleigh = std::get<int>(dictionary::get()->lookup("meanRayleigh"));
const int pedestrian::surviveReward = 100000;
const int pedestrian::deadReward = -1000; 
const int pedestrian::stepReward = -1;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pedestrian::pedestrian(const int age, const int gender, const int hhType, const int hhId, node* originNode)
    : idPedestrian(counter++),
      age(age),
      gender(gender),
      hhType(hhType),
      hhId(hhId),
      originNode(originNode),
      initialTime(calculateRayleighDistribution(calculateScaleRayleigh())),
      position(originNode->getCoordinate()),
      startNodePtr(originNode),
      endNodePtr(nullptr),
      pedestrianDirection(),
      pedestrianVelocity(),
      pedestrianState(passive),
      reward(0),
      previousIntersectionTime(0),
      intersection(true),
      stateMatrixCurrentPtr(nullptr),
      QCurrentPtr(nullptr),
      QPreviousPtr(nullptr),
      linkCurrentPtr(nullptr)
{
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void pedestrian::setPosition(vector2D position) {
//     (*this).position = position;
// }
void pedestrian::setStartNode(node* startNode){
    (*this).startNodePtr = startNode;
}
// void pedestrian::setEndNode(node* endNode) {
//     (*this).endNode = endNode;
// }
// void pedestrian::setPreviousStartNode(node* previousStartNode) {
//     (*this).previousStartNode = previousStartNode;
// }
// void pedestrian::setCurrentLink(link* currentLink) {
//     (*this).currentLink = currentLink;
// }
// void pedestrian::setPreviousLink(link *previousLink) {
//     (*this).previousLink = previousLink;
// }
void pedestrian::setPedestrianDirection(vector2D pedestrianDirection) {
    (*this).pedestrianDirection = pedestrianDirection;
}
void pedestrian::setPedestrianVelocity(double pedestrianVelocity) {
    (*this).pedestrianVelocity.setMagnitude(pedestrianVelocity);
}
void pedestrian::setPedestrianState(pedestrianStatus pedestrianState) {
    (*this).pedestrianState = pedestrianState;
}
// void pedestrian::setPreviousLinkOrientation(vector2D previousLinkOrientation) {
//     (*this).previousLinkOrientation = previousLinkOrientation;
// }
// void pedestrian::setFinalTime(int finalTime) {
//     (*this).finalTime = finalTime;
// }
// void pedestrian::setStartedWalking(bool startedWalking) {
//     (*this).startedWalking = startedWalking;
// }
// void pedestrian::setFirstTime(bool firstTime) {
//     (*this).firstTime = firstTime;
// }
// void pedestrian::setJumpedLink(bool jumpedLink) {
//     (*this).jumpedLink = jumpedLink;
// }
void pedestrian::setReward(int reward) {
    (*this).reward = reward;
}
void pedestrian::setIntersection(bool intersection) {
    (*this).intersection = intersection;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int pedestrian::getIdPedestrian() const {
    return idPedestrian;
}
const int pedestrian::getAge() const{
    return age;
}
const int pedestrian::getGender() const {
    return gender;
}
const int pedestrian::getHHType() const{
    return hhType;
}
const int pedestrian::getHHId() const{
    return hhId;
}
const node* pedestrian::getOriginNode() const {
    return originNode;
}
const int pedestrian::getInitialTime() const {
    return initialTime;
}
vector2D pedestrian::getPosition() const{
    return position;
}
node* pedestrian::getStartNode() const{
    return startNodePtr;
}
node* pedestrian::getEndNode() const {
    return endNodePtr;  
}
vector2D pedestrian::getPedestrianDirection() const {
    return pedestrianDirection;
}
velocity& pedestrian::getPedestrianVelocity() {
    return pedestrianVelocity;
}
pedestrianStatus& pedestrian::getPedestrianState() {
    return pedestrianState;
}
int pedestrian::getReward() const {
    return reward;  
}
int pedestrian::getPreviousIntersectionTime() const{
    return previousIntersectionTime;
}
bool pedestrian::getIntersection() const {
    return intersection;    
}
link* pedestrian::getLinkCurrent() const {
    return linkCurrentPtr;
}
stateMatrix *pedestrian::getStateMatrixCurrent() const {
    return stateMatrixCurrentPtr;    
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool pedestrian::operator==(const pedestrian& pedestrian2) const{
    /* compares whether two pedestrians are equal*/
    // the comparison is by id
    return idPedestrian == pedestrian2.idPedestrian;
}
void pedestrian::walk() {
    /* displacement formula*/
    const vector2D velocity = pedestrianDirection * pedestrianVelocity.getMagnitude();
    position += velocity * simulationTime::get()->getDeltaT();
}
double pedestrian::calculateIdSublink() {
    /* Calculates the location of the person in the subLink array*/
    // distance from the person to node 1 of the street
    const double subdivisionWidth = linkCurrentPtr->getSubdivisionWidth();
    const double index_x = position.getX() - linkCurrentPtr->getNode1Ptr()->getCoordinate().getX();
    const double index_y = position.getY() - linkCurrentPtr->getNode1Ptr()->getCoordinate().getY();
    int index_hypotenuse = std::sqrt(std::pow(index_x,2) + pow(index_y, 2)) / subdivisionWidth;
    // if it is in a subdivision beyond the ones that exist, it is about to enter an intersection
    if (index_hypotenuse >= linkCurrentPtr->getSubdivisionCount()) {
        intersection = true;
        // index_hypotenuse = linkCurrentPtr->getSubdivisionCount() - 1;    
    }
    return index_hypotenuse;
}
bool pedestrian::checkEndLink() const {
    // Calculates the Euclidean distance between the current coordinates and the target point
    const double threshold = pedestrianVelocity.getMagnitude();
    const double distance = std::sqrt(std::pow(position.getX() - endNodePtr->getCoordinate().getX(), 2) + std::pow(position.getY() - endNodePtr->getCoordinate().getY(), 2));
    // Checks whether the distance is less than or equal to the threshold
    return distance <= threshold;
}
int pedestrian::calculateIdEndSublink() const {
    /* Determines which is the last sublink*/
    // find out whether I am at the end or at the start
    if (startNodePtr == linkCurrentPtr->getNode1Ptr()) {
        // if at the start, it is the last subdivision
        return linkCurrentPtr->getSubdivisions().size() - 1; 
    } 
    // it is at the start
    else {
        return 0;
    }
}
link* pedestrian::generalLinkChoice() const {
    // the first choice must be random
    if (simulationTime::get()->getTimeValue() == initialTime and std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration") {
        return randomLinkChoice();
    }
    if (pedestrianState == evacuated) {
        return nullptr;
    }
    // Configure the random number generator
    else {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<double> dis(0.0, 1.0);
        // Generate a random number in the range [0.0, 1.0)
        const double randomNumber = dis(gen);
        // compares the random number with the optimal choice rate
        // the latter must decrease as there are more simulations
        // the more simulations, the more use of the sarsa choice
        switch (randomNumber <= simulationTime::get()->getRandomChoiceRate() ? 1 : 2) {
            case 1:
                return randomLinkChoice();
            case 2:
                return sarsaLinkChoice();
        }
    }
    return nullptr;
}
link* pedestrian::randomLinkChoice() const {
    /* The person is at an intersection and has multiple options for choosing a street.
        the street to take is decided randomly and will be stored in currentLink.*/
    // Mersenne Twister engine algorithm
    static std::random_device rd;
    static std::mt19937 generator(rd());
    // linkConnection of the start node 
    const std::vector<link*> linkConnection = startNodePtr->getLinkConnectionsPtr();
    const int max_limit = linkConnection.size() - 1 ;
    // Create a uniform distribution using the specified range
    std::uniform_int_distribution<size_t> distribution(0, max_limit);
    // choice of a street at random
    const size_t random_number = distribution(generator);
    // choice of the new street, main part of the function
    return linkConnection.at(random_number);
}
link* pedestrian::sarsaLinkChoice() const {
    // finds the largest Q element of the experienced stateMatrix
    const Q* Qmax = stateMatrixCurrentPtr->findQMax();
    return const_cast<link*>(Qmax->getStreetPtr());
}
// void pedestrian::twoConsecutiveStreetsChoice() {
//     // currentLink is the street about to change
//     if (!(startNode->getLinkConnection().at(0)->getIdLink() == currentLink->getIdLink())) {
//         // setCurrentLink(&dbLinkTotal.at(getStartNode()->getIdLinkConnection().at(0)));
//         // setCurrentLink(links::get()->getDbLinkTotal().at(getStartNode()->getIdLinkConnection().at(0)).get());
//         setCurrentLink(startNode->getLinkConnection().at(0));
//         // sending action information to the stateMatrix
//         stateMatrixPedestrian.getActionValue().setILinkConnection(0);
//         stateMatrixPedestrian.getActionValue().setIdLink(currentLink->getIdLink());
//         // knowing the street, define the final node.
//         calculateEndNode();
//         // check whether the final node is an evacuation node.
//         checkPedestrianEvacuation();
//     }
//     else {
//         // setCurrentLink(&dbLinkTotal.at(getStartNode()->getIdLinkConnection().at(1)));
//         // setCurrentLink(links::get()->getDbLinkTotal().at(getStartNode()->getIdLinkConnection().at(1)).get());
//         setCurrentLink(startNode->getLinkConnection().at(0));
//         // sending action information to the stateMatrix
//         stateMatrixPedestrian.getActionValue().setILinkConnection(1);
//         stateMatrixPedestrian.getActionValue().setIdLink(currentLink->getIdLink());
//         // knowing the street, define the final node.
//         calculateEndNode();
//         // check whether the final node is an evacuation node.
//         checkPedestrianEvacuation();
//     }
// }
int pedestrian::calculateNumberSign(double number) {
    if (number >= 0) {
        return 1;
    }
    else {
        return -1;
    }
}
void pedestrian::calculatePedestrianDirection() {
    pedestrianDirection = linkCurrentPtr->getLinkOrientation() * calculateDirectionSign();
}
vector2D pedestrian::calculateDirectionSign() {
    double x = calculateNumberSign(endNodePtr->getCoordinate().getX() - startNodePtr->getCoordinate().getX());
    double y = calculateNumberSign(endNodePtr->getCoordinate().getY() - startNodePtr->getCoordinate().getY());
    return vector2D(x,y);
}
int pedestrian::calculateReward() const {
    /* calculation of the reward per step*/
    const int travelTime = calculateTravelTime();
    const int steps = travelTime / simulationTime::get()->getDeltaT();
    return steps * stepReward;
}
int pedestrian::calculateTravelTime() const {
   /* calculates the next time at which the pedestrian will be at an intersection*/
    const int elapsedTime = simulationTime::get()->getTimeValue() - previousIntersectionTime;
    return  elapsedTime;
}
void pedestrian::modelPedestrian() {
    if(!(pedestrianState == evacuated)){
        const int currentTime = simulationTime::get()->getTimeValue();
        // when the person is passive, the state changes to evacuated when their departure time arrives
        if (pedestrianState == passive && initialTime == currentTime) {
            pedestrianState = evacuating;
        }
        // performs the movement only when evacuating
        if (pedestrianState == evacuating or pedestrianState==evacuated) {
                // modeling when the person is at an intersection
            if (intersection) {
                // if not yet evacuated, do the following
                // if it is a time different from the initial one, save the present into the past
                if(!(initialTime == currentTime)){
                    // resets the reward value
                    reward = 0;
                    // saves the QCurrent before it is changed
                    // stateMatrixPreviousPtr = stateMatrixCurrentPtr;
                    QPreviousPtr = QCurrentPtr;
                    // now the final intersection is the initial intersection.
                    startNodePtr = endNodePtr;
                    // position correction when arriving close to the node.
                    position = {startNodePtr->getCoordinate().getX(), startNodePtr->getCoordinate().getY()};
                    // check whether I am at an evacuation point
                }
                // startNodePtr->showNode();
                pedestrianState = startNodePtr->pedestrianStateAtNode();
                // observes the state of the node or nodeEvacuation
                const std::vector<int> observedState = startNodePtr->observedState();
                // get stateMatrix
                stateMatrixCurrentPtr = stateMatrix::createOrGetStateMatrix(startNodePtr, observedState);
               // stateMatrixCurrentPtr->showStateMatrix();
                // choice of the street
                linkCurrentPtr = generalLinkChoice();
                if (pedestrianState == evacuating) {
                    // // add the people on the street
                    // linkCurrentPtr->addPedestrian(this);
                    // get final node
                    endNodePtr = const_cast<node*>(startNodePtr->findEndNode(linkCurrentPtr));
                    // direction of the person on the street.
                    calculatePedestrianDirection();
                    // calculate idEndSublink
                    idEndSublink = calculateIdEndSublink();
                }
                // get Qcurrent
                QCurrentPtr = stateMatrixCurrentPtr->findQ(linkCurrentPtr);
                // increase observation
                QCurrentPtr->increase1Observation();
                // except at the start
                if (pedestrianState == evacuated) {
                    dynamic_cast<nodeEvacuation*>(startNodePtr)->registerPerson(this);
                }
                if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration"){
                    if(!(initialTime == currentTime)){
                        reward = calculateReward();
                        sarsa::sarsaUpdateQ(QPreviousPtr->getValue(), QCurrentPtr->getValue(), reward);
                    }
                }
                // saves the previous intersection
                previousIntersectionTime = currentTime;
                // move onto the street
                intersection=false;
            }
            else {
                // modeling when the person is inside the street
                if (pedestrianState == evacuating) {
                    // only adds when not at the intersection
                    if (simulationTime::get()->getPedestrianCountPeriod()) {
                        // speed with random
                        pedestrianVelocity.calculateRandomAdjustment();
                    }
                    // the person walks
                    walk();    
                   // calculates position in subdivision 
                    const int idSublink = calculateIdSublink();
                    // when in a sublink close to endNode
                    if (idSublink == idEndSublink and intersection == false) {
                        // checks when it is close to an intersection
                        intersection = checkEndLink();
                    }
                    // checks how often it must count
                    if (simulationTime::get()->getPedestrianCountPeriod()) {
                        // only adds when not at the intersection
                        if (intersection==false) {
                            // adds person in sublink
                            linkCurrentPtr->addPedestrianSublink(this, idSublink);
                        }
                    }
                }
            }
        }
    }
}
void pedestrian::reset() {
    /* reset values for next simulation*/
    startNodePtr = const_cast<node*>(originNode);
    position = startNodePtr->getCoordinate();
    pedestrianState = passive;
    intersection = true;
    // reward = 0;
}
void pedestrian::showMovementPedestrian() const {
    /* shows the start and end intersection of a street, when
        the person.*/
    if (pedestrianState == evacuating or pedestrianState== evacuated) {
        std::cout << idPedestrian << ' ';
        std::cout << std::setw(6) << startNodePtr->getIdNode() << ' ';
        std::cout << "start: ";
        // decimals for printing
        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::setw(5) << startNodePtr->getCoordinate().getX() << ' ';
        std::cout << std::setw(5) << startNodePtr->getCoordinate().getY() << ' ';
        std::cout << "now: ";
        std::cout << std::setw(5) << position.getX() << " ";
        std::cout << std::setw(5) << position.getY() << " ";
        std::cout << "end: ";
        
        std::cout << std::setw(5) << endNodePtr->getCoordinate().getX() << ' ';
        std::cout << std::setw(5) << endNodePtr->getCoordinate().getY() << ' ';
        std::cout << std::setw(5) << getReward() << ' ';
        std::cout << std::endl;
    }
}
void pedestrian::showPedestrian() const {
    std::cout << idPedestrian << ' ';
    std::cout << startNodePtr->getIdNode() << ' ';
    std::cout << "ti: " << initialTime;
}
void pedestrian::printPedestrianPosition(fileIO* file) const {
    // if (getEvacuated()) {
    // decimals for saving to files.
    file->getFileFstream() << std::fixed << std::setprecision(2);
    file->getFileFstream() << position.getX() << " ";
    file->getFileFstream() << position.getY() << " ";
    file->getFileFstream() << std::endl;
    // }
}
void pedestrian::printPedestrianVelocity(fileIO* file) const{
    file->getFileFstream() << pedestrianVelocity.getMagnitude() << " ";
    file->getFileFstream() << std::endl;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static metods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const double pedestrian::calculateScaleRayleigh() {
    return static_cast<double>(meanRayleigh) * std::pow((2.0/M_PI), 0.5);
}
double generate_uniform_random(std::mt19937& gen) {
    // Generate a uniform random number in the range (0, 1)
    return std::generate_canonical<double, std::numeric_limits<double>::digits>(gen);
}
double pedestrian::calculateRayleighDistribution(const double sigma) {
    /* calculates a number according to the Rayleigh distribution, as a parameter it needs
        the variable sigma, which is the scaleRayleigh */
    std::random_device rd;
    std::mt19937 gen(rd());
    // Generate a uniform random number
    double u = generate_uniform_random(gen);
    // Calculate the random number according to the Rayleigh distribution
    return sigma * std::sqrt(-2.0 * std::log(1.0 - u));
}
void pedestrian::plotPedestrians(fileIO* const file) {
    
    // Initialize maximum and minimum values
    static double minX = std::numeric_limits<double>::max();
    static double maxX = std::numeric_limits<double>::lowest();
    static double minY = std::numeric_limits<double>::max();
    static double maxY = std::numeric_limits<double>::lowest();
    // read it only at the start 
    if (simulationTime::get()->getTimeValue() == 1) {
        const auto& streetLines = links::get()->getDbLinkTotal();
        for (const auto lc : streetLines) {
            // Get start and end points for the line
            const auto startPoint = lc->getNode1Ptr();
            const auto endPoint = lc->getNode2Ptr();
            
            // Get the coordinates
            double x1 = startPoint->getCoordinate().getX();
            double y1 = startPoint->getCoordinate().getY();
            double x2 = endPoint->getCoordinate().getX();
            double y2 = endPoint->getCoordinate().getY();
            
            // Update the maximum and minimum values
            minX = std::min(minX, std::min(x1, x2));
            maxX = std::max(maxX, std::max(x1, x2));
            minY = std::min(minY, std::min(y1, y2));
            maxY = std::max(maxY, std::max(y1, y2));
        }
    }
    FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe) {
        // Configure Gnuplot
        fprintf(gnuplotPipe, "set output '%s'\n", file->getFullPath().c_str());
        fprintf(gnuplotPipe, "set terminal png size 1920,1080\n");
        fprintf(gnuplotPipe, "set yrange [%lf:%lf]\n", minY, maxY);
        fprintf(gnuplotPipe, "set xrange [%lf:%lf]\n", minX, maxX);
        fprintf(gnuplotPipe, "unset border\n");
        fprintf(gnuplotPipe, "set palette rgbformulae 10,13,22\n");
        fprintf(gnuplotPipe, "set colorbox\n");
        fprintf(gnuplotPipe, "set cbrange [0.2:1.2]\n"); // Replace min and max with your fixed values
        fprintf(gnuplotPipe, "set grid\n");
        fprintf(gnuplotPipe, "set bmargin 3\n"); // Increased bottom margin
        int minutes =  simulationTime::get()->getTimeValue() / 60; // Divide to get whole minutes
        int seconds =  simulationTime::get()->getTimeValue() % 60; // Remainder for the leftover seconds
        fprintf(gnuplotPipe, "set label 't = %d.%d min, evacuated: %d' at screen 0.5, 0.02 center\n", minutes, seconds, nodeEvacuation::totalEvacuatedPeople);

        // plot creation
        std::string plotCommand = "plot";
        plotCommand += " '-' with lines lc 'black' notitle,";
        // check whether there are pedestrians evacuating
        bool evacuatedPedestrians = false;
        for (const pedestrian& ped : pedestrians::get()->getDbPedestrianTotal()) {
            if (ped.pedestrianState == evacuating) {
                evacuatedPedestrians = true;
                break;
            }
        } 
        if (evacuatedPedestrians) {
            plotCommand += " '-' with points pt 7 palette notitle,";
        }
        // always add the evacuation points
        plotCommand += " '-' with points pt 12 ps 3.0 lc 'red' notitle";
        fprintf(gnuplotPipe, "%s\n", plotCommand.c_str());
        const auto& dbPedestrianTotal = pedestrians::get()->getDbPedestrianTotal();
        const auto& evacuationPoints = nodes::get()->getDbNodeEvacuation();
        const auto& streetLines = links::get()->getDbLinkTotal();
        // plotting of street lines
        for (const auto lc : streetLines) {
            // Get start and end points for the line
            const auto startPoint = lc->getNode1Ptr();
            const auto endPoint = lc->getNode2Ptr();
            fprintf(gnuplotPipe, "%lf %lf\n", startPoint->getCoordinate().getX(), startPoint->getCoordinate().getY());
            fprintf(gnuplotPipe, "%lf %lf\n", endPoint->getCoordinate().getX(), endPoint->getCoordinate().getY());
            fprintf(gnuplotPipe, "\n");  // Space between the lines
            // std::cout << startPoint->getCoordinate().getX() << " " << startPoint->getCoordinate().getY() << " ";
            // std::cout << endPoint->getCoordinate().getX() << " " << endPoint->getCoordinate().getY() << std::endl;
        }
        fprintf(gnuplotPipe, "e\n");
        // fprintf(gnuplotPipe, "e\n");
        // Iterate over the pedestrian vector using iterators
        if (evacuatedPedestrians) {
            for (const pedestrian& ped : dbPedestrianTotal) {
                if(ped.pedestrianState == evacuating){
                    fprintf(gnuplotPipe, "%lf %lf %lf\n", ped.position.getX(), ped.position.getY(), ped.pedestrianVelocity.getMagnitude());
                    // std::cout <<  ped.position.getX() << " " <<  ped.position.getY()<< " "<<ped.pedestrianVelocity.getMagnitude() << std::endl;
                }
            }
            fprintf(gnuplotPipe, "e\n");
        }
        // Second series of points (example: initial position)
        for (const nodeEvacuation* const pe : evacuationPoints) {
            fprintf(gnuplotPipe, "%lf %lf\n", pe->getCoordinate().getX() , pe->getCoordinate().getY());
        }
        fprintf(gnuplotPipe, "e\n");
        pclose(gnuplotPipe);
    }
    if (simulationTime::get()->getTimeValue() == simulationTime::get()->getEndTime() or nodeEvacuation::checkTotalEvacuation()) {
        const std::string directory = file->getDirectory()->getFullPath();
        const std::string command = "ffmpeg -y -framerate 10 -i "+ directory + "Figure-%d.png -c:v libx264 -pix_fmt yuv420p " + directory + "animation.mp4 > /dev/null 2>&1";
        int result = system(command.c_str());
        const std::string deleteCommand = "rm -f " + directory + "Figure-*.png";
        int deleteResult = system(deleteCommand.c_str());
    }

}

