#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<list>
using namespace std;

struct LabLocation{
    int binNumber;
    char shelfcode[10];
};
union TempId{
    int numericId;
    char charCode[4];
};

//enum used for component ki category 
enum ComponentCategory { ELECTRONICS, HARDWARE, POWER };

class Part{
    private:
    int id;
    string name;
    ComponentCategory category;
    int quantity;
    string location;

    public:
    static int totalPartsCount;

    //default constructor
    Part(): id(0), name("ROBOT"),category(HARDWARE), quantity(0), location("mp"){
    totalPartsCount++;           
};
    //parameterized constructor
    Part(int pId,
        string pName, 
        ComponentCategory pCat, 
        int pQty=1,
        string pLoc="BIN-A1"):
        id(pId),
        name(pName),
        category(pCat),
        quantity(pQty),
        location(pLoc)
        {totalPartsCount++;}

//Destructor use here
~Part(){
    totalPartsCount--;
};

//Inline Functions
inline int getId() const{return id;}
inline int getQuantity() const{return quantity;}
inline string getName() const{return name;}

void displayPart() const{
    cout<<"ID: "<<id<<"Name"<<name<<"Quantity: "<<quantity<<"Location"<<location;
    if(quantity<=2){
        cout<<"LOW STOCK ALERT!";
    }
    cout<<endl;
}
//type conversion Operator(class to basic int)
operator int() const{
    return quantity;
}
};
//Static Member Initialization
int Part::totalPartsCount=0;

//Abstract Class & Derived Class
class LogItem{
    protected:
    string timestamp;
    public:
    LogItem(string ts):timestamp(ts){}
    virtual void printLog() const=0; //pure virtual function
    virtual ~LogItem(){}
};
//concrete class
class BuildLogEntry: public LogItem{
    private:
    string author;
    string description;
    public:
    BuildLogEntry(string ts, string auth, string desc): LogItem(ts), author(auth), description(desc){}
void printLog() const override{
    cout<<""<<timestamp<<""<<author<<""<<description<<endl;
}
   string toFileString() const {
    return timestamp+" "+author+" "+description;
   } 
};

class clubEntity{
    protected:
       int entityId;
    public:
      clubEntity(int id=0):entityId(id){}
      virtual void displayHeader(){
         cout<<"Entity ID: "<<entityId<<endl;
      }
};

//this base class 1 made to prevent from diamond problem =hybrid
class resorceTracked: virtual public clubEntity{
    public:
    resorceTracked(int id=0): clubEntity(id){}
};

//this base class 2 made to prevent from diamond problem =hybrid
class workTracked: virtual public clubEntity{
    public:
    workTracked(int id=0): clubEntity(id){}
};

class project: public resorceTracked, public workTracked{
    private:
      string projectName;
      string teamlead;
      list<BuildLogEntry> logs; //this is STL list contain LOGS
    public:
      project(int pId=0, string pName="AI human", string pLead="unassigned"): clubEntity(pId), resorceTracked(pId),workTracked(pId),projectName(pName),teamlead(pLead){}
    
    int getId() const{return entityId;}
    string getproject() const{return projectName;}

    //build log add krne k liye function
    void addBuildLog(const BuildLogEntry& log){
        logs.push_back(log);
    }
    const list<BuildLogEntry>& getLogs() const{ return logs;
    }
void displayHeader() override{
    cout << "--- Build Logs ---\n";
    if(logs.empty()){
        cout<<"NO build logs recorded.\n";
    }
    else{
        for(list<BuildLogEntry>::const_iterator it=logs.begin(); it !=logs.end(); ++it){
            it->printLog();
        }
    }
}

string toFileString() const {
        return to_string(entityId) + "|" + projectName + "|" + teamlead;
    }
};

class RoboticsSystemManager {
private:
    vector<Part> inventory;
    vector<project> projects;

public:
    RoboticsSystemManager() {
        loadDataFromFiles();
    }

    ~RoboticsSystemManager() {
        saveDataToFiles();
    }

    // text file for save data
    void saveDataToFiles() {
        ofstream outInv("inventory.txt", ios::out | ios::trunc);
        if (outInv.is_open()) {
            for (const auto& part : inventory) {
                outInv << part.getId() << "|" << part.getName() << "|" 
                       << static_cast<int>(part) << "\n";
            }
            outInv.close();
        }

        ofstream outProj("projects.txt", ios::out | ios::trunc);
        if (outProj.is_open()) {
            for (const auto& proj : projects) {
                outProj << "PROJ|" << proj.toFileString() << "\n";
                for (const auto& log : proj.getLogs()) {
                    outProj << "LOG|" << log.toFileString() << "\n";
                }
            }
            outProj.close();
        }
    }

    // data load from file 
    void loadDataFromFiles() {
        ifstream inInv("inventory.txt", ios::in);
        if (!inInv.is_open()) return;

        string line;
        while (getline(inInv, line)) {
            if (line.empty()) continue;
            size_t pos1 = line.find('|');
            size_t pos2 = line.rfind('|');

            if (pos1 != string::npos && pos2 != string::npos) {
                int id = stoi(line.substr(0, pos1));
                string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
                int qty = stoi(line.substr(pos2 + 1));
                
                inventory.push_back(Part(id, name, HARDWARE, qty, "Lab Storage"));
            }
        }
        inInv.close();
    }

    // for adding new parts
    void addNewPart() {
        int id, qty;
        string name;

        cout << "\nEnter Part ID: ";
        cin >> id;
        cin.ignore();
        cout << "Enter Part Name: ";
        getline(cin, name);
        cout << "Enter Quantity: ";
        cin >> qty;

        try {
            if (qty < 0) throw "Quantity cannot be negative !";
            inventory.push_back(Part(id, name, HARDWARE, qty, "Bin-A1"));
            cout << "Part successfully add ho gaya!\n";
        } catch (const char* msg) {
            cout << "[Error]: " << msg << endl;
        }
    }

    // for see the Inventory 
    void displayInventory() {
        cout << "\n--- CURRENT INVENTORY ---\n";
        if (inventory.empty()) {
            cout << "Inventory khali hai.\n";
            return;
        }
        for (const auto& item : inventory) {
            item.displayPart();
        }
    }

    // for register new project
    void createProject() {
        int id;
        string title, lead;

        cout << "\nEnter Project ID: ";
        cin >> id;
        cin.ignore();
        cout << "Enter Project Title: ";
        getline(cin, title);
        cout << "Enter Project Lead: ";
        getline(cin, lead);

        projects.push_back(project(id, title, lead));
        cout << "Project register ho gaya!\n";
    }

    // for adding Build log 
    void addLogToProject() {
        int projId;
        string date, author, desc;

        cout << "\nEnter Project ID: ";
        cin >> projId;
        cin.ignore();

        for (auto& proj : projects) {
            if (proj.getId() == projId) {
                cout << "Enter Date (YYYY-MM-DD): ";
                getline(cin, date);
                cout << "Enter Author Name: ";
                getline(cin, author);
                cout << "Enter Work Description: ";
                getline(cin, desc);

                proj.addBuildLog(BuildLogEntry(date, author, desc));
                cout << "Build log is added!\n";
                return;
            }
        }
        cout << "Error: no project!\n";
    }

    // Projects aur logs dikhane ke liye
    void displayProjects() {
        if (projects.empty()) {
            cout << "\nno active project\n";
            return;
        }
        for (auto& proj : projects) {
            proj.displayHeader();
        }
    }
};


int main() {
    RoboticsSystemManager manager;
    int choice = 0;

    while (choice != 6) {
       
        cout << "   ROBOTICS CLUB PARTS & BUILD LOG       \n";
        cout << "1. Add New Part to Inventory\n";
        cout << "2. View All Inventory & Alerts\n";
        cout << "3. Register New Project\n";
        cout << "4. Add Daily Build Log Entry\n";
        cout << "5. View Projects & Build History\n";
        cout << "6. Save & Exit\n";
        cout << "Enter choice (1-6): ";
        cin >> choice;

        switch (choice) {
            case 1: 
                manager.addNewPart(); 
                break;
            case 2: 
                manager.displayInventory(); 
                break;
            case 3: 
                manager.createProject(); 
                break;
            case 4: 
                manager.addLogToProject(); 
                break;
            case 5: 
                manager.displayProjects(); 
                break;
            case 6: 
                manager.saveDataToFiles(); 
                cout << "Data is saved. Program exit now!\n"; 
                break;
            default: 
                cout << "Wrong choice! try again.\n";
        }
    }
    return 0;
}

        
        


