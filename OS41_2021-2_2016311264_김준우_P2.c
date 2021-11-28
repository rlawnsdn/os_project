#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int i, j, k;
int N, M, W, K;

int is_page_in_frame(int* frame, int* string, int time)
{  
    for (i = 0; i < M; i++)
    {
        if (frame[i] == string[time]) { return 0;} // page exist in frame
    }
    return 1; // page doesn't page exist in frame
}

int is_page_in_woking_set(int* woking_set, int* string, int time)
{  
    for (i = 0; i < W+1; i++)
    {
        if (woking_set[i] == string[time]) { return 0;} // page exist in woking_set
    }
    return 1; // page doesn't page exist in frame
}

void print_frame(int* frame, int* string, int time)
{
    printf("%d --- %d ---", time, string[time]);
    for (i = 0; i < M; i++)
    {
        if (frame[i] == -1) { printf("| - "); }
        else { printf("| %d ", frame[i]); }            
    }
    printf("|");
}

void reset_frame(int* frame)
{
    for (i = 0; i < M; i++) // insert -1 in frame (-1 means frame is empty)
    {
        frame[i] = -1;
    }
}

//MIN
void change_page_MIN(int* frame, int* string, int time)
{
    if (frame[M-1] == -1) // if frame is not full, insert page
    {
        for (i = 0; i < M; i++)
        {
            if (frame[i] == -1)
            {
                frame[i] = string[time];
                return;
            }
        }
    }
    else
    {
        int* change_list = (int*)malloc(M * sizeof(int));
        int sum;
        reset_frame(change_list);
        for (i = time; i < K; i++) // find page needs to be replaced
        {
            j = 0;
            while(string[i] != frame[j] && j < M)
            {
                j++;
            }
            if (j < M)
            {
                change_list[j] = 0;
            }
            sum = 0;
            for(j = 0; j < M; j++)
            {
                sum += change_list[j];
            }
            if(sum == -1)
            {
                break;
            }
        }
        i = 0;
        while(change_list[i] != -1)
        {
            i++;
        }
        frame[i] = string[time];
        return;
    }
}


//FIFO
int change_page_FIFO(int* frame, int* string, int time, int old_load)
{
    for (i = 0; i < M; i++)
    {
        if (frame[i] == -1) // if frame is not full, insert page
        {
            frame[i] = string[time];
            return old_load;
        }
    }  
    frame[old_load] = string[time];
    return (old_load + 1) % M;
}


//LRU
void relocate_stack(int* stack, int* string, int time)
{
    for (i = 0; i < M; i++)
    {
        if (stack[i] == string[time])
        {
            for(j = i; j < M-1; j++)
            {
                stack[j] = stack[j+1];
            }
            stack[M-1] = -1;
            j = 0;
            while (stack[j] != -1)
            {
                j++;
            }
            stack[j] = string[time];
            return;
        }
    }
}

void change_page_LRU(int* frame, int* string, int* stack, int time)
{
    if (stack[M-1] == -1) //frame is not full
    {
        for (i = 0; i < M; i++)
        {
            if (stack[i] == -1)
            {
                stack[i] = string[time];
                frame[i] = string[time];
                return;
            }
        }
    }
    else //frame is full
    {
        int out_page = stack[0];
        for(i = 0; i < M-1; i++)
        {
            stack[i] = stack[i+1];
        }
        stack[M-1] = string[time];
        for(i = 0; i < M; i++)
        {
            if (out_page == frame[i])
            {
                frame[i] = string[time];
                return;
            }
        }
    }
}

//LFU
void reset_list(int* list)
{
    for (i = 0; i < N; i++) // insert -1 in frame (-1 means frame is empty)
    {
        list[i] = 0;
    }
}

void change_page_LFU(int* frame, int* string, int* stack, int* used_list, int time)
{
    if (stack[M-1] == -1) //frame is not full
    {
        for (i = 0; i < M; i++)
        {
            if (stack[i] == -1)
            {
                stack[i] = string[time];
                frame[i] = string[time];
                return;
            }
        }
    }
    else //frame is full
    {
        int smallest = K;
        int* smallest_list = (int*)malloc(M * sizeof(int));
        reset_frame(smallest_list); // smallest list has frame index of smallest elements
        for (i = 0; i < M; i++) // find less frequently elements
        {
            if (used_list[frame[i]] == smallest) // tie with smallest
            {
                j = 0;
                while (smallest_list[j] != -1) //append element to smallest list
                {
                    j++;
                }
                smallest_list[j] = i;
            }
            else if (used_list[frame[i]] < smallest) // find smaller element
            {
                k = i;
                reset_frame(smallest_list);
                i = k;
                smallest_list[0] = i;
                smallest = used_list[frame[i]];
            }
        }
        if (smallest_list[1] != -1) // tiebreaker
        {
            i = 0;
            j = M;
            int temp;
            while (smallest_list[i] != -1 && i < M)
            {
                int temp = 0;
                while (frame[smallest_list[i]] != stack[temp])
                {
                    temp++;
                }
                if( temp < j)
                {
                    j = temp;
                }
                i++;
            }
        }
        else // only one
        {
            j = 0;
            while (frame[smallest_list[0]] != stack[j])
            {
                j++;
            }
        }
        int out_page = stack[j];
        for(i = j; i < M-1; i++)
        {
            stack[i] = stack[i+1];
        }
        stack[M-1] = string[time];
        for(i = 0; i < M; i++)
        {
            if (out_page == frame[i])
            {
                frame[i] = string[time];
                return;
            }
        }
    }
}

//WS
void reset_working_set(int* working_set)
{
    for (i = 0; i < W + 1; i++) 
    {
        working_set[i] = -1;
    }
}

void reset_pages(int* pages)
{
    for (i = 0; i < N; i++) 
    {
        pages[i] = -1;
    }
}


void check_working_set(int* pages, int* string, int* working_set, int time)
{
    reset_working_set(working_set);
    reset_pages(pages);
    for (i = 0; i < W+1; i++)
    {
        if (time - i < 0) { break; }
        pages[string[time - i]] = 0;
        working_set[i] = string[time - i];
    }
    
}

void print_pages(int* pages, int* string, int time)
{
    printf("%d --- %d ---", time, string[time]);
    for (i = 0; i < N; i++)
    {
        if (pages[i] == -1) { printf("| - "); }
        else { printf("| %d ", i); }            
    }
    printf("|");
}




int main()
{
    FILE *file;
    file = fopen("test1.txt", "r");
    int page;
    int time = 0; 
    fscanf(file, "%d %d %d %d", &N, &M, &W, &K);
    if (N > 100 || M > 20 || W > 100 || K > 1000)
    {
        printf("NMWK 입력값이 잘못되었습니다.");
        exit(1);
    }
    int* string = (int*)malloc(K * sizeof(int));
    for (i = 0; i < K; i++)
    {
        fscanf(file, "%d", &page);
        if (page > N - 1)
        {
            printf("string 입력값이 잘못되었습니다.");
            exit(1);  
        }
        string[i] = page;
    }
    fclose(file);
    
    int* frame = (int*)malloc(M * sizeof(int));
    reset_frame(frame);
    int fault = 0;

    //MIN
    printf("< MIN > \n");
    printf("time  page  ");
    for (i = 0; i < M; i++)
    {
        printf("pf%d ", i);
    }
    printf("\n");
    for (time = 0; time < K; time++) 
    {
        if (is_page_in_frame(frame, string, time)) //fault
        {
            change_page_MIN(frame, string, time);
            print_frame(frame, string, time);
            printf("fault\n");
            fault++;
        }
        else //not fault
        {
            print_frame(frame, string, time);
            printf("\n");
        }
    }
    printf("총 page fault 횟수: %d", fault);
    printf("\n\n");


    //FIFO
    int old_load =  0;
    reset_frame(frame);
    fault = 0;
    printf("< FIFO > \n");
    printf("time  page  ");
    for (i = 0; i < M; i++)
    {
        printf("pf%d ", i);
    }
    printf("\n");
    for (time = 0; time < K; time++) 
    {
        if (is_page_in_frame(frame, string, time)) //fault
        {
            old_load = change_page_FIFO(frame, string, time, old_load); 
            print_frame(frame, string, time);
            printf("fault\n");
            fault++;
        }
        else //not fault
        {
            print_frame(frame, string, time);
            printf("\n");
        }
        
    }
    printf("총 page fault 횟수: %d", fault);
    printf("\n\n");


    //LRU
    reset_frame(frame);
    fault = 0;    
    int *stack = (int*)malloc(M * sizeof(int));
    reset_frame(stack);
    printf("< LRU > \n");
    printf("time  page  ");
    for (i = 0; i < M; i++)
    {
        printf("pf%d ", i);
    }
    printf("\n");
    for (time = 0; time < K; time++)
    {
        if (is_page_in_frame(frame, string, time)) //fault
        {
            change_page_LRU(frame, string, stack, time);
            print_frame(frame, string, time);
            printf("fault\n");
            fault++;
        }
        else //not fault
        {
            relocate_stack(stack, string, time);
            print_frame(frame, string, time);
            printf("\n");
        }
    }
    printf("총 page fault 횟수: %d", fault);
    printf("\n\n");


    //LFU
    reset_frame(frame);
    fault = 0;
    int *used_list = (int*)malloc(N * sizeof(int));
    reset_list(used_list);
    reset_frame(stack);
    printf("< LFU > \n");
    printf("time  page  ");
    for (i = 0; i < M; i++)
    {
        printf("pf%d ", i);
    }
    printf("\n");
    for (time = 0; time < K; time++)
    {
        if (is_page_in_frame(frame, string, time)) //fault
        {
            change_page_LFU(frame, string, stack, used_list, time);
            used_list[string[time]]++;
            print_frame(frame, string, time);
            printf("fault\n");
            fault++;
        }
        else //not fault
        {
            relocate_stack(stack, string, time); // for tiebreaking
            used_list[string[time]]++; // add reference counts
            print_frame(frame, string, time);
            printf("\n");
        }
    }
    printf("총 page fault 횟수: %d", fault);
    printf("\n\n");


    //WS
    fault = 0;
    int* pages = (int*)malloc(N * sizeof(int));
    int* working_set = (int*)malloc((W+1) * sizeof(int));
    reset_pages(pages);
    reset_working_set(working_set);
    printf("< WS > \n");
    printf("time  page  ");
    for (i = 0; i < N; i++)
    {
        printf("pg%d ", i);
    }
    printf("\n");
    for (time = 0; time < K; time++) 
    {
        if (is_page_in_woking_set(working_set, string, time)) //fault
        {
            check_working_set(pages, string, working_set, time);
            print_pages(pages, string, time);
            printf("fault\n");
            fault++;
        }
        else //not fault
        {
            check_working_set(pages, string, working_set, time);
            print_pages(pages, string, time);
            printf("\n");
        }
        
    }
    printf("총 page fault 횟수: %d", fault);
    printf("\n\n");
















    return 0;
}