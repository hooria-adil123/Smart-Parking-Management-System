#include <iostream>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <stdlib.h>
using namespace std;
using namespace std::chrono;

system_clock::time_point loadBaseTime() {
    ifstream file("last_exit_time.txt");
    time_t savedTime;
    if (file >> savedTime) {
        return system_clock::from_time_t(savedTime);
    } else {
        return system_clock::now();
    }
}

void saveBaseTime(system_clock::time_point timeToSave) {
    ofstream file("last_exit_time.txt");
    time_t t = system_clock::to_time_t(timeToSave);
    file << t;
}

int simulateTime(system_clock::time_point entryTime, system_clock::time_point baseTime) {
    auto now = system_clock::now();
    auto realSeconds = duration_cast<seconds>(now - baseTime).count();
    int codeMinutes = (realSeconds / 10); 
    return codeMinutes;
}

string printParkingTime(system_clock::time_point entryTime, system_clock::time_point baseTime) {
    string parkTime;
    int codeMinutes = simulateTime(entryTime, baseTime);
    int baseHours = 10;
    int baseMinutes = 0;
    int totalMinutes = baseHours * 60 + baseMinutes + codeMinutes;
    int hours = totalMinutes / 60;
    int minutes = totalMinutes % 60;
    parkTime = to_string(hours) + ":" + (minutes < 10 ? "0" : "") + to_string(minutes);
    return parkTime;
}

struct Car{
    const int slots = 40;
    string plates[40];
    system_clock::time_point carEntry[40];
    float carFee = 3.00;
    float carTotal = 0;
};

struct Bike{
    const int slots = 30;
    string plates[30];
    system_clock::time_point bikeEntry[30];
    float bikeFee = 1.50;
    float bikeTotal = 0;
};

struct Van{
    const int slots = 20;
    string plates[20];
    system_clock::time_point vanEntry[20];
    const float vanFee = 4.50;
    float vanTotal = 0; 
};

struct Truck{
    const int slots = 10;
    string plates[10];
    system_clock::time_point truckEntry[10];
    const float truckFee = 6.00;
    float truckTotal = 0;
};

void printmenu(){
cout << "---Smart Parking Lot System---" << endl;
cout << "1. Park Vehicle" << endl;
cout << "2. Remove Vehicle" << endl;
cout << "3. View Available Slots" << endl;
cout << "4. Admin Panel" << endl;
cout << "5. Search Vehicle" << endl;
cout << "6. Exit" << endl;
}

void adminMenu(){
    cout << "ADMIN MENU" << endl;
    cout << endl;
    cout << "1. View Vehicles" << endl;
    cout << "2. View Revenue" << endl;
    cout << "3. Return To Main Menu" << endl;
    cout << endl;
}

void vehicleMenu(){
    cout << "1. Car" << endl;
    cout << "2. Bike" << endl;
    cout << "3. Van" << endl;
    cout << "4. Truck" << endl;
    cout << endl;
}

bool loadVehicle(Car &carData, Bike &bikeData, Truck &truckData, Van &vanData) {
	string type, plate, line, time, status;
    int carCount = 0, bikeCount = 0, truckCount = 0, vanCount = 0;
    int logHour, logMinute;
    bool loaded = false;
    ifstream parkinglog("parkinglog.txt");
    if (!parkinglog.is_open()){
        return false;
    }
    system_clock::time_point now = system_clock::now();

    while (getline(parkinglog, line)) {
        stringstream ss(line);
        ss >> status >> type >> plate >> time;

        if (sscanf(time.c_str(), "%d:%d", &logHour, &logMinute) != 2) { 
            cout << "Invalid time format in log: " << time << endl; 
            continue;
        }

        int entryMinutes = (logHour - 10) * 60 + logMinute;

        auto entryTime = now - minutes(entryMinutes * 10);

        if(status == "EXIT"){
            if (type == "Car") {
                carData.carTotal += carData.carFee; 
                for(int i = 0; i < carData.slots; i++){
                    if(carData.plates[i] == plate){
                        carData.plates[i] = "";
                        carData.carEntry[i] = system_clock::time_point();
                        break;
                    }
                }
            } else if (type == "Bike") {
                bikeData.bikeTotal += bikeData.bikeFee;
                for(int i = 0; i < bikeData.slots; i++){
                    if(bikeData.plates[i] == plate){
                        bikeData.plates[i] = "";
                        bikeData.bikeEntry[i] = system_clock::time_point();
                        break;
                    }
                }
            } else if (type == "Truck"){
                truckData.truckTotal += truckData.truckFee;
                for(int i = 0; i < truckData.slots; i++){
                    if(truckData.plates[i] == plate){
                        truckData.plates[i] = "";
                        truckData.truckEntry[i] = system_clock::time_point();
                        break;
                    }
                }
            } else if (type == "Van"){
                vanData.vanTotal += vanData.vanFee;
                for(int i = 0; i < vanData.slots; i++){
                    if(vanData.plates[i] == plate){
                        vanData.plates[i] = "";
                        vanData.vanEntry[i] = system_clock::time_point();
                        break;
                    }
                }
            }
            loaded = true;
        } else if (status == "PARKED"){
            if (type == "Car" && carCount < carData.slots){
                carData.plates[carCount] = plate;
                carData.carEntry[carCount] = entryTime;
                carCount++;
                loaded = true;
            } else if (type == "Bike" && bikeCount < bikeData.slots){
                bikeData.plates[bikeCount] = plate;
                bikeData.bikeEntry[bikeCount] = entryTime;
                bikeCount++;
                loaded = true;
            } else if (type == "Truck" && truckCount < truckData.slots){
                truckData.plates[truckCount] = plate;
                truckData.truckEntry[truckCount] = entryTime;
                truckCount++;
                loaded = true;
            } else if (type == "Van" && vanCount < vanData.slots){
                vanData.plates[vanCount] = plate;
                vanData.vanEntry[vanCount] = entryTime;
                vanCount++;
                loaded = true;
            }
        }
    }
    parkinglog.close();
    return loaded;
}

void parkCar(Car &carData, system_clock::time_point baseTime) {
    string plate, carTime;
    bool parked = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter Plate Number for Car: ";
    cin >> plate;
    for (int x = 0; x < carData.slots; x++){
        if (plate == carData.plates[x]){
            cout << "Duplicate Plate Number Found" << endl;
            cout << endl;
            parkingfile.close();
            return;
        }
    }
    for (int i = 0; i < carData.slots; i++) {
        if (carData.plates[i] == "") {
            carData.plates[i] = plate;
            carData.carEntry[i] = system_clock::now();
            carTime = printParkingTime(carData.carEntry[i], baseTime);
            cout << "PARKED | Vehicle Type: Car | Plate: " << plate << " | Slot: " << i + 1 << " | Time: " << carTime << endl;
            parkingfile << "PARKED" << " " << "Car" << " " << plate << " " << carTime << endl;
            cout << endl;
            parked = true;
            break;
        }
    }
    if (!parked){
            cout << "No car slots available." << endl;
    }
    parkingfile.close();
}

void parkBike(Bike &bikeData, system_clock::time_point baseTime) {
    string plate, bikeTime;
    bool parked = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter Plate Number for Bike: ";
    cin >> plate;
    for (int x = 0; x < bikeData.slots; x++){
        if (plate == bikeData.plates[x]){
            cout << "Duplicate Plate Number Found" << endl;
            cout << endl;
            parkingfile.close();
            return;
        }
    }
    for (int i = 0; i < bikeData.slots; i++) {
        if (bikeData.plates[i] == "") {
            bikeData.plates[i] = plate;
            bikeData.bikeEntry[i] = system_clock::now();
            bikeTime = printParkingTime(bikeData.bikeEntry[i], baseTime);
            cout << "PARKED | Vehicle Type: Bike | Plate: " << plate << " | Slot: " << i + 1 << " | Time: " << bikeTime << endl;
            parkingfile << "PARKED" << " " << "Bike" << " " << plate << " " << bikeTime << endl;
            cout << endl;
            parked = true;
            break;
        }
    }
    if (!parked){
    cout << "No Bike slots available." << endl;
    }
    parkingfile.close();
}

void parkVan(Van &vanData, system_clock::time_point baseTime) {
    string plate, vanTime;
    bool parked = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter Plate Number for Van: ";
    cin >> plate;
    for (int x = 0; x < vanData.slots; x++){
        if (plate == vanData.plates[x]){
            cout << "Duplicate Plate Number Found" << endl;
            cout << endl;
            parkingfile.close();
            return;
        }
    }
    for (int i = 0; i < vanData.slots; i++) {
        if (vanData.plates[i] == "") {
            vanData.plates[i] = plate;
            vanData.vanEntry[i] = system_clock::now();
            vanTime = printParkingTime(vanData.vanEntry[i], baseTime);
            cout << "PARKED | Vehicle Type: Van | Plate: " << plate << " | Slot: " << i + 1 << " | Time: " << vanTime << endl;
            parkingfile << "PARKED" << " " << "Van" << " " << plate << " " << vanTime << endl;
            cout << endl;
            parked = true;
            break;
        }
    }
    if (!parked){
    cout << "No Van slots available." << endl;
    }
    parkingfile.close();
}

void parkTruck(Truck &truckData, system_clock::time_point baseTime) {
    string plate, truckTime;
    bool parked = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter Plate Number for Truck: ";
    cin >> plate;
    for (int x = 0; x < truckData.slots; x++){
        if (plate == truckData.plates[x]){
            cout << "Duplicate Plate Number Found" << endl;
            cout << endl;
            parkingfile.close();
            return;
        }
    }
    for (int i = 0; i < truckData.slots; i++) {
        if (truckData.plates[i] == "") {
            truckData.plates[i] = plate;
            truckData.truckEntry[i] = system_clock::now();
            truckTime = printParkingTime(truckData.truckEntry[i], baseTime);
            cout << "PARKED | Vehicle Type: Truck | Plate: " << plate << " | Slot: " << i + 1 << " | Time: " << truckTime << endl;
            parkingfile << "PARKED" << " " << "Truck" << " " << plate << " " << truckTime << endl;
            cout << endl;
            parked = true;
            break;
        }
    }
    if (!parked){
    cout << "No Truck slots available." << endl;
    }
}

bool checkRemoved(string plate){
    string type, line, time, status, platex;
    bool parked = false;
    bool removed = false;
    ifstream parkingfile("parkinglog.txt");
    if (!parkingfile.is_open()){
        cout << "Error Opening File" << endl;
        return false;
    }
    while (getline(parkingfile, line)) {
        stringstream ss(line);
        ss >> status >> type >> platex >> time;
        
        if (platex == plate){
            if (status == "PARKED"){
                parked = true;
                removed = false;
            } else if (status == "EXIT"){
                removed = true;
            }
        }
    }
    parkingfile.close();
    return removed;
}


void removeCar(Car &carData, system_clock::time_point baseTime){
    string plate, carExit;
    bool found = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter the car plate number you want to remove: ";
    cin >> plate;
    
    if (checkRemoved(plate)){
        cout << "Error. Car Already Removed" << endl;
        cout << endl;
        parkingfile.close();
        return;
    }

    for (int i = 0; i < carData.slots; i++) {
        if (carData.plates[i] == plate) {
            found = true;
            carExit = printParkingTime(system_clock::now(), baseTime);
            parkingfile << "EXIT" << " " << "Car" << " " <<  plate << " " << carExit << endl;
            carData.plates[i] = ""; 
            carData.carEntry[i] = system_clock::time_point();
            cout << endl;
            cout << "---------------------------" << endl;
            cout << "       Parking Receipt" << endl;
            cout << endl;
            cout << "Vehicle Type: Car" << endl;
            cout << "Plate: " << plate << endl;
            cout << "Fee: $" << carData.carFee << endl;
            cout << "    Thank You For Parking" << endl;
            cout << "---------------------------" << endl;
            cout << endl;
            carData.carTotal += carData.carFee;
            break;
        }
    }
    if (!found) {
        cout << "Car not found!" << endl;
    }
    parkingfile.close();
}

void removeBike(Bike &bikeData, system_clock::time_point baseTime){
    string plate, bikeExit;
    bool found = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter Bike Plate Number To remove: ";
    cin >> plate;
    
    if (checkRemoved(plate)){
        cout << "Error. Bike Already Removed" << endl;
        cout << endl;
        parkingfile.close();
        return;
    }
    for (int i = 0; i < bikeData.slots; i++) {
        if (bikeData.plates[i] == plate) {
            found = true;
            bikeExit = printParkingTime(system_clock::now(), baseTime);
            parkingfile << "EXIT" << " " << "Bike" << " " << plate << " " << bikeExit << endl;
            bikeData.plates[i] = ""; 
            bikeData.bikeEntry[i] = system_clock::time_point();
            cout << endl;
            cout << "---------------------------" << endl;
            cout << "       Parking Receipt" << endl;
            cout << endl;
            cout << "Vehicle Type: Bike" << endl;
            cout << "Plate: " << plate << endl;
            cout << "Fee: $" << bikeData.bikeFee << endl;
            cout << "    Thank You For Parking" << endl;
            cout << "---------------------------" << endl;
            cout << endl;
            bikeData.bikeTotal += bikeData.bikeFee;
            break;
        }
    }
    if (!found) {
        cout << "Bike not found!" << endl;
    }
    parkingfile.close();
}

void removeVan(Van &vanData, system_clock::time_point baseTime){
    string plate, vanExit;
    bool found = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter Van Plate Number To remove: ";
    cin >> plate;
    
    if (checkRemoved(plate)){
        cout << "Error. Van Already Removed" << endl;
        cout << endl;
        parkingfile.close();
        return;
    }
    for (int i = 0; i < vanData.slots; i++) {
        if (vanData.plates[i] == plate) {
            found = true;
            vanExit = printParkingTime(system_clock::now(), baseTime);
            parkingfile << "EXIT" << " " << "Van" << " " << plate << " " << vanExit << endl;
            vanData.plates[i] = ""; 
            vanData.vanEntry[i] = system_clock::time_point();
            cout << endl;
            cout << "---------------------------" << endl;
            cout << "       Parking Receipt" << endl;
            cout << endl;
            cout << "Vehicle Type: Van" << endl;
            cout << "Plate: " << plate << endl;
            cout << "Fee: $" << vanData.vanFee << endl;
            cout << "    Thank You For Parking" << endl;
            cout << "---------------------------" << endl;
            cout << endl;
            vanData.vanTotal += vanData.vanFee;
            break;
        }
    }
    if (!found) {
        cout << "Van not found!" << endl;
    }
    parkingfile.close();
}

void removeTruck(Truck &truckData, system_clock::time_point baseTime){
    string plate, truckExit;
    bool found = false;
    ofstream parkingfile("parkinglog.txt", ios::app);
    if(!parkingfile.is_open()){
        cout << "Unable to open File" << endl;
        return;
    }
    cout << "Enter Truck Plate Number To remove: ";
    cin >> plate;
    
    if (checkRemoved(plate)){
        cout << "Error. Truck Already Removed" << endl;
        cout << endl;
        parkingfile.close();
        return;
    }
    for (int i = 0; i < truckData.slots; i++) {
        if (truckData.plates[i] == plate) {
            found = true;
            truckExit = printParkingTime(system_clock::now(), baseTime);
            parkingfile << "EXIT" << " " << "Truck" << " " << plate << " " << truckExit << endl;
            truckData.plates[i] = ""; 
            truckData.truckEntry[i] = system_clock::time_point();
            cout << endl;
            cout << "---------------------------" << endl;
            cout << "       Parking Receipt" << endl;
            cout << endl;
            cout << "Vehicle Type: Truck" << endl;
            cout << "Plate: " << plate << endl;
            cout << "Fee: $" << truckData.truckFee << endl;
            cout << "    Thank You For Parking" << endl;
            cout << "---------------------------" << endl;
            cout << endl;
            truckData.truckTotal += truckData.truckFee;
            break;
        }
    }
    if (!found) {
        cout << "Truck not found!" << endl;
    }
    parkingfile.close();
}

void countSlots(Car &carData, Bike &bikeData, Van &vanData, Truck &truckData){
    int carCounter = 0, bikeCounter = 0, truckCounter = 0, vanCounter = 0;
    for (int i = 0; i < carData.slots; i++)
    {
        if (carData.plates[i] == ""){ 
            carCounter++;
        }
    }
    for (int i = 0; i < bikeData.slots; i++) {
        if (bikeData.plates[i] == ""){
            bikeCounter++;
        }
    }
    for (int i = 0; i < truckData.slots; i++) {
        if (truckData.plates[i] == ""){ 
            truckCounter++;
        }
    }
    for (int i = 0; i < vanData.slots; i++) {
        if (vanData.plates[i] == ""){ 
            vanCounter++;
        }
    }
    cout << endl;
    cout << "--- Remaining Parking Slots ---" << endl;
    cout << "Car Slots Available: " << carCounter << " out of " << carData.slots << endl;
    cout << "Bike Slots Available: " << bikeCounter << " out of " << bikeData.slots << endl;
    cout << "Truck Slots Available: " << truckCounter << " out of " << truckData.slots << endl;
    cout << "Van Slots Available: " << vanCounter << " out of " << vanData.slots << endl;
    cout << endl;
}

void printlog(){
    ifstream parkinglog("parkinglog.txt");
    string line;
    if(parkinglog.is_open()){
        while(getline(parkinglog, line)){
            cout << line << endl;
        }
        parkinglog.close();
    } else {
        cout << "Unable To Open Parking File" << endl;
    }
}

void adminPanel(Car &carData, Bike &bikeData, Van &vanData, Truck &truckData){
    string storeduser, storedpw;
	string userinput, pwinput;
	int tries = 0, choice;
    ifstream parkLog("parkinglogin.txt");
	if(!parkLog.is_open()){
		cout << "Error. Credentials Could Not be Fetched" << endl;
		return;
	}
    getline(parkLog, storeduser);
	getline(parkLog, storedpw);
	parkLog.close();
    cin.ignore();

    while(tries < 3){
        cout << "Enter Username: ";
		getline(cin, userinput);
		cout << "Enter Password: ";
		getline(cin, pwinput);
		if (storeduser == userinput && storedpw == pwinput) {
		cout << endl;
        cout << "Login Successfull" << endl;
        cout << endl;
        adminMenu();
            do{
                cout << "Enter Option: ";
                cin >> choice;
                switch (choice){
                case 1:
                    cout << endl;
                    printlog();
                    cout << endl;
                    break;
                case 2:
                    cout << endl;
                    cout << "-----Revenue-----" << endl;
                    cout << "Car Revenue: $" << carData.carTotal << endl;
                    cout << "Bike Revenue: $" << bikeData.bikeTotal << endl;
                    cout << "Van Revenue: $" << vanData.vanTotal << endl;
                    cout << "Truck Revenue: $" << truckData.truckTotal << endl;
                    cout << "Total Revenue: $" << carData.carTotal + bikeData.bikeTotal + vanData.vanTotal + truckData.truckTotal << endl;
                    cout << endl;
                    break;
                case 3:
                    cout << "Exiting..." << endl;
                    return;
                }
            } while (choice != 3);
            } else {
                tries++;
			    cout << "Wrong Username or Password. Try Again" << endl;
			    if (tries >= 3) {
				    cout << "Maximum attempts reached. Returning to Main Menu...." << endl;
                    return;
                }
            } 
        }  
}

void searchVehicle(Car &carData, Bike &bikeData, Van &vanData, Truck &truckData){
    string platenumber;
    cout << "Enter the Plate Number of the vehicle you are searching: ";
    cin >> platenumber;
    bool found = false;
    
    for (int i = 0; i < carData.slots; i++){
            if (carData.plates[i] == platenumber){
                cout << "FOUND" << endl;
                cout << "Vehicle Type: Car | Plate number " << platenumber << " | Slot: " << i + 1 << endl;
                cout << endl;
                found = true;
                break;
            }
        }
        if (!found){
            for (int j = 0; j < bikeData.slots; j++){
                if (bikeData.plates[j] == platenumber){
                    cout << "FOUND" << endl;
                    cout << "Vehicle Type: Bike | Plate number " << platenumber << " | Slot: " << j + 1 << endl;
                    cout << endl;
                    found = true;
                    break;
                }
            }
        }
        if (!found){
            for (int x = 0; x < truckData.slots; x++){
                if (truckData.plates[x] == platenumber){
                    cout << "FOUND" << endl;
                    cout << "Vehicle Type: Truck | Plate number " << platenumber << " | Slot: " << x + 1 << endl;
                    cout << endl;
                    found = true;
                    break;
                }
            }
        }
        if (!found){
            for (int y = 0; y < vanData.slots; y++){
                if (vanData.plates[y] == platenumber){
                    cout << "FOUND" << endl;
                    cout << "Vehicle Type: Van | Plate number " << platenumber << " | Slot: " << y + 1 << endl;
                    cout << endl;
                    found = true;
                    break;
                }
            }
        }
        if (!found){
            cout << "Vehicle Doesn't Exist" << endl;
            cout << endl;
        }
}

int main(){
    Car carData;
    Bike bikeData;
    Van vanData;
    Truck truckData;
    int choice, parkchoice, removechoice;
    bool found = false;
    string vehicletype;
    system("Color 0A");
    system_clock::time_point baseTime = loadBaseTime();

    bool dataLoaded = loadVehicle(carData, bikeData, truckData, vanData);
    
    do {
        printmenu();
        cout << "Enter choice: ";
        cin >> choice;
        if (choice < 1 || choice > 6) {
            cout << "Invalid option! Please enter a choice between 1 and 6." << endl;
            cout << endl;
            continue;
        }
        switch(choice){
            case 1:
                vehicleMenu();
                cout << "Enter Option: ";
                cin >> parkchoice;
                cout << endl;
                switch(parkchoice){
                    case 1:
                        parkCar(carData, baseTime);
                        break;
                    case 2:
                        parkBike(bikeData, baseTime);
                        break;
                    case 3:
                        parkVan(vanData, baseTime);
                        break;
                    case 4:
                        parkTruck(truckData, baseTime);
                        break;
                }
                break;
            case 2:
                vehicleMenu();
                cout << "Enter Option For Exit: ";
                cin >> removechoice;
                cout << endl;
                switch(removechoice){
                    case 1:
                        removeCar(carData, baseTime);
                        break;
                    case 2:
                        removeBike(bikeData, baseTime);
                        break;
                    case 3:
                        removeVan(vanData, baseTime);
                        break;
                    case 4:
                        removeTruck(truckData, baseTime);
                        break;
                }
                break;
            case 3:
                countSlots(carData, bikeData, vanData, truckData);
                cout << endl;
                break;
            case 4:
                if (dataLoaded){
                    cout << endl;
                    cout << "Vehicle Data Loaded Successfully" << endl;
                    cout << endl;
                } else {
                    cout << endl;
                    cout << "No Previous Data" << endl;
                    cout << endl;
                }
                adminPanel(carData, bikeData, vanData, truckData);
                cout << endl;
                break;
            case 5:
                searchVehicle(carData, bikeData, vanData, truckData);
                break;
            case 6:
                cout << "Exiting System......" << endl;
                string exitTime = printParkingTime(system_clock::now(), baseTime);
                saveBaseTime(baseTime);
                return 0;
        }
    } while (choice > 0);
    system("pause");
}