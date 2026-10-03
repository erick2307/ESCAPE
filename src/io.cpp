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
    if (!verificarDirExists()) {
        crearDir();
    }
}
dirIO::dirIO(const std::string& dirName, const dirIO* directory) :
    dirName(dirName),
    directory(directory),
    fullPath(directory->getFullPath() + dirName + '/')
{
    if (!verificarDirExists()) {
        crearDir();
    }
}
dirIO::dirIO(const std::string& dirName, const dirIO* directory, const bool checkCreation) :
    dirName(dirName),
    directory(directory),
    fullPath(directory->getFullPath() + dirName + '/')
{
    if (checkCreation) {
        if (!verificarDirExists()) {
            crearDir();
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
void dirIO::crearDir() {
    // directory permissions S_ to execute, read and write
    if (directory == nullptr) {
        mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    else {
        std::string dir = directory->fullPath + dirName.c_str();
        mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
}
bool dirIO::verificarDirExists() const{
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
    crearFile();
}
fileIO::fileIO(const std::string& fileName, const bool checkFile) :
    fileName(fileName),
    fullPath(fileName),
    directory(nullptr)
{
    // only create if checkFile is true
    if (checkFile) {
        crearFile();
    }
}
fileIO::fileIO(const std::string& fileName, const std::string extension,const bool checkFile) :
    fileName(fileName),
    fullPath(fileName + "." +  extension),
    directory(nullptr)
{
    // only create if checkFile is true
    if (checkFile) {
        crearFile();
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
    crearFile();
}
fileIO::fileIO(const std::string& fileName,const std::string extension, const dirIO* directory) :
    fileName(fileName),
    directory(directory),
    fullPath(directory->getFullPath() + fileName + "." + extension)
{
    crearFile();
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
void fileIO::crearFile() {
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
size_t io::tamanoElementosIO = 10;
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
fileIO io::figureTotalEvacuadosVsSimulacion("figureTotalEvacuadosVsSimulacion", "png", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
fileIO io::figureEvacuadosVsTiempo("figureEvacuadosVsTiempo", "png", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
fileIO io::tableTotalEvacuadosVsSimulacion("tableTotalEvacuadosVsSimulacion", "csv", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
fileIO io::tableEvacuadosVsTiempo("tableEvacuadosVsTiempo", "csv", "out", std::get<std::string>(dictionary::get()->lookupDefault("process"))=="calibration", &directoryData);
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
//     return fileTotalPersonasEvacuadas;
// }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// metods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dirIO* io::crearCarpetaTiempo() {
    /* Create folders for each evacuation time*/
    dirIO* dirTime = new dirIO(std::to_string(tiempo::get()->getValorTiempo()), &directoryTime);
    // create folder for the times in seconds to store information
    mkdir(dirTime->getFullPath().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return dirTime;
}
void io::imprimirOutput() {
    /* handling of the outputs*/
    // export when trained
    if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "trained") {
        if (tiempo::get()->verificarGraphicPrintoutPeriod()) {
            // create time folders
            dirIO* dirTiempo = crearCarpetaTiempo();
            // print pedestrian data: position and velocity
            fileIO xy("xy", dirTiempo);
            fileIO U("U", dirTiempo);
            pedestrians::get()->imprimirPedestrians(&xy, &U);
            // print pedestrian data: number of evacuated pedestrians
            fileIO cantPedestrianEvacuated("cantPedestrianEvacuated", dirTiempo);
            nodeDestino::imprimirVariableTotalPersonasEvacuadas(&cantPedestrianEvacuated);
            delete dirTiempo;
            dirTiempo = nullptr; 
            // export table files
            // prints total evacuated pedestrians
            nodeDestino::imprimirTotalPersonasEvacuadas(&fileTotalEvacuatedCount);
            // prints pedestrians evacuated per evacuation point
            nodeDestino::imprimirNodeEvacuation(&fileEvacuatedCount);
            // print pedestrians on the streets
            std::string nombreArchivo = "Figure-" + std::to_string(tiempo::get()->getValorTiempo()); // or the format you want
            fileIO figure(nombreArchivo, "png", &directoryFigure);
            pedestrian::plotearPedestrians(&figure);
        }
    }
    // export during calibration
    else if (std::get<std::string>(dictionary::get()->lookupDefault("process")) == "calibration") {
        // prints statematrix data at the end of each simulation
        // only prints it at the end of the evacuation
        if (tiempo::get()->getValorTiempo() == tiempo::get()->getEndTime()) {
            fileIO stateMatrice(stateMatrixs::get()->creacionFileStateMatrix(), &directoryStateMatrices);
            stateMatrixs::get()->imprimirDbStateMatrixs(&stateMatrice);
            // plot mortality per simulation
            nodeDestino::plotearEvacuadosVsTiempo(&figureEvacuadosVsTiempo);
            nodeDestino::imprimirEvacuadosVsTiempo(&tableEvacuadosVsTiempo);
        }
        // plot total evacuated pedestrians per simulation
        nodeDestino::plotearTotalEvacuadosXSimulacion(&figureTotalEvacuadosVsSimulacion);
        nodeDestino::imprimirTotalEvacuadosXSimulacion(&tableTotalEvacuadosVsSimulacion);
        // print actionDb
        if (std::get<bool>(dictionary::get()->lookupDefault("pythonVersion")) == true and std::get<std::string>(dictionary::get()->lookupDefault("pythonOption")) == "out") {
            // when the simulation number is 1 and it is the end of the evacuation
            if (tiempo::get()->getINumberSimulation() == tiempo::get()->getStartNumberSimulation()
            and tiempo::get()->getValorTiempo() == tiempo::get()->getEndTime()) {
                nodes::get()->imprimirActionsDb(fileActionsDb.getFileFstream());
                nodes::get()->imprimirTransitionsDb(fileTranstionsDb.getFileFstream());
            }
        }
    }
}
