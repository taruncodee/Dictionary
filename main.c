#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

//structre used to store login information - username(string) and password(string)
struct logInfo{
	char user[12];
	char pass[12];
};

//structure used to store a record of a word - id(int), word(string), meaning of word(string),
//no. of synonyms(int), synonmyms(array of strings), no. of antonyms(int), antonyms(array of strings)
struct record{
	int id;
	char word[20];
	char mean[145];
	int synNo;
	char syn[7][20];
	int antNo; 
	char ant[7][20];
};

//GLOBAL VARIABLES
//mode used to keep track of what mode user is operating the dictionary app in
//if operating mode is admin, mode is 1, currUser stores info about current user, it stores
//username and password of currently logged in user
//if user is operating in regular mode, mode is 0
int mode = 0;
//used to stored currently entered login info by user
//also used to greet admin on various screens if info is correct
struct logInfo currUser = {0};

//all the function declarations, used in this project
//there are 2 functions for each screen
//1 displays static things regarding screen, like header, guide(input directions) etc
//2 handles user interactions and any dynamic output

void gotoxy(short col, short row);

void start();
void welcome();

void my_exit(FILE **f, FILE **g);

void readyWordFile(FILE **f);
void readyLoginFile(FILE **f);

void standardHeader();

int adminCheck(FILE **f, FILE **g);
void admLoginDisp();

void adminHome(FILE **f, FILE **G);
void adminHomeDisp();

int addWord(FILE **f);
void addWordDisp();

void browseDisp();
void browse(FILE **f);

void search(FILE **f);
void searchDisp();

void removeWordDisp();
void removeWord(FILE **f);

void modifyWordDisp();
void modifyWord(FILE **f);

void regHomeDisp();
void regHome(FILE **f, FILE **g);

char getChoiceYN();
struct record getRecord();
void putRecord(struct record w);
void clearSubScreen(int a);
int searchSubPart(char ch, FILE **f);


//main function, where project execution starts
int main(){
	//call to start() is where actual execution start of dictionary
	start();
	return 0;
}

//this is the first screen, user has to choose between 2 operating modes
//regular mode - allows only read operation, browse and search, can't do any modifications
void start(){
	char ch1;	//used to get choice of user regarding which mode they want to use
	int ch2;	//used to store return value of return value of adminCheck(), if it is one, means user entered correct login info
	FILE *wor, *log;	//file pointers used - wor - points to file which has records of words, log - points to file which has login info, usernames & passwords
	
	readyWordFile(&wor);	//function used to setup wor file(records of words) for use
	readyLoginFile(&log);	//function used to setup log file(records of usernames & passwords) for use
	
	welcome();	//function used to display static stuff of this screen like any heading, guide(tips what key does what)
	
	while(1){
		fflush(stdin);
		gotoxy(43, 10);
		ch1 = getch();
		gotoxy(20, 23);
		printf("                              "); //used to clear error message if happened earlier
		switch(ch1){
			case '1':
				regHomeDisp();	//display static aspect of regular mode home screen
				regHome(&wor, &log);	//regular mode home screen function to handle user interactions & dynamic logic
				welcome();
				break;
			case '2':
				admLoginDisp();	//logic screen static stuff presentation
				//adminCheck() checks the input information against usernames and password in log file,
				//if it matches, 1 is returned, meaning user can operate in admin mode
				//if not matches 0 is returned, and user is taken back to start() screen, where mode is selected
				//return value is stored in ch2 and mode
				//if ch2 is 1, user is taken into admin mode
				//if mode is 1, the header will contain 'username' of current user on all screens, and greet them
				//if mode is 0, then simple 'regular user' is written in header
				mode = ch2 = adminCheck(&wor, &log);
				if(ch2 == 1){
					adminHomeDisp();
					adminHome(&wor, &log);
					mode=0;
				}
				welcome();
				break;
			case '0':
				my_exit(&wor, &log);	//exits the app, closes all the files
				return;
			default:
				gotoxy(20, 23);	//takes cursor to specific position
				printf("*Enter a valid choice");	//shows error on screen if any invalid input in given by user
				break;
		}
	}
}

//this function is used to take the cursor to specific position in console window
//takes 2 arguments, column and row, where cursor is to be placed
void gotoxy(short col, short row){
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD pos = {col, row};
	SetConsoleCursorPosition(h, pos);
}

//used to setup the wor(records of words) file
//uses double pointer to file, so the the original file pointer is used
//rather than new local file pointer(argument) which will get destroyed on function return
void readyWordFile(FILE **f){
	*f = NULL;
	*f = fopen("word.dat","rb+");
	//if opening in rb+ mode returns null, means file doesn't not exist or any other error
	//so we try to open file in wb+ mode, which will create file even if it doesn't exist already
	if(*f == NULL){
		*f = fopen("word.dat", "wb+");
		//if after opening in wb+ mode still returns null means, there is some error
		//and project is stopped, as without wor file , we can't operate
		if(*f == NULL){
			system("cls");
			gotoxy(25, 12);
			printf("CAN'T OPEN FILE. EXITING NOW.");
			gotoxy(0, 14);
			exit(1);
		}
		//if opening in wb+ mode doesn't return null, we close the file, and open it in rb+ mode
		//although wb+ mode will still work, i prefer to open it in rb+ mode, as most times there will always be file
		//and project will run in rb+ mode
		fclose(*f);
		*f = fopen("word.dat","rb+");
	}
}

//used to setup the log(records of usernames and passwords) file
//uses double pointer to file, so the the original file pointer is used
//rather than new local file pointer(argument) which will get destroyed on function return
void readyLoginFile(FILE **f){
	struct logInfo defLog = {"admin", "admin"};
	
	*f = NULL;
	*f = fopen("login.dat","rb+");
	//if opening in rb+ mode returns null, means file doesn't not exist or any other error
	//so we try to open file in wb+ mode, which will create file even if it doesn't exist already
	if(*f == NULL){
		*f = fopen("login.dat", "wb+");
		//if after opening in wb+ mode still returns null means, there is some error
		//and project is stopped, as without wor file , we can't operate
		if(*f == NULL){
			system("cls");
			gotoxy(25, 12);
			printf("CAN'T OPEN FILE. EXITING NOW.");
			gotoxy(0, 14);
			exit(1);
		}
		//if opening in wb+ mode doesn't return null, we enter the default login info using fwrite
		//default username = admin
		//default password = admin
		//then we close the file, and open it in rb+ mode
		//although wb+ mode will still work, i prefer to open it in rb+ mode, as most times there will always be file
		//and project will run in rb+ mode
		fwrite(&defLog, sizeof(defLog), 1, *f);
		fclose(*f);
		*f = fopen("login.dat", "rb+");
	}
}

//used to display static portion of start(), which is first screen
//where user has to select between 2 modes of operation
//regular mode and admin mode
//this function prints header, and gives guide at the bottom, which is input directions
void welcome(){
	int i;
	
	system("cls");
	for(i=0; i<79; i++)
		printf("-");
	gotoxy(27, 1);
	for(i=0; i<25; i++){
		gotoxy(27+i, 1);
		printf("*");
		gotoxy(27+i, 7);
		printf("*");
	}
	for(i=1; i<=5; i++){
		gotoxy(27, 1+i);
		printf("*");
		gotoxy(51, 1+i);
		printf("*");
	}
	gotoxy(30, 3);
	printf("PERSONAL DICTIONARY");
	gotoxy(36, 5);
	printf("WELCOME");
	gotoxy(0, 8);
	for(i=0; i<79; i++)
		printf("-");
	
	gotoxy(30, 10);
	printf("Select Mode:");
	gotoxy(30, 12);
	printf("1. Regular");
	gotoxy(30, 13);
	printf("2. Admin");
	
	gotoxy(0, 24);
	printf("Guide: 0-Exit");
}

//used to display static portion of adminCheck(), which is the screen, where user authentication takes place
//where user has to enter username and password
//this function prints header(using standardHeader() function), and gives guide at the bottom, which is input directions,
// and other basic static output
//to help user understand what is happening
void admLoginDisp(){
	standardHeader();
	
	gotoxy(30, 8);
	printf("Enter Username and Password:");
	gotoxy(30, 10);
	printf("Username : ");
	gotoxy(30, 11);
	printf("Password : ");
	
	gotoxy(0, 24);
	printf("Guide: 0-Exit| 1-Start Screen| 2-Switch Fields| Space-Proceed");
	gotoxy(41, 10);
}

//used to display header for various screens, in the project
//makes uses of global variable mode, to check which operating mode is currently active
//based on mode, either greets an admin user, or just type 'regular user'
//gotoxy function is used to format the outputs on the screen
void standardHeader(){
	int i;
	
	system("cls");
	for(i=0; i<79; i++)
		printf("-");
	for(i=0; i<25; i++){
		gotoxy(27+i, 1);
		printf("*");
		gotoxy(27+i, 5);
		printf("*");
	}
	for(i=1; i<=3; i++){
		gotoxy(27, 1+i);
		printf("*");
		gotoxy(51, 1+i);
		printf("*");
	}
	gotoxy(30, 3);
	printf("PERSONAL DICTIONARY");
	
	gotoxy(0, 6);
	for(i=0; i<79; i++)
		printf("-");
	
	for(i=0; i<=4; i++){
		gotoxy(16, 1+i);
		printf("*");
	}
	
	//mode checking, based on which either admin is greeted,
	//or just 'regular mode' is printed in header
	if(mode==0){
		gotoxy(3, 2);
		printf("Regular");
		gotoxy(3,3);
		printf("User");
	}
	else if(mode==1){
		gotoxy(3, 2);
		printf("Welcome!");
		gotoxy(3,3);
		printf("%s", currUser.user);
	}
}

//used to close program and close any opened files
//files are first checked if they have been opened, if they are
//then they are closed and program is closed
void my_exit(FILE **f, FILE **g){
	int i;
	
	if(*f != NULL)
		fclose(*f);
	if(*g != NULL)
		fclose(*g);
	system("cls");
	
	gotoxy(20, 1);
	for(i = 0; i<40; i++){
		gotoxy(20+i, 1);
		printf("*");
		gotoxy(20+i, 5);
		printf("*");
	}
	
	for(i=1; i<4; i++){
		gotoxy(20, 1+i);
		printf("*");
		gotoxy(59, 1+i);
		printf("*");
	}
	
	gotoxy(24, 3);
	printf("All Files Closed. Now Exiting");
	gotoxy(0, 7);
	exit(0);
}

//function to handle user interactions, and handle dynamic aspect of user-login screen
int adminCheck(FILE **f, FILE **g){
	struct logInfo logDet={0};
	char user[12], pass[12], ch;
	int u, p;
	int flg =0;
	int i=0;	//used to keep track of which input field is currently active, username or password
	
	u=p=0;
	while(1){
		fflush(stdin);	//used to clear the input stream buffer, which may interfere with getch
		ch = getch();	//used to get a char input, based on which different actions are performed
		if(ch == '0')	//ch=0, project is closed, my_exit() is called
			my_exit(f, g);
		if(ch == '1')	//ch=1, function is returned, which takes user back to start() screen, where mode selection is
			return 0;
		//ch=2, the input variable in which input is currently going is changed
		//transitions i from 0 to 1 or 1 to 0
		//if i=0, char inputs go to username field
		//if i=1, char inputs go to password field
		if(ch == '2'){
			
			gotoxy(20, 23);
			printf("                         ");
			gotoxy(30, 13);
			printf("                     ");
			
			//transitioning i, hence input fields
			i++;
			i = i%2;
			//changing position of cursor based on value of i
			if(i == 0)
				gotoxy(41+u, 10);
			if(i == 1)
				gotoxy(41+p, 11);
			continue;
		}
		
		//if ch=' ' i.e. space, currently entered value of username and password are matched up against
		//those in log file, if any of those matches, 1 is returned
		//else printed on screen, incorrect input
		if(ch == ' '){
			
			//used to clear any previous error messages displayed
			gotoxy(20, 23);
			printf("                         ");
			gotoxy(30, 13);
			printf("                     ");
			
			//null characters appeneded to values of user[] and pass[] to make them strings, rather than just array of char
			user[u]='\0';
			pass[p]='\0';
			//putting value into global variable currUser
			strcpy(currUser.user, user);
			strcpy(currUser.pass, pass);
			
			//matching values of currUser with various entries of usernames and passwords in log file
			fseek(*g, 0, SEEK_SET);
			while(fread(&logDet, sizeof(logDet), 1, *g) == 1){
				if(strcmp(currUser.user, logDet.user)==0 && strcmp(currUser.pass, logDet.pass)==0){
					flg = 1;	//used to keep track, if entered info matches with any in log file
					break;
				}
			}
			if(flg){
				gotoxy(30, 13);
				printf("LOGIN SUCCESSFUL!");
				gotoxy(30, 14);
				printf("Press any key to continue...");
				getch();
				return 1;
			}
			else{
				gotoxy(30, 13);
				printf("Wrong Password!");
				continue;
			}
		}
		
		//if ch is any alphabet, of any case, then that value is appended to the currently active field
		if((ch>64 && ch<91) || (ch>96 && ch<123)){
			
			//used to clear any previous error messages displayed
			gotoxy(20, 23);
			printf("                           ");
			gotoxy(30, 13);
			printf("                     ");
			
			if(i==0){
				if(u == 11){
					gotoxy(20, 23);
					printf("*Max limit reached.");	//tell user than maximum allowed characters have been entered
					continue;
				}
				gotoxy(41+u, 10);
				putch(ch);
				user[u] = ch;
				u++;
				continue;
			}
			if(i==1){
				if(p == 11){
					gotoxy(20, 23);
					printf("*Max limit reached.");	//tell user than maximum allowed characters have been entered
					continue;
				}
				gotoxy(41+p, 11);
				putch(ch);
				pass[p] = ch;
				p++;
				continue;
			}
		}
		
		//if ch=8 which is for backspace, in the currently active field, character added last is removed, and the size variables are all 
		//adjusted accordingly
		if(ch == 8){
			//used to clear any previous error messages displayed
			gotoxy(20, 23);
			printf("                         ");
			gotoxy(30, 13);
			printf("                     ");
			
			//placing the cursor in correct input field, using value of variable i, which keeps track of which field is active
			if((u==0 && i==0) || (p==0 && i==1)){
				gotoxy(41, 10+i);
				continue;
			}	
			
			//size variable decremented, space is printed on the screen to show character deletion
			//then cursor position adjustment
			if(i==0){
				u--;
				gotoxy(41+u, 10);
				putch(' ');
				gotoxy(41+u, 10);
				continue;
			}
			if(i==1){
				p--;
				gotoxy(41+p, 11);
				putch(' ');
				gotoxy(41+p, 11);
				continue;
			}
		}
		gotoxy(20, 23);
		printf("*Enter a valid choice");	//error message if user enters wrong input
	}
}

//used to display static portion of adminHome(), which is Home screen, when user successfully logins as admin
//user gets a lot of options to choose from, both reading and modification of records is allowed
//this function prints header(using standardHeader() function), and gives guide at the bottom, which is input directions,
// and menu, options which user has
void adminHomeDisp(){
	standardHeader();
	
	gotoxy(30, 8);
	printf("Choose Operation: ");
	gotoxy(30, 10);
	printf("A - Browse Dictionary");
	gotoxy(30, 11);
	printf("B - Add a new Word");
	gotoxy(30, 12);
	printf("C - Modify a Word");
	gotoxy(30, 13);
	printf("D - Delete a Word");
	gotoxy(30, 14);
	printf("E - Search a Word");
	
	gotoxy(0, 24);
	printf("Guide: 0-Exit| 1-Start Screen");
	gotoxy(47, 8);
}

//used to display static portion of addWord(), which is the screen, where addition of new record takes place
//this function prints header(using standardHeader() function)
// and other basic static output
//to help user understand what is happening
void addWordDisp(){
	standardHeader();
	
	gotoxy(30, 8);
	printf("Adding a new Word: ");
}

//used to enter a new record(word) into the dictionary(file)
//makes use of getRecord() function, which returns a struct record variable, in which 
//user is asked to input information regarding new word to be added
int addWord(FILE **f){
	struct record w = {0};
	char ch;
	
	gotoxy(0, 10);
	w = getRecord(); //get new record's details, and stores them in struct record variable
	
	printf("\nAdd the record? Confirm (y/n) ");
	ch = getChoiceYN();	//final confirmation before adding new record
	//if users choses y or Y, means record is to be added
	//file pointer is taken to the end of file, and new record is added there
	if(ch == 'y' || ch=='Y'){
		fseek(*f, 0, SEEK_END);	
		//ftell() used to get total size of file, then divide by size of struct record,
		//to get the number of records currently in file
		//this is used to assign a 'id' to the new word, which is just a number different from rest of records
		//'id' used during modification and deletion
		w.id = ((ftell(*f)+1)/sizeof(w))+1;
		fwrite(&w, sizeof(w), 1, *f);
		return 1;
	}
	else{
		return 0;
	}
}

//basic function to get a char input from user
//along with error-control, which means, user can only choose from pre-selected options
//if user chooses anything else, then they are asked to input again
char getChoiceYN(){
	char ch;
	while(1){
		fflush(stdin);
		ch = getche();
		if(ch=='y' || ch=='Y' || ch=='n' || ch=='N')
			return ch;
		printf("\nEnter valid input. Allowed = y,Y,n,N - ");
	}
}

//used to create a new struct record variable, which is returned at the end
//this functions is used in addition and modification operation
struct record getRecord(){
	struct record w = {0};
	char temp[145], ch;
	
	while(1){
		printf("Word : ");
		fflush(stdin);
		gets(temp);
		//if user enters a string more than size of variable they are asked to enter again
		//until input string is of valid length
		if(strlen(temp) > 19){
			printf("***Maximum word length allowed = 19. Enter word again***\n");
		}
		else{
			strcpy(w.word, temp);
			break;
		}
	}
	
	while(1){
		printf("Meaning : ");
		fflush(stdin);
		gets(temp);
		//if user enters a string more than size of variable they are asked to enter again
		//until input string is of valid length
		if(strlen(temp) > 144){
			printf("\n***Maximum word length allowed = 144. Enter meaning again***\n");
		}
		else{
			strcpy(w.mean, temp);
			break;
		}
	}
	
	//whenever user chooses to enter a synonym, synNo is incremented
	//it is used during read operation and also to keep check that user doesn't enter more than max limit allowed of synonyms
	w.synNo = 0;
	printf("Do you want to add a Synonym? (y/n) ");
	ch = getChoiceYN();
	
	while(ch == 'y' || ch == 'Y'){
		//checking if number of synonyms already entered are at max limit allowed
		if(w.synNo == 7){
			printf("***Maximum number of Synonyms entered***\n");
			break;
		}
		
		printf("\nSynonym : ");
		fflush(stdin);
		gets(temp);
		//if user enters a string more than size of variable they are asked to enter again
		//until input string is of valid length
		if(strlen(temp) > 19){
			printf("\n***Maximum synonym length allowed = 19. Enter synonym again***\n");
			continue;
		}
		else{
			strcpy(w.syn[w.synNo], temp);
			w.synNo++;
		}
		
		printf("Do you want to add another Synonym? (y/n) ");
		ch = getChoiceYN();
	}
	
	//whenever user chooses to enter a antonym, antNo is incremented
	//it is used during read operation and also to keep check that user doesn't enter more than max limit allowed of antonyms
	w.antNo = 0;
	printf("\nDo you want to add an Antonym? (y/n) ");
	ch = getChoiceYN();
	
	while(ch == 'y' || ch == 'Y'){
		//checking if number of antonyms already entered are at max limit allowed
		if(w.antNo == 7){
			printf("***Maximum number of Antonyms entered***\n");
			break;
		}
		
		printf("\nAntonym : ");
		fflush(stdin);
		gets(temp);
		//if user enters a string more than size of variable they are asked to enter again
		//until input string is of valid length
		if(strlen(temp) > 19){
			printf("\n***Maximum antonym length allowed = 19. Enter antonym again***\n");
			continue;
		}
		else{
			strcpy(w.ant[w.antNo], temp);
			w.antNo++;
		}
		
		printf("Do you want to add another Antonym? (y/n) ");
		ch = getChoiceYN();
	}
	
	return w;
}


//used to display static portion of 'browse' screen, which is where user can browse through dictionary, two words at a time on screen
//this function prints header(using standardHeader() function), and guide (input help - which keys do what)
// and other basic static output
//to help user understand what is happening
void browseDisp(){
	standardHeader();
	
	gotoxy(30, 8);
	printf("Browsing Words: ");
	
	gotoxy(0, 24);
	printf("Guide: 1-Menu| 7 - Prev Words| 8 - Next Words");
}

//used to display static portion of 'search' screen, which is where user can search for a word in dictionary
//this function prints header(using standardHeader() function), and guide (input help - which keys do what)
// and other basic static output
//to help user understand what is happening
void searchDisp(){
	standardHeader();
	
	gotoxy(30, 8);
	printf("Search for a Word!");
	
	gotoxy(30, 10);
	printf("A - By Word");
	gotoxy(30, 11);
	printf("B - By a Synonym");
	gotoxy(30, 12);
	printf("C - By an Antonym");
	
	gotoxy(0, 24);
	printf("Guide: 1-Menu");
}

//function handles user interationcs on browse screen, clears screen when users moves back/forward
//uses putRecord(), to display a struct record variable
void browse(FILE **f){
	char ch;
	int j, offset, flg=0;
	struct record w = {0};
	
	fseek(*f, 0, SEEK_SET);
	while(1){
		gotoxy(0, 10);
		//loop condition allows only for at most two records to be displayed at a time
		for(j=0; j<2; j++){
			
			if(getc(*f) == EOF){
				break;
			}
			fseek(*f, -1, SEEK_CUR);
			fread(&w, sizeof(w), 1, *f);
			putRecord(w);
			
			flg++;
			flg %= 2;
		}
		
		//getting user input, for further actions to be taken, error control applied, any input other than allowed
		//will cause error message to be displayed on screen
		while(1){
			fflush(stdin);
			ch = getch();
			gotoxy(0,23);
			printf("                                                                    ");
			if(ch=='1' || ch=='7' || ch=='8')
				break;
			gotoxy(0,23);
			printf("***Enter any of the valid inputs only, given in guide at bottom.***");	//error message
		}
		
		//if input is 8, then next two records are shown on screen if they exist
		//pressing 7, causes previous 2 records to be displayed if they exist
		//if 1 is pressed, we are returned from the function and taken to the home screen of either regular or admin mode, whichever is active
		if(ch == '8'){
			if(getc(*f) != EOF){
				fseek(*f, -1, SEEK_CUR);
				clearSubScreen(23);
			}
		}
		if(ch == '7'){
			if(ftell(*f) <= 2*sizeof(w)){
				fseek(*f, 0, SEEK_SET);
			}
			else{
				if(flg == 1){
					offset = 3*sizeof(w);
				}
				if(flg == 0){
					offset = 4*sizeof(w);
				}
				flg = 0;
				fseek(*f, -offset, SEEK_CUR);
				clearSubScreen(23);
			}
		}
		if(ch == '1'){
			fseek(*f, 0, SEEK_SET);
			return;
		}
	}
}

//function to display a struct record variable on the screen,
//takes struct record variable to be shown as argument
void putRecord(struct record w){
	int i;
	
	printf("Word No.: %d | ", w.id);
	printf("Word: %s\n", w.word);
	printf("Meaning: %s\n", w.mean);
	printf("Synonyms are: ");
	for(i=0; i<w.synNo; i++){
		if(i == w.synNo-1){
			printf("%s",w.syn[i]);
			continue;
		}
		printf("%s, ", w.syn[i]);
	}
	printf("\n");
	printf("Antonyms are: ");
	for(i=0; i<w.antNo; i++){
		if(i == w.antNo-1){
			printf("%s",w.ant[i]);
			continue;
		}
		printf("%s, ", w.ant[i]);
	}
	printf("\n\n");
}

//function to search for a record in the file, a 'word' in 'dictionary'
//searching can be done using 'word' itself
//or using an synonym or an antonym
void search(FILE **f){
	char ch;
	
	searchDisp();
	while(1){
		//error control - user can't move formward unless valid input is given
		while(1){
			fflush(stdin);
			ch = getch();
			gotoxy(0,23);
			printf("                                                                    ");
			if(ch == '1' || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C')
				break;
			gotoxy(0,23);
			printf("***Enter any of the valid inputs only, given in guide at bottom.***");	//error message, in case of invalid input
		}
		
		fseek(*f, 0, SEEK_SET);
		switch(ch){
			case '1':
				return;	//takes user back to 'home' screen- which can be 'regular' or 'admin' based on mode
			case 'a':
			case 'A':
				//search by word name
				searchSubPart(ch, f);
				break;
			case 'b':
			case 'B':
				//search by synonym
				searchSubPart(ch, f);
				break;
			case 'c':
			case 'C':
				//seach by antonym
				searchSubPart(ch, f);
				break;
		}
		searchDisp();	//display basic layout of search screen
	}
}

//used ot clear a certain portion of screen, used in browse, search, modify, delete to clear specific portion of screen
//which can be later used to display different information
void clearSubScreen(int a){
	int k, j;
	for(k=10; k<=a; k++){
		gotoxy(0, k);
		for(j=0; j<80; j++)
			printf(" ");
	}
}

//searching functionality broken into another function, so that it can be reused in modify and delete operation
int searchSubPart(char ch, FILE **f){
	char string[30];	//in which string to searched is stored
	//flg is used to check if word matching criteria is found in current iteration of search, if yes, then it is displayed on screen
	//flg2 is used to keep track if at least 1 matching record is found or not, in either case appropriate message is shown
	int flg, flg2, k;
	struct record w = {0};
	
	clearSubScreen(24);	//clearing certain portion of the screen to dispaly search results
	gotoxy(0, 10);
	
	//based on choice of user, user will be asked to search by word, synonym or antonym
	if(ch == 'a' || ch == 'A')
		printf("Enter word to be searched: ");
	else if(ch == 'b' || ch == 'B')
		printf("Enter synonym of the word to be searched: ");
	else if(ch == 'c' || ch == 'C')
		printf("Enter antonym of the word to be searched: ");
	
	fflush(stdin);
	gets(string);
	flg2=0;
	fseek(*f, 0, SEEK_SET);
	while(fread(&w, sizeof(w), 1, *f) == 1){
		flg = 0;
		
			//search by word - input string compared against 'word' member of struct record
			if(ch=='a' || ch=='A'){
				if(strcmp(string, w.word) == 0){
					flg = 1;
					if(flg2 != 1){
						gotoxy(34, 9);
						printf("***Found***\n\n\n\n");
						flg2 = 1;
					}
				}
			}
			else if(ch=='b' || ch=='B'){
				//search by synonym - input string compared against each of 'syn' array member of struct record
				for(k=0; k<w.synNo; k++){
					if(strcmp(string, w.syn[k]) == 0){
						flg = 1;
						if(flg2 != 1){
							gotoxy(34, 9);
							printf("***Found***\n\n\n\n");
							flg2 = 1;
						}
						break;
					}
				}
			}
			else if(ch=='c' || ch=='C'){
				//search by antonym - input string compared against each of 'ant' array member of struct record
				for(k=0; k<w.antNo; k++){
					if(strcmp(string, w.ant[k]) == 0){
						flg = 1;
						if(flg2 != 1){
							gotoxy(34, 9);
							printf("***Found***\n\n\n\n");
							flg2 = 1;
						}
						break;
					}
				}
			}
		
		//if word found in current scan/iteration through file, it is displayed, else not
		if(flg == 1){
			putRecord(w);
		}
		
	}
	
	//if at least 1 match found
	if(flg2 == 1){
		printf("                                Press any key to continue");
		fflush(stdin);
		getch();
	}
	else if(flg == 0){
		//if no match found, message is display, and function returns, to search menu screen
		gotoxy(15, 9);
		printf("***Not Found, Search again with different input.***");
		gotoxy(30, 23);
		printf("Press any key to continue");
		fflush(stdin);
		getch();
	}
	return flg2;
}

//admin home screen -  all the operations that are allowed to admin user are interactable through this screen
//error message is shown if user enter an invalid input
//user can choose through - browse, add word, modify, delete, search, back to main user mode selection screen, and exit out of application
void adminHome(FILE **f, FILE **g){
	char ch;
	int flg;
	
	while(1){
		fflush(stdin);
		ch = getch();
		
		gotoxy(30,23);
		printf("                          ");
		
		switch(ch){
			case 'a':
			case 'A':
				//browse()
				browseDisp();
				browse(f);
				adminHomeDisp();
				break;
			case 'b':
			case 'B':
				//add()
				addWordDisp();
				flg = addWord(f);
				adminHomeDisp();
				gotoxy(55, 11);
				if(flg == 1){
					printf("Word added successfully!");
				}
				if(flg == 0){
					printf("Word not added!");
				}
				break;
			case 'c':
			case 'C':
				//modify()
				modifyWord(f);
				adminHomeDisp();
				break;
			case 'd':
			case 'D':
				//delete()
				removeWord(f);
				adminHomeDisp();
				break;
			case 'e':
			case 'E':
				//search()
				search(f);
				adminHomeDisp();
				break;
			case '1':
				//main screen
				return;
			case '0':
				//exit
				my_exit(f, g);
			default:
				gotoxy(30,23);
				printf("Enter a valid choice");
				break;
		}
		gotoxy(47, 8);
	}
}

//display static aspect of 'regular' mode home screen
//this function prints header(using standardHeader() function), and guide (input help - which keys do what)
//displays all the options available to 'regular' user
void regHomeDisp(){
	standardHeader();
	
	gotoxy(30, 8);
	printf("Choose Operation: ");
	gotoxy(30, 10);
	printf("A - Browse Dictionary");
	gotoxy(30, 11);
	printf("B - Search a Word");
	
	gotoxy(0, 24);
	printf("Guide: 0-Exit| 1-Start Screen");
	gotoxy(47, 8);
}

//regular home screen -  all the operations that are allowed to regular user are interactable through this screen
//error message is shown if user enter an invalid input
//user can choose through - browse, search, back to main user mode selection screen, and exit out of application
void regHome(FILE **f, FILE **g){
	char ch;
	
	while(1){
		fflush(stdin);
		ch = getch();
		
		gotoxy(30,23);
		printf("                          ");
		
		switch(ch){
			case 'a':
			case 'A':
				//browse()
				browseDisp();
				browse(f);
				regHomeDisp();
				break;
			case 'b':
			case 'B':
				//search()
				search(f);
				regHomeDisp();
				break;
			case '1':
				//main screen
				return;
			case '0':
				//exit
				my_exit(f, g);
			default:
				gotoxy(30,23);
				printf("Enter a valid choice");
				break;
		}
		gotoxy(47, 8);
	}
}

//used to display static portion of 'remove' screen, which is where user can search for a word in dictionary, and then delete it
//this function prints header(using standardHeader() function), and guide (input help - which keys do what)
// and other basic static output
//to help user understand what is happening
void removeWordDisp(){
	searchDisp();
	gotoxy(30, 8);
	printf("Delete a word, look for the word - ");
}

//used to display static portion of 'modify' screen, which is where user can search for a word in dictionary, and then modify it
//this function prints header(using standardHeader() function), and guide (input help - which keys do what)
// and other basic static output
//to help user understand what is happening
void modifyWordDisp(){
	searchDisp();
	gotoxy(30, 8);
	printf("Modify a word, look for the word - ");
}

//function handling deletion of a record
//first user has to search for the word to be deleted, if word is found, then user is asked to input id of word to be deleted
//on successful deletion, a message is printed
void removeWord(FILE **f){
	char ch;
	int flg, flg2, delid, count=1;
	struct record w = {0};
	FILE *ftemp;	//temp file used to copy the contents of original file except record to be deleted
	
	removeWordDisp();	//displays static aspect of remove screen
	while(1){
		//input from user - error control, no invalid input is accepted
		while(1){
			fflush(stdin);
			ch = getch();
			gotoxy(0,23);
			printf("                                                                    ");
			if(ch == '1' || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C')
				break;
			gotoxy(0,23);
			printf("***Enter any of the valid inputs only, given in guide at bottom.***");
		}
		
		//first searching takes place, if words matching criteria are found they are displayed
		fseek(*f, 0, SEEK_SET);
		switch(ch){
			case '1':
				return;
			case 'a':
			case 'A':
				//search by word name
				flg = searchSubPart(ch, f);
				break;
			case 'b':
			case 'B':
				//search by synonym
				flg = searchSubPart(ch, f);
				break;
			case 'c':
			case 'C':
				//seach by antonym
				flg = searchSubPart(ch, f);
				break;
		}
		
		//if at least 1 word is found flg is 1 (return value of searchSubPart() function)
		//then user is provided to enter the id of word to be deleted
		if(flg == 1){
			count=1;
			flg2=0;
			printf("\n\nEnter the id of the word to be deleted: ");
			scanf("%d", &delid);
			fseek(*f, 0, SEEK_SET);
			ftemp = fopen("temp.dat", "wb");	//temp file created
			//all records of original file copied into temp file except record to be deleted
			while(fread(&w, sizeof(w), 1, *f)==1){
				if(w.id != delid){
					w.id = count;
					fwrite(&w, sizeof(w), 1, ftemp);
					count++;
				}
				else
					flg2=1;
			}
			//original and temp files closed
			//original file deleted
			//temp file renamed to original file's name
			fclose(ftemp);
			fclose(*f);
			remove("word.dat");
			rename("temp.dat", "word.dat");
			readyWordFile(f);	//file opened again for futher application use
			if(flg2==1)
				printf("                         Word Deleted! Press any key to continue");
			else if(flg2==0)
				printf("                         Word with input id doesn't exist");
			fflush(stdin);
			getch();
		}
		removeWordDisp();
	}
}

//function handling modification of a record
//first user has to search for the word to modify, if word is found, then user is asked to input id of word to be modified
//on successful modification, a message is printed
void modifyWord(FILE **f){
	char ch;
	int flg2, modid, count=1, strucsize;
	struct record w = {0}, neww = {0};
	
	strucsize=sizeof(w);
	modifyWordDisp();
	while(1){
		//error control - only if valid input is given, program moves forward
		while(1){
			fflush(stdin);
			ch = getch();
			gotoxy(0,23);
			printf("                                                                    ");
			if(ch == '1' || ch == 'a' || ch == 'A' || ch == 'b' || ch == 'B' || ch == 'c' || ch == 'C')
				break;
			gotoxy(0,23);
			printf("***Enter any of the valid inputs only, given in guide at bottom.***");	//if valid input is not given, error message is printed
		}
		
		//first searching takes place, which can be done by 'word', 'synonym' or 'antonym'
		fseek(*f, 0, SEEK_SET);
		switch(ch){
			case '1':
				return;
			case 'a':
			case 'A':
				//search by word name
				flg2 = searchSubPart(ch, f);
				break;
			case 'b':
			case 'B':
				//search by synonym
				flg2 = searchSubPart(ch, f);
				break;
			case 'c':
			case 'C':
				//seach by antonym
				flg2 = searchSubPart(ch, f);
				break;
		}
		
		//if at least 1 word is found flg is 1 (return value of searchSubPart() function)
		//then user is provided to enter the id of word to be modified
		if(flg2 == 1){
			printf("\n\nEnter the id of the word to be modified: ");
			scanf("%d", &modid);
			fseek(*f, 0, SEEK_SET);
			//search through file takes place, when 'word' which has modid(id of 'word' to be modified)
			//the file pointer is moved a record backword, so that when we write modified word, 
			//previous 'word' gets overwritten
			while(fread(&w, sizeof(w), 1, *f)==1){
				if(w.id == modid){
					fseek(*f, -strucsize, SEEK_CUR);
					clearSubScreen(24);
					gotoxy(0, 10);
					printf("                                Older word details:\n\n");
					putRecord(w);	//displaying original record
					
					neww = getRecord();	//getting new record
					neww.id = w.id;
					
					printf("\nConfirm modification of word? Y/N ");
					ch = getChoiceYN();
					
					//final confirmation before modification
					if(ch == 'y' || ch == 'Y'){
						fwrite(&neww, sizeof(neww), 1, *f);
						printf("\n\n                         Word Modified! Press any key to continue");
						getch();
						fseek(*f, 0, SEEK_SET);
						return;
					}
					else{
						printf("\n\n                       Word Not Modified! Press any key to continue");
						getch();
						fseek(*f, 0, SEEK_SET);
						return;
					}
				}
			}
		}
		modifyWordDisp();
	}
}

