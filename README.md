# Bus Management System
The goal of the Bus Management System software is to provide a software solution that can automate and streamline a bus transportation company's daily operations. The management of bus schedules, ticket sales, passenger data, and bus maintenance responsibilities will be made simpler by the software. The objective is to develop a thorough system that can maximise the effectiveness and financial success of the bus transportation industry. The programme enables transportation businesses to increase operational effectiveness, cut costs, and deliver better customer service by offering a centralised platform for bus scheduling, ticket sales, passenger information, and bus maintenance.

## Installation

You must be having [MySQL](https://dev.mysql.com/downloads/installer/) database to store the data into it.

For linux User user MariaDB

Use below command to create a database in mysql

```bash
mysql -u abhishek -p bus_management_system < bus_management_system_no_data.sql

```

Use below command to compile and run the program

```bash
gcc -o bus_client_system bus_client_system.c `mysql_config --cflags --libs`
```


## Usage


```bash
./bus_client_system
```
## Welcome Page
![iamge of welcome page](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2018_41_34.png?raw=true)

## Home Page
![iamge of welcome page](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2018_41_47.png?raw=truee)

## Module Description

### 1. Login Module.

Module Purpose:- Responsible for login the existing customer i.e if a user has a existing account can login to acess the page.

Module Dependencies: - Database for matching userid and password.

Module Input:- User id and password. 

Module Output :-Login to page if input data is correct else return invalid input.

![iamge of welcome page](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-14%2000_50_49.png?raw=true)

### 2. Signup module

Module Purpose:- Responsible for create account for new user i.e. if user is new to the system can create the account entering the required information.

Module Dependencies: Database for storing the data of new user and login module.

Module Input:- User name, User id, password, Age, Mobile no., Address.

Module Output:-Redirect to login page after creating account to make them login.

![iamge of welcome page](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-14%2000_55_58.png?raw=true)

### 3. Seat availability

Module Purpose:- To let user know about available seat i.e if user want to book a ticket either single or in group can see the available seat, he can see the available seats.

Module Dependencies:-Database to fetch available seat.

Module Input:-Date journey,Source and Destination location

Module Output:-Display available seats for enterd location.

![iamge of seat available](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2018_42_19.png?raw=true)

### 4. Book Ticket

Module Purpose:-To let user to book Availble seat and seat availability module i.e. user can book their desired available seat for the journey.

Module Dependencies:-Database to book selected seat.

Module Input:- enter desired seat to book.

Module Output:-After booking, pop-up showing that seat booked.

### 5. Cancel Ticket

Module Purpose:-To let user cancel booked ticket i.e if a user want to cancel the booked ticket he can cancel by selecting the seat.

Module Dependencies:-Database to delete booked seat and cancel the reservation.

Module Input:-User select the input for cancel the ticket.

Module Output:- Pop-up showing ticket canceled.


![iamge of seat available](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2018_56_44.png?raw=true)

### 6. Manage account

Module Purpose:-To let user know their completed journey, upcoming journey and cancel journey i.e user can see the details of journey he did and give the feedback of the journey.

Module Dependencies:-Database to fetch detail and Cancel ticket module.

Module Input:-User to see their journey and cancel the ticket.

Module Output:-User will see their journey and and cancel ticket.


![manage account iamge](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2018_58_16.png?raw=true)

### 7. Add Bus

Module Purpose:-To add new bus to the system i.e if bus owner or administrator want a new bus over a new route or existing route he can add a new bus on that route.

Module Dependencies:-Depend on Database to store data for new bus.

Module Input:-Bus name, source location, destination location,arriving time, departure time, fair charge and total seat, rating.

Module Output:-Show that a new bus has been added.


![add bus iamge](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2019_00_12.png?raw=true)

### 8. Change bus details

Module Purpose:-To modify the bus details for the journey i.e. bus owner o adminstrator can change the arival departure , fair charge etc of existing bus.

Module Dependencies:-Database to update the details of the bus.

Module Input:-Change bus name,bus seat, source and setination location,delete bus.

Module Output:-Show the changes done.

![add bus iamge](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2019_00_23.png?raw=true)

### 9. Change Permission Level

Module Purpose:-To chage permission level so that a limited features will be provided to different level of users. i.e. user bus owner or adminstrator respectively as 1,2 and 3.

       Level 1 : Normal User
       Level 2 : Bus Owner
       Level 3 : Administrator
Module Dependencies:-Depend on database for changing the level of user i.e Normal user , Bus owner and Adminstator.

Module Input:-User id by user to ectract level of user.

Module Output:- Output show as below:

  normal user: book seat, cancel ticket, add bus,login.

   Adminstrator or Bus owner: along with module of normal user, add bus, modify bus and permission level.


![add bus iamge](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/main/sample_images/Screenshot%202023-04-13%2019_14_44.png?raw=true)
## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License

[MIT](https://github.com/codebyAbhishekBharti/bus_mangement_system/blob/729a0372d49d67209adf2390f9a967b2400ac96c/LICENSE)
