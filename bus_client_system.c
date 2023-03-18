#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

char* return_today_date();
int check_existance_in_array(int num,int arr[],int t_array_value);
char** list_of_bus(char source_location[50], char destination_location[50]) {
	/* This funcation returns the list of bus going in following roots ie. between source location and destination_location */
	char str[200];
	// printf("%s %s %s",date,source_location,destination_location);
	// select * from route_details where from_location='Amritsar' and to_location='Jalandhar'
	// mysql_query(conn, "SHOW DATABASES");
	// sprintf(str, "select * from route_details where from_location='%s' and to_location='%s'", source_location, destination_location);
	sprintf(str,"SELECT bus_name FROM bus_details JOIN route_details ON bus_details.bus_id = route_details.bus_id WHERE route_details.from_location = '%s' AND route_details.to_location = '%s'", source_location, destination_location);

	int ret = mysql_query(conn, str);
	res = mysql_store_result(conn);   //stores the result of the query
	// printf("%d %d\n",ret, res);
	int num_fields = mysql_num_fields(res); //this returns total no of records in the table
	// printf("%d total no of record\n",num_fields);
	char** bus_list = (char**) malloc(sizeof(char*) * num_fields); // create an array of num_field string pointers
	for (int i = 0; i <= num_fields; ++i)
	{
		bus_list[i]="0";
	}
	int y=0;
	// printf("%s",mysql_fetch_row(res)[2]);
	while (row = mysql_fetch_row(res)) {
		// printf("%s",row[0]);
		bus_list[y]=strdup(row[0]); //strdump send the array pointer of the value used while using malloc
		y++;
	}
	mysql_free_result(res);
	return bus_list;
}
int check_available_bus_or_not(int total_bus,char **bus_list){
	/* This funcation helps us to find if there is any bus between two location or not
		it will return 0 if theres will be not bus between station
		else if there will be any bus between station then it will return 1 */
	int count=0;
	for (int i = 0; i <= total_bus; ++i)
	{
		if (strcmp(bus_list[i],"0")!=0) count++;
	}
	return count;
}
int show_availability_chart(int total_bus,char ** bus_list){
	int selected_bus_code=1;
	// printf("\nEnter the bus code to check seat availability: ");
	// scanf("%d",&selected_bus_code);
	if (selected_bus_code>=total_bus+1)
	{
		printf("\n--- Enter valied bus code ---\n");
		// show_availability_chart(total_bus,bus_list);
	}
	else {
		return selected_bus_code;
	}
}
int check_existance_in_array(int num,int arr[],int t_array_value){
	/* This funcation will check if a number is present in the array or not
	if number will be in the array then it will return 1
	else it will return 0 */
	for (int i = 0; i <= t_array_value; ++i)
	{
		if (arr[i]==num) return 1;
	}
	return 0;
}
int check_already_booked(int bus_id,char date[],char *token){
	/* this function will check if all the seats selected by the user is vacant or not
	if all the seats are not vacant then it will return 1
	else it will return 0*/
	char str[200];
	// sprintf(str,"select count(booking_id) from booking_details where bus_id=%d and journey_date='%s' and seat_no=%s and cancel_status=0",bus_id,date,token);
    while (token != NULL) {
    sprintf(str,"select count(booking_id) from booking_details where bus_id=%d and journey_date='%s' and seat_no=%s and cancel_status=0",bus_id,date,token);
	mysql_query(conn, str);
	res=mysql_store_result(conn);
	if (atoi(mysql_fetch_row(res)[0])!=0)
	{
		return 1;
	}
    token = strtok(NULL, " ");
	}	
	return 0;
}
int check_range_seat(int total_seats,char *token){
	/* this function will check if all the seats nunmber given by user to book is in the
	 range of the total seat in the bus or no
	 if there will be any one seat which is not in range it will return 1
	 else it will return 0 */
    while (token != NULL) {
        // printf("%s\n", token);
        if (atoi(token)>total_seats) return 1;
        token = strtok(NULL, " ");
    }	
	return 0;
}
int book_ticket(int bus_id,char journey_date[11],int u_id,int route_id){
	/* this funcation will help to book the desired seat by the passenger */
	char payment_choice;
	// printf("%d %s %d\n",bus_id,journey_date,u_id);
	char seats[200]="13",str[200];
	int total_seat_to_book=0;
	// printf("\n*Enter the seat not one space seperated if you want to book multiple seat. \n");
	// printf("Enter the seat no : ");
	// scanf("%[^\n]%*c",seats);
	// printf("%s",seats);
	char *token = strtok(seats, " ");
	char *token_copy = token;
	char *token_copy2= token;
	char *token_copy3= token;

	char * today_date=return_today_date(); 

	while (token_copy3 != NULL) {  
		//this will count total no of seats to book and store the data in total_seat_to_to_book variable
	    total_seat_to_book++;
	    token_copy3 = strtok(NULL, " ");
	}

	// printf("%d total no seats to book",total_seat_to_book);

    // insert into booking_details (bus_id,journey_date,u_id,seat_no) values (1,'2023-03-08',1,5);
    sprintf(str,"select total_seats from bus_details where bus_id=%d",bus_id);
    mysql_query(conn, str);
	res=mysql_store_result(conn);
	int total_seats=atoi(mysql_fetch_row(res)[0]);
    // printf("%d",checked_status_already_booked);
    if (check_range_seat(total_seats,token_copy2)==0)
    {
	    if (check_already_booked(bus_id,journey_date,token_copy)==0)
	    {
			while (token != NULL) {
			    // printf("%s\n", token);
				// select fare from route_details where bus_id=2 and from_location='Amritsar' and to_location='Jalandhar';
			    sprintf(str,"select fare from route_details where route_id=%d",route_id);
			    mysql_query(conn,str);
			    res=mysql_store_result(conn);
			    float price_per_seat=atoi(mysql_fetch_row(res)[0]);
			    // printf("%0.1f",price_per_seat);
			    printf("--------------------------------------------------------------------------------------------------\n");
			    printf("       TOTAL FARE CHARGE : %0.2f\n",price_per_seat*total_seat_to_book);
			    printf("--------------------------------------------------------------------------------------------------\n");
			    printf("\nPress 'y' to pay the amount: ");
			    scanf("%c",&payment_choice);
			    if (payment_choice=='y')
			    {
				    sprintf(str,"insert into booking_details (bus_id,booking_date,journey_date,u_id,seat_no) values (%d,'%s','%s',%d,%s)",bus_id,today_date,journey_date,u_id,token);
					mysql_query(conn, str);
					printf("\n--- Congratulation! Your seats has been booked ---");
			    }
			    else printf("\n---- Payment Failed !!!! ----\nSorry! Unable to book ticket.... \n");

			    token = strtok(NULL, " ");
			}	
	    }
	    else printf("\n--- Sorry! Your selected seat are already booked ---\n");
	}
	else printf("\n--- Sorry! Please enter the correct Seat number ---\n");
}

void seat_availability(int u_id) {
	/* This funcation will help to show the lists of all the vacant seats for a particular route */
	char date[11] = "2023-03-08", source_location[50] = "Amritsar", destination_location[50] = "Jalandhar";
	int selected_bus_code,main_bus_code,total_seats;
	float rating;
	char str[200];
	// printf("Enter the journey date (2023-03-08) :- ");
	// scanf("%[^\n]%*c",&date);
	// printf("Enter the source location:- ");
	// scanf("%[^\n]%*c",&source_location);
	// printf("Enter the destination location:- ");
	// scanf("%[^\n]%*c",&destination_location);
	// printf("%s %s %s\n ",date,source_location,destination_location);

	char** bus_list = list_of_bus(source_location, destination_location); // call the function and get the string array pointer
	int total_bus=sizeof(bus_list)/sizeof(bus_list[0]);
	int total_available_bus = check_available_bus_or_not(total_bus,bus_list);
	int i=0;
	char status[8];	
	char command;
	// printf("%d\n",total_available_bus);

	if (total_available_bus!=0)
	{
		printf("==================\n");
		printf("Sl.No.    Bus Name\n");
		printf("==================\n");
		for (int i = 0; i <= total_bus; i++) {
			if (strcmp(bus_list[i],"0")!=0)
		    printf("%d         %s \n",i+1, bus_list[i]);
		}
		int selected_bus_code= show_availability_chart(total_available_bus,bus_list);
		// printf("Your bus code is %d",selected_bus_code);
		sprintf(str,"select bus_id from bus_details where bus_name='%s'",bus_list[selected_bus_code-1]);
		mysql_query(conn, str);
		res = mysql_store_result(conn);  
		main_bus_code = atoi(mysql_fetch_row(res)[0]);
		mysql_free_result(res);
		sprintf(str,"select rating,total_seats from bus_details where bus_id=%d",main_bus_code);
		mysql_query(conn,str);
		res=mysql_store_result(conn);
		while(row=mysql_fetch_row(res)){
			rating=atof(row[0]);
			total_seats=atoi(row[1]);
		}
		mysql_free_result(res);

		// printf("%d is the main bus code\n",main_bus_code);
		// printf("%f is teh rating of that bus\n",rating);
		// printf("%d is the total not of seats\n",total_seats);

		//select seat_no from booking_details where bus_id=1 and  journey_date='2023-03-08' and cancel_status=0;
		sprintf(str,"select seat_no from booking_details where bus_id=%d and  journey_date='%s' and cancel_status=0",main_bus_code,date);
		mysql_query(conn,str);
		res=mysql_store_result(conn);
		int booked_seat_array[total_seats];
		for (int i = 0; i <= total_seats; ++i)
		{
			booked_seat_array[i]=0;  //creating an empty array
		}
		i=0;
		while(row=mysql_fetch_row(res)){
			booked_seat_array[i]=atoi(row[0]); //setting the data to the array
			i++;
		}
		mysql_free_result(res);
		
		printf("\e[1;1H\e[2J");    //this will clear the terminal screen
		printf("\n Source Location: %s           Destination Location: %s             Date: %s\n",source_location,destination_location,date);
		printf("\n Bus Name: %s                  Rating: %0.1f\n",bus_list[selected_bus_code-1],rating);	
		printf("==================================================================================================\n");
		printf("                                       SEAT AVAILABILITY DATA                                     \n");
		printf("==================================================================================================\n");
		printf("\n");

		// int arr[7]={1,4,17,6,19,5,13};
		for (int i = 1; i <=total_seats ; ++i)
		{
			printf("      [%d] ",i);
			(i<10)?printf(" "):printf("");
			(check_existance_in_array(i,booked_seat_array,total_seats))?printf(" Booked  "):printf("         ");
			if (i%5==0)	printf("\n\n");
		}
		// printf("\n Do you want to book the seat (Yes/No)? ");
		// scanf("%c",&command);
		command='y'; //setting default value for command to book ticket or not
		if ((int)command==89 || (int)command==121)
		{
			sprintf(str,"select route_id from route_details where bus_id=%d and from_location='%s' and to_location='%s'",main_bus_code,source_location,destination_location);
			// printf("%s",str);
			mysql_query(conn,str);
			res=mysql_store_result(conn);
			int route_id = atoi(mysql_fetch_row(res)[0]);
			// printf("%d\n", route_id);
			book_ticket(main_bus_code,date,u_id,route_id);
		}


	}
	else printf("\n--- SORRY, THERE IS NO BUS FOR THIS LOCATION ---\n");
	
}
// create trigger lowercase_from_location before insert on route_details for each row set new.from_location = new.from_location;
int add_bus()
{
	char str[200];
	char bus_name[50]="Dharam Rath",from_location[50]="Amritsar",to_location[50]="Jalandhar",departure_time[6]="08:00",arrival_time[6]="08:30";
	float rating=5,fare=40;
	int total_seats=20;

	printf("\n============  ENTER THE DETAILS TO ADD BUS  ============\n\n");
	printf("Enter bus name:                ");
	scanf("%[^\n]%*c",&bus_name);
	printf("Souce location of bus:         ");
	scanf("%[^\n]%*c",&from_location);
	printf("Destination location of bus:   ");
	scanf("%[^\n]%*c",&to_location);
	printf("Departure time of bus:         ");
	scanf("%[^\n]%*c",&departure_time);
	printf("Arrival time of bus:           ");
	scanf("%[^\n]%*c",&arrival_time);
	printf("Rating of bus:                 ");
	scanf("%f",&rating);
	printf("Fare of bus:                   ");
	scanf("%f",&fare);
	printf("Total seats in bus:            ");
	scanf("%d",&total_seats);

	sprintf(str,"insert into bus_details (bus_name,rating,total_seats) values ('%s',%0.1f,%d)",bus_name,rating,total_seats);
	int check_query = mysql_query(conn, str);
	printf("Check query data %d",check_query);
	sprintf(str,"select bus_id from bus_details where bus_name='%s'",bus_name);
	mysql_query(conn, str);
	res = mysql_store_result(conn);   //stores the result of the query
	int bus_id = atoi( mysql_fetch_row(res)[0]);  //atoi funcation is used to convert string pointer to integer 
	// printf("%d\n",bus_id);
	sprintf(str,"insert into route_details (bus_id,from_location,to_location,departure_time,arrival_time,fare) values (%d,'%s','%s','%s','%s',%0.1f)",bus_id,from_location,to_location,departure_time,arrival_time,fare);
	mysql_query(conn, str);
	printf("BUS DETAILS HAS BEEN SUCESSFULLY ADDED");
	return 0;
}
void mysql_booking_data_printer(char str[300]){
	/* this function will fetch the result from mysql database and prints it beautifully on screen */
	mysql_query(conn, str);
	// printf("%s\n",str );
	res = mysql_store_result(conn);   //stores the result of the query
	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	printf("--------------------------------------------------------------------------------------------------\n");
	printf(" BUS NUMBER      BUS NAME          SOURCE            DESTINATION       SEAT NUMBER       DATE    \n");
	printf("--------------------------------------------------------------------------------------------------\n");
	while (row=mysql_fetch_row(res)){
		printf("  %-14s %-17s %-17s %-17s %-14s %-10s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);
	}
}
void manage_booking(int u_id){
	/* this funcation will show the user their complted ,upcoming and canceld ticket */
	int choice=1; //stores the choice of user to display according 
	char str[300];  //stores mysql command to execute
    char * today_date=return_today_date(); //stores todays date
    // printf("%s\n",today_date);
	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	while(1){ //starting while loop so to handle if the user has entered unexpected data 
		printf("==================================================================================================\n");
		printf("                                          MANAGE ACCOUNT                                          \n");
		printf("==================================================================================================\n");
		printf("\nEnter number to show details: \n");
		printf("1. Upcomig Journey\n");
		printf("2. Completed Journey\n");
		printf("3. Canceled Journey\n");
		printf("99. To exit program\n");
		printf("\nEnter your choice: ");
		// scanf("%d",&choice);
		if (scanf("%d",&choice)==1)
		{
			switch (choice){ //started switch cased based on data by user to print mysql info on screen
			case 1:
				// this will store the sql comand for fetching the data of upcoming journey
				sprintf(str,"SELECT bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.journey_date > '%s'",u_id,today_date);
				break;
			case 2:
				// this will store the sql comand for fetching the data of completed journey
				sprintf(str,"SELECT bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.journey_date < '%s'",u_id,today_date);
				break;
			case 3:
				// this will store the sql comand for fetching the data of canceled ticket
				sprintf(str,"SELECT bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.cancel_status=1",u_id);
				break;
			case 99:
				break;
			default:
				printf("Input out of range !!!!\n");  //this will print on screen if user has entered greater than 3 or other than 99
			}
			if (str[0]!='\0')
			{
				mysql_booking_data_printer(str); //this will print the data on the screen returned by mysql
			}
			break;  //getting out of while loop
		}
		else{
			printf("Please enter the right value !!!\n");   //this will print if the user has entered anything other than integer in input
			while (getchar() != '\n'); // clear input buffer
		}
	}
}

char* return_today_date() {
    char *date = malloc(11 * sizeof(char)); // Allocate memory for string;
    time_t t;
    t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date,"%d-%d-%d", tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday);
    return date; // Return pointer to string
}

void cancel_seat(int u_id){
	char str[300];
	int i=0;
	char * today_date=return_today_date();
	int default_size=1;
	int cancel_seat_no;
	char cancel_surity;
	int* booking_id_array = (int*) malloc(default_size * sizeof(default_size)); // Allocate initial memory for array
	// printf("%s",today_date);
	sprintf(str,"SELECT bk.booking_id,bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.journey_date > '%s' and bk.cancel_status=0",u_id,today_date);
	mysql_query(conn, str);
	printf("%s\n",str );
	res = mysql_store_result(conn);   //stores the result of the query
	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	printf("--------------------------------------------------------------------------------------------------\n");
	printf("Sl No    BUS NUMBER     BUS NAME       SOURCE         DESTINATION       SEAT NUMBER       DATE    \n");
	printf("--------------------------------------------------------------------------------------------------\n");
	while (row=mysql_fetch_row(res)){
		booking_id_array[i]=atoi(row[0]);
		// printf("%d\n", booking_id_array[i]);
		booking_id_array = (int*) realloc(booking_id_array, (i+1) * sizeof(int)); // Reallocate memory with new size
		printf(" %-7d %-14s %-14s %-14s %-17s %-14s %-10s\n",i+1,row[1],row[2],row[3],row[4],row[5],row[6],row[7]);
		i++;
	}

	while (1){
		printf("\nEnter the 'Sl No' to cancel the seat: ");
		// scanf("%d",&cancel_seat_no);
		if (scanf("%d",&cancel_seat_no)==1)
		{
			if (cancel_seat_no<=i && cancel_seat_no>0)
			{
				printf("\nAre you sure you want to cancel the seat: ");
				getchar(); // consume newline character
				scanf("%c",&cancel_surity);
				printf("%c\n",cancel_surity );
				if (cancel_surity=='y')
				{
					// printf("%d\n",booking_id_array[cancel_seat_no-1] );
					sprintf(str,"update booking_details set cancel_status=1 where booking_id=%d",booking_id_array[cancel_seat_no-1]);
					printf("%s\n",str);
					mysql_query(conn,str);
					printf("---- Ticket Cancelled Successfully !!!! ----\n");
					break;
				}
				else printf("Aborted Ticket Cancellation !!!!\n");		
				break;
			}
			else printf("\n---- Enter the correct 'Sl No' !!!!! ----\n");
		}
		else {
			printf("Please enter right value !!!!!!\n");
			break;
		}

	}
}

int login(char username[],char password[]){
	/* This funcation will return the user if login is sussessfull else it will return 0*/
	char str[200]; //this variable will store the mysql command to be executed
	int status; //will store status of login
	int u_id=0; //will store the user id of user if successful login else 0

	sprintf(str,"select count(*) ,u_id from users where user_name='%s' and password='%s'",username,password);  //store the mysql query to str variable
	// printf("%s\n",str );
	mysql_query(conn,str);  //running sql query
	res = mysql_store_result(conn);   //stores the result of the query
	row=mysql_fetch_row(res); //fetches the row of the mysql data 
	status=atoi(row[0]); //storing user login status
	// printf("%d\n", status);
	if (status==1) //if status is 1 means users data is correct 
	{
		u_id=atoi(row[1]); //storing user id
		return u_id; //returing userid of the user
	}
	else{
		return 0; //returing default value if the users data is incorrect
	}

}

int signup(){
	/* this function will will the user to sign up 
	data will be stored in users table of the database */
	char str[200]; //stores the mysql command to execute
	char name[100]="Subhadeep Kumar",user_name[50]="Abhishek",email[50]="subhadeep@gmail.com",password[250]="subhadeep";  //initiating different variable to store the data in databse
	int age=18,mob=1234567890;
	while(1){
		//storing the data in variables
		//check variable will store the data if correct data is inverted in variable or not
		printf("\nEnter your name:                    ");
		int check1 = scanf("%[^\n]%*c",&name);
		printf("Enter username you want to create:  ");
		int check2 = scanf("%[^\n]%*c",&user_name);
		printf("Enter email address:                ");
		int check3 = scanf("%[^\n]%*c",&email);
		printf("Enter password:                     ");
		int check4 = scanf("%[^\n]%*c",&password);
		printf("Enter your age:                     ");
		int check5 = scanf("%d",&age);
		printf("Enter mobile number:                ");
		int check6 = scanf("%d",&mob);

		if (check1==1&&check2==1&&check3==1&&check4==1&&check5==1&&check6==1) //check if all the inputs are correct or not 
			break;
		else{
			printf("\e[1;1H\e[2J");    //this will clear the terminal screen
			printf("Please enter valid data !!!!\n");
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
	}
	while (getchar() != '\n'); // clear input buffer
    while(1){  
    	//running while loop to check the user name is valid or not if not valid then it will retake the input from user
        sprintf(str, "SELECT COUNT(user_name) FROM users WHERE user_name='%s'", user_name); //stores the query in str variable to execute in mysql
        mysql_query(conn, str); //executes the mysql command
        int check_availability_of_id = atoi(mysql_fetch_row(mysql_store_result(conn))[0]); //stores the count of total no of id which has the same user name provided by the user
        if (check_availability_of_id > 0) { 
        	//if check availability > 0 then there is duplicacy of user name so retake the input
            printf("\nThat username is already taken !!!!!\n");
            printf("Enter different user name:        ");
	        if (scanf("%49[^\n]%*c", &user_name) != 1) { //checking for valid input
	        	while (getchar() != '\n'); // clear input buffer
	            printf("\nInvalid input. Please try again.\n");
	            continue; //jump to next iteration to check if the new user name is valid or not
	        }   
        }
        else break;
	}
	sprintf(str,"insert into users (name,user_name,age,email,mob,password) values ('%s','%s',%d,'%s',%d,'%s')",name,user_name,age,email,mob,password);  //storing sql command to create new user
	// printf("%s\n", str);
	mysql_query(conn,str); //executes mysql query
	printf("\n\n----- Account has been Successfully created !!!!! -----\n");
	return 0;
}

int main(int argc, char const *argv[])
{
	conn = mysql_init(NULL);
	// printf("%d",sizeof(conn));

	if (conn == NULL) {
		fprintf(stderr, "Failed to initialize MySQL connection\n");
		return 1;
	}

	if (!mysql_real_connect(conn, "localhost", "abhishek", "abhi7549", "bus_management_system", 0, NULL, 0)) {
		fprintf(stderr, "Failed to connect to MySQL server: %s\n", mysql_error(conn));
		mysql_close(conn);
		return 1;
	}

	int uid_status = login("Abhishek","abhishek");   //sending userid or password to login function to get user id if successful login or 0
	// printf("%d\n", uid_status);
	signup(); //started signup module if the user wants to register into the software

	// seat_availability(uid_status);
	
	// add_bus();

	// manage_booking(uid_status);

	// cancel_seat(uid_status);

	return 0;
}