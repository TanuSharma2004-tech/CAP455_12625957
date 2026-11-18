#include <iostream>
#include <string>
using namespace std;

const int MAX_ITEMS = 50;

enum ComponentCategory { ELECTRONICS, HARDWARE, POWER };

class Part {
private:
    int id;
    string name;
    ComponentCategory category;
    int quantity;
    string location;

public:
    static int totalPartsCount;

    void setPartData(int id, string name, ComponentCategory category, int quantity, string location) {
        this->id = id;
        this->name = name;
        this->category = category;
        this->quantity = quantity;
        this->location = location;
        totalPartsCount++;
    }

    // Inline member functions
    inline int getId() const { return id; }
    inline int getQuantity() const { return quantity; }
    inline string getName() const { return name; }

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

    // Friend function declaration
    friend void auditPart(const Part* p);
};

int Part::totalPartsCount = 0;

// Friend function accessing private members via pointer
void auditPart(const Part* p) {
    if (p != nullptr) {
        cout << "[AUDIT] ID: " << p->id << " | Name: " << p->name << " | Location: " << p->location << endl;
    }
}

class Project {
private:
    int projectId;
    string projectName;
    string teamLead;

public:
    void setProjectData(int projectId, string projectName, string teamLead) {
        this->projectId = projectId;
        this->projectName = projectName;
        this->teamLead = teamLead;
    }

    inline int getId() const { return projectId; }

    void displayProject() const {
        cout << "Project ID: " << projectId
             << " | Title: " << projectName
             << " | Lead: " << teamLead << endl;
    }
};

// Duplicate Key Checks using pointers
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

int main() {
    Part inventory[MAX_ITEMS];
    Project projects[MAX_ITEMS];
    int partCount = 0;
    int projCount = 0;
    int choice = 0;

    while (choice != 6) {
        cout << "\n=== ROBOTICS CLUB SYSTEM (CA1: DESIGN & CREATE) ===\n";
        cout << "1. Add New Part to Inventory\n";
        cout << "2. View All Inventory & Alerts (Total Parts: " << Part::totalPartsCount << ")\n";
        cout << "3. Register New Project\n";
        cout << "4. View All Projects\n";
        cout << "5. Audit Part (Friend & Pointer Demo)\n";
        cout << "6. Exit\n";
        cout << "Enter choice (1-6): ";
        cin >> choice;

        if (choice == 1) {
            if (partCount >= MAX_ITEMS) {
                cout << "Inventory full!\n";
                continue;
            }
            int id, qty, catChoice;
            string name, loc;

            cout << "Enter Part ID: ";
            cin >> id;
            if (id <= 0 || isPartIdDuplicate(inventory, partCount, id)) {
                cout << "Error: Invalid or Duplicate Part ID!\n";
                continue;
            }
            cin.ignore();
            cout << "Enter Part Name: ";
            getline(cin, name);
            if (name.empty()) {
                cout << "Error: Part Name cannot be empty!\n";
                continue;
            }
            cout << "Category (0=ELECTRONICS, 1=HARDWARE, 2=POWER): ";
            cin >> catChoice;
            if (catChoice < 0 || catChoice > 2) {
                cout << "Error: Invalid Category!\n";
                continue;
            }
            ComponentCategory cat = static_cast<ComponentCategory>(catChoice);

            cout << "Enter Quantity: ";
            cin >> qty;
            if (qty < 0) {
                cout << "Error: Quantity cannot be negative!\n";
                continue;
            }
            cin.ignore();
            cout << "Enter Location (e.g., BIN-A1): ";
            getline(cin, loc);
            if (loc.empty()) {
                cout << "Error: Location cannot be empty!\n";
                continue;
            }

            (inventory + partCount)->setPartData(id, name, cat, qty, loc);
            partCount++;
            cout << "Part successfully added to memory!\n";

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
            string title, lead;

            cout << "Enter Project ID: ";
            cin >> id;
            if (id <= 0 || isProjectIdDuplicate(projects, projCount, id)) {
                cout << "Error: Invalid or Duplicate Project ID!\n";
                continue;
            }
            cin.ignore();
            cout << "Enter Project Title: ";
            getline(cin, title);
            if (title.empty()) {
                cout << "Error: Project Title cannot be empty!\n";
                continue;
            }
            cout << "Enter Project Lead: ";
            getline(cin, lead);
            if (lead.empty()) {
                cout << "Error: Project Lead cannot be empty!\n";
                continue;
            }

            (projects + projCount)->setProjectData(id, title, lead);
            projCount++;
            cout << "Project registered in memory!\n";

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
            cout << "Exiting program.\n";
        } else {
            cout << "Wrong choice! Try again.\n";
        }
    }
    return 0;
}