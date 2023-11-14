# Web-Client.-Rest-API-Communication
The project revolves around a client-side application written in C that interacts with a server using JSON parsing via the parson.h library. 
This application serves as a library management system, allowing users to perform various commands through a command-line interface.

For parsing JSON, I've chosen the parson.h library as it seemed easy to use in the code.

The entire code is written in client.c and is divided into functions for each command

-main: In main, there's a continuous while loop that takes commands from the keyboard and enters the necessary function for each. 
       It stops when the 'exit' command is encountered.
       
-register_command: We use the create_user_json_string function to create a new user and check if it already exists on the server. 
                   We handle the server's response accordingly.
                   
-login_command: We use the create_user_json_string function to create the user, verify if the entered data is correct, update the cookie, 
                and store in a variable (successful_connex) that we are logged into the server.
                
-logout: Using the successful_connex variable, we first check if we are connected. 
         If we are, we send the information to the server and display an appropriate message.
         
-enter_library: In this function, we enter the library. First, we check if we are logged in and if we are already in the library, using successful_connex and already_in. 
                If it's our first time accessing the library, we parse the content of the token received from the server and remember that we've entered the library (already_in = 1).
                
-get_books: We check if we are in the library; if yes, we display all the books received from the server.

-get_book: It has the same functionality as get_books, but it displays only one book with its information based on the assigned ID.
           We handle cases where the book ID doesn't exist or isn't a number, displaying the respective errors.
           
 -add_book: We check if we are in the library. If yes, we create a new book with all the mentioned data as per the requirements and verify that the number of pages is valid. 
            Then we send the new object to the server.
            
-delete_book: Again, we check if we are in the library. If yes, we verify that the ID is in numeric form and finally, as with get_book, we check if that ID exists on the server.
