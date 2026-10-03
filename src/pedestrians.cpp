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

#include "pedestrians.h"
#include "pedestrian.h"
#include "tiempo.h"
#include <vector>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pedestrians* pedestrians::pedestriansInstance = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pedestrians::pedestrians() {
    // creation of the people data
    leerPedestrians(std::get<std::string>(dictionary::get()->lookupDefault("populationsFile")));
    // start time according to the Rayleigh distribution
    // tiempoInicioDistribution();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::vector<pedestrian>& pedestrians::getDbPedestrianTotal() {
    return dbPedestrianTotal; 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
pedestrians* pedestrians::get() {
    /* if it does not exist yet, creates the only instance of pedestrians*/
    if (!pedestriansInstance) {
        pedestriansInstance =  new pedestrians();
    }
    return pedestriansInstance;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// metods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// double generate_uniform_random(std::mt19937& gen) {
//     // Generate a uniform random number in the range (0, 1)
//     return std::generate_canonical<double, std::numeric_limits<double>::digits>(gen);
// }
// double generate_rayleigh_random(double sigma) {
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     // Set the parameters for the Rayleigh distribution
//     double meanRayleigh = 7 * 60;
//     double scaleRayleigh = meanRayleigh * std::pow((2/M_PI), 0.5);

//     // Generate a uniform random number
//     double u = generate_uniform_random(gen);

//     // Calculate the random number according to the Rayleigh distribution
//     double random_number = sigma * sqrt(-2.0 * log(1.0 - u));

//     return random_number;
// }
void pedestrians::leerPedestrians(std::string fileName){
    std::fstream file;
    file.open(fileName, std::ios::in);
    // check whether the file exists
    if (file.fail()) {
        std::cout << "Error opening the file " <<fileName << std::endl;
        exit(1);
    }
    // Variables of one row of the nodes file, which would be a single node
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // edad                 |-->| AGE OF THE PERSON
    // gender               |-->| GENDER OF THE PERSON
    // hhType               |-->| 
    // hhId                 |-->| 
    // idNodeInicio         |-->| ID OF THE START NODE 
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    std::string line;
    std::string a1_str, a2_str, a3_str, a4_str, a5_str;
    std::string edad_str, gender_str, hhType_str, hhId_str, idNodeArranque_str;
    while (std::getline(file, line)) {
        // If the file has comments with #, do not read them.
        if (line[0] == '#') {
            continue;
        }
        // Save each line in the variable line. 
        std::istringstream iss(line);
        // Save each value in the variables.
        std::getline(iss, edad_str, ',');
        std::getline(iss, gender_str, ',');
        std::getline(iss, hhType_str, ',');
        std::getline(iss, hhId_str, ',');
        std::getline(iss, idNodeArranque_str, '\n');
        // Convert from str to int
        int edad = std::stoi(edad_str);
        int gender = std::stoi(gender_str);
        int hhType = std::stoi(hhType_str);
        int hhId = std::stoi(hhId_str);
        int idNodeArranque = std::stoi(idNodeArranque_str);
        // Creation of each person in the data base.
        dbPedestrianTotal.push_back(pedestrian(edad, gender, hhType, hhId, nodes::get()->getDbNodeTotal().at(idNodeArranque).get()));
    }
    file.close(); 
}
// void pedestrians::tiempoInicioDistribution() {
//     /* calculates the start time, using the Rayleigh distribution*/
//     std::random_device rd;
//     std::mt19937 gen(rd());
//     // Set the parameters for the Rayleigh distribution
//     double meanRayleigh = 7 * 60;
//     double scaleRayleigh = meanRayleigh * std::pow((2/M_PI), 0.5);

//     std::vector<int> tiempoi;
//     for (int i = 0; i < dbPedestrianTotal.size(); ++i) {
//         // std::cout << "g: " << gen << std::endl;
//         double random_number = generate_rayleigh_random(scaleRayleigh);
//         // must be improved,
//         // I have problems when the person starts moving at 0
//         if(random_number < 2.0){
//             dbPedestrianTotal.at(i).setTiempoInicial(2);
//         }
//         else {
//             dbPedestrianTotal.at(i).setTiempoInicial(random_number);
//         }
//             tiempoi.push_back(random_number);  // Store the initial time

//     }

//     FILE* gnuplotPipe = popen("gnuplot -persistent", "w");
//     fprintf(gnuplotPipe, "set terminal png size 800,600\n");
//     fprintf(gnuplotPipe, "set output 'tiempos_iniciales.png'\n");
//     fprintf(gnuplotPipe, "set xlabel 'Initial Time (s)'\n");
//     fprintf(gnuplotPipe, "set ylabel 'Number of Pedestrians'\n");
//     fprintf(gnuplotPipe, "set title 'Distribution of Initial Times'\n");
//     fprintf(gnuplotPipe, "binwidth = 5\n");
//     fprintf(gnuplotPipe, "bin(x,width) = width*floor(x/width) + width/2.0\n");
    
//     // Pass the data to Gnuplot
//     fprintf(gnuplotPipe, "$DATA << EOD\n");
//     for (double tiempo : tiempoi) {
//         fprintf(gnuplotPipe, "%f\n", tiempo);
//     }
//     fprintf(gnuplotPipe, "EOD\n");
        
//     // Plot the histogram
//     fprintf(gnuplotPipe, "plot $DATA using (bin($1,binwidth)):(1.0) smooth freq with boxes lc rgb 'blue' notitle\n");
    
//     // Close the pipe
//     pclose(gnuplotPipe);
//     // }

// }
void pedestrians::reiniciarPedestrians() {
    // returns the person to their initial departure position before the evacuation starts
    for (int i = 0; i < dbPedestrianTotal.size(); i++) {
        dbPedestrianTotal.at(i).reiniciar();
    }
}
void pedestrians::reiniciarConteoPedestrians() {
    //  
    for (auto it = dbPedestrianTotal.begin(); it != dbPedestrianTotal.end(); ++it) {
        // it->modelamientoPedestrian();
    }
}
void pedestrians::modelamientoPedestrians() {
    //  
    for (auto it = dbPedestrianTotal.begin(); it != dbPedestrianTotal.end(); ++it) {
        it->modelamientoPedestrian();
    }
}
void pedestrians::mostrarDbPedestrianMovimiento() {
    // for (int i = 0; i < dbPedestrianTotal.size(); i++) {
        dbPedestrianTotal.at(0).mostrarMovimientoPedestrian();
        if (dbPedestrianTotal.at(0).getTiempoInicial() < tiempo::get()->getValorTiempo() and getDbPedestrianTotal().at(0).getInterseccion()) {
            dbPedestrianTotal.at(0).getStateMatrixCurrent()->mostrarStateMatrix();
            dbPedestrianTotal.at(0).getNodeFinal()->mostrarNode();
        }
    // }
}
void pedestrians::mostrarDbPedestrianTotal() const {
    for (int i = 0; i < dbPedestrianTotal.size(); i++) {
        dbPedestrianTotal.at(i).mostrarPedestrian();
        std::cout << std::endl;
    }
}
void pedestrians::imprimirPedestrians(fileIO* file1, fileIO* file2){
    /* print position data, number of evacuees and speed.*/
    // if the option is active it will print, active by default
    if (std::get<bool>(dictionary::get()->lookupDefault("graphicPrintout")) == true) {
        // printing of variables
        for (auto it = dbPedestrianTotal.begin(); it != dbPedestrianTotal.end(); ++it) {
            if (tiempo::get()->getValorTiempo() >= it->getTiempoInicial()) {
                it->imprimirPedestrianPosition(file1);
                it->imprimirPedestrianVelocity(file2);
            }
        }
    }
}

