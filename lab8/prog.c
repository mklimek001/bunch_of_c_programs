#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>


#define BILLION  1000000000.0;
int global_width;

struct all_data{
    int start;
    int stop;
    int colors_scale;
    int width;
    int height;
    unsigned char* given_photo;
    unsigned char* result_photo;
    unsigned char* is_changed;
};

struct thread_time{
    double time;
    pthread_t thread_id;
};


void* numbers_reverse(void* data){
    struct timespec start, stop;
    double accum;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    struct all_data *thread_data = (struct all_data*) data;
    //printf("numbers start : %d, stop: %d\n", thread_data->start, thread_data->stop);
    int w = thread_data->width;
    for(int i = 0; i < thread_data->height; i++){
        for(int j = 0; j < thread_data->width; j++){
            if(thread_data->is_changed[i*w+j] == 0 && thread_data->given_photo[i*w+j] >= thread_data->start && thread_data->given_photo[i*w+j] < thread_data->stop){
                thread_data->result_photo[i*w+j] = thread_data->colors_scale - thread_data->given_photo[i*w+j];
                thread_data->is_changed[i*w+j] = 1;
            }
        }
    }

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }
    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/BILLION
    struct thread_time to_return;
    to_return.thread_id = pthread_self();
    to_return.time = accum;

    return (void*)thread_data;
}

void* block_reverse(void* data){
    struct timespec start, stop;
    double accum;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    struct all_data *thread_data = (struct all_data*) data;
    //printf("numbers start : %d, stop: %d\n", thread_data->start, thread_data->stop);
    int w = thread_data->width;
    for(int i = thread_data->start; i < thread_data->stop; i++){
        for(int j = 0; j < thread_data->width; j++){
            thread_data->result_photo[i*w+j] = thread_data->colors_scale - thread_data->given_photo[i*w+j];
            thread_data->is_changed[i*w+j] = 1;
        }
    }

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }
    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/BILLION
    struct thread_time to_return;
    to_return.thread_id = pthread_self();
    to_return.time = accum;

    return (void*)thread_data;
}

int main(int argc, char* args[]){
    if(argc != 5){
        printf("Program needs 4 arguments: \nnumber of therads, \ntype of separation, \ninput file name, \noutput file name\n");
        return 0;
    }

    int num_of_threads = atoi(args[1]);
    printf("Given num of threads: %d\n", num_of_threads);
    char *separation_type = args[2];
    char *input_path = args[3];
    char *output_path = args[4];

    if(num_of_threads <= 0){
        printf("Number of threads must be positive\n");
        return 0;
    }

    int image_to_process = open(input_path,O_RDONLY);
    int image_to_write = open(output_path,O_CREAT | O_WRONLY, 0644);

    if(image_to_process == -1){
        printf("Error with opening input file!\n");
        return 0;
    }

    char curr_char;
    int curr_sumator = 0;
    int new_line_counter = 0;
    int width, height, colors_scale;

    while(new_line_counter < 2){
        read(image_to_process,&curr_char,1);
        write(image_to_write, &curr_char, 1);
        if(curr_char == '\n') {
            new_line_counter++;
        }
    }

    do{
        read(image_to_process,&curr_char,1);
        write(image_to_write, &curr_char, 1);
        if(curr_char == ' '){
            width = curr_sumator;
            curr_sumator = 0;
        } else if (curr_char - '0' < 0 || curr_char - '0' > 9){
            height = curr_sumator;
        } else{
            curr_sumator *= 10;
            curr_sumator += curr_char - '0';
        }
    } while (curr_char != '\n');

    printf("width : %d, height: %d\n", width, height);
    curr_sumator = 0;

    read(image_to_process,&curr_char,1);
    write(image_to_write, &curr_char, 1);

    while (curr_char - '0' >= 0 && curr_char - '0' <= 9){
        curr_sumator *= 10;
        curr_sumator += curr_char - '0';
        read(image_to_process,&curr_char,1);
        write(image_to_write, &curr_char, 1);
    }

    colors_scale = curr_sumator;
    if(colors_scale != 255){
        //printf("%d is not supported number of colors\n", colors_scale);
        //return 0;
    }

    curr_sumator = -1;
    int int_char;
    int line = 0;
    int pos = 0;

    unsigned char* given_photo = malloc(height * width * sizeof(unsigned char*));
    unsigned char* result_photo = malloc(height * width * sizeof(unsigned char*));
    unsigned char* changed = calloc(height * width, sizeof(unsigned char*));

    while(read(image_to_process,&curr_char,1) == 1){
        int_char = curr_char - '0';
        if(int_char >= 0 && int_char <= 9){
            if(curr_sumator == -1){
                curr_sumator = int_char;
            } else{
                curr_sumator *= 10;
                curr_sumator += int_char;
            }
        } else{
            if(curr_sumator > 0){
                given_photo[line*width+pos] = curr_sumator;
                pos++;
                if(pos >= width){
                    line++;
                    pos %= width;
                }
                curr_sumator = -1;
            }
        }
    }

    /*
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            result_photo[i][j] = colors_scale - given_photo[i][j];
        }
    }
    */
    struct timespec start, stop;
    double accum;

    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    pthread_t threads_ids[num_of_threads];
    struct all_data threads_data[num_of_threads];

    if(strcmp(separation_type, "numbers") == 0){
        printf("Choosen option of separation : numbers\n");
        int nums_to_thread = colors_scale/num_of_threads;
        int start_color = 0;
        int stop_color = nums_to_thread;

        for(int i = 0; i < num_of_threads; i++){
            threads_data[i].start = start_color;
            threads_data[i].stop = stop_color;
            if(i == num_of_threads-1){
                threads_data[i].stop = colors_scale+1;
            }
            threads_data[i].colors_scale = colors_scale;
            threads_data[i].width = width;
            threads_data[i].height = height;
            threads_data[i].given_photo = given_photo;
            threads_data[i].result_photo = result_photo;
            threads_data[i].is_changed = changed;
            start_color += nums_to_thread;
            stop_color += nums_to_thread;
            pthread_create(&threads_ids[i], NULL, &numbers_reverse, &threads_data[i]);
        }

    } else if(strcmp(separation_type, "block") == 0){
        printf("Choosen option of separation : block\n");
        int blocks_to_thread = height/num_of_threads;
        int start_block = 0;
        int stop_block = blocks_to_thread;

        for(int i = 0; i < num_of_threads; i++){
            threads_data[i].start = start_block;
            threads_data[i].stop = stop_block;
            if(i == num_of_threads-1){
                threads_data[i].stop = height+1;
            }
            threads_data[i].colors_scale = colors_scale;
            threads_data[i].width = width;
            threads_data[i].height = height;
            threads_data[i].given_photo = given_photo;
            threads_data[i].result_photo = result_photo;
            threads_data[i].is_changed = changed;
            start_block += blocks_to_thread;
            stop_block += blocks_to_thread;
            pthread_create(&threads_ids[i], NULL, &block_reverse, &threads_data[i]);
        }

    } else{
        printf("%s is not suported type of separation.\n", separation_type);
        return 0;
    }

    for (int i = 0; i < num_of_threads; i++){
        pthread_join(threads_ids[i], NULL);
    }

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }

    accum = ( stop.tv_sec - start.tv_sec ) + ( stop.tv_nsec - start.tv_nsec )/BILLION
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int curr_int_num = result_photo[i*width+j];
            char curr_char_num[] = {' ',' ',' '};
            if(curr_int_num/100 > 0){
                curr_char_num[0] = curr_int_num/100 + '0';
                curr_int_num = result_photo[i*width+j]%100;

            } if(curr_int_num/10 > 0 || curr_char_num[0] != ' '){
                curr_char_num[1] = curr_int_num/10 + '0';
                curr_int_num = result_photo[i*width+j]%10;
            }
            curr_char_num[2] = curr_int_num + '0';

            write(image_to_write, &curr_char_num, 3);
            write(image_to_write, " ", 1);
        }
        write(image_to_write, "\n", 1);
    }


    printf("Dimensions: %d x %d, number of threads: %d, separation type: %s\n", height, width, num_of_threads, separation_type);
    printf("    Time of operation : %f\n", accum);

    close(image_to_process);
    close(image_to_write);

    free(given_photo);
    free(result_photo);
    free(changed);

    return 0;
}