// Name: Luke Ambray
// Course: COP 4600
// Semester: Spring 2022
// NID: 4562758
// Homework 3: mysh


#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

// definitions
const char * MYSH = "mysh.history";

// convenient functions
void loadHistory(void);
void tokenInput(string const &userInput, vector<string> &out);
void runCommand(vector<string> line);

// required commands
int displayHistory(string argument);
void byebye(void);
int replay(vector<string> arguments);
int startProcess(vector<string> arguments);
int backgroundProcess(vector<string> arguments);
int terminateProcess(vector<string> arguments);

// extra credit
int repeatProcess(vector<string> arguments);
void printChildren();
int terminateAllProcesses();

// global variables
fstream histFile;
vector<int> pidList;
vector<vector<string>> prevCommands;

int main(void)
{
	// load history file into memory (if it exists)
	loadHistory();

	// Read in user input
	string userInput;
	while(true) // continuously read in user input
	{
		cout << "# "; // print prompt # and wait for input
		getline(cin, userInput); // read in line
		vector<string> out;
		tokenInput(userInput, out); // tokenize user input
		prevCommands.push_back(out); // store the command in history
		runCommand(out);
 	}

	return 0;
}

void tokenInput(string const &userInput, vector<string> &out)
{
	stringstream ss(userInput); // construct a stream from userInput

	string token;
	while (getline(ss, token, ' ')) // delimit arguments by whitespace
	{
		out.push_back(token);
	}

	return;
}

void runCommand(vector<string> out)
{
	// compare command with those implemented
	if (out[0].compare("history") == 0)
	{
		if (out.size() > 1) displayHistory(out[1]);
		else displayHistory(" ");
	}
	else if (out[0].compare("byebye") == 0)
		byebye();
	else if (out[0].compare("replay") == 0)
	{
		if (replay(out) != 0)
			cout << "Failed to replay command\n";
	}
	else if (out[0].compare("background") == 0)
	{
		int pid = backgroundProcess(out);
		if (pid == -1)
			cout << "Failed to start background process\n";
		else
		{
			 cout << "Child PID: " << pid << endl;
			 pidList.push_back((int) pid);
		}
	}
	else if (out[0].compare("terminate") == 0)
	{
		if (terminateProcess(out) != 0)
			cout << "Failed to terminate process\n";
	}
	else if (out[0].compare("repeat") == 0)
	{
		if (repeatProcess(out) != 0)
			cout << "Unable to repeat command\n";
	}
	else if (out[0].compare("terminateall") == 0)
	{
		if (terminateAllProcesses() != 0)
			cout << "Failed to terminate all processes\n";
	}
	else if (out[0].compare("start") == 0)
	{
		if (startProcess(out) != 0)
			cout << "Failed to start process\n";
	}
	/*
	else if (out[0].compare("children") == 0) // for debug use only
		printChildren();
	*/
	else
		cout << out[0] << ": command not found" << endl;
}

void loadHistory()
{
	histFile.open(MYSH, ios::in); // open and close file to ensure it exists
	histFile.close();
	histFile.open(MYSH, ios::in); // open the file to read from it
	if (histFile.is_open()) // check if file is open
	{
		string currentLine;

		while (getline(histFile, currentLine)) // read data from line and put in string
		{
			vector<string> out;
 			tokenInput(currentLine, out);
			prevCommands.push_back(out);
		}

		histFile.close(); // close history file
   }
}

int displayHistory(string argument)
{
	if (argument.compare(" ") == 0) // no argument -> print history
	{
		// create reverse iterator
		vector<vector<string>>::reverse_iterator rit = prevCommands.rbegin();
		int i = 0;
		while (rit != prevCommands.rend())
		{
			cout << i << ": "; // print out n
			for (string s: *rit)
			{
				cout << s << " ";
			}
			cout << endl;
			rit++;
			i++;
		}
	}
	else if (argument.compare("-c") == 0) // "-c" -> clear history
	{
		vector<string> historyClear = prevCommands.back();
		prevCommands.clear();
		prevCommands.push_back(historyClear);
	}
	else
	{
		cout << "Syntax error: Invalid parameter ''" << argument << "'" << endl;
		return -1;
	}

	// function finished = success
	return 0;
}

void byebye()
{
	// delete existing histFile
	remove(MYSH);

	// create new histFile
	histFile.open(MYSH, ios::in); // open and close file to ensure it exists
	histFile.close();
	histFile.open(MYSH, ios::out);

	/// copy contents of prevCommands into histFile
	for (vector<string> commands : prevCommands)
	{
		string fullCommand;
		for (string s : commands)
		{
			fullCommand.append(s);
			fullCommand.append(" ");
		}
		if (fullCommand.size () > 0)  // remove extra space
			fullCommand.resize (fullCommand.size () - 1);
		histFile << fullCommand << endl; // write command to history file
	}

	histFile.close(); // close the history file
	exit(0); // exit the shell
}

int replay(vector<string> arguments)
{
	// check if an argument was provided
	if (arguments.size() < 2)
	{
		cout << "Command 'replay' requires an integer argument" << endl;
		return -1;
	}

	// check valid argument is provided
	char * temp = (char *) arguments[1].c_str();
	if (!isdigit(temp[0])) // argument isn't an integer
	{
		cout << "Command 'replay' requires an integer argument" << endl;
		return -1;
	}
	else // argument is a valid integer
	{
		int i = stoi(temp, nullptr, 10);
		if (i > (prevCommands.size() - 1)) // check if number is in bounds
		{
			cout << "Command number is invalid" << endl;
			return -1;
		}
		else
		{
			vector<string> command = prevCommands.at(prevCommands.size() - i - 2); // get command
			runCommand(command); // call start to execute command
			return 0;
		}
	}

}

int startProcess(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() <= 1)
	{
		cout << "Command 'start' requires a valid process to run" << endl;
		return -1;
	}

	// convert vector<string> to char * []
	char * paraList[arguments.size()];
	for (size_t i = 1; i < arguments.size(); i++)
	{
		char * arg0 = (char *) arguments[i].c_str();
		paraList[i - 1] = arg0;
	}
	paraList[arguments.size() - 1] = NULL;

	// Forking a child
	pid_t pid = fork();

	if (pid < 0) // error occurred
	{
			cout << "Failed forking child..";
			return -1;
	}
	else if (pid == 0) //	Display an error message if the specified program cannot be found or cannot be executed.
	{
		if (execv(paraList[0], paraList) < 0) // check if command wasn't successful
			cout << "Failed to start process\n";
			exit(-1);
	}
	else
		waitpid(pid, NULL, 0); // waiting for child to terminate

	return 0;
}

int backgroundProcess(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() <= 1)
	{
		cout << "Command 'background' requires a valid process to run" << endl;
		return -1;
	}

	char * paraList[arguments.size()];
	for (size_t i = 1; i < arguments.size(); i++)
	{
		char * arg0 = (char *) arguments[i].c_str();
		paraList[i - 1] = arg0;
	}
	paraList[arguments.size() - 1] = NULL;

	// Forking a child
	pid_t pid = fork();

	if (pid < 0) { // error occurred
			cout << "Failed forking child..";
			kill(pid, SIGKILL);
			return -1;
	}
	else if (pid == 0) // child process
		if (execv(paraList[0], paraList) < 0) // check if command wasn't successful
		{
			cout << "Failed to start background process\n";
			return -1;
		}

	return pid;
}

int terminateProcess(vector<string> arguments)
{
	// check if an argument was provided
	if (arguments.size() < 2)
	{
		cout << "Command 'terminate' requires a valid PID" << endl;
		return -1;
	}

	// check valid argument is provided
	char * temp = (char *) arguments[1].c_str();
	if (!isdigit(temp[0])) // argument isn't an integer
	{
		cout << "Command 'terminate' requires a valid PID" << endl;
		return -1;
	}
	else // argument is a valid integer
	{
		int pid = stoi(temp);
		if (find(pidList.begin(), pidList.end(), pid) != pidList.end() || pid == 0) // send signal to kill
		{
			kill(pid, SIGKILL);
			pidList.erase(remove(pidList.begin(), pidList.end(), pid), pidList.end()); // remove pid from list
			cout << pid << " successfully terminated\n";;
			waitpid(pid, NULL, 0); // kill process
			return 0;
		}
		else // failed to kill process
		{
			cout << pid << " could not be terminated\n";;
			return 0;
		}
	}
}

int repeatProcess(vector<string> arguments)
{
	// check valid integer argument is provided
	int success = 0;
	char * temp = (char *) arguments[1].c_str();
	if (!isdigit(temp[0])) // argument isn't an integer
	{
		cout << "Command 'repeat' requires an integer argument" << endl;
		return -1;
	}
	else // argument is a valid integer
	{
		int occurrences = stoi(temp, nullptr, 10);

		// convert vector to char**
		char * paraList[arguments.size() - 2];
		for (size_t i = 2; i < arguments.size(); i++)
		{
			char * arg0 = (char *) arguments[i].c_str();
			paraList[i - 2] = arg0;
		}
		paraList[arguments.size() - 2] = NULL;

		for (int i = 0; i < occurrences; i++)
		{
			// Forking a child
			pid_t pid = fork();

			if (pid == -1) {
					printf("\nFailed forking child..");
					return -1;
			}
			else if (pid == 0) //	Display an error message if the specified program cannot be found or cannot be executed.
			{
				if (execv(paraList[0], paraList) < 0) // check if command wasn't successful
				{
					cout << "Could not execute command." << endl; // print error
					success = -1;
				}
			}
			else // Child was started successfully
			{
				// print PIDs
				if (success != -1)
				{
					if (i == 0)
						cout << "Child PID: " << pid << ", ";
					else if (i == (occurrences - 1))
						cout << pid << endl;
					else cout << pid << ", ";

					// add pid to list of running background processes
					pidList.push_back((int) pid);
				}
			}
		}

	}

	return success;
}

int terminateAllProcesses()
{
	if (pidList.size() == 0) // check if list is empty
		cout << "No processes to terminate\n";
	else
	{
		cout << "Terminating " << (pidList.size()) << " processes: ";
		for (int i = pidList.size() - 1; i >= 0; i--)
		{
			pid_t pid = pidList.back();
			pidList.pop_back();
			if (kill(pid, SIGKILL) == 0) // send signal to kill
			{
				// print PIDs
				if (i == 0)
					cout << pid << endl;
				else cout << pid << ", ";
			}
			else
			{
				cout << endl << pid << " could not be terminated, stopping terminations" << endl;
				return -1;
			}
		}
	}

	// function finished = success
	return 0;
}

void printChildren()
{
	if (pidList.size() == 0) // check if list is empty
		cout << "No processes running\n";
	else
		for (int pid : pidList)
			cout << pid << endl;
}
