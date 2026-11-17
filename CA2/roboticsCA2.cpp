#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

const int MAX_ITEMS = 50;
const char* INV_FILE = "inventory.dat";
const char* PROJ_FILE = "projects.dat";

enum ComponentCategory { ELECTRONICS, HARDWARE, POWER };

class Part {
private:
    int id;
    char name[40];          
    ComponentCategory category;
    int quantity;
    char location[20];      
public:
    static int totalPartsCount;

    // 1. Default Constructor
    Part() {
        id = 0;
        strcpy(name, "ROBOT");
        category = HARDWARE;
        quantity = 0;
        strcpy(location, "MP");
    }

    // 2. Parameterized Constructor
    Part(int id, const char* name, ComponentCategory category, int quantity, const char* location) {
        this->id = id;
        strncpy(this->name, name, 39);
        this->name[39] = '\0';
        this->category = category;
        this->quantity = quantity;
        strncpy(this->location, location, 19);
        this->location[19] = '\0';
    }

    // 3. Copy Constructor
    Part(const Part& other) {
        this->id = other.id;
        strcpy(this->name, other.name);
        this->category = other.category;
        this->quantity = other.quantity;
        strcpy(this->location, other.location);
    }

    // 4. Destructor
    ~Part() {}

    void setPartData(int id, const char* name, ComponentCategory category, int quantity, const char* location) {
        this->id = id;
        strncpy(this->name, name, 39);
        this->name[39] = '\0';
        this->category = category;
        this->quantity = quantity;
        strncpy(this->location, location, 19);
        this->location[19] = '\0';
        totalPartsCount++;
    }

    void updateData(int quantity, const char* location) {
        this->quantity = quantity;
        strncpy(this->location, location, 19);
        this->location[19] = '\0';
    }

    // Inline 
    inline int getId() const { return id; }
    inline int getQuantity() const { return quantity; }
    inline const char* getName() const { return name; }

    void displayPart() const {
        cout << "ID: " << id 
             << " | Name: " << name 
             << " | Category: " << (category == 0 ? "ELECTRONICS" : (category == 1 ? "HARDWARE" : "POWER"))
             << " | Quantity: " << quantity 
             << " | Location: " << location;
        if (quantity <= 2) {
            cout << " [LOW STOCK ALERT!]";
        }
        cout << endl;
    }

    friend void auditPart(const Part* p);
};

int Part::totalPartsCount = 0;

void auditPart(const Part* p) {
    if (p != nullptr) {
        cout << "[AUDIT] ID: " << p->id << " | Name: " << p->name << " | Location: " << p->location << endl;
    }
}

class Project {
private:
    int projectId;
    char projectName[40];   
    char teamLead[30];      
public:
    // 1. Default Constructor
    Project() {
        projectId = 0;
        strcpy(projectName, "AI human");
        strcpy(teamLead, "unassigned");
    }

    // 2. Parameterized Constructor
    Project(int projectId, const char* projectName, const char* teamLead) {
        this->projectId = projectId;
        strncpy(this->projectName, projectName, 39);
        this->projectName[39] = '\0';
        strncpy(this->teamLead, teamLead, 29);
        this->teamLead[29] = '\0';
    }

    // 3. Copy Constructor
    Project(const Project& other) {
        this->projectId = other.projectId;
        strcpy(this->projectName, other.projectName);
        strcpy(this->teamLead, other.teamLead);
    }

    // 4. Destructor
    ~Project() {}

    void setProjectData(int projectId, const char* projectName, const char* teamLead) {
        this->projectId = projectId;
        strncpy(this->projectName, projectName, 39);
        this->projectName[39] = '\0';
        strncpy(this->teamLead, teamLead, 29);
        this->teamLead[29] = '\0';
    }

    inline int getId() const { return projectId; }

    void displayProject() const {
        cout << "Project ID: " << projectId 
             << " | Title: " << projectName 
             << " | Lead: " << teamLead << endl;
    }
};

// Duplicate Key 
bool isPartIdDuplicate(const Part* arr, int count, int id) {
    for (int i = 0; i < count; i++) {
        if ((arr + i)->getId() == id) return true;
    }
    return false;
}

bool isProjectIdDuplicate(const Project* arr, int count, int id) {
    for (int i = 0; i < count; i++) {
        if ((arr + i)->getId() == id) return true;
    }
    return false;
}

void loadDataFromFiles(Part* inventory, int& partCount, Project* projects, int& projCount) {
    ifstream inInv(INV_FILE, ios::binary);
    if (inInv) {
        Part temp;
        while (partCount < MAX_ITEMS && inInv.read(reinterpret_cast<char*>(&temp), sizeof(Part))) {
            inventory[partCount] = temp;
            partCount++;
            Part::totalPartsCount++;
        }
        inInv.close();
    }

    ifstream inProj(PROJ_FILE, ios::binary);
    if (inProj) {
        Project tempProj;
        while (projCount < MAX_ITEMS && inProj.read(reinterpret_cast<char*>(&tempProj), sizeof(Project))) {
            projects[projCount] = tempProj;
            projCount++;
        }
        inProj.close();
    }
}

// 2. Append single fixed-size Part record to binary file
void appendPartToFile(const Part& p) {
    ofstream outFile(INV_FILE, ios::binary | ios::app);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(&p), sizeof(Part));
        outFile.close();
    }
}

// 3. Append single fixed-size Project record to binary file
void appendProjectToFile(const Project& proj) {
    ofstream outFile(PROJ_FILE, ios::binary | ios::app);
    if (outFile) {
        outFile.write(reinterpret_cast<const char*>(&proj), sizeof(Project));
        outFile.close();
    }
}

// 4. Direct random-access search in binary file using seekg
void searchPartInFile(int searchId) {
    ifstream inFile(INV_FILE, ios::binary);
    if (!inFile) {
        cout << "No saved data file found yet.\n";
        return;
    }

    inFile.seekg(0, ios::end);
    int totalRecords = inFile.tellg() / sizeof(Part);

    Part temp;
    for (int i = 0; i < totalRecords; i++) {
        inFile.seekg(i * sizeof(Part), ios::beg);
        inFile.read(reinterpret_cast<char*>(&temp), sizeof(Part));

        if (temp.getId() == searchId) {
            cout << "[FILE SEARCH - Record #" << i << "] ";
            temp.displayPart();
            inFile.close();
            return;
        }
    }
    cout << "Part ID " << searchId << " not found in binary file.\n";
    inFile.close();
}

// 5. Basic in-place update in binary file using seekp
void updatePartInFileAndMemory(Part* inventory, int partCount, int targetId) {
    fstream file(INV_FILE, ios::binary | ios::in | ios::out);
    if (!file) {
        cout << "No saved data file found yet.\n";
        return;
    }

    file.seekg(0, ios::end);
    int totalRecords = file.tellg() / sizeof(Part);

    Part temp;
    for (int i = 0; i < totalRecords; i++) {
        streampos recordOffset = i * sizeof(Part);
        file.seekg(recordOffset, ios::beg);
        file.read(reinterpret_cast<char*>(&temp), sizeof(Part));

        if (temp.getId() == targetId) {
            cout << "Current Record: ";
            temp.displayPart();

            int newQty;
            char newLoc[20];
            cout << "Enter New Quantity: ";
            cin >> newQty;
            if (newQty < 0) {
                cout << "Error: Quantity cannot be negative!\n";
                file.close();
                return;
            }
            cin.ignore();
            cout << "Enter New Location: ";
            cin.getline(newLoc, 20);

            temp.updateData(newQty, newLoc);

            // Overwrite exact record position using seekp
            file.seekp(recordOffset, ios::beg);
            file.write(reinterpret_cast<const char*>(&temp), sizeof(Part));
            file.close();

            // Sync in-memory array as well
            for (int j = 0; j < partCount; j++) {
                if (inventory[j].getId() == targetId) {
                    inventory[j].updateData(newQty, newLoc);
                    break;
                }
            }
            cout << "Record updated in binary file (via seekp) and memory!\n";
            return;
        }
    }
    cout << "Part ID " << targetId << " not found.\n";
    file.close();
}

int main() {
    Part inventory[MAX_ITEMS];
    Project projects[MAX_ITEMS];
    int partCount = 0;
    int projCount = 0;
    int choice = 0;

    loadDataFromFiles(inventory, partCount, projects, projCount);

    while (choice != 8) {
        cout << "\n=== ROBOTICS CLUB SYSTEM (CA2: BINARY PERSISTENCE) ===\n";
        cout << "1. Add New Part to Inventory (Save to File)\n";
        cout << "2. View All Inventory & Alerts (Total Parts: " << Part::totalPartsCount << ")\n";
        cout << "3. Register New Project (Save to File)\n";
        cout << "4. View All Projects\n";
        cout << "5. Audit Part (Friend & Pointer Demo)\n";
        cout << "6. Search Part in Binary File (seekg)\n";
        cout << "7. Update Part Quantity/Location in File (seekp)\n";
        cout << "8. Exit\n";
        cout << "Enter choice (1-8): ";
        cin >> choice;

        if (choice == 1) {
            if (partCount >= MAX_ITEMS) {
                cout << "Inventory full!\n";
                continue;
            }
            int id, qty, catChoice;
            char name[40], loc[20];

            cout << "Enter Part ID: ";
            cin >> id;
            if (id <= 0 || isPartIdDuplicate(inventory, partCount, id)) {
                cout << "Error: Invalid or Duplicate Part ID!\n";
                continue;
            }
            cin.ignore();
            cout << "Enter Part Name: ";
            cin.getline(name, 40);
            cout << "Category (0=ELECTRONICS, 1=HARDWARE, 2=POWER): ";
            cin >> catChoice;
            ComponentCategory cat = (catChoice == 0) ? ELECTRONICS : (catChoice == 2 ? POWER : HARDWARE);
            
            cout << "Enter Quantity: ";
            cin >> qty;
            if (qty < 0) {
                cout << "Error: Quantity cannot be negative!\n";
                continue;
            }
            cin.ignore();
            cout << "Enter Location (e.g., BIN-A1): ";
            cin.getline(loc, 20);

            (inventory + partCount)->setPartData(id, name, cat, qty, loc);
            
            // CA2: Copy Constructor demo + save fixed-size record to binary file
            Part copyPart(inventory[partCount]);
            appendPartToFile(copyPart);

            partCount++;
            cout << "Part successfully added & saved to " << INV_FILE << "!\n";

        } else if (choice == 2) {
            cout << "\n--- CURRENT INVENTORY ---\n";
            if (partCount == 0) cout << "Inventory is empty.\n";
            for (int i = 0; i < partCount; i++) {
                (inventory + i)->displayPart();
            }

        } else if (choice == 3) {
            if (projCount >= MAX_ITEMS) {
                cout << "Project list full!\n";
                continue;
            }
            int id;
            char title[40], lead[30];

            cout << "Enter Project ID: ";
            cin >> id;
            if (id <= 0 || isProjectIdDuplicate(projects, projCount, id)) {
                cout << "Error: Invalid or Duplicate Project ID!\n";
                continue;
            }
            cin.ignore();
            cout << "Enter Project Title: ";
            cin.getline(title, 40);
            cout << "Enter Project Lead: ";
            cin.getline(lead, 30);

            (projects + projCount)->setProjectData(id, title, lead);
            
            appendProjectToFile(projects[projCount]);

            projCount++;
            cout << "Project registered & saved to " << PROJ_FILE << "!\n";

        } else if (choice == 4) {
            cout << "\n--- REGISTERED PROJECTS ---\n";
            if (projCount == 0) cout << "No active projects.\n";
            for (int i = 0; i < projCount; i++) {
                (projects + i)->displayProject();
            }

        } else if (choice == 5) {
            int searchId;
            cout << "Enter Part ID to audit: ";
            cin >> searchId;
            bool found = false;
            for (int i = 0; i < partCount; i++) {
                if (inventory[i].getId() == searchId) {
                    auditPart(&inventory[i]);
                    found = true;
                    break;
                }
            }
            if (!found) cout << "Part not found.\n";

        } else if (choice == 6) {
            int searchId;
            cout << "Enter Part ID to search in binary file: ";
            cin >> searchId;
            searchPartInFile(searchId);

        } else if (choice == 7) {
            int targetId;
            cout << "Enter Part ID to update in binary file: ";
            cin >> targetId;
            updatePartInFileAndMemory(inventory, partCount, targetId);

        } else if (choice == 8) {
            cout << "Exiting program. All records remain saved on disk.\n";
        } else {
            cout << "Wrong choice! Try again.\n";
        }
    }
    return 0;
}