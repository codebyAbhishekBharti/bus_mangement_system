#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include<time.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

char* return_today_date();
void check_user_response();
int check_existance_in_array(int num, int arr[], int t_array_value);
int check_existance_in_array(int num, int arr[], int t_array_value) {
	/* This funcation will check if a number is present in the array or not
	if number will be in the array then it will return 1
	else it will return 0 */
	for (int i = 0; i <= t_array_value; ++i)
	{
		if (arr[i] == num) return 1;
	}
	return 0;
}
int check_already_booked(int bus_id, char date[], char *token) {
	/* this function will check if all the seats selected by the user is vacant or not
	if all the seats are not vacant then it will return 1
	else it will return 0*/
	char str[200];
	while (token != NULL) {
		sprintf(str, "select count(booking_id) from booking_details where bus_id=%d and journey_date=STR_TO_DATE('%s', '%%d-%%m-%%Y') and seat_no=%s and cancel_status=0", bus_id, date, token);
		mysql_query(conn, str);
		res = mysql_store_result(conn);
		if (atoi(mysql_fetch_row(res)[0]) != 0) return 1;
		token = strtok(NULL, " ");
	}
	return 0;
}
int check_range_seat(int total_seats, char *token) {
	/* this function will check if all the seats nunmber given by user to book is in the
	 range of the total seat in the bus or no
	 if there will be any one seat which is not in range it will return 1
	 else it will return 0 */
	while (token != NULL) {
		// printf("%s\n", token);
		if (atoi(token) > total_seats) return 1;
		token = strtok(NULL, " ");
	}
	return 0;
}

int book_ticket(int bus_id, char journey_date[11], int u_id, int route_id, int total_seats) {
	/* this funcation will help to book the desired seat by the passenger */
	char query[200];  //this variable will store the sql command to execute
	char seats[200]; //string which will contain which seat user want to book
	char payment_choice;  //this will store the users choice whether he wants to book the ticket or not
	char * today_date = return_today_date();  //stores todays date
	while (1) {  //running this loop to handle if user enters wrong value
		int total_seat_to_book = 0; //setting initial value for total seats user wants to book
		int is_all_integer = 1; //setting value to check the value entered by the user is all numeric or not
		int check_presence_list[total_seats]; // initialize a interger array to very that user must not entered the same seat no
		for (int i = 0; i < total_seats; ++i) check_presence_list[i] = 0; //setting all value to 0
		printf("\nEnter 99 to cancel booking");
		printf("\n*Enter the seat not one space separated if you want to book multiple seats. \n");
		printf("Enter the seat no : ");
		scanf("%[^\n]%*c", seats);  //taking string input from the user
		if (strcmp(seats, "99") == 0)  //checkig if user wants to exit the booking window
		{
			printf("----------- Exiting Booking -----------\n");
			exit(0);
		}
		if (seats[0] == NULL)  //checking if user has entered any value or not if user has not entered any value the just restarting the loop
		{
			while (getchar() != '\n'); //clearing input buffer
			printf("\n!!!!!! Please enter the right seat no. !!!!!!\n\n");
			continue;  //starting the loop again
		}
		char *token_copy = strdup(seats); // make a copy of the original input string for checking if all the value is integer
		char *token_copy2 = strdup(seats);  //copy the original input string to make the token for inserting all the seat no in the database
		char *token = strtok(token_copy, " ");  //creating the token for checking if all the value is integer or not
		while (token != NULL) {  //loop for iterating over all the tokens
			int num = atoi(token); //converting the token data into integer
			if (num == 0 && *token != '0' || num > total_seats  || check_presence_list[num] == 1) { //checking if the token value is integer and not repeated
				is_all_integer = 0; //setting the value if any non integer no found
				break; //breaking this while loop
			}
			else {
				check_presence_list[num] = 1; //setting the value in array so to check for no duplicacy
				total_seat_to_book += 1; //increating the count of total seats to book
			}
			token = strtok(NULL, " ");  //removing that token
		}

		if (is_all_integer && total_seat_to_book > 0) { //works only if users has entered all integer value and non repeating value also total seats to book should be greater than 0
			token = strtok(seats, " ");  //creating this token to check if all seats are available for booking or not
			if (check_already_booked(bus_id, journey_date, token) == 0)  //check_already_booked function is checking whether seats are available for booking or not
			{
				// below sql command gets the fare of bus between stops
				sprintf(query, "select fare from route_details where route_id=%d", route_id);
				mysql_query(conn, query);  //executing sql command
				res = mysql_store_result(conn); //storing the result of sql
				float price_per_seat = atof(mysql_fetch_row(res)[0]); //storing the fare charge
				printf("--------------------------------------------------------------------------------------------------\n");
				printf("       TOTAL FARE CHARGE : %0.2f\n", price_per_seat * total_seat_to_book);
				printf("--------------------------------------------------------------------------------------------------\n");
				printf("\nPress 'y' to pay the amount: ");
				scanf(" %c", &payment_choice);  //taking input from user to varify whether user wants to book the ticket or not
				if (payment_choice == 'y')
				{
					token = strtok(token_copy2, " "); //creating this token to book all the seats in database.
					if (mysql_query(conn, "start transaction") != 0)  //starting transaction so to protect again any insertion error
					{
						printf("Unable to start transaction\n");
						exit(0);
					}
					while (token != NULL) { //iterating over all the values entred by the user
						//below sql command books the seat for the user by saving the booking data in booking_details table of the mysql server
						sprintf(query, "insert into booking_details (bus_id,booking_date,journey_date,u_id,seat_no) values (%d,'%s',STR_TO_DATE('%s', '%%d-%%m-%%Y'),%d,%s)", bus_id, today_date, journey_date, u_id, token);
						if (mysql_query(conn, query) != 0) {  //executing command and checking for any error which can occur
							printf("!!!!!!!!! Failed to book seats !!!!!!!!!\n");
							mysql_query(conn, "ROLLBACK"); //in case of any error all tranaction will will back
						}
						token = strtok(NULL, " ");
					}
					if (mysql_query(conn, "commit") != 0 && mysql_affected_rows(conn) != 0) { //finally commiting all the values to the database if there is not error
						printf("!!!!!!!!! Failed to book seats !!!!!!!!!\n");
						mysql_query(conn, "ROLLBACK"); // if there will be any error and roll back all the transaction
					}
					else printf("\n--- Congratulation! Your seats has been booked ---\n");
				}
				else {
					printf("\n---- Payment Failed !!!! ----\nSorry! Unable to book ticket.... \n");
					int command;
					printf("Enter 99 to go back : ");
					if (scanf("%d", &command) == 1 && command == 99)
					{
						printf("\e[1;1H\e[2J");    //this will clear the terminal screen
					}
					else {
						printf("\n                 ----------------------------------------------------------\n");
						printf("                 -----------    You have entered wrong input    -----------\n");
						printf("                 -----------------    EXITING PROGRAM    ------------------\n");
						exit(0);
					}
				}
				break;
			}
			else printf("\n!!!!!! Seat is already booked. Choose another seat !!!!!!");
		} else printf("\n!!!!!! Please enter the right seat no. !!!!!!\n\n");
	}
	return 0;
}

void seat_availability(int u_id) {
	/* This funcation will help to show the lists of all the vacant seats for a particular route */
	char date[11] = "08-03-2023", source_location[50] = "Amritsar", destination_location[50] = "Jalandhar", bus_name[50]; //stores differentt details to search about seat availability
	char arrival_time[50], departure_time[50];
	int total_seats, selected_bus; //stores the total seats in the bus and selected bus not in the list
	float rating, fare; //stores the rating and fare of bus
	int day, month, year; //stores day, month , year of journey
	char query[200]; //stores sql command to execute
	int default_size = 1; //default size of array
	int* bus_id_array = (int*) malloc(default_size * sizeof(default_size)); // Allocate initial memory for array to store bus id
	int* route_id_array = (int*) malloc(default_size * sizeof(default_size)); // Allocate initial memory for array to store route id
	char command;  //stores if the user wants to book the ticket or not
	printf("\n\n");
	while (1) {
		printf("Enter the journey date (dd-mm-yyyy):- ");
		int check1 = scanf("%02d-%02d-%04d", &day, &month, &year);
		while (getchar() != '\n'); //clearing input buffer
		printf("Enter the source location:- ");
		int check2 = scanf(" %[^\n]%*c", &source_location);
		printf("Enter the destination location:- ");
		int check3 = scanf("%[^\n]%*c", &destination_location);
		// printf("%d %d %d\n",check1,check2,check3 );
		if (check1 == 3 && check2 == 1 && check3 == 1)
		{
			sprintf(date, "%02d-%02d-%04d", day, month, year);  //stores the day,month,year in date variable in particular format
			break;
		}
		else {
			printf("\e[1;1H\e[2J");    //this will clear the terminal screen
			printf("Please enter valid data !!!!\n\n");
			// while (getchar() != '\n'); // clear input buffer
			continue;
		}
	}
	// printf("%s %s %s\n ",date,source_location,destination_location);

	//below sql command fetches bus id, bus name and route id for the bus which goes to the source and destination location given by the user
	sprintf(query, "select bd.bus_id,bd.bus_name,rd.route_id from bus_details bd join route_details rd on bd.bus_id = rd.bus_id where rd.from_location='%s' AND rd.to_location='%s'", source_location, destination_location);
	// printf("%s\n",query );
	mysql_query(conn, query); //executes sql command
	res = mysql_store_result(conn);   //stores the result of the query
	int total_bus_count = mysql_num_rows(res); //stores total bus in the route
	if (total_bus_count != 0)
	{
		printf("--------------------------------------------------------------------------------------------------\n");
		printf("                             Sl No.               BUS NAME                                        \n");
		printf("--------------------------------------------------------------------------------------------------\n");
		int i = 0; //initializing i for printig details
		while (row = mysql_fetch_row(res)) {  //this loop iterates till all the rows in the mysql result of bus details
			bus_id_array[i] = atoi(row[0]); //storing bus id of bus in an array
			route_id_array[i] = atoi(row[2]); //storing route id of bus in an array
			bus_id_array = (int*) realloc(bus_id_array, (i + 1) * sizeof(int)); // Reallocate memory with new size
			route_id_array = (int*) realloc(route_id_array, (i + 1) * sizeof(int)); // Reallocate memory with new size

			printf("                             %-21d %s \n", i + 1, row[1]); //printing details
			i++;
		}
		while (1) { //initilizing this loop for restrict false inputs
			printf("\nEnter the bus no. you want to check for seat availability: ");
			// if (scanf("%d", &selected_bus) == 1 && selected_bus < i && selected_bus > 0) //validiates if right bus serial no is selected
			selected_bus = 1 ;
			if (selected_bus < i && selected_bus > 0) //validiates if right bus serial no is selected
			{
				//below sql command stores sql command to fetch bus name, rating, total seats, departure time, arrival time and fare of the bus
				sprintf(query, "select bd.bus_name, bd.rating, bd.total_seats, rd.departure_time, rd.arrival_time, rd.fare from bus_details bd join route_details rd on rd.bus_id = bd.bus_id where bd.bus_id=%d", bus_id_array[selected_bus - 1]);
				mysql_query(conn, query); //executing sql command
				res = mysql_store_result(conn); //storing result of sql command
				row = mysql_fetch_row(res); //fetching the first row of sql result
				strcpy(bus_name, row[0]); // storing bus name in bus_name variable
				rating = atof(row[1]); //storing rating of bus
				total_seats = atoi(row[2]); //storing total seats in the bus
				strcpy(departure_time, row[3]); //store the departure time of the bus
				strcpy(arrival_time, row[4]); //stores the arrival time of bus to destination
				fare = atof(row[5]); //stores the fare prince of that bus
				// below line stores sql command to get all the seats which is booked for the bus on the particular date
				sprintf(query, "select seat_no from booking_details where bus_id=%d and  journey_date=STR_TO_DATE('%s', '%%d-%%m-%%Y') and cancel_status=0", bus_id_array[selected_bus - 1], date);
				mysql_query(conn, query); //executes sql command
				res = mysql_store_result(conn); //storing sql command
				int booked_seat_array[total_seats];  //initializing array which stores all the booked seats for the bus
				for (int i = 0; i <= total_seats; ++i) booked_seat_array[i] = 0; //creating an empty array
				i = 0; //initilizing i to reset value
				while (row = mysql_fetch_row(res)) { //fetching row of the sql result
					booked_seat_array[i] = atoi(row[0]); //setting the data to the array
					i++;
				}
				mysql_free_result(res);				//clearing sql result

				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("\n Source Location: %s           Destination Location: %s             Date: %s", source_location, destination_location, date);
				printf("\n Departure Time:  %s           Arrival Time:         %s              Fare: %0.2f", departure_time, arrival_time, fare);
				printf("\n Bus Name: %s                  Rating: %0.1f\n", bus_name, rating);
				printf("==================================================================================================\n");
				printf("                                       SEAT AVAILABILITY DATA                                     \n");
				printf("==================================================================================================\n");
				printf("\n");
				for (int i = 1; i <= total_seats ; ++i) // executing this loop for iterating every seats for printing the booking details in beautiful manner
				{
					printf("      [%d] ", i); //priting the seat no
					(i < 10) ? printf(" ") : printf(""); //adding extra space if seat no is less than 10
					(check_existance_in_array(i, booked_seat_array, total_seats)) ? printf(" Booked  ") : printf("         "); //printing if the seat is booked or not
					if (i % 5 == 0)	printf("\n\n"); //moving to new line if 5 seat detail is already printed
				}
				// printf("\n Do you want to book the seat (Yes/No)? ");
				// scanf(" %c",&command); //storing value to ask user if he wants to book seat or not
				command = 'y'; //setting default value for command to book ticket or not
				if ((int)command == 89 || (int)command == 121) { //checking for the valid input y=121 and Y=89
					int bus_id = bus_id_array[selected_bus - 1]; //storing the bus id
					int route_id = route_id_array[selected_bus - 1]; //storing route id
					book_ticket(bus_id, date, u_id, route_id, total_seats); //going to book_ticket module if user want to book the seat
				}
				break;
			}
			while (getchar() != '\n'); // clear input buffer
			printf("Please enter valid input !!!!!\n");
		}
	}
	else {
		printf("\n--- SORRY, THERE IS NO BUS FOR THIS LOCATION ---\n");
		check_user_response();   //this will ask user if he wants to continue or exit the program
	}
}

int add_bus(int u_id)
{
	char query[200]; //stores sql query
	char bus_name[50] = "Dharam Rath", from_location[50] = "Amritsar", to_location[50] = "Jalandhar", departure_time[6] = "08:00", arrival_time[6] = "08:30"; //stores different information to add bus to the db
	float rating = 5, fare = 40; //stores the rating and fare information
	int total_seats = 20; //stores total seats
	int check; //checks for right input entered by the user
	int buff_size = 50; //setting the size of buffer to the length of string to be entered by the user

	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	printf("\n                   ============  ENTER THE DETAILS TO ADD BUS  ============\n\n");
	while (getchar() != '\n')  //clearing input buffer
		check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Enter bus name:                             ");
		fgets(bus_name, buff_size, stdin);   //taking bus name input
		if (bus_name[strcspn(bus_name, "\n")] != '\n') { //checking if user has entered more character than buffer size
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
		bus_name[strcspn(bus_name, "\n")] = '\0';  //setting the last value of character for termination and dealing with buffer over flow
		if (bus_name[0] != '\0') check = 1; // if value is correct than getting out of loop
		else printf("\nPlease enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Source location of bus:                     ");
		fgets(from_location, buff_size, stdin);  //taking source location from the user
		if (from_location[strcspn(from_location, "\n")] != '\n') { //checking if user has entered more character than buffer size
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
		from_location[strcspn(from_location, "\n")] = '\0';  //setting the last value of character for termination and dealing with buffer over flow
		if (from_location[0] != '\0') check = 1;  // if value is correct than getting out of loop
		else printf("\nPlease enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Destination location of bus:                ");
		fgets(to_location, buff_size, stdin); //taking final location from the user
		if (to_location[strcspn(to_location, "\n")] != '\n') { //checking if user has entered more character than buffer size
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
		to_location[strcspn(to_location, "\n")] = '\0'; //setting the last value of character for termination and dealing with buffer over flow
		if (to_location[0] != '\0') check = 1;   // if value is correct than getting out of loop
		else printf("\nPlease enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		int hour, min; //stores hour and min data
		printf("Departure time of bus (HH:MM):              ");
		if (scanf("%02d:%02d", &hour, &min) == 2 && hour < 24 && hour >= 0 && min < 60 && min >= 0)  //taking input of time at the same time checking if the time entered by the user is correct or not
		{
			sprintf(departure_time, "%02d:%02d:00", hour, min);   //concatenating the value of hour and min in departure time
			check = 1; //getting out of loop
			continue;
		}
		while (getchar() != '\n'); // clear input buffer
		printf("Please enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		int hour, min; //stores hour and min data
		printf("Arrival time of bus (HH:MM):                ");
		if (scanf("%02d:%02d", &hour, &min) == 2 && hour < 24 && hour >= 0 && min < 60 && min >= 0)  //taking input of time at the same time checking if the time entered by the user is correct or not
		{
			sprintf(arrival_time, "%02d:%02d:00", hour, min);   //concatenating the value of hour and min in arrival time
			check = 1; //getting out of loop
			continue;
		}
		while (getchar() != '\n'); // clear input buffer
		printf("Please enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Rating of bus(?/5):                 ");
		if (scanf("%f", &rating) == 1 && rating > 0 && 5 >= rating) check = 1;  //taking input from the user at the same checking if the value are valid or not
		else {
			while (getchar() != '\n'); // clear input buffer
			printf("Please enter valid input !!!!!\n");
		}
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Fare of bus:                        ");
		if (scanf("%f", &fare) == 1  && fare > 0) check = 1;  //taking input from the user at the same checking if the value are valid or not
		else {
			while (getchar() != '\n'); // clear input buffer
			printf("Please enter valid input !!!!!\n");
		}
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Total seats in bus:                 ");
		if (scanf("%d", &total_seats) == 1 && total_seats > 0) check = 1;  //taking total seat input from the user and at the same time checking if the value or in range or not
		else {
			while (getchar() != '\n'); // clear input buffer
			printf("Please enter valid input !!!!!\n");
		}
	}
	while (!check);

	//below sql command inserts the details to the bus_details table
	sprintf(query, "insert into bus_details (bus_name,owner_id,rating,total_seats) values ('%s',%d,%0.1f,%d)", bus_name, u_id , rating, total_seats);
	mysql_query(conn, query);  //executing sql query
	if (mysql_affected_rows(conn) == 1) //checking if insertion is successfull or not
	{
		//below sql query gets the bus id of the bus which is just inserted in db
		sprintf(query, "select bus_id from bus_details where bus_name='%s' and owner_id='%d' and rating='%0.1f' and total_seats=%d", bus_name, u_id, rating, total_seats);
		mysql_query(conn, query); //executing sql command
		res = mysql_store_result(conn);   //stores the result of the query
		int bus_id = atoi( mysql_fetch_row(res)[0]);  //atoi funcation is used to convert string pointer to integer
		//below sql query inserts the route details to route_details table of the database
		sprintf(query, "insert into route_details (bus_id,from_location,to_location,departure_time,arrival_time,fare) values (%d,'%s','%s','%s','%s',%0.1f)", bus_id, from_location, to_location, departure_time, arrival_time, fare);
		mysql_query(conn, query); //executing sql query
		if (mysql_affected_rows(conn) == 1) {
			printf("\n                   ---------  BUS DETAILS HAS BEEN SUCESSFULLY ADDED  ---------\n");
			check_user_response();
		}
		else printf("                   Unable to add bus !!!!!!!!!\n");
	}
	else printf("                   Unable to add bus !!!!!!!!!\n");
	return 0;
}
void check_user_response() {
	/* this funcation will check if user wants to stayd in program or he/she want to quit */
	int command;
	printf("\nEnter 99 to go back : ");
	if (scanf("%d", &command) == 1 && command == 99)
	{
		printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	}
	else {
		printf("\n                 ----------------------------------------------------------\n");
		printf("                 -----------    You have entered wrong input    -----------\n");
		printf("                 -----------------    EXITING PROGRAM    ------------------\n");
		exit(0);
	}
}
void mysql_booking_data_printer(char str[300]) {
	/* this function will fetch the result from mysql database and prints it beautifully on screen */
	mysql_query(conn, str);
	// printf("%s\n",str );
	res = mysql_store_result(conn);   //stores the result of the query
	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	printf("--------------------------------------------------------------------------------------------------\n");
	printf(" BUS NUMBER      BUS NAME          SOURCE            DESTINATION       SEAT NUMBER       DATE    \n");
	printf("--------------------------------------------------------------------------------------------------\n");
	while (row = mysql_fetch_row(res)) {
		printf("  %-14s %-17s %-17s %-17s %-14s %-10s\n", row[0], row[1], row[2], row[3], row[4], row[5], row[6]);
	}
	check_user_response();  //this will ask user if he wants to continue or exit the program
}
void manage_booking(int u_id) {
	/* this funcation will show the user their complted ,upcoming and canceld ticket */
	int choice = 1; //stores the choice of user to display according
	char str[300];  //stores mysql command to execute
	char * today_date = return_today_date(); //stores todays date
	// printf("%s\n",today_date);
	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	while (1) { //starting while loop so to handle if the user has entered unexpected data
		printf("\e[1;1H\e[2J");    //this will clear the terminal screen
		printf("==================================================================================================\n");
		printf("                                          MANAGE ACCOUNT                                          \n");
		printf("==================================================================================================\n");
		printf("\nEnter number to show details: \n");
		printf("1. Upcomig Journey\n");
		printf("2. Completed Journey\n");
		printf("3. Canceled Journey\n");
		printf("99. To go back\n");
		printf("\nEnter your choice: ");
		// scanf("%d",&choice);
		if (scanf("%d", &choice) == 1 && (choice > 0 && choice < 4 || choice == 99) )
		{
			switch (choice) { //started switch cased based on data by user to print mysql info on screen
			case 1:
				// this will store the sql comand for fetching the data of upcoming journey
				sprintf(str, "SELECT bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.journey_date > '%s'", u_id, today_date);
				break;
			case 2:
				// this will store the sql comand for fetching the data of completed journey
				sprintf(str, "SELECT bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.journey_date < '%s'", u_id, today_date);
				break;
			case 3:
				// this will store the sql comand for fetching the data of canceled ticket
				sprintf(str, "SELECT bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.cancel_status=1", u_id);
				break;
			case 99:
				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("                     -------------     Got Out Of Manage Account       -------------\n");
				break;
			default:
				printf("Input out of range !!!!\n");  //this will print on screen if user has entered greater than 3 or other than 99
			}
			if (str[0] != '\0')
			{
				mysql_booking_data_printer(str); //this will print the data on the screen returned by mysql
			}
			break;  //getting out of while loop
		}
		else {
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
	sprintf(date, "%d-%d-%d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
	return date; // Return pointer to string
}

void cancel_seat(int u_id) {
	char str[300];   //stores sql query to execute
	int i = 0;  //initializing iteration for printing
	char * today_date = return_today_date(); //stores todays date
	int default_size = 1;   //defining default size of array to store booking id
	int cancel_seat_no;   //stores the seat no to cancel
	char cancel_surity;   //stores data to ensure user actually want to cancel the ticket
	int* booking_id_array = (int*) malloc(default_size * sizeof(default_size)); // Allocate initial memory for array
	//below sql code gets booking id, bus id, bus name, source location, destination location, seat no , and journey date from the database which is after todays date
	sprintf(str, "SELECT bk.booking_id,bd.bus_id, bd.bus_name, rd.from_location, rd.to_location, bk.seat_no, bk.journey_date FROM bus_details bd JOIN route_details rd ON bd.bus_id = rd.bus_id JOIN booking_details bk ON bd.bus_id = bk.bus_id  WHERE bk.u_id=%d and bk.journey_date > '%s' and bk.cancel_status=0", u_id, today_date);
	mysql_query(conn, str);  //execute sql command
	res = mysql_store_result(conn);   //stores the result of the query
	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	printf("--------------------------------------------------------------------------------------------------\n");
	printf("Sl No    BUS NUMBER     BUS NAME       SOURCE         DESTINATION       SEAT NUMBER       DATE    \n");
	printf("--------------------------------------------------------------------------------------------------\n");
	while (row = mysql_fetch_row(res)) {  //this will get each row of data which we get from sql
		booking_id_array[i] = atoi(row[0]); //storind booking id to the array location
		booking_id_array = (int*) realloc(booking_id_array, (i + 1) * sizeof(int)); // Reallocate memory with new size
		printf(" %-7d %-14s %-14s %-14s %-17s %-14s %-10s\n", i + 1, row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
		i++;
	}

	while (1) {
		printf("\nEnter 99 to go back\n");
		printf("Enter the 'Sl No' to cancel the seat: ");
		if (scanf("%d", &cancel_seat_no) == 1)  //checking for right user input
		{
			if (cancel_seat_no <= i && cancel_seat_no > 0 || cancel_seat_no == 99) //checking for valid input range
			{
				if (cancel_seat_no == 99) //checking if user wants to go back
				{
					printf("\e[1;1H\e[2J");    //this will clear the terminal screen
					printf("              --------------      Aborted Ticket Cancellation !!!!      --------------\n");
					break;
				}
				printf("\nAre you sure you want to cancel the seat(y/n): ");
				getchar(); // consume newline character
				scanf("%c", &cancel_surity);  //this will take input from user than he/she wants is sure to cancel the ticket
				if (cancel_surity == 'y')  //checkig for surity
				{
					//below sql code updates the database by changing the cancel status of the ticket
					sprintf(str, "update booking_details set cancel_status=1 where booking_id=%d", booking_id_array[cancel_seat_no - 1]);
					mysql_query(conn, str); //executing sql command
					printf("---- Ticket Cancelled Successfully !!!! ----\n");
					break;
				}
				else {
					printf("\e[1;1H\e[2J");    //this will clear the terminal screen
					printf("              --------------      Aborted Ticket Cancellation !!!!      --------------\n");
				}
				break;
			}
			else printf("\n---- Enter the correct 'Sl No' !!!!! ----\n");
		}
		else {
			while (getchar() != '\n');        //clearing input buffer
			printf("Please enter right value !!!!!!\n");
		}
	}
}

int login(char username[], char password[]) {
	/* This funcation will return the user if login is sussessfull else it will return 0*/
	char str[200]; //this variable will store the mysql command to be executed
	int status; //will store status of login
	int u_id = 0; //will store the user id of user if successful login else 0

	sprintf(str, "select count(*) ,u_id from users where user_name='%s' and password='%s'", username, password); //store the mysql query to str variable
	// printf("%s\n",str );
	mysql_query(conn, str); //running sql query
	res = mysql_store_result(conn);   //stores the result of the query
	row = mysql_fetch_row(res); //fetches the row of the mysql data
	status = atoi(row[0]); //storing user login status
	// printf("%d\n", status);
	if (status == 1) //if status is 1 means users data is correct
	{
		u_id = atoi(row[1]); //storing user id
		return u_id; //returing userid of the user
	}
	else {
		return 0; //returing default value if the users data is incorrect
	}

}

int count_int_length(int num) {
	/* this function returns total length of integer passed */
	int count = 0;
	while (num != 0) {
		num /= 10;
		count++;
	}
	return count;
}

int signup() {
	/* this function will will the user to sign up
	data will be stored in users table of the database */
	char str[200]; //stores the mysql command to execute
	char name[100] = "Subhadeep Kumar", user_name[50] = "Abhishek", email[50] = "subhadeep@gmail.com", password[250] = "subhadeep"; //initiating different variable to store the data in databse
	int age = 18, mob = 1234567890;  //stores age and mobile no of the user
	int check; //stoes value to check if right value is entered
	int buff_size = 50; //default buffer length for fixing the length to be entered by the user

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Enter your name:                    ");
		fgets(name, buff_size, stdin);   //taking name input
		if (name[strcspn(name, "\n")] != '\n') {
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
		name[strcspn(name, "\n")] = '\0';  //setting the last value of character for termination and dealing with buffer over flow
		if (name[0] != '\0') check = 1; // if value is correct than getting out of loop
		else printf("\nPlease enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Enter username you want to create:  ");
		fgets(user_name, buff_size, stdin);   //taking user name input
		if (user_name[strcspn(user_name, "\n")] != '\n') {
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
		user_name[strcspn(user_name, "\n")] = '\0';  //setting the last value of character for termination and dealing with buffer over flow
		if (user_name[0] != '\0') check = 1; // if value is correct than getting out of loop
		else printf("\nPlease enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Enter email address:                ");
		fgets(email, buff_size, stdin);   //taking email input
		if (email[strcspn(email, "\n")] != '\n') {
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
		email[strcspn(email, "\n")] = '\0';  //setting the last value of character for termination and dealing with buffer over flow
		if (email[0] != '\0') check = 1; // if value is correct than getting out of loop
		else printf("\nPlease enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Enter password:                     ");
		fgets(password, buff_size, stdin);   //taking password input
		if (password[strcspn(password, "\n")] != '\n') {
			while (getchar() != '\n'); // clear input buffer
			continue;
		}
		password[strcspn(password, "\n")] = '\0';  //setting the last value of character for termination and dealing with buffer over flow
		if (password[0] != '\0') check = 1; // if value is correct than getting out of loop
		else printf("\nPlease enter valid input !!!!!\n");
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Enter your age:                     ");
		if (scanf("%d", &age) == 1 && age > 0) check = 1;  //taking age input from the user and checking if the value or in range or not
		else {
			while (getchar() != '\n'); // clear input buffer
			printf("Please enter valid input !!!!!\n");
		}
	}
	while (!check);

	check = 0; //Initializing to check for right value entered by ther user
	do {
		printf("Enter mobile number:                ");
		if (scanf("%d", &mob) == 1 && count_int_length(mob) == 10) check = 1; //taking mobile no input from the user and checking if mob no is of 10 digit or not
		else {
			while (getchar() != '\n'); // clear input buffer
			printf("Please enter valid input !!!!!\n");
		}
	}
	while (!check);

	while (getchar() != '\n'); // clear input buffer
	while (1) {
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
	sprintf(str, "insert into users (name,user_name,age,email,mob,password) values ('%s','%s',%d,'%s',%d,'%s')", name, user_name, age, email, mob, password); //storing sql command to create new user
	// printf("%s\n", str);
	mysql_query(conn, str); //executes mysql query
	printf("\e[1;1H\e[2J"); //clearing the terminal screen
	printf("\n\n                      ----- Account has been Successfully created !!!!! -----\n");
	return 0;
}

int change_bus_details(int u_id) {
	/* this function is responsible for change the details of bus, route details or deleting a bus*/
	float fare; //stores fare of the bus in a particular route
	int seats; //stores total no of seats in a particular bus
	char departure_time[6] = "\0\0\0\0\0\0", arrival_time[6] = "\0\0\0\0\0\0"; //stores data about departure and arrival timing of the bus
	char from_location[50], to_location[50]; //stores data about the source and destination location of the bus route
	int choice; //stores data about what variable details you user want to change
	char query[200]; //stores mysql query to be executed
	char surety; //stores data if the user is sure to change details
	int i = 0; //serial no. iteration for printing details of bus
	int default_size = 1; //default size of array
	int* id_array = (int*) malloc(default_size * sizeof(default_size)); // Allocate initial memory for array
	printf("\e[1;1H\e[2J");    //this will clear the terminal screen
	printf("==================================================================================================\n");
	printf("                                         CHANGE BUS DETAILS                                       \n");
	printf("==================================================================================================\n");
	printf("\nSelect the option to change details: \n");
	printf("1. Change bus fare\n");
	printf("2. Change total bus seats\n");
	printf("3. Change arrival/departure time\n");
	printf("4. Change source/destination location\n");
	printf("5. Delete bus\n");
	printf("99. Go back\n");
	while (1) {
		printf("Enter your choice here: ");
		if (scanf("%d", &choice) == 1)
		{
			switch (choice) {
			case 1:
				//below line stores the sql command to fetch bus name, from location, to location and fare of bus where bus owner is user
				sprintf(query, "select bd.bus_name,rd.from_location,rd.to_location,rd.fare,rd.route_id from bus_details bd join route_details rd on bd.bus_id = rd.bus_id where bd.owner_id=%d", u_id);
				mysql_query(conn, query); //running sql query
				res = mysql_store_result(conn); //storing sql result
				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("                        -------------     CHANGE BUS FARE       -------------\n");
				printf("--------------------------------------------------------------------------------------------------\n");
				printf("Sl. No.      Bus Name         Source Location        Destination Location         Fare            \n");
				printf("--------------------------------------------------------------------------------------------------\n");
				while (row = mysql_fetch_row(res)) { //iterating through all the row present in the result of mysql
					id_array[i] = atoi(row[4]); //storing route id of bus in an array
					id_array = (int*) realloc(id_array, (i + 1) * sizeof(int)); // Reallocate memory with new size
					printf("%-12d %-16s %-22s %-28s %s\n", i + 1, row[0], row[1], row[2], row[3], row[4]); //printing sql data
					i++;
				}

				while (1) { //starting loop for handling wrong inputs
					printf("\nEnter the Serial Number of bus whose fare charge you want to change: ");
					if (scanf("%d", &choice) == 1 && (choice <= i && choice > 0 || choice == 99)) //condition to check for valid input
					{
						if (choice == 99) //termination condition
						{
							printf("Transaction Canceled fare not changed !!!!!");
							break;  //getting out of while loop without changeing fare
						}
						while (1) { //starting loop to handle wrong inputs
							printf("\nEnter the new fare price: ");
							char buffer[100];
							if (scanf("%f%s", &fare, &buffer) == 1 && fare > 0)
							{
								printf("\n\n Are you sure you want to change the fare(y/n): ");
								while (getchar() != '\n'); // clear input buffer
								scanf("%c", &surety);
								if (surety == 'y') //checking if user surely wants to change the fare or not
								{
									sprintf(query, "update route_details set fare=%0.2f where route_id=%d", fare, id_array[choice - 1]);
									mysql_query(conn, query);
									printf("\n                        --------  Fare has been successfully changed --------\n\n");
									check_user_response();
								}
								else {
									printf("\e[1;1H\e[2J");    //this will clear the terminal screen
									printf("\n                      --------  Transaction Canceled fare not changed --------\n");
								}
								break;
							}
							while (getchar() != '\n'); // clear input buffer
							printf("Please enter valid input !!!!!\n");
							continue;
						}
						break;
					}
					while (getchar() != '\n'); // clear input buffer
					printf("Please enter valid input !!!!!\n");
					continue;
				}
				break;
			case 2:
				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("                        -------------     CHANGE TOTAL SEATS       -------------\n");
				sprintf(query, "select bus_id,bus_name,total_seats from bus_details where owner_id=%d", u_id); //storing mysql query to fetch bus id, bus name and total bus which is owned by that user
				mysql_query(conn, query); //executing sql query
				res = mysql_store_result(conn); //stroing the result of mysql
				printf("--------------------------------------------------------------------------------------------------\n");
				printf("          Sl No.                               BUS NAME                      TOTAL SEATS          \n");
				printf("--------------------------------------------------------------------------------------------------\n");
				while (row = mysql_fetch_row(res)) { //iterating till last row to print details of bus
					id_array[i] = atoi(row[0]); //storing the bus_id for change the total seats details of that bus
					id_array = (int*) realloc(id_array, (i + 1) * sizeof(int)); // Reallocate memory with new size
					printf("             %-30d %-35s %s\n", i + 1, row[1], row[2]); //printing data
					i++;  //increasing the value of i so that next serial as well as reallocation of memory can be done
				}
				while (1) { //starting loop for handling wrong inputs
					printf("\nEnter the Serial Number of bus whose total seats you want to change: ");
					if (scanf("%d", &choice) == 1 && (choice <= i && choice > 0 || choice == 99)) //condition to check for valid input
					{
						if (choice == 99) //termination condition
						{
							printf("Transaction Canceled total seats not changed !!!!!");
							break;  //getting out of while loop without changing seats
						}
						while (1) { //starting loop to handle wrong inputs
							printf("\nEnter the total new seats: ");
							if (scanf("%d", &seats) == 1)
							{
								printf("\n\n Are you sure you want to change total seats(y/n): ");
								while (getchar() != '\n'); // clear input buffer
								scanf("%c", &surety);
								if (surety == 'y') //checking if user surely wants to change the total seats or not
								{
									//below line stores the sql query to change the total seats provided as by bus owner
									sprintf(query, "update bus_details set total_seats=%d where bus_id=%d", seats, id_array[choice - 1]);
									mysql_query(conn, query); //executing sql command
									printf("\n                      --------  Total seats has been successfully changed --------\n\n");
									check_user_response();
								}
								else {
									printf("\e[1;1H\e[2J");    //this will clear the terminal screen
									printf("                    -------------  Transaction Canceled fare not changed   -------------\n");
								}
								break;
							}
							while (getchar() != '\n'); // clear input buffer
							printf("Please enter valid input !!!!!\n");
							continue;
						}
						break;
					}
					while (getchar() != '\n'); // clear input buffer
					printf("Please enter valid input !!!!!\n");
					continue;
				}
				break;
			case 3:
				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("                  -------------     CHANGE ARRIVAL/DEPARTURE TIME       -------------\n");				
				char **depart_time_array = malloc(default_size * sizeof(char *));   // Allocate initial memory for array to store departure time of bus
				char **arrival_time_array = malloc(default_size * sizeof(char *));  // Allocate initial memory for array to store arrival time of bus
				//below is sql command which fetch route id, bus name, source location, destination location, departure time, arrival time of the bus to modify the time deatils if required
				sprintf(query, "select rd.route_id,bd.bus_name,rd.from_location,rd.to_location,DATE_FORMAT(rd.departure_time, '%%H:%%i'),DATE_FORMAT(rd.arrival_time, '%%H:%%i') from bus_details bd join route_details rd on bd.bus_id=rd.bus_id where bd.owner_id=%d", u_id);
				mysql_query(conn, query); //executing sql query
				res = mysql_store_result(conn); //storing sql result
				printf("--------------------------------------------------------------------------------------------------\n");
				printf("Sl No.  BUS NAME       SOURCE LOCATION    DESTINATION LOCATION     DEPARTURE TIME     ARRIVAL TIME\n");
				printf("--------------------------------------------------------------------------------------------------\n");
				while (row = mysql_fetch_row(res)) { //iterating through all the row provided by sql of the details of bus
					id_array[i] = atoi(row[0]); //storing the bus_id for change the arrival/departure details of that bus
					depart_time_array[i] = row[4]; //storing the departure time of bus
					arrival_time_array[i] = row[5]; //storing the arrival tiem of bus
					id_array = (int*) realloc(id_array, (i + 1) * sizeof(int)); // Reallocate memory with new size
					depart_time_array = realloc(depart_time_array, (i + 2) * sizeof(char *)); //rellocate memory with new size
					arrival_time_array = realloc(arrival_time_array, (i + 2) * sizeof(char *)); //reallocate memory with new size
					printf(" %-6d %-16s %-20s %-23s %-18s %s \n", i + 1, row[1], row[2], row[3], row[4], row[5] ); //printing details
					i++;  //increasing the value for next iteration
				}
				while (1) { //starting loop for handling wrong inputs
					printf("\nEnter the Serial Number of bus whose arrival/departure time you want to change: ");
					if (scanf("%d", &choice) == 1 && (choice <= i && choice>0 || choice == 99)) //condition to check for valid input
					{
						if (choice == 99) //termination condition
						{
							printf("Transaction Canceled arrival/departure time not changed !!!!!");
							break;  //getting out of while loop without changing arrival/departure time
						}
						while (getchar() != '\n'); // clear input buffer
						printf("\nEnter new departure time (HH:MM, hit enter to skip): ");
						gets(departure_time);
						if (departure_time[0] == '\0' || departure_time[0] == '\n') {
							/*copying the data of the array to a variable
							 where strcspn is fetching the first new line character in the data and getting with index
							 strncpy is copying the data to new varibale */
							// departure_time=depart_time_array[choice-1];
							strncpy(departure_time, depart_time_array[choice - 1], strcspn(depart_time_array[choice - 1], "\n"));
						}
						printf("\nEnter new arrival time (HH:MM, hit enter to skip): ");
						gets(arrival_time);
						if (arrival_time[0] == '\0' || arrival_time[0] == '\n') {
							// copying the data from the array and storing the data pointer to a variable
							// arrival_time=arrival_time_array[choice-1];
							strncpy(arrival_time, arrival_time_array[choice - 1], strcspn(arrival_time_array[choice - 1], "\n"));
						}
						// storing sql command to query variable which will update the required value to the database
						sprintf(query, "update route_details set departure_time='%s', arrival_time='%s' where route_id=%d", departure_time, arrival_time, id_array[choice - 1]);
						// printf("%s\n",query );
						mysql_query(conn, query); //executing sql command
						int check_consistency = mysql_affected_rows(conn);  //checking if query ran successfully or not this funcation will return -1 if error in command , 0 if there is no row affected else total now affected
						if (check_consistency >= 1) printf("\n--------  Departure/Arrival time has been successfully changed --------\n\n"); //printing this details if any row is getting updated
						else printf("\n--------  Transaction Canceled arrival/departure time not changed !!!!! --------\n\n");  //printing if there is not change in database
						break;  //getting out of switch case
					}
					while (getchar() != '\n'); // clear input buffer
					printf("Please enter valid input !!!!!\n");
					continue;
				}
				break;
			case 4:
				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("                -------------     CHANGE ARRIVAL/DEPARTURE LOCATION       -------------\n");				
				char **source_location_array = malloc(default_size * sizeof(char *));   // Allocate initial memory for array to store departure time of bus
				char **destination_location_array = malloc(default_size * sizeof(char *));  // Allocate initial memory for array to store arrival time of bus
				//below is sql command which fetch route id, bus name, source location, destination location, departure time, arrival time of the bus to modify the time deatils if required
				sprintf(query, "select rd.route_id,bd.bus_name,rd.from_location,rd.to_location,DATE_FORMAT(rd.departure_time, '%%H:%%i'),DATE_FORMAT(rd.arrival_time, '%%H:%%i') from bus_details bd join route_details rd on bd.bus_id=rd.bus_id where bd.owner_id=%d", u_id);
				mysql_query(conn, query); //executing sql query
				res = mysql_store_result(conn); //storing sql result
				printf("--------------------------------------------------------------------------------------------------\n");
				printf("Sl No.  BUS NAME       SOURCE LOCATION    DESTINATION LOCATION     DEPARTURE TIME     ARRIVAL TIME\n");
				printf("--------------------------------------------------------------------------------------------------\n");
				while (row = mysql_fetch_row(res)) { //iterating through all the row provided by sql of the details of bus
					id_array[i] = atoi(row[0]); //storing the bus_id for change the arrival/departure details of that bus
					source_location_array[i] = row[2]; //storing the source location of bus in array
					destination_location_array[i] = row[3]; //storing the destination location of bus in array
					id_array = (int*) realloc(id_array, (i + 1) * sizeof(int)); // Reallocate memory with new size
					source_location_array = realloc(source_location_array, (i + 2) * sizeof(char *)); //rellocate memory with new size
					destination_location_array = realloc(destination_location_array, (i + 2) * sizeof(char *)); //reallocate memory with new size
					printf(" %-6d %-16s %-20s %-23s %-18s %s \n", i + 1, row[1], row[2], row[3], row[4], row[5] ); //printing details
					i++;  //increasing the value for next iteration
				}
				while (1) { //starting loop for handling wrong inputs
					printf("\nEnter the Serial Number of bus whose source/destination date you want to change: ");
					if (scanf("%d", &choice) == 1 && (choice <= i || choice == 99)) //condition to check for valid input
					{
						if (choice == 99) //termination condition
						{
							printf("Transaction Canceled source/destination date not changed !!!!!");
							break;  //getting out of while loop without changing arrival/departure time
						}
						while (getchar() != '\n'); // clear input buffer
						printf("\nEnter new source location time (hit enter to skip): ");
						gets(from_location);
						if (from_location[0] == '\0') {
							/*copying the data of the array to a variable
							 where strcspn is fetching the first new line character in the data and getting with index
							 strncpy is copying the data to new varibale */
							strncpy(from_location, source_location_array[choice - 1], strcspn(source_location_array[choice - 1], "\n") + 1);
						}
						printf("\nEnter new departure location time (hit enter to skip): ");
						gets(to_location);
						if (to_location[0] == '\0') {
							// copying the data from the array and storing the data pointer to a variable
							strncpy(to_location, destination_location_array[choice - 1], strcspn(destination_location_array[choice - 1], "\n") + 1);
						}
						// storing sql command to query variable which will update the required value to the database
						sprintf(query, "update route_details set from_location='%s', to_location='%s' where route_id=%d", from_location, to_location, id_array[choice - 1]);
						// printf("%s\n",query );
						mysql_query(conn, query); //executing sql command
						int check_consistency = mysql_affected_rows(conn);  //checking if query ran successfully or not this funcation will return -1 if error in command , 0 if there is no row affected else total now affected
						if (check_consistency >= 1) printf("\n--------  Source/Destination location has been successfully changed --------\n\n"); //printing this details if any row is getting updated
						else printf("\n--------  Transaction Canceled Source/Destination location not changed !!!!! --------\n\n");  //printing if there is not change in database
						break;  //getting out of switch case
					}
					while (getchar() != '\n'); // clear input buffer
					printf("Please enter valid input !!!!!\n");
					continue;
				}
				break;
			case 5:
				//below sql command gets all the bus that is owned by the user
				sprintf(query, "select bus_name,bus_id from bus_details where owner_id=%d", u_id);
				// printf("%s\n",query );
				mysql_query(conn, query); //executing sql query
				res = mysql_store_result(conn); //storing sql result
				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("--------------------------------------------------------------------------------------------------\n");
				printf("                             Sl No.               BUS NAME                                        \n");
				printf("--------------------------------------------------------------------------------------------------\n");
				while (row = mysql_fetch_row(res)) {
					id_array[i] = atoi(row[1]); //storing the bus_id for deleting the bus
					printf("                             %-21d %s \n", i + 1, row[0]); //printing details
					i++; //incrementaing the interation for serializing
				}
				while (1) {
					printf("\nEnter the Sl. No. of bus which you want to delete: ");
					if (scanf("%d", &choice) == 1 && (choice <= i || choice == 99)) //condition to check for valid input
					{

						if (choice == 99) //termination condition
						{
							printf("Transaction Canceled source/destination date not changed !!!!!");
							break;  //getting out of while loop without deleting bus
						}
						while (getchar() != '\n'); // clear input buffer
						printf("\n\n Are you sure you want to change total seats(y/n): ");
						scanf("%c", &surety);
						if (surety == 'y') //checking if user surely wants to delete the bus or not
						{
							int bus_id = id_array[choice - 1];
							//below sql code will delete all the bus details from bus_details table
							sprintf(query, "delete from bus_details where bus_id=%d", bus_id);
							mysql_query(conn, query); //executing sql command
							//below sql code will delete all the route detials of the bus that user want to delete
							sprintf(query, "delete from route_details where bus_id=%d", bus_id);
							mysql_query(conn, query); //executing sql command
							printf("\n--------  Bus details has been successfully deleted --------\n\n");
						}
						else printf("Transaction Canceled bus not deleted!!!!!\n");
						break;
					}
					while (getchar() != '\n'); // clear input buffer
					printf("Please enter valid input !!!!!\n");
					continue;
				}
				break;
			case 99:
				printf("\e[1;1H\e[2J");    //this will clear the terminal screen
				printf("              -------------     Got Out Of Change Bus Details       -------------\n");
				break;
			default:
				printf("\nPlease enter valid details !!!!!\n");
				continue;
			}
			break;
		}
		while (getchar() != '\n'); // clear input buffer
		printf("\nPlease enter valid details!!!!!\n");
		continue;
	}
	return 0;
}
int change_permission(int u_id) {
	/* this function is responsible to change the permission of different users */
	int id = 0; //stoes id of users to change the permission levels
	char query[100]; //stores mysql command
	int permission_level = 0;  //stores the permission level to set for the user
	while (1) {  //entering the while loop to handle the error
		printf("\nEnter the user id you want to change the permission level: ");
		if (scanf("%d", &id) == 1)  //checking for only integer input by the user
		{
			//below sql command checks if there is present in the database with the id given as an input
			sprintf(query, "select count(u_id) from users where u_id=%d", id);
			mysql_query(conn, query); //executing sql command
			res = mysql_store_result(conn); //storing the result of sql
			row = mysql_fetch_row(res); //getting the first row of sql
			if (atoi(row[0]) == 1) //checking if there is user or not with the user id given in input
			{
				while (1) { //starting loop to handle errors
					printf("Enter the permssion level for the user:- \n");
					printf("Level 1 : Normal User\n");
					printf("Level 2 : Bus Owner\n");
					printf("Level 3 : Administrator\n");
					printf("Enter 99 to exit\n");
					printf("Enter the permission level in numeric value: ");
					if (scanf("%d", &permission_level) == 1 && permission_level > 0 && (permission_level < 4 || permission_level == 99)) //checking conditions to verify if the users has given the right permssion level to enter
					{
						if (permission_level == 99)  //if user enters 99 get out of permssion change module
						{
							printf("Aborted permission change\n");
							break;
						}
						//below sql command updates the persmssion of the u_id given by the user
						sprintf(query, "update users set permission=%d where u_id=%d", permission_level, id);
						mysql_query(conn, query); //executing sql command
						int check = mysql_affected_rows(conn); //getting total affected row in the db to check if the effect was successful or not
						if (check == 1) printf("Permission changed successfully !!!!!!!!!\n"); //checking for successfull effect
						else printf("Failed to change permission !!!!!!!!\n"); //printing if changes get failed
						break;
					}
					while (getchar() != '\n'); // clear input buffer
					printf("\nPlease enter valid input !!!!!");
				}
				break;
			}
			else {
				printf("Invalid user id !!!!!!!!\n");
				continue;
			}
		}
		while (getchar() != '\n'); // clear input buffer
		printf("\nPlease enter valid input !!!!!");
	}
}
int check_permission_level(int u_id) {
	/* This funcation returns the permission level of the user */
	char query[50];  //stores sql query to execute
	//below sql query gets the permission level of user
	sprintf(query, "select permission from users where u_id=%d", u_id);
	mysql_query(conn, query); //executing sql command
	res = mysql_store_result(conn); //storing the result of sql
	row = mysql_fetch_row(res); //geting the row from the sql
	return atoi(row[0]); //returning the value in the form of integer
}
void welcome() {
	/* this will be displayed when user just opens the application */
	printf("\e[1;1H\e[2J"); //clearing terminal screen
	printf("==================================================================================================\n");
	printf("======                     WELCOME TO ELECTRIC BUS TICKET GENERATOR                         ======\n");
	printf("==================================================================================================\n");
	printf("*********************************************************************************\n");
	printf("**                               **                                            **\n");
	printf("**  -------------------------    **         ---------------------------------  **\n");
	printf("**  -  Already a member   -      **         -   Don't have a account yet?   -  **\n");
	printf("**  - Press 1 for 'Login' -      **         - Press 2 to 'Create a account' -  **\n");
	printf("**  -------------------------    **           -------------------------------  **\n");
	printf("**                               **                                            **\n");
	printf("*********************************************************************************\n");
	printf("\n\n\n           *****************************\n");
	printf("           ***  Press 'q' for Exit   ***\n");
	printf("           *****************************\n");
}
int login_signup_page_control() {
	while (1) {
		char choice[2];         //Creating string for user input
		printf("\nEnter your Choice: ");
		scanf("%s", &choice);      //taking input from user for Login create account or Quit

		char valid1[] = "1";        //Declaring String for Login
		char valid2[] = "2";        //Declaring String for Create account
		char valid3[] = "q";        //String for Quit
		if (strcmp(valid1, choice) == 0 )           //Comparision for Login
		{
			printf("\e[1;1H\e[2J");
			char u_id[50], password[50];
			printf("Enter your user id: ");
			scanf("%s", &u_id);
			printf("Enter your password: ");
			scanf("%s", &password);
			int uid_status = login(u_id, password);
			if (uid_status == 0)
			{
				printf("\e[1;1H\e[2J");                 //clearing terminal screen
				printf("\n!!!!!! You have entered wrong username/password !!!!!!!");
				welcome();                              //calling welcome function
				continue;
			}
			else return uid_status;                         //calling login function
		}
		else if (strcmp(valid2, choice) == 0)       //Comaprision for Create Account
		{
			printf("\e[1;1H\e[2J");
			getchar();                              //to remove input buffer
			signup();                               //calling signup function
			welcome();                              //calling welcome function
		}
		else if (strcmp(valid3, choice) == 0)       //Comparision for Quit statement
		{
			printf("\e[1;1H\e[2J");
			printf("                    ****************************\n");
			printf("                    ****      Visit Again    ***\n");
			printf("                    ****       Take Care     ***\n");
			printf("                    ****************************\n");
			exit(0);
		}
		else                                      // If other than 1 or 2 or Q pressed
		{
			printf("                                  Invalid Input.          \n");
		}
	}
}
void homepage(int u_id, int permission_level) {
	printf("\e[1;1H\e[2J");                 //clearing terminal screen
	while (1) {
		printf("==================================================================================================\n");
		printf("======                     WELCOME TO ELECTRIC BUS TICKET GENERATOR                         ======\n");
		printf("==================================================================================================\n");
		printf("\n\n\n");
		printf("            [1.] Book Ticket                                          [2.] Cancel Ticket \n");
		printf("            [3.] Seat Availabiliy                                     [4.] Manage booking\n");
		if (permission_level == 1)  printf("            [99.] Exit.\n");
		else if (permission_level == 2)
		{
			printf("            [5.] Add Bus                                              [6.] Change Bus Details.      \n");
			printf("            [99.] Exit.\n\n");
		}
		else
		{
			printf("            [5.] Add Bus                                              [6.] Change Bus Details.      \n");
			printf("            [7.] Change permission level.                             [99.] Exit.\n\n");
		}
		int command;
		printf("\n       Enter Choice: ");
		if (scanf("%d", &command) == 1 && (((permission_level == 1 && command < 5) || (permission_level == 2 && command < 7) || (permission_level == 3 && command < 8)) && command > 0 || command == 99) )
		{
			switch (command) {
			case 1:
				seat_availability(u_id);
				break;
			case 2:
				cancel_seat(u_id);
				break;
			case 3:
				seat_availability(u_id);
				break;
			case 4:
				manage_booking(u_id);
				break;
			case 5:
				add_bus(u_id);
				break;
			case 6:
				change_bus_details(u_id);
				break;
			case 7:
				change_permission(u_id);
				break;
			case 99:
				printf("\n    --------- Exiting Program --------------\n");
				exit(0);
			}
			// break;
		}
		else {
			printf("\e[1;1H\e[2J");                 //clearing terminal screen
			printf("                       ----------------   Invalid Input.   ----------------       \n");
			while (getchar() != '\n'); // clear input buffer
			continue;
		};

	}

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
	// printf("\e[1;1H\e[2J");    //this will clear the terminal screen

	int u_id = login("Abhishek", "abhishek");  //sending userid or password to login function to get user id if successful login or 0
	// printf("%d\n", uid_status);
	// signup(); //started signup module if the user wants to register into the software

	// seat_availability(uid_status);

	// add_bus(uid_status);

	// manage_booking(uid_status);

	// cancel_seat(uid_status);

	// change_bus_details(uid_status);

	// change_permission(uid_status);

	// welcome();
	// int u_id = login_signup_page_control() ;
	// // printf("%d\n",response );
	int permission_level = check_permission_level(u_id);
	homepage(u_id, permission_level);
	// printf("%d\n",permission_level );

	return 0;
}