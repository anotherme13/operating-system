#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <bits/stdc++.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

using namespace std;

typedef struct
{
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;


typedef struct
{   
    int start,end,process;
} gantt;

vector<gantt> ans;
PCB Input[10];
PCB ReadyQueue[10];
PCB TerminatedArray[10];
int timestart[20],timeend[20],timearrive[20];
int Waiting[20] = {0};
int Bursttime[20];

int iNumberOfProcess,TQuantum;

void inputProcess(int n, PCB P[])
{
    // srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        P[i].iPID = i + 1;
        printf("Enter arrival time and burst time of Process P%d: ", P[i].iPID);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        P[i].iStart = P[i].iFinish = P[i].iWaiting = P[i].iResponse = P[i].iTaT = 0;
        Bursttime[i + 1] = P[i].iBurst;
        timearrive[i + 1] = P[i].iArrival;
    }
}
void printProcess(int n, PCB P[])
{
    printf("| PID | Arrival | Burst |\n");
    for (int i = 0; i < n; i++)
    {
        printf("| P%d  |\t%d\t|  %d\t|\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
}

void exportGanttChart(int n, PCB Gantt[]) 
{
    printf("\nGantt Chart:\n");
    for (auto i : ans)
    {
       printf("| %d - %d | P%d ",i.start,i.end,i.process);
    }
    printf("\n-------------------------------------------\n");
}

void pushProcess(int *n, PCB P[], PCB Q)
{
    P[*n] = Q;
    (*n)++;
}
void removeProcess(int *n, int index, PCB P[])
{
    for (int i = index; i < *n - 1; i++)
    {
        P[i] = P[i + 1];
    }
    (*n)--;
}
int swapProcess(PCB *P, PCB *Q)
{
    PCB temp = *P;
    *P = *Q;
    *Q = temp;
    return 1;
}
int partition(PCB P[], int low, int high, int iCriteria)
{
    PCB pivot = P[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++)
    {
        if (iCriteria == SORT_BY_ARRIVAL)
        {
            if ((P[j].iArrival < pivot.iArrival) || (P[j].iArrival == pivot.iArrival && P[j].iBurst < pivot.iBurst))
            {
                i++;
                swapProcess(&P[i], &P[j]);
            }
        }

        if (iCriteria == SORT_BY_PID && P[j].iPID < pivot.iPID)
        {
            i++;
            swapProcess(&P[i], &P[j]);
        }

        if (iCriteria == SORT_BY_BURST && P[j].iBurst < pivot.iBurst)
        {
            i++;
            swapProcess(&P[i], &P[j]);
        }
    }
    swapProcess(&P[i + 1], &P[high]);
    return (i + 1);
}
void quickSort(PCB P[], int low, int high, int iCriteria)
{
    if (low < high)
    {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}


void calculateAWT()
{
    int totalWaiting = 0;
    for (int index = 0; index < iNumberOfProcess; index++)
    {
        for (auto i : ans)
        {
            if (i.process == index + 1)
            {
                timestart[index + 1] = i.start;
                break;
            }
        }

        for (int i = ans.size() - 1; i >= 0 ; i--)
        {
           
            if (ans[i].process == index + 1)
            {
                timeend[index + 1] = ans[i].end;
                break;
            }

        }
    }

    printf("Waiting Time: ");
    for (int i = 0; i <iNumberOfProcess; i++)
    {
        Waiting[i + 1] = timeend[i + 1] - timestart[i + 1] - Bursttime[i + 1] + timestart[i + 1] - timearrive[i + 1];
        printf("P%d = %d | ", i + 1, Waiting[i + 1]);
        totalWaiting += Waiting[i + 1];
    }
    printf("\n");
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / iNumberOfProcess);
    printf("-------------------------------------------\n");
}







int main()
{
    
    
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    printf("Please input Time Quantum: ");
    scanf("%d", &TQuantum);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1,
              SORT_BY_ARRIVAL);
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);
    int currentTime = ReadyQueue[0].iArrival;
    while (iTerminated < iNumberOfProcess)
    {
        while (iRemain > 0)
        {
            if (Input[0].iArrival <= currentTime + min(TQuantum,ReadyQueue[0].iBurst))
            {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
                continue;
            }
            else
                break;
        }

        ans.push_back(gantt{currentTime,currentTime + min(ReadyQueue[0].iBurst,TQuantum),ReadyQueue[0].iPID});

        if (ReadyQueue[0].iBurst <= TQuantum)
        {
            currentTime += ReadyQueue[0].iBurst;
            iTerminated++;
        }
        else
        {
            currentTime += TQuantum;
            ReadyQueue[0].iBurst -= TQuantum;
            pushProcess(&iReady, ReadyQueue, ReadyQueue[0]);
        }
        removeProcess(&iReady, 0, ReadyQueue);
        
    }
    printf("\n===== FCFS Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    calculateAWT();
    
   
    return 0;
}