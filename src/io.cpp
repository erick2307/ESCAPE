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

#include "io.h"
#include "dictionary.h"
#include "nodeEvacuation.h"
#include "pedestrians.h"
#include "subLink.h"
#include "simulationTime.h"
#include "stateMatrixs.h"
#include <ios>
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                  dirIO
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dirIO::dirIO(const std::string& dirName) :
    dirName(dirName),
    directory(nullptr),
    fullPath(dirName + '/')
{
    if (!checkDirExists()) {
        createDir();
    }
}
dirIO::dirIO(const std::string& dirName, const dirIO* directory) :
    dirName(dirName),
    directory(directory),
    fullPath(directory->getFullPath() + dirName + '/')
{
    if (!checkDirExists()) {
        createDir();
    }
}
dirIO::dirIO(const std::string& dirName, const dirIO* directory, const bool checkCreation) :
    dirName(dirName),
    directory(directory),
    fullPath(directory->getFullPath() + dirName + '/')
{
    if (checkCreation) {
        if (!checkDirExists()) {
            createDir();
        }
    }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const std::string dirIO::getDirName() const {
    return dirName;
}
const std::string dirIO::getFullPath() const {
    return fullPath;    
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void dirIO::createDir() {
    // directory permissions S_ to execute, read and write
    if (directory == nullptr) {
        mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    else {
        std::string dir = directory->fullPath + dirName.c_str();
        mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
}
bool dirIO::checkDirExists() const{
    struct stat info;
    if (stat(dirName.c_str(), &info) != 0) {
        // Could not access the directory (it may not exist)
        return false;
    } else if (info.st_mode & S_IFDIR) {
        // It exists and is a directory
        return true;
    } else {
        // It exists but is not a directory
        return false;
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                fileIO
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
fileIO::fileIO(const std::string& fileName) :
    fileName(fileName),
    fullPath(fileName),
    directory(nullptr)
{
    createFile();
}
fileIO::fileIO(const std::string& fileName, const bool checkFile) :
    fileName(fileName),
    fullPath(fileName),
    directory(nullptr)
{
    // only create if checkFile is true
    if (checkFile) {
        createFile();
    }
}
fileIO::fileIO(const std::string& fileName, const std::string extension,const bool checkFile) :
    fileName(fileName),
    fullPath(fileName + "." +  extension),
    directory(nullptr)
{
    // only create if checkFile is true
    if (checkFile) {
        createFile();
    }
}
fileIO::fileIO(const std::string& fileName,  const std::string extension, const std::string& inoutStr, const bool checkFile) :
    fileName(fileName),
    fullPath(fileName + "." +  extension),
    directory(nullptr)
{
    // only create if checkFile is true
    if (checkFile) {
        openFile(inoutFile(inoutStr));
    }
}
fileIO::fileIO(const std::string& fileName,const dirIO* directory) :
    fileName(fileName),
    directory(directory),
    fullPath(directory->getFullPath() + fileName)
{
    createFile();
}
fileIO::fileIO(const std::string& fileName,const std::string extension, const dirIO* directory) :
    fileName(fileName),
    directory(directory),
    fullPath(directory->getFullPath() + fileName + "." + extension)
{
    createFile();
}
fileIO::fileIO(const std::string& fileName,  const std::string extension, const std::string& inoutStr, const bool checkFile, const dirIO* directory) :
    fileName(fileName),
    directory(directory),
    fullPath(directory->getFullPath() + fileName + "." + extension)
{
    // only create if checkFile is true
    if (checkFile) {
        openFile(inoutFile(inoutStr));
    }
}




//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const std::string fileIO::getFileName() const {
    return fileName;
}
std::fstream& fileIO::getFileFstream() {
    return fileFstream; 
}
const std::string fileIO::getFullPath() const {
    return fullPath;
}
const dirIO* const fileIO::getDirectory() const {
    return directory;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void fileIO::createFile() {
    fileFstream.open(fullPath, std::ios::out);
}
void fileIO::openFile(const std::ios_base::openmode& inout) {
    fileFstream.open(fullPath, inout);
}
std::ios_base::openmode fileIO::inoutFile(const std::string& inout) {
    if (inout == "in") {
        return std::ios::in;    
    }
    else if (inout == "out") {
        return std::ios::out;
    }
    else {
        throw std::runtime_error("Could not open the file: ");
    }
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                                io
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
io* io::ioInstance = nullptr;
size_t io::ioElementSize = 10;
dirIO io::directoryData("data");
dirIO io::directoryTime("time", &directoryData, std::get<std::string>(dictionary::get()->lookupDefault("process"))=="trained");
dirIO io::directoryPostprocessing("postprocessing");
dirIO io::directorySnapshot("snapshot", &directoryPostprocessing);
dirIO io::directoryFigure("figure", &directoryData, std::get<std::string>(dictionary::get()->lookupDefault("process"))=="trained");
dirIO io::directoryStateMatrices("stateMatrices");
fileIO io::fileTotalEvacuatedCount("totalEvacuatedCount", "csv", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="trained", &directoryData);
fileIO io::fileEvacuatedCount("evacuatedCount", "csv", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="trained", &directoryData);
fileIO io::fileActionsDb("actionsdb", "csv", std::get<std::string>(dictionary::get()->lookupDefault("pythonOption")), std::get<bool>(dictionary::get()->lookupDefault("pythonVersion")));
fileIO io::fileTranstionsDb("transitionsdb", "csv", std::get<std::string>(dictionary::get()->lookupDefault("pythonOption")) == "out");
fileIO io::figureEvacuatedVsTime("figureEvacuatedVsTime", "png", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
fileIO io::figureTotalEvacuatedVsSimulation("figureTotalEvacuatedVsSimulation", "png", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
fileIO io::tableEvacuatedVsTime("tableEvacuatedVsTime", "csv", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
fileIO io::tableTotalEvacuatedVsSimulation("tableTotalEvacuatedVsSimulation", "csv", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
fileIO io::figurePedestrians("figurePedestrians", "png", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="trained", &directoryData);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
io::io() {
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
io* io::get() {
    /* if it does not exist yet, create the single instance of nodes*/
    if (!ioInstance) {
        ioInstance =  new io();
    }
    return ioInstance;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// std::fstream& io::getFileEvacuatedCount() {
//     return fileTotalEvacuatedPeople;
// }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// metods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dirIO* io::createTimeFolder() {
    /* Create folders for each evacuation time*/
    dirIO* dirTime = new dirIO(std::to_string(simulationTime::get()->getTimeValue()), &directoryTime);
    // create folder for the times in seconds to store information
    mkdir(dirTime->getFullPath().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return dirTime;
}
void io::printOutput() {
    /* handling of the outputs*/
    // export when trained
    if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "trained") {
        if (simulationTime::get()->checkGraphicPrintoutPeriod()) {
            // create time folders
            dirIO* timeDir = createTimeFolder();
            // print pedestrian data: position and velocity
            fileIO xy("xy", timeDir);
            fileIO U("U", timeDir);
            pedestrians::get()->printPedestrians(&xy, &U);
            // print pedestrian data: number of evacuated pedestrians
            fileIO evacuatedPedestrianCount("evacuatedPedestrianCount", timeDir);
            nodeEvacuation::printTotalEvacuatedPeopleVariable(&evacuatedPedestrianCount);
            delete timeDir;
            timeDir = nullptr; 
            // export table files
            // prints total evacuated pedestrians
            nodeEvacuation::printTotalEvacuatedPeople(&fileTotalEvacuatedCount);
            // prints pedestrians evacuated per evacuation point
            nodeEvacuation::printNodeEvacuation(&fileEvacuatedCount);
            // print pedestrians on the streets
            std::string figureName = "Figure-" + std::to_string(simulationTime::get()->getTimeValue()); // or the format you want
            fileIO figure(figureName, "png", &directoryFigure);
            pedestrian::plotPedestrians(&figure);
        }
    }
    // export during calibration
    else if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration") {
        // prints statematrix data at the end of each simulation
        // only prints it at the end of the evacuation
        if (simulationTime::get()->getTimeValue() == simulationTime::get()->getEndTime()) {
            fileIO stateMatrice(stateMatrixs::get()->createStateMatrixFile(), &directoryStateMatrices);
            stateMatrixs::get()->printDbStateMatrixs(&stateMatrice);
            // plot mortality per simulation
            nodeEvacuation::plotTotalEvacuatedVsSimulation(&figureTotalEvacuatedVsSimulation);
            nodeEvacuation::printTotalEvacuatedVsSimulation(&tableTotalEvacuatedVsSimulation);
        }
        // plot total evacuated pedestrians per simulation
        nodeEvacuation::plotEvacuatedVsTime(&figureEvacuatedVsTime);
        nodeEvacuation::printEvacuatedVsTime(&tableEvacuatedVsTime);
        // print actionDb
        if (std::get<bool>(dictionary::get()->lookupDefault("pythonVersion")) == true and std::get<std::string>(dictionary::get()->lookupDefault("pythonOption")) == "out") {
            // when the simulation number is 1 and it is the end of the evacuation
            if (simulationTime::get()->getINumberSimulation() == simulationTime::get()->getStartNumberSimulation()
            and simulationTime::get()->getTimeValue() == simulationTime::get()->getEndTime()) {
                nodes::get()->printActionsDb(fileActionsDb.getFileFstream());
                nodes::get()->printTransitionsDb(fileTranstionsDb.getFileFstream());
            }
        }
    }
}
