// This is main file for the project.

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>



// Defining the structure of buffer to store the data
typedef struct {
    char* buffer; // Pointer to the buffer
    size_t buffer_length; // Length of the buffer
    ssize_t input_length; // Length of the input
} InputBuffer; // Name of the structure


// Defining the structure of execute statement
typedef enum {
    EXECUTE_SUCCESS, // Success
    EXECUTE_TABLE_FULL // Table full
} ExecuteResult; // Name of the structure

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
    PREPARE_UNRECOGNIZED_STATEMENT, // Unrecognized statement
    PREPARE_SYNTAX_ERROR, // Syntax error
    PREPARE_STRING_TOO_LONG, // String too long
} PrepareResult; // Name of the structure




// Defining the structure of StatementType
typedef enum {
    STATEMENT_INSERT, // Insert statement
    STATEMENT_SELECT // Select statement
} StatementType; // Name of the structure

// Defining the structure of Row
#define COLUMN_USERNAME_SIZE 32 // Size of the username
#define COLUMN_EMAIL_SIZE 255 // Size of the email
typedef struct {
    uint32_t id; // ID of the row
    char username[COLUMN_USERNAME_SIZE]; // Username of the row
    char email[COLUMN_EMAIL_SIZE]; // Email of the row
} Row; // Name of the structure




// Defining the structure of Statement
typedef struct {
    StatementType type; // Type of the statement
    Row row_to_insert; // Row to insert
} Statement; // Name of the structure

// this code is used to get the size of the attribute
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute) // Size of the attribute

// Defining the structure of Table (unit32_t is used to store the unsigned integer)
const uint32_t ID_SIZE = size_of_attribute(Row, id); // Size of the ID
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username); // Size of the username
const uint32_t EMAIL_SIZE = size_of_attribute(Row, email); // Size of the email
// offset means the position of the attribute in the row
const uint32_t ID_OFFSET = 0; // Offset of the ID
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE; // Offset of the username
const uint32_t EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE; // Offset of the email
const uint32_t ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE; // Size of the row

const uint32_t PAGE_SIZE = 4096; // Size of the page
#define TABLE_MAX_PAGES 100 // Maximum pages
const uint32_t ROWS_PER_PAGE = PAGE_SIZE / ROW_SIZE; // Rows per page
const uint32_t TABLE_MAX_ROWS = ROWS_PER_PAGE * TABLE_MAX_PAGES; // Maximum rows



// Defining the structure of Table
typedef struct {
    void* pages[TABLE_MAX_PAGES]; // Pages in the table
    uint32_t num_rows; // Number of rows
} Table; // Name of the structure

// Function to print table
void print_row(Row* row) {
    printf("(%d, %s, %s)\n", row->id, row->username, row->email); // Print the row
}

// Function to serialize the row
void serialize_row(Row* source, void* destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE); // Copy the ID
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE); // Copy the username
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE); // Copy the email
}

// Function to deserialize the row
void deserialize_row(void* source, Row* destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE); // Copy the ID
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE); // Copy the username
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE); // Copy the email
}

// Function to find the row slot
void* row_slot(Table* table, uint32_t row_num) {
    uint32_t page_num = row_num / ROWS_PER_PAGE; // Page number
    void* page = table->pages[page_num]; // Page
    if (!page) { // If the page is NULL
        page = table->pages[page_num] = malloc(PAGE_SIZE); // Allocate memory for the page
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE; // Row offset
    uint32_t byte_offset = row_offset * ROW_SIZE; // Byte offset
    return page + byte_offset; // Return the page + byte offset
}

// Function to initialize the table
Table* new_table() {
    Table* table = (Table*)malloc(sizeof(Table)); // Allocating memory for the table
    table->num_rows = 0; // Initializing the number of rows to 0
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) { // Loop to iterate through the pages
        table->pages[i] = NULL; // Assigning the pages to NULL
    }
    return table; // Returning the table
}

// Function to free the table
void free_table(Table* table) {
    for (int i = 0; table->pages[i]; i++) { // Loop to iterate through the pages
        free(table->pages[i]); // Free the pages
    }
    free(table); // Free the table
}

// Defining the structure of InputBuffer
InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer)); // Allocating memory for the input buffer
    input_buffer->buffer = NULL; // Initializing the buffer to NULL
    input_buffer->buffer_length = 0; // Initializing the buffer length to 0
    input_buffer->input_length = 0; // Initializing the input length to 0
    return input_buffer; // Returning the input buffer
}


void close_input_buffer(InputBuffer* input_buffer); // Function prototype


// Function to do the meta command
MetaCommandResult do_meta_command(InputBuffer* input_buffer, Table* table) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) { // If the input is ".exit"
        close_input_buffer(input_buffer); // Close the input buffer
        free_table(table); // Free the table
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
        int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id), statement->row_to_insert.username, statement->row_to_insert.email); // Assigning the values to the statement
        if(args_assigned < 3) { // If the arguments assigned is less than 3
            return PREPARE_SYNTAX_ERROR; // Return the syntax error
        }
        return PREPARE_SUCCESS; // Return the success
    }
    if (strcmp(input_buffer->buffer, "select") == 0) { // If the input is select
        statement->type = STATEMENT_SELECT; // Assigning the statement type
        return PREPARE_SUCCESS; // Return the success
    }
    return PREPARE_UNRECOGNIZED_STATEMENT; // Return the unrecognized statement
}


// Function to execute the statement
ExecuteResult execute_insert(Statement* statement, Table* table) {
    if (table->num_rows >= TABLE_MAX_ROWS) { // If the number of rows is greater than or equal to the maximum rows
        return EXECUTE_TABLE_FULL; // Return the table full
    }
    Row* row_to_insert = &(statement->row_to_insert); // Row to insert
    serialize_row(row_to_insert, row_slot(table, table->num_rows)); // Serialize the row
    table->num_rows += 1; // Increment the number of rows
    return EXECUTE_SUCCESS; // Return the success
}


// Function to execute the select statement
ExecuteResult execute_select(Statement* statement, Table* table) {
    Row row; // Row
    for (uint32_t i = 0; i < table->num_rows; i++) { // Loop to iterate through the rows
        deserialize_row(row_slot(table, i), &row); // Deserialize the row
        print_row(&row); // Print the row
    }
    return EXECUTE_SUCCESS; // Return the success
}



// Function to execute the statement
ExecuteResult execute_statement(Statement* statement, Table *table) {
    switch(statement->type) { // Switch case based on the statement type
        case (STATEMENT_INSERT): // If the statement is insert
           return execute_insert(statement, table); // Execute the insert statement
        case (STATEMENT_SELECT): // If the statement is select
            return execute_select(statement, table); // Execute the select statement
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
    Table* table = new_table(); // Creating the table
    InputBuffer* input_buffer = new_input_buffer(); // Creating the input buffer
    while(true){
        print_prompt(); // Print the prompt
        
        read_input(input_buffer); // Read the input

        if (input_buffer->buffer, ".exit") { // If the input is meta command
            exit(EXIT_SUCCESS); // Exit the program
        } else{
            printf("Unrecognized command '%s'.\n", input_buffer->buffer); // Print the unrecognized command
            if(input_buffer->buffer[0] == '.'){
                switch(do_meta_command(input_buffer, table)){
                    case (META_COMMAND_SUCCESS):
                        continue;
                    case (META_COMMAND_UNRECOGNIZED_COMMAND):
                        printf("Unrecognized command '%s'.\n", input_buffer->buffer);
                        continue;
                }
            }
        }

            Statement statement; // Statement
            switch(prepare_statement(input_buffer, &statement)) { // Prepare the statement
                case (PREPARE_SUCCESS): // If the statement is success
                    break; // Break the loop
                case (PREPARE_SYNTAX_ERROR): // If the statement is syntax error
                    printf("Syntax error. Could not parse statement.\n"); // Print the syntax error
                    continue; // Continue the loop
                case (PREPARE_UNRECOGNIZED_STATEMENT): // If the statement is unrecognized
                    printf("Unrecognized keyword at start of '%s'.\n", input_buffer->buffer); // Print the unrecognized keyword
                    continue; // Continue the loop
            }

            switch(execute_statement(&statement, table)) { // Execute the statement
                case (EXECUTE_SUCCESS): // If the statement is success
                    printf("Executed.\n"); // Print executed
                    break; // Break the loop
                case (EXECUTE_TABLE_FULL): // If the table is full
                    printf("Error: Table full.\n"); // Print table full
                    break; // Break the loop
            }
        
        }
    }
