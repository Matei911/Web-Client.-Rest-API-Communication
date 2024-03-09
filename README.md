# Web-Client-Rest-API-Communication

This project focuses on a client-side application developed in C, designed to interact with a server via REST API. It utilizes JSON parsing through the `parson.h` library, facilitating a command-line interface for a library management system. The choice of `parson.h` for JSON parsing is attributed to its simplicity and ease of integration into the project code.

## Key Components and Functionalities

The application is encapsulated in `client.c`, segmented into distinct functions corresponding to user commands:

### Main Loop
- **Functionality:** Continuously listens for commands inputted via the keyboard.
- **Exit Condition:** The loop terminates upon receiving the 'exit' command.

### Register Command
- **Functionality:** Utilizes `create_user_json_string` to register a new user, verifying the user's existence on the server.
- **Server Interaction:** Handles server responses to register requests.

### Login Command
- **Functionality:** Employs `create_user_json_string` for user authentication, updates the session cookie, and flags a successful connection (`successful_connex`).
- **Authentication:** Confirms correctness of user input and updates login status.

### Logout Command
- **Pre-Check:** Verifies an existing connection via `successful_connex`.
- **Server Notification:** Informs the server of the logout action and displays a confirmation message.

### Enter Library
- **Access Control:** Ensures user is logged in and not already in the library using `successful_connex` and `already_in`.
- **Library Entry:** Parses server-provided token for library access, marking entry with `already_in = 1`.

### Get Books
- **Library Check:** Confirms user presence in the library.
- **Display:** Shows all books retrieved from the server.

### Get Book
- **Functionality:** Similar to `get_books`, but focuses on displaying a single book's details based on its ID.
- **Error Handling:** Manages non-existent or invalid book IDs with appropriate messages.

### Add Book
- **Library Check:** Verifies user's library access.
- **Book Creation:** Assembles a new book object with required data, ensuring validity of the number of pages before submission to the server.

### Delete Book
- **Library Check:** Confirms user's presence in the library.
- **ID Verification:** Checks for numeric ID format and validates the book's existence on the server prior to deletion.

## Project Highlights

- **JSON Parsing:** Efficiently managed through the `parson.h` library for seamless server communication.
- **User-Friendly CLI:** Designed for ease of use with clear command functionalities.
- **Robust Error Handling:** Ensures smooth user experience and operational integrity.
