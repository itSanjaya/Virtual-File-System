#include<iostream>
#include<sstream>
#include<fstream>
#include<string>
#include<queue>
#include<stack>
#include<stdlib.h>
#include<exception>
#include<iomanip>
#include<ctime>

using namespace std;

class iNode;
class List;
//===================================================
class PathNotExist: public exception
{
	public:
		virtual const char* what() const throw()
		{
			return "No such path exists."; 
		}
};
//===================================================
class FileNotFound: public exception
{
	public:
		virtual const char* what() const throw()
		{
			return "No such folder/file exists.";
		}

};
//===================================================
class NameExists: public exception
{
	public:
		virtual const char* what() const throw()
		{
			return "This name already exists.";
		}

};
//===================================================
class BinEmpty: public exception
{
	public:
		virtual const char* what() const throw()
		{
			return "The bin is empty.";
		}

};
//===================================================
string tDay()       // function for creating date
{
	time_t ct = time(NULL);	
 	tm * now = localtime(&ct);
 	int year = 1900 + now->tm_year;
 	int month = 1 + now->tm_mon;
 	int day = now->tm_mday;
 	string date = to_string(day) + "-" + to_string(month) + "-" + to_string(year);
 	return date;
 	
}
//===================================================
//List class (doubly linked list) to store children
class List{
    private:
        iNode* head;
        iNode* tail;
    public:
    	List();
    	~List();
    	bool empty();
    	void addFront(iNode* tmp);
    	void removeFront();
        friend class Tree;
};

//===================================================
// Inode class/struct for files and folders
class iNode {
    private:
        bool type;						//type (File/Folder)
        int size;						//size of file/folder
        string date;					//file/folder created date
        string name;					//name of file/folder
        List* children;					//Doubly linked list for children
        iNode* parent = NULL;			//pointer for parent node
        iNode* next = NULL; 			//pointer to next node
        iNode* prev = NULL; 			//pointer to previous node
        bool isdeleted = false;			//to know if the inode is delted or not.
        friend class List;
        friend class Tree;

    public:
    	iNode() {} //empty constructor
        iNode(string filename, bool type, int size = 10): name(filename), type(type), size(size), date(tDay()) {if (!type) children = new List;}; //constructor for folder
        iNode(string filename, bool type, int size, string date): name(filename), type(type), size(size), date(date) {if (!type) children = new List;}; //constructor for files
};
//===================================================
class Tree
{
	private:
		iNode* rootlocation;			//pointer to root
		iNode* currentlocation;			//pointer to current
		iNode* previouslocation;		//pointer to previous
	public:     
        iNode* findRoot();										//return root
        void addRoot(int size, string date = tDay());			//adds root
        void addNode(string path, string size, string date);	//for inserting node
        string pwd();											//prints the path of current node
        void realpath(string name);								//prints the path of a given filename
        void ls();												//prints the children of the current node
        void ls(string s);										//prints the children in sorted way
        void isSort();											//ordering by descending file size, using bubble sort
        void change(iNode* tmp);								//to swap the nodes while doing bubble sort
        bool isthereAlready(iNode* tmp, string filename);		//checks if Fname already exists
        void mkdir(string folder);								//creates folder
        void touch(string name, int size);						//creates files
        bool cd(string s);										//changes directory
        void find(string filename, iNode* tmp);					//find folder/file
        bool isFile = false;									//checks if file is or not
        bool isPath(iNode* tmp);								//traces the path
        void mv(string filename, string foldername);			//move the files/folders
        void rm(string filename);								//remove the files/folders
        void size(string fname);								//returns size of folder/file
        void emptybin();										//empties the bin
        void showbin();											//shows the oldest node with its path
        void recover();											//reinstates the oldest node to its original position in tree
        void dumpData(iNode* tmp);								//dumping in a file
    	void exit();											//exits from the system by saving the file
        bool isValid(string filename, bool type);				//checks validity of Fname
        void sizeUpdate(iNode* tmp, int size, bool type);		//updates the size of the parents if inserted or removed
        void remove(iNode* tmp);								//remove all nodes
};
//===================================================
Tree* mytree = new Tree;										//creating object
queue<iNode*> bin;												//creating bin using queue
int MAXBIN = 10;												//maximum size of bin
int sbin = 0;	
string write;
//===================================================
List::List()													//constructor 
{
    head = new iNode;
    tail = new iNode;
    head->next = tail;
    tail->prev = head;
}
//===================================================			
List::~List()													// destructor to clean up all nodes
{
	while (!empty())
    	removeFront();
}
//===================================================
bool List::empty()												// is list empty?
{
	return head->next ==  tail;
}
//===================================================
void List::addFront(iNode* tmp)									// add to front of list
{
	tmp->next = head->next;
    tmp->prev = head;
    head->next->prev = tmp;
    head->next = tmp;
}
//===================================================
void List::removeFront()										// remove front item from list
{
	iNode* temp = head;
    head = temp->next;
    delete temp;
}
//===================================================
iNode* Tree::findRoot()											//method to return root pointer
{
    return rootlocation;
}
//===================================================
void Tree::addRoot(int size, string date) 						//method to insert root
{
    iNode* r = new iNode("", false, size, date);
    rootlocation = r;
    currentlocation = rootlocation;
    previouslocation = currentlocation;
}
//===================================================
void Tree::addNode(string path, string size, string date) 		//method to insert other nodes
{
    stringstream ss(path);
    string fl = "";
    getline(ss, fl, '/');

    while(getline(ss, fl, '/')) {								//separating the path into each file and we traverse into the folder if we find it
        iNode* ctr = currentlocation->children->head->next;
        while (ctr != currentlocation->children->tail) {
            if (ctr->name == fl) {
                currentlocation = ctr;
                break;
            }
            ctr = ctr->next;
        }
    }
    bool ifFile = fl.find('.') != string::npos;					// otherwise we create the file or folder and store it in the specified location
         
    iNode* tmp = new iNode(fl, ifFile, stoi(size), date);
    tmp->parent = currentlocation;
    currentlocation->children->addFront(tmp);

    currentlocation = rootlocation;
}
//===================================================
string Tree::pwd() 												// method to print the path
{
    string last;
    bool ean;
    iNode* tmp = currentlocation;
    stack<string> mystack;
    while (tmp != rootlocation) {
        mystack.push(tmp->name);
        tmp = tmp->parent;
    }
    
    last += "/";
    while (!mystack.empty()) {
        last += mystack.top();
        mystack.pop();
        if (!mystack.empty())
            last += "/";
    }
    return last;
}
//===================================================			// method to find the file and print its path
void Tree::realpath(string name) 
{
    iNode* ctr = currentlocation->children->head->next;
    iNode* tmp = currentlocation;
    bool fd = false;

    while (ctr != currentlocation->children->tail) {
        if (ctr->name != name) {
            ctr = ctr->next;
        } else {
            fd = true;
            currentlocation = ctr;
            break;
        }
    }

    if (fd) {
        cout << pwd() << endl;
        currentlocation = tmp;
    } 
    else {
        throw PathNotExist();
    }
}
//===================================================	
void Tree::ls()															//method to list children of current node
{
	if (!currentlocation->children->empty()){														
	    if (!currentlocation->type) 
	    {
	        iNode* ctr = currentlocation->children->head->next;
	        cout << "Type" << "\t" << "Name" << "\t" << "Size" << "\t" << "Date" << endl;

	        while (ctr->next != NULL) {
	            string p = ctr->type ? "File" : "Folder";
	            cout << p << "\t" << ctr->name << "\t" << ctr->size << "\t" << ctr->date << endl;
	            ctr = ctr->next;
	        }
	    } 
	    else {
	        cout << "This method cannt be used for files." << endl;
	    }
	}
	else{
		cout<<"There are no files/folders."<<endl;
	}
}
//===================================================
void Tree::ls(string s)													//method to print children in sorted way
{
	if (s == "sort"){
		mytree->isSort();
		mytree->ls();
	}
	else{
		mytree->ls();
	}
}
//===================================================
void Tree::isSort()														// method to sort the files using bubble sort 
{	
    if (!currentlocation->type) 
    {
        bool exchange = true;

        while (exchange == true) {
            iNode* ctr = currentlocation->children->head->next;
            exchange = false;

            while (ctr->next != currentlocation->children->tail && ctr->next != NULL) {
                if (ctr->size < ctr->next->size) {
                    mytree->change(ctr);
                    exchange = true;
                } 
                else{
                    ctr = ctr->next;
                }
            }
        }
    } 
    else{
        cout << "This method cannot be used for files." << endl;
    }
}
//===================================================
void Tree::change(iNode* tmp)											//method to swap to inodes in the children list while doing bubble sort
{
    iNode* stn = tmp->next;
    tmp->prev->next = stn;  
    tmp->next = stn->next;
    stn->prev = tmp->prev;
    stn->next = tmp;
    tmp->prev = stn;
}
//===================================================
bool Tree::isthereAlready(iNode* tmp, string filename)					// method to see if the name already exists
{
    iNode* ctr = tmp->children->head->next;
    bool fn = false;

    while (ctr != tmp->children->tail) {
        if (ctr->name != filename) {
            ctr = ctr->next;
        }
        else{
            fn = true;
            break;
        }
    }

    return fn;
}
//===================================================					// checking for the validity of file/folder name we enter
bool Tree::isValid(string filename, bool type) 
{
    string s = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    if (type == true) s = s+ ".";
    int sz = filename.length();
    bool b = true;

    for (int i = 0; i < sz; i++) {
        if (s.find(filename[i]) == string::npos) {
            b = false;
            break;
        }
    }
    return b;
}
//===================================================
void Tree::sizeUpdate(iNode* tmp, int size, bool type)					//method to update the size of parents if inserted or removed
{
    while (tmp != NULL) {
        if (type == true) {
            tmp->size += size;
        } 
        else {
            tmp->size -= size;
        }
        tmp = tmp->parent;
    }
}
//===================================================
void Tree::mkdir(string folder)											// method to create a folder
{
    if (!isthereAlready(currentlocation, folder)) {
        if (isValid(folder, false)) {
            iNode* nf = new iNode(folder, false);
            currentlocation->children->addFront(nf);
            nf->parent = currentlocation;
            sizeUpdate(currentlocation, 10, true);
            // previouslocation = currentlocation;
            // currentlocation = nf;
        }
        else{
            cout << "Sorry, this name cannot be used." << endl;
        }
    } else {
        throw NameExists();
    }
}
//===================================================
void Tree::touch(string filename, int size) 							// method to create a file
{
    if (!isthereAlready(currentlocation, filename)) {
        if (filename.find('.') != string::npos) {
            if (isValid(filename, true)) {
                iNode* fl = new iNode(filename, true, size);
                currentlocation->children->addFront(fl);
                fl->parent = currentlocation;
                sizeUpdate(currentlocation, size, true);
            } 
            else{
                cout << "Sorry, this name cannot be used." << endl;
            }
        } 
        else{
            cout << "Please use the extension(.) for the files." << endl;
        }
    } 
    else{
        throw NameExists();
    }
}
//===================================================
bool Tree::cd(string s) 												// method to change the directory
{
    if (s == "..") {													// changes current node to parent
        if (currentlocation != rootlocation) {
            previouslocation = currentlocation;
            currentlocation = currentlocation->parent; 
        }
    } 
    else if (s == "-") {												// changes to previous working directory
        if (!previouslocation->isdeleted || isPath(previouslocation)) {
            iNode* pr = previouslocation;
            previouslocation = currentlocation;
            currentlocation = pr;
        } 
        else {
            cout << "Previous location has been deleted" << endl;
        }
    } 
    else if (s == "") {													 //changes to the root
        previouslocation = currentlocation;
        currentlocation = rootlocation;
    }
     else {																 // checking if the given string is a path
        if (s.find('/') != string::npos && s[0] == '/') {
            stringstream ss(s);
            string fl;
            getline(ss, fl, '/');

            iNode* bg = currentlocation;
            bool are = false;

            currentlocation = rootlocation;
            while(getline(ss, fl, '/')) {
                are = mytree->cd(fl);									// recursive function to traverse to the file in the path
                if (are) {
                    currentlocation = bg;
                    break;
                }
            }
            if (!are) 
            	previouslocation = bg;
        } 
        else {														     // traverse to the entered file
            bool ean = true;
            iNode* ctr = currentlocation->children->head->next;
            bool dn = false;
        
            while (ctr != currentlocation->children->tail) {
                if (ctr->name == s) {
                    if (ctr->type) {
                        cout << "Cannot do cd for a file." << endl;
                    } else {
                        ean = false;
                        dn = true;
                        previouslocation = currentlocation;
                        currentlocation = ctr;
                    }
                    break;
                }
                ctr = ctr->next;
            }
            if (!dn) {
                cout << "The location cannot be found." << endl;
            }
            return ean;
        }
    }
}
//===================================================
//void Tree::find(string filename, iNode* tmp) 							// method to return the path of a file or folder
void Tree::find(string fname, iNode* ptr) {
    iNode* counter = ptr;

    if (counter->name == fname) {
        iNode* temp = currentlocation;
        currentlocation = counter;
        cout << pwd() << endl;
        currentlocation = temp;
        mytree->isFile = true;
    }
    if (!counter->type) {
        iNode* counterChild = counter->children->head->next;
        while (counterChild != counter->children->tail) {
            find(fname, counterChild);
            counterChild = counterChild->next;
        }
    }
}
//===================================================
bool Tree::isPath(iNode* tmp) 											//method to see if the path still exists
{
    bool isPath = true;
    
    iNode* ctr = tmp->parent;
    while (ctr != rootlocation) {
        if (ctr->isdeleted == true) {
            isPath = false;
            break;
        }
        ctr = ctr->parent;
    }

    return isPath;
}
//===================================================
void Tree::mv(string filename, string foldername)						// method to move a file to the specified folder
{
    iNode* ctr = currentlocation->children->head->next;
    iNode* fl;
    iNode* fr;
    bool ffl;
    bool ffr;
        
    while (ctr != currentlocation->children->tail) {
        if (ctr->name == filename) {
            fl = ctr;
            ffl = true;
        }
        if (ctr->name == foldername) {
            fr = ctr;
            ffr = true;
        } 
        ctr = ctr->next;
    }

    if (ffl && ffr) {
        fl->prev->next = fl->next;
        fl->next->prev = fl->prev;
        fl->parent = fr;
        fr->children->addFront(fl);
    } else {
        throw FileNotFound();
    }
}
//===================================================
void Tree::rm(string filename) 												// method to remove a file or folder and put it into the bin
{
    iNode* ctr = currentlocation->children->head->next;
    bool fnd = false;

    if (sbin >= MAXBIN){
    	cout<<"Bin is full."<<endl;
    }
    else{
	    while (ctr != currentlocation->children->tail){
		    if (ctr->name == filename) {
		        ctr->prev->next = ctr->next;
		        ctr->next->prev = ctr->prev;
		        ctr->isdeleted = true;
		        bin.push(ctr);
		        fnd = true;
		        sbin += 1;

	        	sizeUpdate(currentlocation, ctr->size, false);
	        	break;
	        }
	        ctr = ctr->next;
	    }
    	if (!fnd) {
        	throw FileNotFound();
    }	}
}
//===================================================
void Tree::size(string fname) 													// method to return the total size of the specified file or folder
{
    iNode* ctr = currentlocation->children->head; //->children->head->next;
    bool fd = false;
        
    while (ctr != currentlocation->children->tail) {
        if (ctr->name == fname) {
            cout << "Type" << "\t" << "Name" << "\t" << "Size" << endl;
            string what = ctr->type ? "File" : "Folder";
            cout << what << "\t" << ctr->name << "\t" << ctr->size << "\t" << endl;
            fd = true;
            break;
        }
        ctr = ctr->next;
    }

    if (!fd) {
        throw FileNotFound();
    }
}
//===================================================
void Tree::emptybin()															//empties the bin
{
	if (!bin.empty()){
		while(!bin.empty()){
			bin.pop();
		}
		sbin = 0;
		cout<<"Bin is emptied."<<endl;
	}
	else{
		cout<<"Bin is already empty."<<endl;
	}
}
//===================================================
void Tree::showbin() 															// shows the oldest inode in the bin
{
    if (!bin.empty()) {
        iNode* ptr = bin.front();
        cout << "Type" << "\t" << "Name" << "\t" << "Size" << "\t" << "Date" << endl;
        string what = ptr->type ? "File" : "Folder";
        cout << what << "\t" << ptr->name << "\t" << ptr->size << "\t" << ptr->date << endl;
        iNode* tmp = currentlocation;
        currentlocation = ptr;
        cout << pwd() << endl;
        currentlocation = tmp;
    } else {
        throw BinEmpty();
    }
}
//===================================================
void Tree::recover() 															// method to recover the oldest inode from the bin
{
    if (!bin.empty()) {
        iNode* ptr = bin.front();
        if (isPath(ptr)) {
            if (!isthereAlready(ptr->parent, ptr->name)) {
                ptr->isdeleted = false;
                ptr->parent->children->addFront(ptr);
                bin.pop();
                sizeUpdate(ptr->parent, ptr->size, true);
            } else {
                cout << "File already exists." << endl;
            }
        } else {
            throw PathNotExist();
        }
    } else {
        throw BinEmpty();
    }
}
//===================================================
void loadData() 															// method to see if the file exists and loading it
{
    string let;
    ifstream file;
    file.open("vfs.dat");

    if (!file) {
        string d = tDay();
        mytree->addRoot(0, d);
    } 
    else {
        bool ctr = true;
        while (getline(file, let)) {
            string p;
            string s;
            string dt;
            stringstream ss(let);
            getline(ss, p, ',');
            getline(ss, s, ',');
            getline(ss, dt);
            
            if (ctr) {
                mytree->addRoot(stoi(s), dt);
                ctr = false;
            } else {
                mytree->addNode(p, s, dt);
            }
        }
    }
    file.close();
}
//===================================================
void Tree::dumpData(iNode* tmp) 										//method to write into a file or create one if doesn't exist
{
    iNode* ctr = tmp;

    iNode* ptr = currentlocation;
    currentlocation = ctr;
    write += pwd() + "," + to_string(currentlocation->size) + "," + currentlocation->date + "\n";
    currentlocation = ptr;

    if (!ctr->type) {
        iNode* ctrc = ctr->children->head->next;
        while (ctrc != ctr->children->tail) {
            dumpData(ctrc);
            ctrc = ctrc->next;
        }
    }
}
//===================================================
void Tree::exit()														//exiting while saving the contents into the file
{
	//cout << "Shutting Down..." << endl;
    mytree->dumpData(mytree->findRoot());
    ofstream file("vfs.dat");
    file << write;
    file.close();
    //delete (&bin);
    //mytree->remove(mytree->findRoot());
}
//===================================================
void Tree::remove(iNode* tmp) 											// method to remove all the linked lists 
{
    iNode* ctr = tmp;

    if (!ctr->type) {
        List* ptr = ctr->children;
        iNode* ctrc = ctr->children->head->next;
        while (ctrc != ctr->children->tail) {
            remove(ctrc);
            ctrc = ctrc->next;
        }
        delete ptr;
    }
}
//===================================================
void listofCommands()													//function to represent all commands	
{
	cout<<"List of available Commands:"<<endl
		<<"help					 : Prints the following menu of commands"<<endl
		<<"pwd					 : Prints the path of the current node"<<endl
		<<"realpath <filename>			 : Prints the full path of a file in the current directory"<<endl
		<<"ls 					 : Prints children of the current node"<<endl
		<<"mkdir <foldername>			 : Creates a folder under the current folder"<<endl
		<<"touch <filename> <size>			 : Creates a file under the current inode location with the specified filename, size, and current datetime"<<endl
		<<"cd <foldername, filename, .. , - , /path>: Change current inode to specified folder/parent/root"<<endl
		<<"find <foldername or filename>		 : Returns the path of the file (or the folder) if it exists"<<endl
		<<"mv					 : Moves a file located under the current inode location, to the specified folder path"<<endl
		<<"rm <foldername or filename>		 : Removes the specified folder or file"<<endl
		<<"size <foldername or filename>		 : Returns the total size of the folder or the file"<<endl
		<<"emptybin				 : Empties the bin"<<endl
		<<"showbin                  		 : Shows the oldest inode of the bin, including its path"<<endl
		<<"recover					 : Reinstates the oldest inode back from the bin to its original position in the tree"<<endl
		<<"exit 					 : Exit the Program"<<endl;
}
//===================================================
int main()
{
	loadData();
	listofCommands();

	//string command;
	string user_input;
	string command;
	string parameter1;
	string parameter2;
	do
	{
		cout<<">";
		getline(cin,user_input);

		// parse userinput into command and parameter(s)
		stringstream sstr(user_input);
		getline(sstr,command,' ');
		getline(sstr,parameter1,' ');
		getline(sstr,parameter2);

		try
		{
		if(command=="help") 					listofCommands();
		else if(command == "pwd")				cout<<mytree->pwd()<<endl;
		else if(command=="realpath" )			{mytree->realpath(parameter1);}
		else if(command=="ls"){					{mytree->ls();}
		// 	string s;
		// 	getline(sstr, s, ' ');
		// 	if (s == "sort"){
		// 		mytree->isSort();
		// 		mytree->ls();
		// 	}
		// 	else{		
		// 		mytree->ls();
		// 	}
		}
		else if (command == "ls sort")			{mytree->ls(parameter1);}
		else if(command=="mkdir")				{mytree->mkdir(parameter1);}			
		else if(command=="touch")				{mytree->touch(parameter1, stoi(parameter2));}
		else if(command=="cd")					{mytree->cd(parameter1);}
		else if(command=="find"){		
			mytree->find(parameter1, mytree->findRoot());
			if(!mytree->isFile){
				throw FileNotFound();
			}
		}
		else if(command== "mv")					{mytree->mv(parameter1, parameter2);}
		else if(command== "rm")					{mytree->rm(parameter1);}
		else if(command== "size")				{mytree->size(parameter1);}
		else if(command== "emptybin")			{mytree->emptybin();}
		else if(command== "showbin")			{mytree->showbin();}
		else if(command== "recover")			{mytree->recover();}
		else if(command== "exit"){
			mytree->exit(); 
			break;
		}
		else
			cout<<"Invalid Command !!"<<endl;
		}
		catch(exception &e)
		{
		cout<<"Exception: "<<e.what()<<endl;
		}
	}
	while(command!="exit");
	return 0;
}