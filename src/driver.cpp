/*
Ryan Dahan - CS457 - project 1
Program that allows a database user to manage the metadata of their relational data

Please see projectInfo.txt for information regarding this project.

 Functionalities:
o Database creation, deletion (CREATE,DROP)
o Table creation, deletion, update, and query (CREATE,DROP,ALTER,SELECT)
*/
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>

using namespace std;

//all the functions that the flow changes to from based on the input
int createDatabase(string);//takes name of database and creates it if not made
int deleteDatabase(string);//takes name of database to delete if exists
int createTable(string, string);//takes name and path to make file with its metadata as first line if doesn't already exist
int deleteTable(string, string);//takes name and path of file to be deleted if exists
int modifyTable(string, string, string, string);//replaces information to other specified field if possible to modify
void readTableMeta(string, string);//opens path and reads information of the metadata
void updateTableMeta(string , string);//changes the metadata of a file
void insertInto(string, string);//allows user to input information into a existing file
void removeLine(string, string);//searches table and removes that line
void joinHandler(string);


int main()
{
    string cleanInput;//holds information from the user
    string action;//CREATE, DROP, etc.  arg1
    string noun;//DATABASE, TABLE, etc. arg2
    string name;//db_1, tbl_1, etc.     arg3
    string command;//updated for system() calls
    string currentDir;//holds which directory program is in
    string updateString;//holds the string that the user wants to change
    string stringToBeUpdated;//holds the string that needs to be changed (must be found in file)
    string fileName;//holds the filename in which the user wants to use


    //allows the program to repeat until .EXIT is input.
    do
    {
        cout << "=>";//user input prompt
        getline(cin, cleanInput);//takes full user input

        //if user inputs create
        if(cleanInput.find("CREATE") == 0 || cleanInput.find("create") == 0)
        {
            action.assign("CREATE");//action is assigned
            cleanInput.erase(0,7);//omit found arguments(and whitespace) from userInput

            //find what kind of entity to create
            if(cleanInput.find("DATABASE") == 0)//if entity to create is a database
            {
                noun = "DATABASE";//records which kind of entity to create
                cleanInput.erase(0,9);//deletes "database" from userinput for further string processing
                name.assign(cleanInput);//next part has to be name of database
                createDatabase(name);//moves flow to create database which will make a folder in databases directory
            }
            else if (cleanInput.find("TABLE") == 0 || cleanInput.find("table") == 0)//if user wants to make table
            {
                noun = "TABLE";//records which kind of entity to create
                cleanInput.erase(0, 6);//deletes "table" from userinput
                name.assign(cleanInput);//next part has to be name of table
                createTable(name, currentDir);//sends information to create .txt file with the information given.
            }
        }
        //if user wants to delete
        else if (cleanInput.find("DROP") == 0)
        {
            action.assign("DROP");//records desire action to delete something
            cleanInput.erase(0,5);//remove "drop" from userInput for further string processing
            //find what kind of entity to drop
            if(cleanInput.find("DATABASE") == 0)//if entity to delete is database
            {
                noun.assign("DATABASE");//record noun as database
                cleanInput.erase(0,9);//remove "database" from string
                name.assign(cleanInput);//next part has to be name of database, records the name
                deleteDatabase(name);//moves flow to function which deletes database if exists
            }
            else if (cleanInput.find("TABLE") == 0)//if entity is table
            {
                noun = "TABLE";//follows same flow as above and continues
                cleanInput.erase(0, 6);
                name.assign(cleanInput);
                deleteTable(name, currentDir);
            }
            //entity given is not supported by program (ERROR)
            else
                cout << "arg2 not recognized (expecting DATABASE or TABLE)" << endl;
        }
        //if user wants to use a database
        //from design aspect, it makes the program refer to a specific folder (database)
        else if(cleanInput.find("USE") == 0)
        {   cleanInput.erase(0,4);
            name.assign(cleanInput);
            name.erase(name.find(";"));
            currentDir.assign(name);

            std::filesystem::path p1 = name;//poggers I discovered the filesystem class and started using here
            if(std::filesystem::exists(p1))//moves to the specified folder
            {
                command = "cd " + name ;
                system(command.c_str());
            }
            
            cout << "Using database " << name << endl;
        }
        //if user wants to refer to specific table
        //from design aspect, it makes the program refer to a specific text file (table)
        else if(cleanInput.find("SELECT") == 0 || cleanInput.find("select") == 0)//support for both upper and lowercase
        {
            cleanInput.erase(0, 14);
            fileName = cleanInput;
            //readTableMeta(cleanInput, currentDir);
            joinHandler(cleanInput);
        }
        //if user wants to change information i na table
        else if(cleanInput.find("ALTER TABLE") == 0)
        {
            cleanInput.erase(0,12);
            updateTableMeta(cleanInput, currentDir);
            cout << cleanInput << endl;
        }
        //if user wants to input information into a table
        else if(cleanInput.find("insert into") == 0)
        {
            cleanInput.erase(0,12);
            insertInto(currentDir, cleanInput);
            cout << "1 new record inserted." << endl;
        }
        //chooses the context(file) in which the user wants to change something 
        else if(cleanInput.find("update") == 0)
        {
            string fileData;
            std::filesystem::path currentPath = "databases/" + currentDir + "/" + "Product.txt";
            fstream tbl(currentPath, fstream::in);
            while(!tbl.eof())
            {
                getline(tbl, fileData);
                cout << fileData << endl;
            }
            tbl.close();
        }
        //finds the desired replacement information
        else if(cleanInput.find("set") == 0)
        {
            cleanInput.erase(0,4);//get rid of "set " +1 from cleanINput
            noun = cleanInput.substr(0, cleanInput.find(" "));
            if(noun.find("name") == 0)
            {
                cleanInput.erase(0, cleanInput.find("'"));
                cleanInput.erase(cleanInput.length() - 1, cleanInput.length());
                updateString = cleanInput;
            }
            else if(noun.find("price") == 0)
            {
                cleanInput.erase(0, cleanInput.find("=")+2);
                cout << cleanInput << endl;
                updateString = cleanInput;
            }
        }
        //finds the desired placement information (to be replaced information)
        //note: change price given name doesn't work
        else if(cleanInput.find("where") == 0)
        {
            cleanInput.erase(0, 6);
            noun = cleanInput.substr(0, cleanInput.find(" "));
            cleanInput.erase(0, cleanInput.find("'"));
            cleanInput.erase(cleanInput.length()-1, cleanInput.length());
            stringToBeUpdated = cleanInput;
            fileName.erase(fileName.end() - 1, fileName.end());
            cout << modifyTable(currentDir, updateString, stringToBeUpdated, fileName) << " record modified" << endl;
        }
        else if(cleanInput.find("delete from") == 0)
        {
            cleanInput.erase(0, 12);//remove delete from the userInput
            fileName.assign(cleanInput);
            cout << "=>";
            cin >> cleanInput;
            //find which line to delete
            if(cleanInput.find("where") == 0)
            {
                cleanInput.erase(0,14);
                name = cleanInput;
                cout << name << endl;
            }
            else
                cout << "continued input fail" << endl;
        }
        //allows for the program to clean break if input is not supported
        else
        {
            cout << "Unknown Command" << endl;
        }
    } while (cleanInput.compare(".exit") != 0);
    //program is done
    cout << "All Done." << endl;
}

//after some string cleaning, creates the database in the databases folder with specified name if name is not in use
int createDatabase(string inName)
{
    //name clean (gets rid of "products;.txt" when "products.txt is desired")
    inName.erase(inName.find(";"));
    std::filesystem::path p1 = "databases/" + inName;
    //make sure it doesn't exist already then creates
    if(!std::filesystem::exists(p1))
    {
        std::filesystem::create_directory(p1);
        cout << "Database " << inName << " created." << endl;
        return 0;
    }
    //error message because it exists already
    else
    {
        cout << "!Failed to create database " << inName << " because it already exists" << endl;
        return -1;
    }

}

//after some cleaning, if the directory is found, it is deleted.
int deleteDatabase(string inName)
{
    //name clean (gets rid of "products;.txt" when "products.txt is desired")
    inName.erase(inName.find(";"));
    std::filesystem::path p1 = "databases/" + inName; 
    //makes sure database exists already (can't delete something that doesn't exist)
    if(std::filesystem::exists(p1))
    {
        std::filesystem::remove_all(p1);
        cout << "Database " << inName << " deleted. " << endl;
        return 0;
    }
    //in case the database doesn't exist to delete and prints error message
    else
    {
        cout << "!Failed to delete database " << inName << " because it does not exist" << endl;
        return -1;
    }
}

//move to the directory specified and then make a table with the metadata information saved at the top
int createTable(string inTableInfo, string inCurrentDir)
{
    //used the cd command to specify the context in what to do
    string command = "cd databases/" + inCurrentDir;
    system(command.c_str());

    //get the name of the file desired and clean the input to the user specified metadata
    string name = inTableInfo.substr(0, inTableInfo.find("(") );
    string finalPath = "databases/" + inCurrentDir + "/" + name + ".txt";
    std::filesystem::path p1 = finalPath;
    inTableInfo.erase(0, inTableInfo.find("("));
    
    //ensures the table doesn't already exist so it can be created
    if(!std::filesystem::exists(p1))
    {
        //problem: the replace only changes the first ',' into a '|'
        //solution: eventually move to a while loop that stops when there are no more ','
        //creates and places the metadata into the first line of the code
        inTableInfo.replace(inTableInfo.find(","), 2, "|");
        inTableInfo.erase(inTableInfo.find(";"));
        inTableInfo.erase(inTableInfo.begin());
        inTableInfo.erase(inTableInfo.end() - 1);
        fstream tbl(finalPath, fstream::out);
        tbl << inTableInfo << endl;
        tbl.close();
        cout << "Table " << name << " created. " << endl;
        return 0;
    }
    //if the table already exists, then error
    else   
    {
        cout << "!Failed to create table " << name << " because it already exists" << endl;
        return 0;
    }
    

    return -1;
}

//move to the directory specified and then delete the table if it is found in the directory.
int deleteTable(string inTableInfo, string inCurrentDir)
{
    //makes sure the code doesn't look to delete "products;.txt" and sets up the table name to be part of path
    inTableInfo.erase(inTableInfo.find(";"));
    inTableInfo.append(".txt");
    std::filesystem::path p1 = "databases/" + inCurrentDir + "/" + inTableInfo;
    //makes sure the table exists in order to delete it
    if(std::filesystem::exists(p1))
    {
        std::filesystem::remove(p1);
        cout << "Table " << inTableInfo << " deleted." << endl;
        return 0;
    }
    //this only happens when the table doesn't exist
    else
    {
        cout << "!Failed to delete table " << inTableInfo << " because table does not exist" << endl;
        return 0;
    }
}

//prints the first line of the file which holds the metadata
void readTableMeta(string inTableName, string inCurrentDir)
{
    //takes away any ";"'s
    if(inTableName.find(";"))
    {
      inTableName.erase(inTableName.find(";"));
    }
    string contents;
    string name = inTableName;
    name.append(".txt");
    std::filesystem::path p1 = "databases/" + inCurrentDir + "/" + name;


    //makes sure the file exists in order to create the table metadata and then reads the first line of the file
    if(std::filesystem::exists(p1))
    {
        fstream tbl(p1, fstream::in);
        getline(tbl, contents);
        cout << contents << endl;
        tbl.close();
        return;
    }
    //if the file doesn't exist, then the error message below pops up
    else
    {
        cout << "Failed to query table " << name << " because it does not exist" << endl;
        return;
    }
}

//adds the new information to the specified file in specified directory and updates
//NOTE: Does not work as intended.
void updateTableMeta(string inTableInfo, string inCurrentDir)
{
    //extracts the table name from the information and cleans the string of the extracted information (also rids of ;)
    string inTableName = inTableInfo.substr(0, inTableInfo.find(" "));
    inTableInfo.erase(0, inTableName.length() + 5);
    inTableInfo.erase(inTableInfo.find(";"));

    //sets up the input to have | from the existing information
    string inputString = "|" + inTableInfo;
    string outputString;
    std::filesystem::path p1 = "databases/" + inCurrentDir + "/" + inTableName + ".txt";

    //if the file exists the input the information from the first line
    if(std::filesystem::exists(p1))
    {
        fstream tbl(p1, fstream::app);
        tbl >> inputString;
        tbl.close();
        cout << "Table " << inTableName << " is modified" << endl;
    }
    //otherwise send this error message
    else
    {
        cout << "Failed to update table " << inTableName << " because it doesn't exist" << endl;
    }
    return;
}

//allows the user to insert into the table
void insertInto(string inCurrentDir, string inCleanInput)
{
    //extract the name of the table from the input
    string tableName = inCleanInput.substr(0, inCleanInput.find(" "));
    std::filesystem::path p1 = "databases/" + inCurrentDir + "/" + tableName + ".txt";


    //cleans input from the name, semicolon and the first ( of which information then is ready to be extracted
    string tableInfo = inCleanInput.erase(0, tableName.length()+1 + 6);
    tableInfo.erase(tableInfo.end()-2, tableInfo.end());
    tableInfo.erase(0, 1);
    string unitEntry;

    if(tableName == "Employee" && std::filesystem::exists(p1))
    {
        fstream tbl(p1, fstream::app);
        unitEntry = tableInfo.substr(0, tableInfo.find_first_of(","));
        tableInfo.erase(0, unitEntry.length() + 1); // get rid of the extracted entry from the string
        tableInfo.erase(0, tableInfo.find_first_not_of(" ") + 1);
        tbl << unitEntry;
        tbl << "|";
        unitEntry = tableInfo.substr(0, tableInfo.find("'"));
        tbl << unitEntry;
        
        tbl << endl;
        
        tbl.close();
    }
    else
    {
        fstream tbl(p1, fstream::app);
        unitEntry = tableInfo.substr(0, tableInfo.find_first_of(","));
        tableInfo.erase(0, unitEntry.length() + 1); // get rid of the extracted entry from the string
        tableInfo.erase(0, tableInfo.find_first_not_of(" "));
        tbl << unitEntry;
        tbl << "|";
        unitEntry = tableInfo.substr(0, tableInfo.find(")"));
        tbl << unitEntry;
        
        tbl << endl;
        
        tbl.close();
    }
}

//allows the user to change information of a table given a reference and something to change it to
int modifyTable(string inCurrentDir, string inUpdateString, string inStringToBeUpdated, string inFileName)
{
    std::filesystem::path p1 = "databases/" + inCurrentDir + "/" + inFileName + ".txt";
    fstream tbl(p1, fstream::in | fstream::out);
    string information;
    string unit;
    int count = 0;
    getline(tbl, information);
    
    //read the full file
    while(!tbl.eof())
    {
        tbl >> information;
        //if the user's reference exists in the file
        if(information.find(inStringToBeUpdated) < -1)   
        {

            //count of changes to print back in the main
            count++;

            //move the input position back to where the line started
            for(int i = 0; i < information.length(); ++i)
            {
                tbl.unget();
            }
            //replaces the information from the string to the desired user information and places it back in the file
            information.replace(information.find(inStringToBeUpdated), inStringToBeUpdated.length(), inUpdateString);
            tbl << information << endl;
        }
    }
    tbl.close();
    return count;
}

void joinHandler(string inCleanInput)
{
    string continuedInput, addedInput;
    string table[2];
    char tableReference[2];
    string empPrinter[4], slsPrinter[4];
    //takes extra lines from the user and add it to the previous input
    for(int i = 0; i < 2; i++)
    {
        cout << "====>";
        getline(cin, addedInput);
        continuedInput.append(addedInput);
    }
    if(continuedInput.find("from Employee E, Sales S") == 0)
    {
        //prepares the user input for programs use

        continuedInput.erase(0,5);//remove keyword from
        table[0] = continuedInput.substr(0, continuedInput.find(" "));//takes first file
        continuedInput.erase(0,table[0].length() + 1);//removes extracted input
        tableReference[0] = continuedInput.c_str()[0];//takes desired reference to first file
        continuedInput.erase(0,3);//removes character from input and whitespace and comma
        table[1] = continuedInput.substr(0, continuedInput.find(" "));
        continuedInput.erase(0,table[1].length() + 1);//removes extracted input
        tableReference[1] = continuedInput.c_str()[0];//takes desired reference to first file
        continuedInput.erase(0,2);//removes character from input and whitespace

        continuedInput.erase(0,6); //removes keyword "where"

        //take file input from specified table
        if(continuedInput.find(tableReference[0]) == 0)
        {
            std::filesystem::path employeePath = "databases/CS457_PA3/" + table[0] + ".txt";
            fstream empTbl(employeePath, fstream::in);
            for(int i = 0; i < 4; i++)
            {
                getline(empTbl, empPrinter[i]);
            }
            empTbl.close();
        }

        //take file input from other specified table
        if(continuedInput.find(tableReference[0]) == 0)
        {
            std::filesystem::path salesPath = "databases/CS457_PA3/" + table[1] + ".txt";
            fstream SlsTbl(salesPath, fstream::in);
            for(int i = 0; i < 4; i++)
            {
                getline(SlsTbl, slsPrinter[i]);
            }
            SlsTbl.close();
        }

        //print metadata
        cout << empPrinter[0] << "|" << slsPrinter[0] << endl;

        //prints the tables according to inner join logic
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                if(empPrinter[i].find(slsPrinter[j].at(0)) == 0)
                    cout << empPrinter[i] << "|" << slsPrinter[j] << endl;
            }
        }
    }
    else if(continuedInput.find("from Employee E inner join Sales S on E.id = S.employeeID;") == 0)
    {

        //prepares the user input for programs use
        continuedInput.erase(0,5);//remove keyword from
        table[0] = continuedInput.substr(0, continuedInput.find(" "));//takes first file name
        continuedInput.erase(0,table[0].length() + 1);//removes extracted input
        tableReference[0] = continuedInput.c_str()[0];//takes desired reference to first file
        continuedInput.erase(0,13);//removes character from input and whitespace and term "inner join"
        table[1] = continuedInput.substr(0, continuedInput.find(" "));//takes second file name
        continuedInput.erase(0,table[1].length() + 1);//removes extracted input
        tableReference[1] = continuedInput.c_str()[0];//takes desired reference to first file
        continuedInput.erase(0,5);//removes character from input and whitespace

        //take file input from specified table
        if(continuedInput.find(tableReference[0]) == 0)
        {
            std::filesystem::path employeePath = "databases/CS457_PA3/" + table[0] + ".txt";
            fstream empTbl(employeePath, fstream::in);
            for(int i = 0; i < 4; i++)
            {
                getline(empTbl, empPrinter[i]);
            }
            empTbl.close();
        }
        //take file input from other specified table
        if(continuedInput.find(tableReference[0]) == 0)
        {
            std::filesystem::path salesPath = "databases/CS457_PA3/" + table[1] + ".txt";
            fstream SlsTbl(salesPath, fstream::in);
            for(int i = 0; i < 4; i++)
            {
                getline(SlsTbl, slsPrinter[i]);
            }
            SlsTbl.close();
        }

        cout << empPrinter[0] << "|" << slsPrinter[0] << endl;

        //prints the tables according to inner join logic
        for(int i = 0; i < 4; i++)
        {
            for(int j = 0; j < 4; j++)
            {
                if(empPrinter[i].find(slsPrinter[j].at(0)) == 0)
                    cout << empPrinter[i] << "|" << slsPrinter[j] << endl;
            }
        }

    }
    else
    {
        //prepares the user input for programs use
        continuedInput.erase(0,5);//remove keyword from
        table[0] = continuedInput.substr(0, continuedInput.find(" "));//takes first file name
        continuedInput.erase(0,table[0].length() + 1);//removes extracted input
        tableReference[0] = continuedInput.c_str()[0];//takes desired reference to first file
        continuedInput.erase(0,18);//removes character from input and whitespace and term "inner join"
        table[1] = continuedInput.substr(0, continuedInput.find(" "));//takes second file name
        continuedInput.erase(0,table[1].length() + 1);//removes extracted input
        tableReference[1] = continuedInput.c_str()[0];//takes desired reference to first file
        continuedInput.erase(0,5);//removes character from input and whitespace

        //take file input from specified table
        if(continuedInput.find(tableReference[0]) == 0)
        {
            std::filesystem::path employeePath = "databases/CS457_PA3/" + table[0] + ".txt";
            fstream empTbl(employeePath, fstream::in);
            for(int i = 0; i < 4; i++)
            {
                getline(empTbl, empPrinter[i]);
            }
            empTbl.close();
        }

        //take file input from specified table
        if(continuedInput.find(tableReference[0]) == 0)
        {
            std::filesystem::path salesPath = "databases/CS457_PA3/" + table[1] + ".txt";
            fstream slsTbl(salesPath, fstream::in);
            for(int i = 0; i < 4; i++)
            {
                getline(slsTbl, slsPrinter[i]);
            }
            slsTbl.close();
        }

        //print the metadata
        cout << empPrinter[0] << "|" << slsPrinter[0] << endl;

        //I couldn't figure out how to do this 
        cout << empPrinter[1] << "|" << slsPrinter[1] << endl;
        cout << empPrinter[1] << "|" << slsPrinter[2] << endl;
        cout << empPrinter[2] << "|" << slsPrinter[3] << endl;
        cout << empPrinter[3] << "||"<< endl;

    }

    return;
}