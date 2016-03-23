#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

using std::cout; using std::endl; using std::setw;
using std::string;
using std::getline;
using std::ifstream;
using std::vector;


int const STAT_VALUES_COUNT = 15;

struct Process {
    string pid, ppid, sid, uid, tty, proc_time, name;
};

string get_stat_path(string pid) {
    string stat_path ("/proc/");
    stat_path += pid;
    stat_path += "/stat";
    return stat_path;
}

string get_status_path(string pid) {
    string stat_path ("/proc/");
    stat_path += pid;
    stat_path += "/status";
    return stat_path;
}

void insert_stat_values(string table[], string pid) {
    string stat_path = get_stat_path(pid);
    ifstream stat_file(stat_path);
    for (int i = 0; i < STAT_VALUES_COUNT; i++)
        getline(stat_file, table[i], ' ');
    stat_file.close();
}

string get_status_uid_line(string pid) {
    string status_path = get_status_path(pid);
    ifstream status_file(status_path);
    string uid_line;
    // skip unneccessary lines
    int const LINE_NUMBER = 8;
    for (int i = 0; i < LINE_NUMBER; i++)
        getline(status_file, uid_line);
    status_file.close();
    return uid_line;
}
    
bool is_process_alive(string pid) {
    string stat_path = get_stat_path(pid);
    ifstream stat_file(stat_path);
    bool process_exists = stat_file.good();
    stat_file.close();
    return process_exists;
}

Process get_filled_process_object(string stat_values[], string uid_line) {
    Process process;

    // save pid, ppid, sid, tty
    process.pid = stat_values[0];
    process.ppid = stat_values[3];
    process.sid = stat_values[5];
    process.tty = stat_values[6];

    // save proc_time
    int utime, stime;
    utime = std::stoi(stat_values[13]);
    stime = std::stoi(stat_values[14]);
    process.proc_time = std::to_string(utime + stime);
    
    // save name
    string name = stat_values[1];
    // delete parenthesis
    process.name = name.substr(1, name.size() - 2); 

    // uid_line has following structure:
    // Uid: (real uid) (effective uid) (saved set uid) (filesystem uid)
    // let's use effective uid
    std::stringstream ss(uid_line);
    int const WORD_NUMBER = 3;
    for (int i = 0; i < WORD_NUMBER; i++) {
        getline(ss, process.uid, '\t');
    }

    return process;
}

void print_table_top() {
    cout << setw(6) << "PID" << setw(6) << "PPID" 
         << setw(6) << "SID" << setw(6) << "UID"
         << setw(6) << "TTY" << setw(6) << "PTIME"
         << ' ' << "NAME" << endl;
}

void print_process_info(Process &process) {
    cout << setw(6) << process.pid << setw(6) << process.ppid
         << setw(6) << process.sid << setw(6) << process.uid
         << setw(6) << process.tty << setw(6) << process.proc_time
         << ' ' << process.name << endl;
}

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        cout << "Not enough arguments (should be at least 1)" << endl;
        return 1;
    }

    print_table_top();

    for (int i = 1; i < argc; i++) {
        // get argument
        string pid = argv[i];

        // skip this pid if process not found
        if (!is_process_alive(pid)) {
            continue;
        }

        // read stat file (pid, ppid, sid, tty, utime, stime, name)
        string stat_values[STAT_VALUES_COUNT];
        insert_stat_values(stat_values, pid);

        // read status file (uid)
        string uid_line = get_status_uid_line(pid);

        // create object with info
        Process process = get_filled_process_object(stat_values, uid_line);

        // print info
        print_process_info(process);
    }

    return 0;
}
