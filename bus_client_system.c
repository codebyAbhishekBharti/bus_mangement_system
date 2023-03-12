#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

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
int check_existance_in_array(int num,int arr[],int total_seats){
	/* This funcation will check if a number is present in the array or not
	if number will be in the array then it will return 1
	else it will return 0 */
	for (int i = 0; i <= total_seats; ++i)
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
	// sprintf(str,"select count(booking_id) from booking_details where bus_id=%d and booking_date='%s' and seat_no=%s and cancel_status=0",bus_id,date,token);
    while (token != NULL) {
    sprintf(str,"select count(booking_id) from booking_details where bus_id=%d and booking_date='%s' and seat_no=%s and cancel_status=0",bus_id,date,token);
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
int book_ticket(int bus_id,char date[11],int u_id){
	/* this funcation will help to book the desired seat by the passenger */
	// printf("%d %s %d\n",bus_id,date,u_id);
	char seats[200]="20",str[200];
	// printf("\n*Enter the seat not one space seperated if you want to book multiple seat. \n");
	// printf("Enter the seat no : ");
	// scanf("%[^\n]%*c",seats);
	// printf("%s",seats);
	char *token = strtok(seats, " ");
	char *token_copy = token;
	char *token_copy2= token;

    // insert into booking_details (bus_id,booking_date,u_id,seat_no) values (1,'2023-03-08',1,5);
    sprintf(str,"select total_seats from bus_details where bus_id=%d",bus_id);
    mysql_query(conn, str);
	res=mysql_store_result(conn);
	int total_seats=atoi(mysql_fetch_row(res)[0]);
    // printf("%d",checked_status_already_booked);
    int checked_seat_range=check_range_seat(total_seats,token_copy2);
    int checked_status_already_booked=check_already_booked(bus_id,date,token_copy);
    if (checked_seat_range==0)
    {
	    if (checked_status_already_booked==0)
	    {
			while (token != NULL) {
			    // printf("%s\n", token);
			    sprintf(str,"insert into booking_details (bus_id,booking_date,u_id,seat_no) values (%d,'%s',%d,%s)",bus_id,date,u_id,token);
				mysql_query(conn, str);
			    token = strtok(NULL, " ");
			}	
			printf("\n--- Congratulation! Your seats has been booked ---");
	    }
	    else printf("\n--- Sorry! Your selected seat are already booked ---\n");
	}
	else printf("\n--- Sorry! Please enter the correct Seat number ---\n");
}

void seat_availability() {
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

		//select seat_no from booking_details where bus_id=1 and  booking_date='2023-03-08' and cancel_status=0;
		sprintf(str,"select seat_no from booking_details where bus_id=%d and  booking_date='%s' and cancel_status=0",main_bus_code,date);
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
			book_ticket(main_bus_code,date,1);
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

	seat_availability();
	
	// add_bus();

	return 0;
}