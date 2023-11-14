#include <stdio.h>		/* printf, sprintf */
#include <stdlib.h>		/* exit, atoi, malloc, free */
#include <unistd.h>		/* read, write, close */
#include <string.h>		/* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>		/* struct host_ipent, gethost_ipbyname */
#include <arpa/inet.h>
#include <stdbool.h>
#include <ctype.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"


char *create_user_json_string(char *username, char *password)
{
	JSON_Value *val = json_value_init_object();
	JSON_Object *obj = json_value_get_object(val);
	json_object_set_string(obj, "username", username);
	json_object_set_string(obj, "password", password);
	char *user = json_serialize_to_string(val);
	return user;
}
void get_credentials(char *username, char *password)
{
	printf("username=");
	scanf("%s", username);
	printf("password=");
	scanf("%s", password);
}
void register_command(int socket, char *host_ip, char *username, char *password)
{
	char *user = create_user_json_string(username, password);

	char *request = compute_post_request(host_ip, REGISTER, "application/json", &user, 1, NULL, 0, NULL);
	send_to_server(socket, request);
	char *response = receive_from_server(socket);
	if (strstr(response, "is taken") != NULL)
	{
		printf("Username already used.\n");
	}
	else
	{
		printf("Register successful.\n");
	}

	json_free_serialized_string(user);
	free(request);
	free(response);
}

void login_command(int socket, char *host_ip, char *username, char *password, char **cookie, int *successful_connex)
{
	char *user = create_user_json_string(username, password);

	char *request = compute_post_request(host_ip, LOGIN, "application/json", &user, 1, NULL, 0, NULL);

	send_to_server(socket, request);

	char *response = receive_from_server(socket);

	const char *cookie_begin = "Set-Cookie: ";
	char *cookie_header = strstr(response, cookie_begin);
	if (cookie_header == NULL)
	{
		printf("Wrong password.\n");
		json_free_serialized_string(user);
		free(request);
		free(response);
		return;
	}

	cookie_header += strlen(cookie_begin);
	char *cookie_header_copy = strdup(cookie_header);
	char *delim = strstr(cookie_header_copy, "; ");
	memcpy(cookie_header, cookie_header_copy, (delim - cookie_header_copy));
	memset(cookie_header + (delim - cookie_header_copy), 0, strlen(cookie_header_copy) - (delim - cookie_header_copy));
	free(cookie_header_copy);

	*cookie = cookie_header;

	printf("Login successful.\n");
	*successful_connex = 1;

	json_free_serialized_string(user);
	free(request);
	free(response);
}

void logout(int socket, char *host_ip, char *token, char **cookies, int *successful_conex, int *already_in)
{
	if (*successful_conex == 0)
	{
		printf("You are not logged in.\n");
		return;
	}
	*successful_conex = 0;
	*already_in = 0;

	char *response = calloc(BUF, sizeof(char));

	char *request = compute_get_request(host_ip, LOGOUT, NULL, cookies, 1, token, "get");

	send_to_server(socket, request);

	response = receive_from_server(socket);

	printf("You are logged out now.\n");

	free(response);
	free(request);
}
void enter_library(int socket, char *host_ip, char *token, char **cookies, int *successful_conex, int *already_in)
{
	if (!*successful_conex)
	{
		printf("You are not logged in.\n");
		return;
	}

	if (*already_in)
	{
		printf("You are already in the library.\n");
		return;
	}
	char *response = calloc(BUF, sizeof(char));

	char *request = compute_get_request(host_ip, ACCESS, NULL, cookies, 1, token, "get");

	send_to_server(socket, request);

	response = receive_from_server(socket);
	char *aux_token = strstr(response, ":\"") + 2;
	char *aux_token_copy = strdup(aux_token);
	memcpy(aux_token, aux_token_copy, strlen(aux_token_copy - 2));
	memset(aux_token + strlen(aux_token) - 2, 0, 2);
	memset(token, 0, strlen(token));
	memcpy(token, aux_token, strlen(aux_token));

	*already_in = 1;
	printf("You have entered the library.\n");

	free(response);
	free(request);
}

void get_books(int socket, char *host_ip, char *token, char **cookies, int already_in)
{
	if (already_in == 0)
	{
		printf("You are not in the library.\n");
		return;
	}
	char *response = calloc(BUF, sizeof(char));
	char *request = compute_get_request(host_ip, BOOKS, NULL, cookies, 1, token, "get");
	send_to_server(socket, request);
	response = receive_from_server(socket);
	printf("%s\n", strstr(response, "["));
}
void get_book(int socket, char *host_ip, char *token, char **cookies, int already_in)
{
	if (already_in == 0)
	{
		printf("You are not in the library.\n");
		return;
	}
	char route[BUF];
	int book_id = 0;
	printf("Enter the book ID: ");
	scanf("%d", &book_id);
	if (book_id <= 0)
	{
		printf("Invalid book ID. Please try again.\n");
		return;
	}
	sprintf(route, "%s/%d", BOOKS, book_id);
	char *response = calloc(BUF, sizeof(char));
	char *request = compute_get_request(host_ip, route, NULL, cookies, 1, token, "get");
	send_to_server(socket, request);
	response = receive_from_server(socket);
	char *book = strstr(response, "{");
	if (book != NULL)
	{
		printf("%s\n", book);
	}
	else
	{
		printf("The book was not found.\n");
	}
}

void add_book(int socket, char *host_ip, char *token, int already_in)
{
	if (already_in == 0)
	{
		printf("You are not in the library.\n");
		return;
	}

	char title[1000], author[1000], genre[1000], publisher[1000], page[1000];
	int no_of_pages;
	printf("title=");
	getchar();
	fgets(title, 1000, stdin);
	title[strlen(title) - 1] = '\0';
	printf("author=");
	fgets(author, 1000, stdin);
	author[strlen(author) - 1] = '\0';
	printf("genre=");
	fgets(genre, 1000, stdin);
	genre[strlen(genre) - 1] = '\0';
	printf("publisher=");
	fgets(publisher, 1000, stdin);
	publisher[strlen(publisher) - 1] = '\0';
	printf("page_count=");
	fgets(page, 1000, stdin);
	no_of_pages = atoi(page);
	if (no_of_pages <= 0)
	{
		printf("Invalid number of pages. Please try again.\n");
		return;
	}
	JSON_Value *val = json_value_init_object();
	JSON_Object *obj = json_value_get_object(val);
	json_object_set_string(obj, "title", title);
	json_object_set_string(obj, "author", author);
	json_object_set_string(obj, "genre", genre);
	json_object_set_string(obj, "publisher", publisher);
	json_object_set_number(obj, "page_count", no_of_pages);
	char *book = json_serialize_to_string(val);
	char *request = compute_post_request(host_ip, BOOKS, "application/json", &book, 1, NULL, 0, token);
	send_to_server(socket, request);
	char *response = receive_from_server(socket);
	json_value_free(val);
	free(book);
	free(request);
	free(response);
}

void delete_book(int socket, char *host_ip, char *token, char **cookies, int already_in)
{
	if (already_in == 1)
	{
		char route[BUF], book[BUF];
		int book_id = 0;
		printf("id=");
		scanf("%s", book);
		book_id = atoi(book);
		if (book_id <= 0)
		{
			printf("The format is wrong.\n");
			return;
		}
		sprintf(route, "%s/%d", BOOKS, book_id);
		char *response = calloc(BUF, sizeof(char));
		char *request = compute_get_request(host_ip, route, NULL, cookies, 1, token, "delete");
		send_to_server(socket, request);
		response = receive_from_server(socket);

		char *deleted = strstr(response, "No book was deleted!");
		if (deleted != NULL)
			printf("The book with this id does not exist.\n");
		else
			printf("The book has been deleted.\n");
		free(response);
		free(request);
	}
	else
		printf("You are not in the library.\n");
}
int main(int argc, char *argv[])
{
	const int FLAG = 0;
	const int port = 8080;
	char *host_ip = "34.254.242.81";
	char **cookies = calloc(1, sizeof(char *));
	char token[BUF];
	char command[BUF];
	int succesful_conex = 0;
	int already_in = 0;
	while (1)
	{
		scanf("%s", command);
		char *username = malloc(BUF * sizeof(char));
		char *password = malloc(BUF * sizeof(char));
		int socket = open_connection(host_ip, port, AF_INET, SOCK_STREAM, FLAG);
		if (strncmp(command, "register", 8) == 0)
		{
			get_credentials(username, password);
			register_command(socket, host_ip, username, password);
		}
		else if (strncmp(command, "login", 5) == 0)
		{
			if (!succesful_conex)
			{
				get_credentials(username, password);
				login_command(socket, host_ip, username, password, cookies, &succesful_conex);
			}
			else
				printf("You are already logged in.\n");
		}
		else if (strncmp(command, "logout", 6) == 0)
		{
			logout(socket, host_ip, token, cookies, &succesful_conex, &already_in);
		}
		else if (strncmp(command, "enter_library", 13) == 0)
		{
			enter_library(socket, host_ip, token, cookies, &succesful_conex, &already_in);
		}
		else if (strncmp(command, "get_books", 9) == 0)
		{
			get_books(socket, host_ip, token, cookies, already_in);
		}
		else if (strncmp(command, "get_book", 8) == 0)
		{
			get_book(socket, host_ip, token, cookies, already_in);
		}
		else if (strncmp(command, "add_book", 8) == 0)
		{
			add_book(socket, host_ip, token, already_in);
		}
		else if (strncmp(command, "delete_book", 11) == 0)
		{
			delete_book(socket, host_ip, token, cookies, already_in);
		}
		else if (strncmp(command, "exit", 4) == 0)
		{
			break;
			close_connection(socket);
			return 0;
		}
	}
}