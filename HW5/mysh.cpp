// Name: Luke Ambray
// Course: COP 4600
// Semester: Spring 2022
// NID: 4562758
// Homework 5: mysh

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>

using namespace std;

// definitions
const char * MYSH = "mysh.history";
const int SUCCESS = 0;
const int FAIL = -1;

// convenient functions
void loadHistory(void);
void tokenInput(string const &userInput, vector<string> &out);
void runCommand(vector<string> line);

// HW3 required commands
int displayHistory(string argument);
void byebye(void);
int replay(vector<string> arguments);
int startProcess(vector<string> arguments);
int backgroundProcess(vector<string> arguments);
int terminateProcess(vector<string> arguments);

// HW5 required commands
int checkExist(vector<string> arguments);
int createFile(vector<string> arguments);
int copyFileToAndFrom(vector<string> arguments);

// HW3 extra credit
int repeatProcess(vector<string> arguments);
void printChildren();
int terminateAllProcesses();

// HW5 extra credit
int copyDirToAndFrom(vector<string> arguments);
int copyDirectory(char * source, char * destination);

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
		if (replay(out) != SUCCESS)
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
		if (repeatProcess(out) != SUCCESS)
			cout << "Unable to repeat command\n";
	}
	else if (out[0].compare("terminateall") == 0)
	{
		if (terminateAllProcesses() != 0)
			cout << "Failed to terminate all processes\n";
	}
	else if (out[0].compare("start") == 0)
	{
		if (startProcess(out) != SUCCESS)
			cout << "Failed to start process\n";
	}
	else if (out[0].compare("dwelt") == 0)
	{
		if (checkExist(out) != SUCCESS)
			cout << "Dwelt not\n";
	}
	else if (out[0].compare("maik") == 0)
	{
		if (createFile(out) != SUCCESS)
			cout << "Failed to create file (already exists)\n";
	}
	else if (out[0].compare("coppy") == 0)
	{
		if (copyFileToAndFrom(out) != SUCCESS)
			cout << "Failed to copy file\n";
	}
	else if (out[0].compare("coppyabode") == 0)
	{
		if (copyDirToAndFrom(out) != SUCCESS)
			cout << "Failed to copy directory\n";
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
		return FAIL;
	}

	// function finished = success
	return SUCCESS;
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
		return FAIL;
	}

	// check valid argument is provided
	char * temp = (char *) arguments[1].c_str();
	if (!isdigit(temp[0])) // argument isn't an integer
	{
		cout << "Command 'replay' requires an integer argument" << endl;
		return FAIL;
	}
	else // argument is a valid integer
	{
		int i = stoi(temp, nullptr, 10);
		if (i > (prevCommands.size() - 1)) // check if number is in bounds
		{
			cout << "Command number is invalid" << endl;
			return FAIL;
		}
		else
		{
			vector<string> command = prevCommands.at(prevCommands.size() - i - 2); // get command
			runCommand(command); // call start to execute command
			return SUCCESS;
		}
	}

}

int startProcess(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() <= 1)
	{
		cout << "Command 'start' requires a valid process to run" << endl;
		return FAIL;
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
			return FAIL;
	}
	else if (pid == 0) //	Display an error message if the specified program cannot be found or cannot be executed.
	{
		if (execv(paraList[0], paraList) < 0) // check if command wasn't successful
			cout << "Failed to start process\n";
			exit(-1);
	}
	else
		waitpid(pid, NULL, 0); // waiting for child to terminate

	return SUCCESS;
}

int backgroundProcess(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() <= 1)
	{
		cout << "Command 'background' requires a valid process to run" << endl;
		return FAIL;
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
			return FAIL;
	}
	else if (pid == 0) // child process
		if (execv(paraList[0], paraList) < 0) // check if command wasn't successful
		{
			cout << "Failed to start background process\n";
			return FAIL;
		}

	return pid;
}

int terminateProcess(vector<string> arguments)
{
	// check if an argument was provided
	if (arguments.size() < 2)
	{
		cout << "Command 'terminate' requires a valid PID" << endl;
		return FAIL;
	}

	// check valid argument is provided
	char * temp = (char *) arguments[1].c_str();
	if (!isdigit(temp[0])) // argument isn't an integer
	{
		cout << "Command 'terminate' requires a valid PID" << endl;
		return FAIL;
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
			return SUCCESS;
		}
		else // failed to kill process
		{
			cout << pid << " could not be terminated\n";;
			return SUCCESS;
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
		return FAIL;
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
					return FAIL;
			}
			else if (pid == 0) //	Display an error message if the specified program cannot be found or cannot be executed.
			{
				if (execv(paraList[0], paraList) < 0) // check if command wasn't successful
				{
					cout << "Could not execute command." << endl; // print error
					success = FAIL;
				}
			}
			else // Child was started successfully
			{
				// print PIDs
				if (success != FAIL)
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
				return FAIL;
			}
		}
	}

	// function finished = success
	return SUCCESS;
}

void printChildren()
{
	if (pidList.size() == 0) // check if list is empty
		cout << "No processes running\n";
	else
		for (int pid : pidList)
			cout << pid << endl;
}

int checkExist(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() != 2)
	{
		cout << "Command 'dwelt' requires a valid file/directory to run" << endl;
		return FAIL;
	}

	struct stat info;
	char * input = (char *) arguments[1].c_str(); // convert input to char *

	if (stat(input, &info) != 0) // input doesn't exist
		return FAIL;
	else if (info.st_mode & S_IFDIR) // check if input is a directory
		cout << "Abode is" << endl;
	else if (S_ISREG(info.st_mode)) // check if input is a regular file
		cout << "Dwelt indeed" << endl;
	else return FAIL; // default statement

	return SUCCESS; // input was a file/directory
}

int createFile(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() != 2)
	{
		cout << "Command 'maik' requires a valid file to run" << endl;
		return FAIL;
	}

	struct stat info;
	char * input = (char *) arguments[1].c_str(); // convert input to char *

	if (stat(input, &info) != 0) // input file doesn't exist -> continue
	{
		fstream newFile;

		// create new file
		newFile.open(input, ios::in); // open and close file to ensure it exists
		newFile.close();
		newFile.open(input, ios::out);

		string newLine = "Draft";
		newFile << newLine << endl; // write "Draft" to new file
		newFile.close(); // close the new file
		return SUCCESS; // successfully created file
	}
	else return FAIL; // input file exists -> abort
}

int copyFileToAndFrom(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() != 3)
	{
		cout << "Command 'coppy' requires 2 valid files to run" << endl;
		return FAIL;
	}

	// convert inputs to char *
	char * source = (char *) arguments[1].c_str();
	char * destination = (char *) arguments[2].c_str();
	int exists = 0;

	struct stat info;

	// check if source file exists
	if (stat(source, &info) != 0) // input doesn't exist
	{
		cout << "Source file doesn't exist" << endl;
		return FAIL;
	}
	else if (S_ISREG(info.st_mode)) // check if input is a regular file
		exists++;

	// check if destination directory exists
	size_t found = arguments[2].find_last_of("/");
	if (found == string::npos) // directories are the same
		exists++;
	else // directories are different
	{
		char * path = (char *) arguments[2].substr(0, found).c_str();
		if (stat(path, &info) != 0) // destination directory doesn't exist
		{
			cout << "Destination path (" << path << ") doesn't exist" << endl;
			return FAIL;
		}
		else if (info.st_mode & S_IFDIR) // check if input is a directory
			exists++;
	}

	// check if destination file exists
	if (stat(destination, &info) != 0) // destination file doesn't exist
		exists++;
	else
	{
		cout << "Destination file exists" << endl;
		return FAIL; // destination file exists -> abort
	}

	// make sure all tests passed
	if (exists != 3)
	{
		cout << "Didn't pass all tests" << endl;
		return FAIL;
	}

	// copy file over
	//ifstream sourceFile(source, std::ios::binary);
	//ofstream destinationFile(destination, std::ios::binary);
	//destinationFile << sourceFile.rdbuf();

	// copy file over
	ifstream sourceFile(source);
	ofstream destinationFile(destination);
	string line;

	if (sourceFile && destinationFile)
	{
		while (getline(sourceFile, line))
			destinationFile << line << "\n";
	}
	else return FAIL; // file copy failed

	// Close files
	sourceFile.close();
	destinationFile.close();

	return SUCCESS; // file copy finished
}

int copyDirToAndFrom(vector<string> arguments)
{
	//check if proper arguments are supplied
	if (arguments.size() != 3)
	{
		cout << "Command 'coppyabode' requires 2 valid paths to run" << endl;
		return FAIL;
	}

	// convert inputs to char *
	char * source = (char *) arguments[1].c_str();
	char * destination = (char *) arguments[2].c_str();
	int exists = 0;
	struct stat info;

	// Check if source directory exists
	if (stat(source, &info) != 0) // input doesn't exist
	{
		cout << "Source not found" << endl;
		return FAIL;
	}
	else if (info.st_mode & S_IFDIR) // check if input is a directory
		exists++;
	else return FAIL; // default statement

	// Check if destination directory exists
	if (stat(destination, &info) != 0) // input doesn't exist
	{
		cout << "Destination not found" << endl;
		return FAIL;
	}
	else if (info.st_mode & S_IFDIR) // check if input is a directory
		exists++;
	else return FAIL; // default statement

	/* Create new subdirectory */
	// Get the name of source directory
	size_t found = arguments[1].find_last_of("/");
	string newDirectoryName;
	if (found == string::npos)  // source directory is current directory
	{
		newDirectoryName = arguments[1];
	}
	else // source directory is not current directory
	{
		newDirectoryName = arguments[1].substr(found, arguments[1].length());
	}

	// Concatenate new subdirectory to destination directory path
	string newFullPath = arguments[2] + "/" + newDirectoryName;
	char * charNewPath = (char *) newFullPath.c_str();

	mkdir(charNewPath, 0777); // create directory

	return copyDirectory(source, charNewPath); // call helper function
}

// helper function to recursively open directories
int copyDirectory(char * source, char * destination)
{
	DIR * dir;
	struct dirent * en;

	dir = opendir(source); //open directory
	if (dir) {
		while ((en = readdir(dir)) != NULL)
		{
			char * fileDirName = en->d_name;
			if (fileDirName[0] != '.' && fileDirName[0] != '.' ) // make sure file/directory is valid
			{
				/* check if file or subdirectory */
				struct stat info;

				// reconstruct item source filepath
				string item(source);
				item.append("/");
				item.append(en->d_name);
				fileDirName = (char *) item.c_str(); // convert item to char *

				if (stat(fileDirName, &info) != 0) // input doesn't exist
				{
					return FAIL; // shouldn't be possible
				}
				else if (S_ISREG(info.st_mode)) // check if item is a regular file
				{
					// construct destination filesp
					string dst(destination);
					dst.append("/");
					dst.append(en->d_name);

					// copy file over
					// can reuse function from coppy
					// but don't need to validate paths because we forsure created them
					ifstream sourceFile(item);
					ofstream destinationFile(dst);
					string line;

					if (sourceFile && destinationFile)
					{
						while (getline(sourceFile, line))
							destinationFile << line << "\n";
					}
					else
					{
						cout << "File copy failed" << endl;
						return FAIL; // file copy failed
					}
					// Close files
					sourceFile.close();
					destinationFile.close();
				}
				else if (info.st_mode & S_IFDIR) // check if item is a directory
				{
					// make new new directory
					string dst(destination);
					dst.append("/");
					dst.append(en->d_name);
					char * charDest = (char *) dst.c_str();
					mkdir(charDest, 0777); // create directory

					string src(source);
					src.append("/");
					src.append(en->d_name);
					char * charSource = (char *) src.c_str();

					copyDirectory(charSource, charDest); // make recursive call
				}
			}
		}
   }
		closedir(dir); //close directory
		return SUCCESS;
}
