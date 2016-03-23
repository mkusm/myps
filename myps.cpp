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

int main(int argc, char *argv[]) 
{
    if (argc < 2) {
        cout << "Not enough arguments (should be at least 1)" << endl;
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        // get argument
        string pid = argv[i];
        cout << pid << endl;

        // get stat file path
        string stat_path = get_stat_path(pid);

        // read stat file
        ifstream stat_file(stat_path);
        if (!stat_file.good()) {
            // skip this pid if process not found
            stat_file.close();
            cout << "Process does not exist" << endl << endl;
            continue;
        }
        int STAT_VALUES_COUNT = 15;
        string stat_values[STAT_VALUES_COUNT];
        for (int i = 0; i < STAT_VALUES_COUNT; i++) {	
            getline(stat_file, stat_values[i], ' ');
        }
        stat_file.close();

        // read status file
        string status_path = get_status_path(pid);
        ifstream status_file(status_path);
        string uid_line;
        // skip unneccessary lines
        int const LINE_NUMBER = 8;
        for (int i = 0; i < LINE_NUMBER; i++)
            getline(status_file, uid_line);
        status_file.close();

        Process process = get_filled_process_object(stat_values, uid_line);

        cout << setw(13) << "PID: " << process.pid << endl
             << setw(13) << "PPID: " << process.ppid << endl
             << setw(13) << "SID: " << process.sid << endl
             << setw(13) << "UID: " << process.uid << endl
             << setw(13) << "TTY: " << process.tty << endl
             << setw(13) << "PROC_TIME: " << process.proc_time << endl
             << setw(13) << "NAME: " << process.name << endl;

        cout << endl;
    }

    return 0;
}
