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

#include "nodeEvacuation.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Extra 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "io.h"
#include "nodes.h"
#include "pedestrians.h"
#include "subLink.h"
#include "pedestrian.h"
#include "simulationTime.h"
#include <iostream>
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int nodeDestino::totalPersonasEvacuadas=0;
int nodeDestino::maxPersonasEvacuadasGlobal=1000;
bool nodeDestino::evacuacionTotal=false;

std::string nodeDestino::getNodeType() {
    return "nodeDestino";
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
nodeDestino::nodeDestino(const int id, const vector2D coordenada)
    : node(id, coordenada),
      maxPersonasEvacuadas(-1),
      lleno(false)
{
}
nodeDestino::nodeDestino(const int id, const vector2D coordenada, const int maxPersonasEvacuadas)
    : node(id, coordenada),
      maxPersonasEvacuadas(maxPersonasEvacuadas),
      lleno(false)
{
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const int nodeDestino::getMaxPersonasEvacuadas() const{
    return maxPersonasEvacuadas;    
}
std::vector<pedestrian*> nodeDestino::getPersonasEvacuadasPtr() const{
    return personasEvacuadasPtr;    
}
bool nodeDestino::getLleno() const{
    return lleno;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool nodeDestino::verificarEvacuacionTotal() {
    /* checks whether all the people were evacuated */
    const int totalPersonas = pedestrians::get()->getDbPedestrianTotal().size();
    return totalPersonasEvacuadas == totalPersonas;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void nodeDestino::reiniciar() {
    personasEvacuadasPtr.clear();
    lleno = false;
}
bool nodeDestino::verificarLLeno() const {
    /* checks whether the evacuation node is full*/
    // compares the size of the list of evacuated people with the maximum
    // number of evacuated people at the evacuation node
    return personasEvacuadasPtr.size() == maxPersonasEvacuadas;

}
estado nodeDestino::estadoPedestrianEnNodo() const {
    /* returns the state of the person according to the type of node where it is */
    return evacuado;
}
bool nodeDestino::verificarNodoEvacuation() const {
    /* checks whether the node is an evacuation node */
    return true;
}
std::vector<int> nodeDestino::stateObservado() const {
    return {0};
}
void nodeDestino::contabilizarPersona(pedestrian* const persona) {
    personasEvacuadasPtr.push_back(persona);
    totalPersonasEvacuadas++;
}
void nodeDestino::imprimirPersonasEvacuadas(std::fstream* file) {
    *file << personasEvacuadasPtr.size();
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static metods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int nodeDestino::getTotalPersonasEvacuadas() {
    return totalPersonasEvacuadas;
}    
void nodeDestino::sumarTotalPersonasEvacuadas() {
    totalPersonasEvacuadas++;
}
void nodeDestino::imprimirNodeEvacuation(fileIO* const file) {
    // checks the dictionary whether the option is enabled, enabled by default
    if (std::get<bool>(dictionary::get()->lookupDefault(file->getFileName())) == true) {
        // printing of node ids, only at time 1 at the start
        if (tiempo::get()->getValorTiempo() == 1) {
            // id 1 2 3 4
            file->getFileFstream() << "id,";
            for (int i = 0; i < nodes::get()->getDbNodeEvacuation().size(); i++) {
                file->getFileFstream() << nodes::get()->getDbNodeEvacuation().at(i)->getIdNode();
                // prints , up to before the last one
                if (i < nodes::get()->getDbNodeEvacuation().size() - 1) {
                    file->getFileFstream() << ",";
                }
            }
            // line break
            file->getFileFstream() << std::endl;
        }
        // printing of people evacuated per evacuation node
        // prints the time
        file->getFileFstream() << tiempo::get()->getValorTiempo() << ",";
        for (int i = 0; i < nodes::get()->getDbNodeEvacuation().size(); i++) {
            // prints the number of people evacuated per node
            file->getFileFstream() << nodes::get()->getDbNodeEvacuation().at(i)->getPersonasEvacuadasPtr().size();
            // prints , up to before the last one
            if (i < nodes::get()->getDbNodeEvacuation().size() - 1) {
                file->getFileFstream() << ",";
            }
        }
        // line break
        file->getFileFstream() << std::endl;
    }
}
void nodeDestino::imprimirTotalPersonasEvacuadas(fileIO* const file) {
    // checks the dictionary whether the option is enabled, enabled by default
    if (std::get<bool>(dictionary::get()->lookupDefault(file->getFileName())) == true) {
        // printing of time and evacuated people
        file->getFileFstream() << tiempo::get()->getValorTiempo() << ",";
        file->getFileFstream() << totalPersonasEvacuadas;
        file->getFileFstream() << std::endl;
    }
}
std::vector<int> stringToVector(const std::string& str) {
    std::vector<int> result;
    std::string item;
    std::string cleanStr = str;

    // Find the delimiter ';' and remove everything after it
    size_t pos = cleanStr.find(';');
    if (pos != std::string::npos) {
        cleanStr = cleanStr.substr(0, pos);
    }

    std::stringstream ss(cleanStr);

    // Split the string into parts using the comma as delimiter
    while (std::getline(ss, item, ',')) {
        int number;
        std::stringstream(item) >> number; // Convert the part to an integer
        result.push_back(number); // Add the integer to the vector
    }

    return result;
}
void nodeDestino::plotearTotalEvacuadosXSimulacion(fileIO* const file) {
    // checks the dictionary whether the option is enabled, enabled by default
    if (std::get<bool>(dictionary::get()->lookupDefault(file->getFileName())) == true) {
        class totalPersonasEvacuadasXSimulacion{
        public:
            std::vector<int> tiempo;
            std::vector<int> totalEvacuados;
        }; 
        // simulation numbers to record: keyword evacuatedVsTimeAt (default 1,2,3 from the dictionary)
        static const std::vector<int> tiempoSimulacion = stringToVector(std::get<std::string>(dictionary::get()->lookupDefault("evacuatedVsTimeAt")));
        static std::vector<totalPersonasEvacuadasXSimulacion> data(tiempoSimulacion.size());
        auto it = std::find(tiempoSimulacion.begin(), tiempoSimulacion.end(), tiempo::get()->getINumberSimulation());
        // only enters at the simulation number that tiempoSimulacion asks for
        if (it != tiempoSimulacion.end()) {
            // Determine the index in the vector `data` based on the position in `tiempoSimulacion`
            int index = std::distance(tiempoSimulacion.begin(), it);
            // Add the current time value to the corresponding vector `tiempo`
            data.at(index).tiempo.push_back(tiempo::get()->getValorTiempo());
            data.at(index).totalEvacuados.push_back(totalPersonasEvacuadas);
        }
        // print the plot at the end of the simulation number and when the evacuation time ends
        if (tiempo::get()->getINumberSimulation() ==  tiempo::get()->getEndNumberSimulation() and tiempo::get()->getValorTiempo() == tiempo::get()->getEndTime()) {
            FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
            if (gnuplotPipe) {
                // Configure Gnuplot
                fprintf(gnuplotPipe, "set terminal png size 800,600\n");
                // Use the full path
                fprintf(gnuplotPipe, "set output '%s'\n", file->getFullPath().c_str());
                fprintf(gnuplotPipe, "set xlabel 'Time (s)'\n");
                fprintf(gnuplotPipe, "set ylabel 'Total evacuated people'\n");
                fprintf(gnuplotPipe, "set title 'Evacuated people over time'\n");
                fprintf(gnuplotPipe, "set grid\n");
                // Position the titles on the left
                fprintf(gnuplotPipe, "set key left\n");
                // creation of plot
                fprintf(gnuplotPipe, "plot ");
                // creating line title
                for (size_t i = 0; i < data.size(); ++i) {
                    fprintf(gnuplotPipe, "'-' using 1:2 with lines title 'Simulation %d'", tiempoSimulacion.at(i));
                    if (i < data.size() - 1) {
                        fprintf(gnuplotPipe, ", ");
                    }
                }
                fprintf(gnuplotPipe, "\n");
                // plotting lines 
                for (size_t i = 0; i < data.size(); ++i) {
                    for (size_t j = 0; j < data.at(i).tiempo.size(); ++j) {
                        fprintf(gnuplotPipe, "%d %d\n", data.at(i).tiempo.at(j), data.at(i).totalEvacuados.at(j));
                    }
                    fprintf(gnuplotPipe, "e\n");
                }
                fflush(gnuplotPipe);
                pclose(gnuplotPipe);
            }
        }
    }
}
void nodeDestino::imprimirTotalEvacuadosXSimulacion(fileIO* const file) {
    // checks the dictionary whether the option is enabled, enabled by default
    if (std::get<bool>(dictionary::get()->lookupDefault(file->getFileName())) == true) {
        // simulation numbers to record: keyword evacuatedVsTimeAt (default 1,2,3 from the dictionary)
        static const std::vector<int> tiempoSimulacion = stringToVector(std::get<std::string>(dictionary::get()->lookupDefault("evacuatedVsTimeAt")));
        auto it = std::find(tiempoSimulacion.begin(), tiempoSimulacion.end(), tiempo::get()->getINumberSimulation());
        // only enters at the simulation number that tiempoSimulacion asks for
        if (it != tiempoSimulacion.end()) {
            // send data to a table file
            file->getFileFstream() << tiempo::get()->getValorTiempo() << ",";
            file->getFileFstream() << totalPersonasEvacuadas;
            file->getFileFstream() << std::endl;
        }
    }
}
void nodeDestino::plotearEvacuadosVsTiempo(fileIO* const file) {
    // checks the dictionary whether the option is enabled, enabled by default
    if (std::get<bool>(dictionary::get()->lookupDefault(file->getFileName())) == true) {
        static std::vector<int> numeroSimulaciones;
        static std::vector<double> survivors;
        // simulation number
        const int numeroSimulacion = tiempo::get()->getINumberSimulation();
        // only saves according to the period in seconds that is assigned
        if (numeroSimulacion % std::get<int>(dictionary::get()->lookupDefault("totalEvacuadosVsSimulacionPeriod")) == 0 or numeroSimulacion == 1) {
            // store data in data to plot it later
            numeroSimulaciones.push_back(numeroSimulacion);
            survivors.push_back(totalPersonasEvacuadas);
        }
        // print the plot at the end of the simulation number and when the evacuation time ends
        if (tiempo::get()->getINumberSimulation() ==  tiempo::get()->getEndNumberSimulation()) {
            FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
            if (gnuplotPipe) {
                // Configure Gnuplot
                fprintf(gnuplotPipe, "set terminal png size 800,600\n");
                // Use the full path
                fprintf(gnuplotPipe, "set output '%s'\n", file->getFullPath().c_str());
                fprintf(gnuplotPipe, "set xlabel 'Number of simulations'\n");
                fprintf(gnuplotPipe, "set ylabel 'Number of survivors'\n"); 
                fprintf(gnuplotPipe, "set grid\n");
                // fprintf(gnuplotPipe, "set xtics 1\n");  // Change '1' to the desired interval
                // fprintf(gnuplotPipe, "set ytics 1\n");  // Change '1' to the desired interval
                // fprintf(gnuplotPipe, "set mytics 4\n");   // 4 sub-ticks between each main tick
                fprintf(gnuplotPipe, "set xrange [1:*]\n"); // Ensures that the x axis starts at 1
                // creation of plot
                fprintf(gnuplotPipe, "plot '-' with linespoints notitle\n");
                for (size_t j = 0; j < numeroSimulaciones.size(); ++j) {
                    fprintf(gnuplotPipe, "%d %lf\n", numeroSimulaciones.at(j), survivors.at(j));
                }
                fprintf(gnuplotPipe, "e\n");
                pclose(gnuplotPipe);
            }
        }
    }
}
void nodeDestino::imprimirEvacuadosVsTiempo(fileIO* const file) {
    // checks the dictionary whether the option is enabled, enabled by default
    if (std::get<bool>(dictionary::get()->lookupDefault(file->getFileName())) == true) {
        static std::vector<int> numeroSimulaciones;
        static std::vector<double> mortalidad;
        // simulation number
        const int numeroSimulacion = tiempo::get()->getINumberSimulation();
        // only saves according to the period in seconds that is assigned
        if (numeroSimulacion % std::get<int>(dictionary::get()->lookupDefault("totalEvacuadosVsSimulacionPeriod")) == 0 or numeroSimulacion == 1) {
            // send it to table file
            file->getFileFstream() << numeroSimulacion << ","; 
            file->getFileFstream() << totalPersonasEvacuadas; 
            file->getFileFstream() << std::endl; 
        }
    }
}
void nodeDestino::imprimirVariableTotalPersonasEvacuadas(fileIO* const file) {
    // printing of time and evacuated people
    file->getFileFstream() << totalPersonasEvacuadas;
}
