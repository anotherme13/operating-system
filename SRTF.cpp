#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct
{
    int iPID;
    int iArrival, iBurst, iRemainingBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
    int iLast;
} PCB;

void inputProcess(int n, PCB P[])
{
    for (int i = 0; i < n; i++)
    {
        P[i].iPID = i + 1;
        printf("Enter arrival time and burst time of Process P%d: ", P[i].iPID);
        scanf("%d %d", &P[i].iArrival, &P[i].iBurst);
        P[i].iRemainingBurst = P[i].iBurst;
        P[i].iStart = P[i].iFinish = P[i].iTaT = -1;
        P[i].iWaiting = P[i].iResponse = 0;
        P[i].iLast = P[i].iArrival;
    }
}

void printProcess(int n, PCB P[])
{
    printf("| PID | Arrival | Burst | Remaining |\n");
    for (int i = 0; i < n; i++)
    {
        printf("| P%d  |\t%d\t|  %d\t|    %d     |\n",
               P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iRemainingBurst);
    }
}

void exportGanttChart(int n, PCB Gantt[])
{
    printf("\nGantt Chart:\n|");
    for (int i = 0; i < n; i++)
    {
        printf(" %d - %d | P%d |", Gantt[i].iStart, Gantt[i].iFinish, Gantt[i].iPID);
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

        if (iCriteria == SORT_BY_BURST && P[j].iRemainingBurst < pivot.iRemainingBurst)
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

void calculateAWT(int n, PCB P[])
{
    int totalWaiting = 0;
    printf("Waiting Time: ");
    for (int i = 0; i < n; i++)
    {
        printf("P%d = %d | ", P[i].iPID, P[i].iWaiting);
        totalWaiting += P[i].iWaiting;
    }
    printf("\n");
    printf("Average Waiting Time: %.2f\n", (float)totalWaiting / n);
    printf("-------------------------------------------\n");
}

void calculateATaT(int n, PCB P[])
{
    int totalTaT = 0;
    printf("Turnaround Time\n");
    for (int i = 0; i < n; i++)
    {
        printf("P%d = %d | ", P[i].iPID, P[i].iTaT);
        totalTaT += P[i].iTaT;
    }
    printf("\n");
    printf("Average Turnaround Time: %.2f\n", (float)totalTaT / n);
    printf("-------------------------------------------\n");
}

int main()
{
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);

    for (int i = 0; i < iNumberOfProcess; i++)
        Input[i].iStart = -1;

    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);
    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;

    printf("Ready Queue\n");
    printProcess(1, ReadyQueue);

    int currentTime = ReadyQueue[0].iArrival;
    while (iTerminated < iNumberOfProcess)
    {
      
        int minRemainingTime = INT_MAX;
        int minRemainingIndex = -1;
        for (int i = 0; i < iReady; i++)
        {
            if (ReadyQueue[i].iArrival <= currentTime && ReadyQueue[i].iRemainingBurst < minRemainingTime)
            {
                minRemainingTime = ReadyQueue[i].iRemainingBurst;
                minRemainingIndex = i;
            }
        }

        if (minRemainingIndex != -1)
        {
            PCB &currentProcess = ReadyQueue[minRemainingIndex];
            currentProcess.iRemainingBurst--;
            currentTime++;

            if (currentProcess.iStart == -1)
            {
                currentProcess.iStart = currentTime - 1;
                currentProcess.iResponse = currentProcess.iStart - currentProcess.iArrival;
            }
            currentProcess.iFinish = currentTime;
            currentProcess.iWaiting = currentProcess.iStart - currentProcess.iArrival;
            currentProcess.iTaT = currentProcess.iFinish - currentProcess.iArrival;
            if (currentProcess.iRemainingBurst == 0)
            {
                pushProcess(&iTerminated, TerminatedArray, currentProcess);
                removeProcess(&iReady, minRemainingIndex, ReadyQueue);
            }
        }

        else
        {
            currentTime++;
        }

        while (iRemain > 0 && Input[0].iArrival <= currentTime)
        {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
            quickSort(ReadyQueue, 0, iReady - 1, SORT_BY_BURST);
        }
    }

    printf("\n===== Preemptive SRTF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
    return 0;
}