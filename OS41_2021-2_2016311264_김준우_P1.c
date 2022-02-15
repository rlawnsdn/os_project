#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define Queuesize 1000
#define Maxburst 1000
#define Maxprocess 1000
int i, j;
int time = 0;

typedef struct Process {
    int pid;
    int arr_t;
    int init_queue;
    int cycle_num;
    int cycle_sum;
    int tt;
    int wt;
    int burst_seq[Maxburst];
} Process;

typedef struct Queue {
    Process* queue[Queuesize + 1];
    int front;
    int rear;
    int cnt;
} Queue;

typedef struct CPU{
    int time_quantum;
    int curr_time;
    int before_queue;
    Process* process;
} CPU;

typedef struct Node{
    struct Node* link;
    Process* process;
    int before_queue;
} Node;

typedef struct IOlist{
    struct Node* link;
    int cnt;
} IOlist;

// 프로세스 관련 함수
Process* structProcess(int pid, int arr_t, int init_queue, int cycle_num)
{
    Process* a = (Process*)malloc(sizeof(Process));
    a->pid = pid;
    a->arr_t = arr_t;
    a->init_queue = init_queue;
    a->cycle_num = cycle_num;
    a->cycle_sum = 0;
    a->tt = 0;
    a->wt = 0;
    return a;
}

// IO 관련 함수들
Node* createIOnode(Process* process)
{
    Node* a = (Node*)malloc(sizeof(Node));
    a->process = process;
    a->link = NULL;
    a->before_queue = -1;
    return a;
}

IOlist* createIOlist()
{
    IOlist* a = (IOlist*)malloc(sizeof(IOlist));
    a->link = NULL;
    a->cnt = 0;
    return a;
}

int IOburstfinish(Process* process)
{
    i = 0;
    while (process->burst_seq[i] == 0)
    {
        i++;
    }
    if (i % 2 == 0) // IO burst는 짝수번째에 있으므로
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void pushIO(IOlist* IOlist, Process* process, int before_queue)
{
    Node* target = createIOnode(process);
    target->link = IOlist->link;
    target->before_queue = before_queue;
    IOlist->link = target;
    (IOlist->cnt)++;
}

int whofinishIO(IOlist* IOlist)
{
    if (IOlist->link != NULL)
    {
        Node* curr = IOlist->link;
        while (!IOburstfinish(curr->process))
        {
            curr = curr->link;
            if (curr == NULL)
            {
                return -1;
            }
        }
        int x = curr->process->pid;
        return x;
    }
    else
    {
        return -1;
    }
}

Node* searchremovenode(IOlist* IOlist, int iofinish)
{
    if (IOlist->link == NULL)
    {
        printf("Error : IOlist가 비었습니다.\n");
        return 0;
    }
    else
    {
        Node* curr = IOlist;
        while (curr->link != NULL)
        {
            if (curr->link->process->pid == iofinish)
            {
                return curr;
            }
            else
            {
                curr = curr->link;
            }
        }
        printf("Error : whoisfinishIO함수가 잘못되었습니다.\n");
        return 0;
    }
   
}

void removenextnode(Node* node)
{
    Node* removenode = node->link;
    node->link = removenode->link;
    free(removenode);
}

void IO_burst(IOlist* IOlist)
{
    if (IOlist->link != NULL)
    {
        Node* curr = IOlist->link;  
        while (curr != NULL)
        {
            i = 0;
            while (curr->process->burst_seq[i] == 0)
            {
                i++;
            }
            curr->process->burst_seq[i] = curr->process->burst_seq[i] - 1;
            curr = curr->link;
        }
    }
}

// cpu 관련 함수들
CPU* createCPU()
{
    CPU* a = (CPU*)malloc(sizeof(CPU));
    a-> process = NULL;
    a-> time_quantum = 0;
    a-> curr_time = 0;
    a-> before_queue = -1;
    return a;
}

int isemptycpu(CPU* cpu)
{
    if (cpu->process == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void pushCPU(CPU* cpu, Process* process, int before_queue)
{
    cpu->process = process;
    cpu->before_queue = before_queue;
    cpu->curr_time = 0;
    if (before_queue == 0)
    {
        cpu->time_quantum = 1;
    }
    else if (before_queue == 1)
    {
        cpu->time_quantum = 2;
    }
    else if (before_queue == 2)
    {
        cpu->time_quantum = 4;
    }
    else if (before_queue == 3)
    {
        cpu->time_quantum = -1;
    }
    else
    {
        printf("before_queue가 잘못되었습니다.\n");
    }
}

Process* popCPU(CPU* cpu)
{
    Process* process = cpu->process;
    cpu->process = NULL;
    cpu-> time_quantum = 0;
    cpu-> curr_time = 0;
    cpu-> before_queue = -1;
    return process;
}

void cpu_burst(CPU* cpu)
{
    if (!isemptycpu(cpu))
    {
        i = 0;
        while (cpu->process->burst_seq[i] == 0)
        {
            i++;
        }
        cpu->process->burst_seq[i] = cpu->process->burst_seq[i] - 1;
        cpu->curr_time++;
        printf("--");
    }
    else
    {
        printf("==");
    }
}

int cpuburstfinish(Process* process)
{
    i = 0;
    while (process->burst_seq[i] == 0)
    {
        i++;
    }
    if (i % 2 == 1) // cpu burst는 홀수번째에 있으므로
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// queue 관련 함수들
void structQueue(Queue* q)
{
    q->rear = Queuesize - 1;
    q->front = 0;
    q->cnt = 0;
}

void enqueue(Queue* q, Process* x)
{
    if (q->cnt >= Queuesize)
    {
        printf("queue is full\n");
    }   
    else
    {
        q->rear = (q->rear + 1) % Queuesize;
        q->queue[q->rear] = x;
        q->cnt = q->cnt + 1;
    }
}

Process* dequeue(Queue* q)
{
    Process* x = (Process*)malloc(sizeof(Process));
    if (q->cnt <= 0)
    {
        printf("queue is empty\n");
    }   
    else
    {
        x = q->queue[q->front];
        q->front = (q->front + 1) % Queuesize;
        q->cnt = q->cnt - 1;
    }
    return x;
}

int isemptyqueue(Queue* q)
{
    if (q->cnt == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// 기타 함수들
int Search_arr_t(Queue* q)
{
    if (isemptyqueue(q))
    {
        return -1;
    }
    int x;
    x = q->queue[q->front]->arr_t;
    return x;
}

int allburstfinish(Process* process)
{
    i = 0;
    while (process->burst_seq[i] == 0)
    {
        i++;
    }
    if (process->burst_seq[i] == -1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int main()
{
    Queue q0, q1, q2, q3, rq, fq;
    structQueue(&q0);
    structQueue(&q1);
    structQueue(&q2);
    structQueue(&q3);
    structQueue(&rq);
    structQueue(&fq);

    FILE *file;
    file = fopen("input4.txt", "r");
    int process_num;
    fscanf(file, "%d", &process_num); //파일열어서 rq에 전부 넣어둠
    int pid, arr_t, init_queue, cycle_num, burst_num, burst_element;
    for (i = 0; i < process_num; i++)
    {
        fscanf(file, "%d %d %d %d", &pid, &arr_t, &init_queue, &cycle_num);
        Process* process = structProcess(pid, arr_t, init_queue, cycle_num);
        if (cycle_num == 0)
        {
            enqueue(&fq, process);
        }
        else
        {
            burst_num = (cycle_num * 2) - 1;
            for (j = 0; j < burst_num; j++)
            {
                fscanf(file, "%d", &burst_element);
                process->burst_seq[j] = burst_element;
                process->cycle_sum += burst_element;
            }
            process->burst_seq[j] = -1; // burst_seq의 끝을 알기 위해서
            enqueue(&rq, process);
        } 
    }   
    fclose(file); 

    int before_queue, IOfinish;
    Process* process = structProcess(0, 0, 0, 0);
    IOlist* IOlist = createIOlist();
    CPU* cpu = createCPU();

    while (1)
    {
        while (Search_arr_t(&rq) == time) //도착시간에 맞게 프로세스를 rq에서 꺼내 최초 큐에 넣어줌
        {
            process = dequeue(&rq); 
            if (process->init_queue == 0)
            {
                enqueue(&q0, process);
            }
            else if (process->init_queue == 1)
            {
                enqueue(&q1, process);
            }
            else if (process->init_queue == 2)
            {
                enqueue(&q2, process);
            }
            else if (process->init_queue == 3)
            {
                enqueue(&q3, process);
            }
            else
            {
                printf("init_queue값이 잘못된 프로세스가 있습니다.\n");
            }
        } 

        if (time != 0)
        {
            if (!isemptycpu(cpu))
            {
                if (cpuburstfinish(cpu->process)) // cpu burst를 끝내면 IO burst로 넘어감
                {
                    before_queue = cpu->before_queue;
                    process = popCPU(cpu);
                    printf("P%d", process->pid);
                    if (allburstfinish(process))
                    {
                        process->tt = time - process->arr_t; // turnaround time 계산
                        process->wt = process->tt - process->cycle_sum; // waiting time 계산
                        enqueue(&fq, process);
                    }
                    else
                    {
                        pushIO(IOlist, process, before_queue);
                    }
                }
            }
            if (!isemptycpu(cpu))
            {
                if (cpu->curr_time == cpu->time_quantum) // cpu burst를 다 못 끝내고 타임퀀텀을 다 쓰면 cpu에서 프로세스를 빼줌
                {
                    before_queue = cpu->before_queue;
                    process = popCPU(cpu);
                    printf("P%d", process->pid);
                    if (before_queue == 0)
                    {
                        enqueue(&q1, process);
                    }
                    else if (before_queue == 1)
                    {
                        enqueue(&q2, process);
                    }
                    else if (before_queue == 2)
                    {
                        enqueue(&q3, process);
                    }
                    else if (before_queue == 3)
                    {
                        enqueue(&q3, process);
                    }
                    else
                    {
                        printf("before_queue가 잘못되었습니다.(time quantum 끝냄)\n");
                    }
                }
            }
            while (whofinishIO(IOlist) != -1) // IO를 끝낸 프로세스들을 모두 다음 큐에 넣어줌
            {
                IOfinish = whofinishIO(IOlist);
                Node* removenode = searchremovenode(IOlist, IOfinish);
                process = removenode->link->process;
                before_queue = removenode->link->before_queue;
                removenextnode(removenode);
                IOlist->cnt--;
                if (before_queue == 0) // before queue가 q0면 io burst를 끝내고 다시 q0로 돌아가도록 함 
                {
                    enqueue(&q0, process);
                }
                else if (before_queue == 1)
                {
                    enqueue(&q0, process);
                }
                else if (before_queue == 2)
                {
                    enqueue(&q1, process);
                }
                else if (before_queue == 3)
                {
                    enqueue(&q2, process);
                }
                else
                {
                    printf("before_queue가 잘못되었습니다.(IO burst 끝냄)\n");
                }      
            }
        }
        if (isemptycpu(cpu)) // cpu가 비어있으면 q0부터 다음 프로세스를 search해서 넣어줌
        {
            if (!isemptyqueue(&q0))
            {
                process = dequeue(&q0);
                pushCPU(cpu, process, 0);
            }
            else if (!isemptyqueue(&q1))
            {
                process = dequeue(&q1);
                pushCPU(cpu, process, 1);
            }
            else if (!isemptyqueue(&q2))
            {
                process = dequeue(&q2);
                pushCPU(cpu, process, 2);
            }
            else if (!isemptyqueue(&q3))
            {
                process = dequeue(&q3);
                pushCPU(cpu, process, 3);
            }
        }
        if (fq.cnt == process_num)
        {
            printf("\n모든 프로세스의 burst가 종료되었습니다.\n");
            break;
        }
        IO_burst(IOlist);
        cpu_burst(cpu);
        time++;
        printf("%d", time);
    }
    float tt_average = 0;
    float wt_average = 0;
    printf("\n\n\n");
    for (i = 0; i < process_num; i++)
    {
        process = dequeue(&fq);
        printf("process %d의 wt / tt : %d / %d\n", process->pid, process->wt, process->tt);
        tt_average += process->tt;
        wt_average += process->wt;
    }
    tt_average = (float)tt_average/process_num;
    wt_average = (float)wt_average/process_num;
    printf("wt_average : %f\ntt_average : %f\n", wt_average, tt_average);

    free(IOlist);
    free(process);
    free(cpu);
    return 0;
}

