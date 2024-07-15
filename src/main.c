// This is main file for the project.

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>



// Defining the structure of buffer to store the data
typedef struct {
    char* buffer; // Pointer to the buffer
    size_t buffer_length; // Length of the buffer
    ssize_t input_length; // Length of the input
} InputBuffer; // Name of the structure


// Defining the structure of MetaCommandResult
typedef enum {
    META_COMMAND_SUCCESS, // Success
    META_COMMAND_UNRECOGNIZED_COMMAND // Unrecognized command
} MetaCommandResult; // Name of the structure

// list of functions in meta command
void list_tables();
void list_schema();
void print_help();
void show_schema(char* table_name);


// Defining the structure of PrepareResult
typedef enum {
    PREPARE_SUCCESS, // Success
    PREPARE_UNRECOGNIZED_STATEMENT // Unrecognized statement
} PrepareResult; // Name of the structure


// Defining the structure of StatementType
typedef enum {
    STATEMENT_INSERT, // Insert statement
    STATEMENT_SELECT // Select statement
} StatementType; // Name of the structure


// Defining the structure of Statement
typedef struct {
    StatementType type; // Type of the statement
} Statement; // Name of the structure

// Defining the structure of InputBuffer
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer)); // Allocating memory for the input buffer
    input_buffer->buffer = NULL; // Initializing the buffer to NULL
    input_buffer->buffer_length = 0; // Initializing the buffer length to 0
    input_buffer->input_length = 0; // Initializing the input length to 0
    return input_buffer; // Returning the input buffer
}


// Function to do the meta command
MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) { // If the input is ".exit"
        exit(EXIT_SUCCESS); // Exit the program
    } else if(strcmp(input_buffer->buffer, ".tables")==0){
        list_tables();
    } else if(strcmp(input_buffer->buffer, ".schema")==0){
        show_schema(input_buffer->buffer);
    } else if(strcmp(input_buffer->buffer, ".help")==0){
        print_help();
    } else if(strcmp(input_buffer->buffer, ".clear")==0){
        system("clear");
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND; // Return the unrecognized command
    }
}

// Function show list of tables
void list_tables(){
    printf("List of tables\n");

}

// Function show schema of table
void show_schema(char* table_name){
    // Parse the input to get the table name
    // strtok is used to tokenize the input
    char* token = strtok(table_name, "\n"); // Tokenizing the input
    if(table_name){
        printf("Schema for table %s(mocks)\n", table_name);
    } else {
        printf("Usage: .schema[table_name]\n");
    }
}

// Function to print help
void print_help(){
    printf("List of commands\n");
    printf(".tables - List of tables\n");
    printf(".schema - Show schema of table\n");
    printf(".clear - Clear the screen\n");
    printf(".exit - Exit the program\n");
    printf(".clear - Clear the screen\n");
}


// Function to print the prompt for the user
void print_prompt() {
    printf("C-db > "); // Printing the prompt
}


// Function to prepare the statement
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) { // If the input is insert
        statement->type = STATEMENT_INSERT; // Assigning the statement type
        return PREPARE_SUCCESS; // Return the success
    }
    if (strcmp(input_buffer->buffer, "select") == 0) { // If the input is select
        statement->type = STATEMENT_SELECT; // Assigning the statement type
        return PREPARE_SUCCESS; // Return the success
    }
    return PREPARE_UNRECOGNIZED_STATEMENT; // Return the unrecognized statement
}

// Function to execute the statement
void execute_statement(Statement* statement) {
    switch (statement->type) { // Switch case based on the statement type
        case STATEMENT_INSERT: // If the statement is insert
            printf("This is where we would do an insert.\n"); // Print the message
            break; // Break the loop
        case STATEMENT_SELECT: // If the statement is select
            printf("This is where we would do a select.\n"); // Print the message
            break; // Break the loop
    }
}


// Function for creating custom getline function
ssize_t custom_getline(char** lineptr, size_t* n, FILE* stream) {
    char* buf = *lineptr; // Assigning the buffer to the lineptr
    size_t capacity = *n; // Assigning the capacity to n
    int c; // Variable to store the character
    ssize_t i = 0; // Variable to store the index
    if (buf == NULL) { // If the buffer is NULL
        buf = (char*)malloc(128); // Allocating memory for the buffer
        if (buf == NULL) { // If the buffer is NULL
            return -1; // Return -1
        }
        capacity = 128; // Assigning the capacity to 128
    }
    while ((c = getc(stream)) != EOF) { // Loop to read the characters from the stream
        if (i == capacity - 1) { // If the index is equal to capacity - 1
            capacity *= 2; // Double the capacity
            buf = (char*)realloc(buf, capacity); // Reallocate the memory for the buffer
            if (buf == NULL) { // If the buffer is NULL
                return -1; // Return -1
            }
        }
        buf[i++] = c; // Assigning the character to the buffer
        if (c == '\n') { // If the character is newline
            break; // Break the loop
        }
    }
    buf[i] = '\0'; // Assigning the null character at the end of the buffer
    *lineptr = buf; // Assigning the buffer to lineptr
    *n = capacity; // Assigning the capacity to n
    return i; // Return the index
}


// Function to read the input from the user
void read_input(InputBuffer* input_buffer) {
    ssize_t bytes_read = custom_getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin); // Reading the input from the user
    if (bytes_read <= 0) { // If the input is less than or equal to 0
        printf("Error reading input\n"); // Print error message
        exit(EXIT_FAILURE); // Exit the program
    }
    input_buffer->input_length = bytes_read - 1; // Assigning the input length
    input_buffer->buffer[bytes_read - 1] = 0; // Assigning the null character at the end of the buffer
}


// Function to close the input buffer
void close_input_buffer(InputBuffer* input_buffer) {
    free(input_buffer->buffer); // Free the buffer
    free(input_buffer); // Free the input buffer
}


// Main function
int main(int argc, char* argv[]) {
    InputBuffer* input_buffer = new_input_buffer(); // Creating the input buffer
    while (true) { // Infinite loop
        print_prompt(); // Print the prompt
        read_input(input_buffer); // Read the input
        if (input_buffer->buffer[0] == '.') { // If the first character is .
            switch (do_meta_command(input_buffer)) { // Switch case based on the meta command
                case META_COMMAND_SUCCESS: // If the meta command is success
                    continue; // Continue the loop
                case META_COMMAND_UNRECOGNIZED_COMMAND: // If the meta command is unrecognized
                    printf("Unrecognized command '%s'\n", input_buffer->buffer); // Print the message
                    continue; // Continue the loop
            }
        }
        Statement statement; // Creating the statement
        switch (prepare_statement(input_buffer, &statement)) { // Switch case based on the prepare statement
            case PREPARE_SUCCESS: // If the prepare statement is success
                break; // Break the loop
            case PREPARE_UNRECOGNIZED_STATEMENT: // If the prepare statement is unrecognized
                printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer); // Print the message
                continue; // Continue the loop
        }
        execute_statement(&statement); // Execute the statement
        printf("Executed.\n"); // Print the message
    }
}