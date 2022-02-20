// Name: Luke Ambray
// Course: EEL 4781
// Semester: Spring 2022
// NID: 4562758
// Homework 3: mysh

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

// definitions
const char * MYSH = "mysh.history";

// function prototypes
void tokenInput(string const &userInput, vector<string> &out);
void loadHistory();
void history(string argument);
void byebye();
void replay(size_t number);
void start(vector<string> arguments);

// global variables
fstream histFile;
vector<vector<string>> prevCommands;

int main()
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
		tokenInput(userInput, out);
		prevCommands.push_back(out);

		// compare command with those implemented
		if (out[0].compare("history") == 0)
		{
			if (out.size() > 1) history(out[1]);
			else history(" ");
		}

		else if (out[0].compare("byebye") == 0)
			byebye();
 	}



	// read in user input
	// tokenize user input
	// call function corresponding to user input
	// else throw error if function doesn't exist
	return 0;
}

void tokenInput(string const &userInput, vector<string> &out)
{
	// construct a stream from userInput
	stringstream ss(userInput);

	string token;
	while (getline(ss, token, ' '))
	{
		out.push_back(token);
	}

}

// read history file
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

// # history [-c]
//	Without the argument, it prints out the recently typed commands (with their arguments),
//	in reverse order, with numbers
// 	If the argument “-c” is passed, it clears the list of recently typed commands.
void history(string argument)
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
		cout << "Syntax error: Invalid parameter " << argument << "'" << endl;
	}

}

// # byebye
//	Terminates the mysh shell (and it should save the history file).
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
	exit(0);
}

// # replay number
//	Re-executes the command labeled with the given number in the history

// # start program [parameters]
//	The argument “program” is the program to execute. If the argument starts with a “/”
//	(such as /usr/bin/xterm, the shell should interpret it as a full path. Otherwise, the
//	program will be interpreted as a relative path starting from the current directory.
//	The program will be executed with the optional “parameters”. mysh should use fork() +
//	exec() to start the program with the corresponding parameters, and wait until the
//	program terminates (use the waitpid() call).
//	For instance
// 		start /usr/bin/xterm –bg green
//	would bring up a terminal with a green background. The prompt would not return until
//	the terminal is closed.
//	Display an error message if the specified program cannot be found or cannot be executed.

// # background program [parameters]
//	It is similar to the run command, but it immediately prints the PID of the program it started,
//	and returns the prompt.

// # terminate PID
//	Immediately terminate the program with the specific PID (presumably started using
//	background). Use the kill() function call to send a SIGKILL signal to the program. Display
//	success or failure.
