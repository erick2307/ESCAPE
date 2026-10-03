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

#include "dictionary.h"
#include <algorithm>
#include <string>

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static member
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dictionary* dictionary::dictionaryInstance = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// constructor
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dictionary::dictionary() {
    leerDictionary();
}
dictionary::dictionary(std::string nameDictionary) {
    leerDictionary();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// setters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// void dictionary::setNameDictionary(std::string nameDictionary) {
//     (*this).nameDictionary = nameDictionary;
// }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
std::string dictionary::getNameDictionary() {
    return nameDictionary;
}
std::map<std::string, std::variant<std::string, int, double, bool>>& dictionary::getControlDict() {
    return controlDict;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static getters
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dictionary* dictionary::get() {
    /* if it does not exist yet, create the single instance of dictionary*/
    if (!dictionaryInstance) {
        dictionaryInstance =  new dictionary();
    }
    return dictionaryInstance;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// methods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void dictionary::leerDictionary() {
    std::string fileName = dictionary::systemCarpet + getNameDictionary();
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
    std::string keyword_str, value_str;
    while (std::getline(file, line)) {
        // If the file has comments with # or is an empty line,
        // do not read them.
        if (line[0] == '#' or line.empty()) {
            continue;
        }
        // Store each line in the variable line. 
        std::istringstream iss(line);
        // Store each value in the variables.
        // stores the first word up to the first space
        iss >> keyword_str;
        std::getline(iss >> std::ws, value_str, ';');
        // stores the values in a dictionary
        // looks up the value type of the element
        if (verificarOptions(keyword_str, value_str)) {
            verificarType(keyword_str, value_str);
        }
    }
    file.close(); 
}
std::variant<std::string, int, double, bool> dictionary::lookup(std::string keyword) {
    /* looks up the keyword and returns its value*/
    auto it = controlDict.find(keyword);
    // if it is not found
    if (it != controlDict.end()) {
        return it->second;
    }
    else {
        std::cout << "The keyword "<< keyword << " is not present in the controlDict.\n";
        // Terminate the program with an error code
        std::exit(EXIT_FAILURE);
    }
    return 0; 
}
std::variant<std::string, int, double, bool> dictionary::lookupDefault(std::string keyword) {
    /* looks up the keyword and returns its value*/
    auto it = controlDict.find(keyword);
    // if it is not found
    if (it != controlDict.end()) {
        return it->second;
    }
    else {
        return controlDictDefault.at(keyword);
    }
}
bool dictionary::verificarOptions(std::string keyword, std::string value) const {
    // checks whether the keyword exists in controlDictOptions
    auto it = controlDictOptions.find(keyword);
    if (it != controlDictOptions.end()) {
        const auto& options = it->second;
        if (!(std::find(options.begin(), options.end(), value) != options.end())) {
            std::cout << "The keyword "<< keyword << " does not have a valid value." << std::endl;
            std::cout << "Possible values: ";
            for (const auto& option : options) {
                std::cout << option << " ";
            }       
            std::cout << std::endl;
            return false;
        }
    }
    return true;
}
bool dictionary::verificarType(std::string keyword, std::string value)  {
    // looks up the requested keyword in typeControlDict and stores it in it
    std::map<std::string, std::string>::const_iterator it = typeControlDict.find(keyword);
    if (it != typeControlDict.end()) {
        std::string type = it->second;
        if (type == "string") {
            controlDict[keyword] = value;
            return true;
        }
        else if (type == "int") {
            controlDict[keyword] = std::stoi(value);
            return true;
        }
        else if (type == "double") {
            controlDict[keyword] = std::stod(value);
            return true;
        }
        else if (type == "bool") {
            if (value=="yes" or value=="true" or value=="si") {
                controlDict[keyword] = true;
                return true;
            }
            else if (value == "no" or value == "false") {
                controlDict[keyword] = false;
                return true;
            }
            else {
                std::cout << "The keyword "<< keyword << " has an invalid value." << std::endl;
                return false;    
            }
        }
    }
    return false;
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// static metods
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void dictionary::mostrarControlDict() {
    std::cout << "Contents of controlDict:" << std::endl;
    for (const auto& entry : controlDictDefault) {
        std::cout << entry.first << ": ";
        std::visit([](auto&& value) { std::cout << value; }, entry.second);
        std::cout << std::endl;
        // std::cout << entry.first << ": " << entry.second << std::endl;
    }
}

