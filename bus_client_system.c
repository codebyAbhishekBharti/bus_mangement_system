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
	for (int i = 0; i < num_fields+1; ++i)
	{
		bus_list[i]="0";
	}
	int y=0;
	// printf("%s",mysql_fetch_row(res)[2]);
	while (row = mysql_fetch_row(res)) {
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
	for (int i = 0; i <= total_bus; ++i)
	{
		if (strcmp(bus_list[1],"0")!=0) return 1;
	}
	return 0;
}
void seat_availability() {
	/* This funcation will help to show the lists of all the vacant seats for a particular route */
	char date[11] = "2023-03-08", source_location[50] = "Amritsar", destination_location[50] = "Jalandhar";
	// printf("Enter the journey date (2023-03-08) :- ");
	// scanf("%[^\n]%*c",&date);
	// printf("Enter the source location:- ");
	// scanf("%[^\n]%*c",&source_location);
	// printf("Enter the destination location:- ");
	// scanf("%[^\n]%*c",&destination_location);
	// printf("%s %s %s\n ",date,source_location,destination_location);

	char** bus_list = list_of_bus(source_location, destination_location);; // call the function and get the string array pointer
	int total_bus=sizeof(bus_list)/sizeof(bus_list[0]);
	int is_bus_available = check_available_bus_or_not(total_bus,bus_list);
	// printf("%d\n",is_bus_available);
	if (is_bus_available!=0)
	{
		printf("==================\n");
		printf("Sl.No.    Bus Name\n");
		printf("==================\n");
		for (int i = 0; i <= total_bus; i++) {
		    printf("%d         %s \n",i+1, bus_list[i]);
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