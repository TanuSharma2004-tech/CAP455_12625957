
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

const int MAX = 50;
const char* INV = "inventory.dat";
const char* PROJ = "projects.dat";

enum Category { ELECTRONICS, HARDWARE, POWER };

void copyText(char* d, const char* s, int n) {
    strncpy(d, s, n - 1);
    d[n - 1] = '\0';
}

class Part {
    int id, qty;
    char name[40], loc[20];
    Category cat;

public:
    static int totalPartsCount;

    Part() : id(0), qty(0), cat(HARDWARE) {
        strcpy(name, "ROBOT");
        strcpy(loc, "MP");
    }

    Part(int i, const char* n, Category c, int q, const char* l) {
        id = i; qty = q; cat = c;
        copyText(name, n, 40);
        copyText(loc, l, 20);
    }

    Part(const Part& p) {
        id = p.id; qty = p.qty; cat = p.cat;
        strcpy(name, p.name);
        strcpy(loc, p.loc);
    }

    ~Part() {}

    void setData(int i, const char* n, Category c, int q, const char* l) {
        id = i; qty = q; cat = c;
        copyText(name, n, 40);
        copyText(loc, l, 20);
        totalPartsCount++;
    }

    void update(int q, const char* l) {
        qty = q;
        copyText(loc, l, 20);
    }

    int getId() const { return id; }

    void display() const {
        cout << "ID: " << id << " | Name: " << name
             << " | Category: "
             << (cat == ELECTRONICS ? "ELECTRONICS" :
                 cat == HARDWARE ? "HARDWARE" : "POWER")
             << " | Quantity: " << qty << " | Location: " << loc;
        if (qty <= 2) cout << " [LOW STOCK ALERT]";
        cout << endl;
    }

    friend void auditPart(const Part*);
};

int Part::totalPartsCount = 0;

void auditPart(const Part* p) {
    if (p)
        cout << "[AUDIT] ID: " << p->id << " | Name: "
             << p->name << " | Location: " << p->loc << endl;
}

class Project {
    int id;
    char name[40], lead[30];

public:
    Project() : id(0) {
        strcpy(name, "AI human");
        strcpy(lead, "unassigned");
    }

    Project(int i, const char* n, const char* l) {
        id = i;
        copyText(name, n, 40);
        copyText(lead, l, 30);
    }

    Project(const Project& p) {
        id = p.id;
        strcpy(name, p.name);
        strcpy(lead, p.lead);
    }

    ~Project() {}

    void setData(int i, const char* n, const char* l) {
        id = i;
        copyText(name, n, 40);
        copyText(lead, l, 30);
    }

    int getId() const { return id; }

    void display() const {
        cout << "Project ID: " << id << " | Title: "
             << name << " | Lead: " << lead << endl;
    }
};

// Common duplicate checking
template <class T>
bool duplicate(T a[], int n, int id) {
    for (int i = 0; i < n; i++)
        if (a[i].getId() == id) return true;
    return false;
}

// Common binary file functions
template <class T>
void load(T a[], int& n, const char* file) {
    ifstream f(file, ios::binary);
    while (n < MAX && f.read((char*)&a[n], sizeof(T)))
        n++;
}

template <class T>
void save(const T& obj, const char* file) {
    ofstream f(file, ios::binary | ios::app);
    if (f) f.write((char*)&obj, sizeof(T));
}

void loadData(Part p[], int& pc, Project pr[], int& rc) {
    load(p, pc, INV);
    load(pr, rc, PROJ);
    Part::totalPartsCount = pc;
}

// Search using seekg
void searchPart(int id) {
    ifstream f(INV, ios::binary);
    Part p;
    int i = 0;

    while (f) {
        f.seekg((streamoff)i * sizeof(Part), ios::beg);
        if (!f.read((char*)&p, sizeof(p))) break;

        if (p.getId() == id) {
            cout << "Record #" << i << ": ";
            p.display();
            return;
        }
        i++;
    }
    cout << "Part not found!\n";
}

// Update using seekp
void updatePart(Part p[], int n, int id) {
    fstream f(INV, ios::binary | ios::in | ios::out);
    Part temp;
    int i = 0;

    if (!f) {
        cout << "File not found!\n";
        return;
    }

    while (true) {
        streamoff pos = (streamoff)i * sizeof(Part);
        f.seekg(pos, ios::beg);

        if (!f.read((char*)&temp, sizeof(temp))) break;

        if (temp.getId() == id) {
            temp.display();

            int q;
            char loc[20];

            cout << "New Quantity: ";
            cin >> q;
            if (q < 0) {
                cout << "Invalid quantity!\n";
                return;
            }

            cin.ignore();
            cout << "New Location: ";
            cin.getline(loc, 20);

            temp.update(q, loc);
            f.seekp(pos, ios::beg);
            f.write((char*)&temp, sizeof(temp));
            f.close();

            for (int j = 0; j < n; j++)
                if (p[j].getId() == id) {
                    p[j].update(q, loc);
                    break;
                }

            cout << "Updated successfully!\n";
            return;
        }
        i++;
    }
    cout << "Part not found!\n";
}

int main() {
    Part parts[MAX];
    Project projects[MAX];
    int pc = 0, rc = 0, ch;

    loadData(parts, pc, projects, rc);

    do {
        cout << "\nROBOTICS CLUB SYSTEM\n"
             << "1. Add Part\n2. View Inventory\n"
             << "3. Register Project\n4. View Projects\n"
             << "5. Audit Part\n6. Search Part\n"
             << "7. Update Part\n8. Exit\nChoice: ";

        cin >> ch;

        switch (ch) {

        case 1: {
            if (pc >= MAX) {
                cout << "Inventory full!\n";
                break;
            }

            int id, q, c;
            char name[40], loc[20];

            cout << "Part ID: ";
            cin >> id;

            if (id <= 0 || duplicate(parts, pc, id)) {
                cout << "Invalid or duplicate ID!\n";
                break;
            }

            cin.ignore();
            cout << "Part Name: ";
            cin.getline(name, 40);

            cout << "Category (0=Electronics, 1=Hardware, 2=Power): ";
            cin >> c;

            if (c < 0 || c > 2) {
                cout << "Invalid category!\n";
                break;
            }

            cout << "Quantity: ";
            cin >> q;

            if (q < 0) {
                cout << "Invalid quantity!\n";
                break;
            }

            cin.ignore();
            cout << "Location: ";
            cin.getline(loc, 20);

            parts[pc].setData(id, name, (Category)c, q, loc);

            Part copy(parts[pc]);
            save(copy, INV);
            pc++;

            cout << "Part added successfully!\n";
            break;
        }

        case 2:
            cout << "\nTotal Parts: " << Part::totalPartsCount << endl;
            for (int i = 0; i < pc; i++) parts[i].display();
            if (!pc) cout << "Inventory empty!\n";
            break;

        case 3: {
            if (rc >= MAX) {
                cout << "Project list full!\n";
                break;
            }

            int id;
            char name[40], lead[30];

            cout << "Project ID: ";
            cin >> id;

            if (id <= 0 || duplicate(projects, rc, id)) {
                cout << "Invalid or duplicate ID!\n";
                break;
            }

            cin.ignore();
            cout << "Project Title: ";
            cin.getline(name, 40);
            cout << "Team Lead: ";
            cin.getline(lead, 30);

            projects[rc].setData(id, name, lead);
            save(projects[rc], PROJ);
            rc++;

            cout << "Project registered!\n";
            break;
        }

        case 4:
            for (int i = 0; i < rc; i++) projects[i].display();
            if (!rc) cout << "No projects!\n";
            break;

        case 5: {
            int id;
            bool found = false;

            cout << "Part ID: ";
            cin >> id;

            for (int i = 0; i < pc; i++)
                if (parts[i].getId() == id) {
                    auditPart(&parts[i]);
                    found = true;
                    break;
                }

            if (!found) cout << "Part not found!\n";
            break;
        }

        case 6: {
            int id;
            cout << "Part ID: ";
            cin >> id;
            searchPart(id);
            break;
        }

        case 7: {
            int id;
            cout << "Part ID: ";
            cin >> id;
            updatePart(parts, pc, id);
            break;
        }

        case 8:
            cout << "Exiting program!\n";
            break;

        default:
            cout << "Invalid choice!\n";
        }

    } while (ch != 8);

    return 0;
}
