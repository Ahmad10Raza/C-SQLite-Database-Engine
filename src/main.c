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

// Defining the structure of InputBuffer

InputBuffer* new_input_buffer() {
    InputBuffer* input_buffer = (InputBuffer*)malloc(sizeof(InputBuffer)); // Allocating memory for the input buffer
    input_buffer->buffer = NULL; // Initializing the buffer to NULL
    input_buffer->buffer_length = 0; // Initializing the buffer length to 0
    input_buffer->input_length = 0; // Initializing the input length to 0
    return input_buffer; // Returning the input buffer
}

// Function to print the prompt for the user
void print_prompt() {
    printf("C-db > "); // Printing the prompt
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
    InputBuffer* input_buffer = new_input_buffer(); // Creating a new input buffer
    while (true) { // Infinite loop
        print_prompt(); // Print the prompt
        read_input(input_buffer); // Read the input
        if (strcmp(input_buffer->buffer, ".exit") == 0) { // If the input is ".exit"
            close_input_buffer(input_buffer); // Close the input buffer
            exit(EXIT_SUCCESS); // Exit the program
        } else {
            printf("Unrecognized command '%s'.\n", input_buffer->buffer); // Print the unrecognized command
        }
    }
}