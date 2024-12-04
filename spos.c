
// Machine code
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// MNEUMONIC AND OPCODE TABLE
struct table 
{
    char mname[10];
    char mop[5];
} mot[10];

// SYMBOL TABLE
struct sym
{
    int srno;
    char sn[10];
    int saddr;
} SYMTAB[10];

// LITERAL TABLE
struct lit
{
    int lrno;
    char ln[10];
    int laddr;
} LITTAB[10];


// MAIN FUNCTION
int main()
{
    FILE *fp;
    fp = fopen("pract1.c", "r");
    int n;
    int lc = 0, sptr = 0, lptr = 0;
    int isflag = 0, symfound = 0, litfound = 0, DLflag = 0;
    char str[10], str1[10];
    int i;

    // MNEUMONIC TABLE IS DEFINED HERE
    strcpy(mot[0].mname, "STOP");
    strcpy(mot[0].mop, "00");
    strcpy(mot[1].mname, "ADD");
    strcpy(mot[1].mop, "01");
    strcpy(mot[2].mname, "SUB");
    strcpy(mot[2].mop, "02");
    strcpy(mot[3].mname, "MULT");
    strcpy(mot[3].mop, "03");
    strcpy(mot[4].mname, "MOVER");
    strcpy(mot[4].mop, "04");
    strcpy(mot[5].mname, "MOVEM");
    strcpy(mot[5].mop, "05");

    if(fp == NULL)
    {
        printf("ERROR: Unable to open file.\n");
        return -1;
    }
    else
    {
        while(fscanf(fp, "%s", str) != EOF)
        {
            isflag = 0;
            symfound = 0;
            litfound = 0;
            DLflag = 0;
            
            // START ASSEMBLER DIRECTIVE
            if(strcmp(str, "START") == 0)
            {
                isflag = 1;
                DLflag = 1;
                printf("\n (AD , 01)");
                fscanf(fp, "%s", str);
                lc = atoi(str);
                printf("  (C %d)", lc);
            }
            // END ASSEMBLER DIRECTIVE
            if(strcmp(str, "END") == 0)
            {
                isflag = 1;
                DLflag = 1;
                printf("\n%d) (AD , 02)", lc);
                // AFTER END STATEMENT LITERALS ARE PROCESSED
                for(i = 0; i < lptr; i++)
                {
                    printf("\n%d) Literal %s is processed", lc, LITTAB[i].ln);
                    LITTAB[i].laddr = lc;
                    lc++;
                }
            }
            // IMPERATIVE STATEMENT
            if(isflag == 0)
            {
                for(i = 0; i < 6; i++)
                {
                    if(strcmp(str, mot[i].mname) == 0)
                    {
                        DLflag = 1;
                        printf("\n%d) (IS , %s)", lc, mot[i].mop);
                        lc++;

                        // OPERAND 1 ALWAYS A REGISTER
                        fscanf(fp, "%s", str);
                        if(strcmp(str, "AREG,") == 0)
                            printf(" (R, 1)");
                        else if(strcmp(str, "BREG,") == 0)
                            printf(" (R, 2)");
                        else if(strcmp(str, "CREG,") == 0)
                            printf(" (R, 3)");
                        else if(strcmp(str, "DREG,") == 0)
                            printf(" (R, 4)");

                        // OPERAND 2 can be LITERAL or SYMBOL
                        fscanf(fp, "%s", str);

                        // Check if it is SYMBOL
                        if(str[0] != '=')
                        {
                            // Check if SYMBOL is present in ST
                            symfound = 0;
                            for(i = 0; i < sptr; i++)
                            {
                                if(strcmp(str, SYMTAB[i].sn) == 0)
                                {
                                    printf(" (S, %d)", i+1);
                                    symfound = 1;
                                    break;
                                }
                            }

                            // If SYMBOL not found, add it to ST
                            if(symfound == 0)
                            {
                                strcpy(SYMTAB[sptr].sn, str);
                                SYMTAB[sptr].srno = sptr + 1;
                                printf(" (S, %d)", sptr + 1);
                                sptr++;
                            }
                        }
                        // Check if it is LITERAL
                        else if(str[0] == '=')
                        {
                            // Check if LITERAL is present in LITTAB
                            litfound = 0;
                            for(i = 0; i < lptr; i++)
                            {
                                if(strcmp(str, LITTAB[i].ln) == 0)
                                {
                                    printf(" (L, %d)", i+1);
                                    litfound = 1;
                                    break;
                                }
                            }

                            // If LITERAL not found, add it to LITTAB
                            if(litfound == 0)
                            {
                                strcpy(LITTAB[lptr].ln, str);
                                LITTAB[lptr].lrno = lptr + 1;
                                printf(" (L, %d)", lptr + 1);
                                lptr++;
                            }
                        }
                    }
                }
            }

            // CODE FOR DL STATEMENT
            if(DLflag == 0)
            {
                // Search whether that symbol is present in ST
                symfound = 0; // Reset symfound flag

                for(i = 0; i < sptr; i++)
                {
                    if(strcmp(str, SYMTAB[i].sn) == 0)
                    {
                        SYMTAB[i].saddr = lc;
                        symfound = 1;
                        break;
                    }
                }

                // If the symbol is not found in the symbol table, add it
                if(symfound == 0)
                {
                    strcpy(SYMTAB[sptr].sn, str);
                    SYMTAB[sptr].srno = sptr + 1;
                    SYMTAB[sptr].saddr = lc;
                    printf(" (S, %d)", sptr + 1);
                    sptr++;
                }

                fscanf(fp, "%s", str);  // DC DS
                fscanf(fp, "%s", str1); // Const

                if(strcmp(str, "DC") == 0)
                {
                    printf("\n%d) (DL , 01) (C, %s)", lc, str1);
                    lc++;
                }
                
                else if(strcmp(str, "DS") == 0)
                {
                    n = atoi(str1);
                    printf("\n%d) (DL , 02) (C, %s)", lc, str1);
                    lc += n; // Increment lc by the specified size
                }
            }
        } // while end
    }

    printf("\n----------------------------------------------");
    printf("\n \t SYMBOL TABLE ");
    printf("\n SRNO. \t SNAME \tSADDR");
    for(i = 0; i < sptr; i++)
    {
        printf("\n %d \t \t %s \t \t%d", SYMTAB[i].srno, SYMTAB[i].sn, SYMTAB[i].saddr);
    }
    printf("\n----------------------------------------------");
    
    printf("\n \t LITERAL TABLE ");
    printf("\n LRNO. \t LNAME \tLADDR");
    for(i = 0; i < lptr; i++)
    {
        printf("\n %d \t \t %s  \t\t%d", LITTAB[i].lrno, LITTAB[i].ln, LITTAB[i].laddr);
    }

    fclose(fp);
    return 0;
}

//  START 100
// MOVER AREG, ='5'
// ADD BREG, X
// MOVEM CREG, ='1'
// MOVER BREG, Y
// X DS 1
// Y DS 1
// END

//////////////////////////////////////////////////////////////////




// Lexical Analyzer
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct keyword{  // keywords
char k[100];
}kw[32];

struct operator{  // operators
char op[10];
}o[10];

struct output{
int sr;
char name[100];
char type[100];
}out[500];   // tabular form

int main()
{
FILE* ptr;  // file pointer
int i,j, a=1, idflag=0;    //i and j are loop counters.
//a is used to keep track of the serial number in the output table.
//idflag is a flag to determine if a word has been categorized.
//str is a character array used to read words from the input file.
char str[100];
ptr=fopen("pract2.c","r");

// Predefined values for keywords
strcpy(kw[0].k, "auto");
strcpy(kw[1].k, "break");
strcpy(kw[2].k, "case");
strcpy(kw[3].k, "char");
strcpy(kw[4].k, "const");
strcpy(kw[5].k, "continue");
strcpy(kw[6].k, "default");
strcpy(kw[7].k, "do");
strcpy(kw[8].k, "double");
strcpy(kw[9].k, "else");
strcpy(kw[10].k, "enum");
strcpy(kw[11].k, "extern");
strcpy(kw[12].k, "float");
strcpy(kw[13].k, "for");
strcpy(kw[14].k, "goto");
strcpy(kw[15].k, "if");
strcpy(kw[16].k, "int");
strcpy(kw[17].k, "long");
strcpy(kw[18].k, "register");
strcpy(kw[19].k, "return");
strcpy(kw[20].k, "short");
strcpy(kw[21].k, "signed");
strcpy(kw[22].k, "sizeof");
strcpy(kw[23].k, "static");
strcpy(kw[24].k, "struct");
strcpy(kw[25].k, "switch");
strcpy(kw[26].k, "typedef");
strcpy(kw[27].k, "union");
strcpy(kw[28].k, "unsigned");
strcpy(kw[29].k, "void");
strcpy(kw[30].k, "volatile");
strcpy(kw[31].k, "while");


// predefined values for operators
strcpy(o[0].op, "*");
strcpy(o[1].op, "+");
strcpy(o[2].op, "=");
strcpy(o[3].op, "-");
// Read the file and compare the strings
if(ptr==NULL)
{
printf("File is empty");
}
else
{
	;while(fscanf(ptr, "%s",str)!= EOF)
	{
	idflag=0;
	for(i=0;i<=32;i++)  // keywords check 
	{  
		if(strcmp(str,kw[i].k)==0)
		{
		  	 out[a].sr=a;
		  	 strcpy(out[a].name,str);
		  	 strcpy(out[a].type,"Keyword");
		  	 a++;
		  	 idflag=1;		  	
		  	 break;
		}
	}
	if(str[0]=='#' || str[0]=='(' || str[0]==')' || str[0]=='{' || str[0]=='}' ||str[0]==';' || str[0]=='<' || str[0]=='>')
	{
	  	 out[a].sr=a;
	  	 strcpy(out[a].name,str);
	  	 strcpy(out[a].type,"Special Symbol");
	  	 a++;
	  	 idflag=1;	  
	}
	if(str[0]=='1' || str[0]=='2' ||str[0]=='3' ||str[0]=='4' ||str[0]=='5' ||str[0]=='6' || str[0]=='7' || str[0]=='8' || str[0]=='9')
	{
	  	out[a].sr=a;
	  	strcpy(out[a].name,str);
	          strcpy(out[a].type,"Constant");
	  	a++;
  		idflag=1;
	}
	for(i=0;i<10;i++)
	{
	 	if(strcmp(str,o[i].op)==0)
	  	{
	  		out[a].sr=a;
	  	 	strcpy(out[a].name,str);
	  	 	strcpy(out[a].type,"Operator");
	  	 	a++;
	  	 	idflag=1;
	  	}
	}
	  	
	if(idflag==0)
	{
	  	 out[a].sr=a;
	  	 strcpy(out[a].name,str);
	  	 strcpy(out[a].type,"Identifier");
	  	 a++;
	  	 idflag=1;
	}
	  
	}
}
// Display output

printf("Sr.No.\tName\tType\t\n");
for(j=1;j<a;j++)
  {
  printf("%d\t%s\t%s\t\n",out[j].sr, out[j].name, out[j].type);
  }
  
  fclose(ptr);
}

// int main() {
//     int x = 5;
//     float y = 10.5;
//     x = x + y;
//     return 0;
// }



///////////////////////////////////////////////





// FCFS and SJF

#include <stdio.h>

struct Process {
    char pname[5];
    int at;   // Arrival Time
    int bt;   // Burst Time
    int ct;   // Completion Timep[]b
    int tat;  // Turnaround Time
    int wt;   // Waiting Time
};

// Function to calculate FCFS Scheduling
void fcfs(struct Process processes[], int n) {
    int currentTime = 0, i;
    for (i = 0; i < n; i++) {
        if (processes[i].at > currentTime) {
            currentTime = processes[i].at;
        }
        processes[i].ct = currentTime + processes[i].bt;
        processes[i].tat = processes[i].ct - processes[i].at;
        processes[i].wt = processes[i].tat - processes[i].bt;
        currentTime = processes[i].ct;
    }

    printf("\nFCFS Scheduling Results:\n");
    printf("\nProcess\tAT\tBT\tCT\tTAT\tWT\n");

    int totalTAT = 0, totalWT = 0;

    for (i = 0; i < n; i++) {
        totalTAT += processes[i].tat;
        totalWT += processes[i].wt;

        printf("%s\t%d\t%d\t%d\t%d\t%d\n", processes[i].pname, processes[i].at, processes[i].bt, processes[i].ct, processes[i].tat, processes[i].wt);
    }

    double avgTAT = (double)totalTAT / n;
    double avgWT = (double)totalWT / n;

    printf("\nAverage TAT: %.2f\n", avgTAT);
    printf("Average WT: %.2f\n", avgWT);
}

 //Function to calculate SJF Scheduling
void sjf(struct Process processes[], int n) {
    int i, j;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (processes[j].bt > processes[j + 1].bt) {
                struct Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }

    fcfs(processes, n); // Call FCFS logic after sorting for SJF
}

int main() {
    int n, i;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    struct Process processes[n];

    for (i = 0; i < n; i++) {
        printf("Enter the name of process P%d: ", i + 1);
        scanf("%s", processes[i].pname);
        printf("Enter the Arrival Time for P%d: ", i + 1);
        scanf("%d", &processes[i].at);
        printf("Enter the Burst Time for P%d: ", i + 1);
        scanf("%d", &processes[i].bt);
    }

    printf("\nRunning FCFS Scheduling:\n");
    fcfs(processes, n);

     printf("\nRunning SJF Scheduling:\n");
     sjf(processes, n);

    return 0;
}
/*
Process AT      BT      CT      TAT     WT
1       0       6       6       6       0
2       2       2       8       6       4
3       3       1       9       6       5
*/


/////////////////////////////////////////////////////////


// Round Robin
#include <stdio.h>
#include <string.h>

// Function to find the waiting time for all processes
void findWaitingTime(int processes[], int n, int bt[], int at[], int wt[], int ct[], int quantum) {
    int rem_bt[n]; // Array to store remaining burst times
    for (int i = 0; i < n; i++)
        rem_bt[i] = bt[i]; // Initialize remaining burst times

    int t = 0; // Current time

    // Traverse processes in round-robin manner
    while (1) {
        int done = 1; // Flag to check if all processes are done

        // Traverse all processes one by one
        for (int i = 0; i < n; i++) {
            // If the process has arrived and has remaining burst time
            if (at[i] <= t && rem_bt[i] > 0) {
                done = 0; // There is a pending process

                if (rem_bt[i] > quantum) {
                    // Increase time by quantum
                    t += quantum;

                    // Decrease burst time of the current process
                    rem_bt[i] -= quantum;
                } else {
                    // Increase time by the remaining burst time
                    t += rem_bt[i];

                    // Calculate waiting time
                    wt[i] = t - at[i] - bt[i];

                    // Set remaining burst time to 0
                    rem_bt[i] = 0;

                    // Calculate completion time
                    ct[i] = t;
                }
            }
        }

        // If all processes are done, break the loop
        if (done == 1)
            break;

        // Increment time if no process has arrived
        if (done == 0 && t < at[n - 1]) 
            t++;
    }
}

// Function to calculate turn-around time
void findTurnAroundTime(int processes[], int n, int bt[], int at[], int wt[], int tat[], int ct[]) {
    // Turnaround time = completion time - arrival time
    for (int i = 0; i < n; i++)
        tat[i] = ct[i] - at[i];
}

// Function to calculate and print average time
void findavgTime(int processes[], int n, int bt[], int at[], int quantum) {
    int wt[n], tat[n], ct[n], total_wt = 0, total_tat = 0;

    // Find waiting time for all processes
    findWaitingTime(processes, n, bt, at, wt, ct, quantum);

    // Find turn-around time for all processes
    findTurnAroundTime(processes, n, bt, at, wt, tat, ct);

    // Display process details
    printf("PN\tAT\tBT\tCT\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        total_wt += wt[i];
        total_tat += tat[i];
        printf("%d\t%d\t%d\t%d\t%d\t%d\n", i + 1, at[i], bt[i], ct[i], wt[i], tat[i]);
    }

    printf("Average waiting time = %.2f\n", (float)total_wt / (float)n);
    printf("Average turn around time = %.2f\n", (float)total_tat / (float)n);
}

// Driver code
int main() {
    // Input number of processes
    int n;
    printf("Enter number of processes: ");
    scanf("%d", &n);

    // Process IDs
    int processes[n];
    int burst_time[n], arrival_time[n];

    // Input process details (arrival time and burst time)
    for (int i = 0; i < n; i++) {
        printf("Enter arrival time and burst time for process %d: ", i + 1);
        scanf("%d %d", &arrival_time[i], &burst_time[i]);
        processes[i] = i + 1;  // Assign process ID
    }

    // Time quantum
    int quantum;
    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    // Calculate average times
    findavgTime(processes, n, burst_time, arrival_time, quantum);

    return 0;
}
// Enter number of processes: 4
// Enter arrival time and burst time for process 1: 0 5
// Enter arrival time and burst time for process 2: 1 4
// Enter arrival time and burst time for process 3: 2 2
// Enter arrival time and burst time for process 4: 3 1


///////////////////////////////////////////////////

// Producer Comsumer

 // C program for the producer consumer problem
#include <stdio.h>
#include <stdlib.h>

// Initialize a mutex to 1 as binary semaphor
int mutex = 1;  

// Number of full slots intially 0
int full = 0; //counting semaphor

// Number of empty slots as size of buffer
int empty = 10;  //counting semaphor

int x = 0; // counter for number of produced and consumed items

// Function to produce an item and add it to the buffer
void producer()
{
	// Decrease mutex value by 1
	--mutex;

	// Increase the number of full slots by 1
	++full;

	// Decrease the number of empty slots by 1
	--empty;

	// Item produced so increment x by 1
	x++;
	printf("\nProducer produces item %d", x);

	// Increase mutex value by 1
	++mutex;
}

// Function to consume an item and remove it from buffer
void consumer()
{
	// Decrease mutex value by 1
	--mutex;
	// Decrease the number of full slots by 1
	--full;
	// Increase the number of empty slots by 1
	++empty;
	printf("\nConsumer consumes item %d",x);
	x--;
	// Increase mutex value by 1
	++mutex;
}

// Driver Code
int main()
{
	int n, i;
	printf("\n1. Press 1 for Producer"
		"\n2. Press 2 for Consumer"
		"\n3. Press 3 for Exit");

	for (i = 1; i > 0; i++) {

		printf("\nEnter your choice:");
		scanf("%d", &n);

		// Switch Cases
		switch (n) {
		case 1:

			// If mutex is 1 and empty
			// is non-zero, then it is
			// possible to produce
			if ((mutex == 1)
				&& (empty != 0)) {
				producer();
			}

			// Otherwise, print buffer
			// is full
			else {
				printf("Buffer is full!");
			}
			break;

		case 2:

			// If mutex is 1 and full
			// is non-zero, then it is
			// possible to consume
			if ((mutex == 1)
				&& (full != 0)) {
				consumer();
			}

			// Otherwise, print Buffer
			// is empty
			else {
				printf("Buffer is empty!");
			}
			break;

		// Exit Condition
		case 3:
			exit(0);
			break;
		}
	}
}


/////////////////////////////////////////////////////

//// Bankers Algorithm

#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, m;

    // Input the number of processes and resources
    printf("Enter the number of processes: ");
    scanf("%d", &n);
    
    printf("Enter the number of resources: ");
    scanf("%d", &m);

    // Dynamically allocate memory for the matrices and arrays
    int **alloc = (int **)malloc(n * sizeof(int *));  // Allocation matrix
    int **max = (int **)malloc(n * sizeof(int *));    // Maximum matrix
    int **need = (int **)malloc(n * sizeof(int *));   // Need matrix
    int *avail = (int *)malloc(m * sizeof(int));      // Available resources
    int *f = (int *)malloc(n * sizeof(int));          // Finish flag array
    int *ans = (int *)malloc(n * sizeof(int));        // Safe sequence array
    int **avail_log = (int **)malloc((n + 1) * sizeof(int *)); // To store available resources at each step

    // Input allocation matrix
    printf("Enter the allocation matrix (%d x %d):\n", n, m);
    for (int i = 0; i < n; i++) {
        alloc[i] = (int *)malloc(m * sizeof(int));
        for (int j = 0; j < m; j++) {
            scanf("%d", &alloc[i][j]);
        }
    }

    // Input maximum matrix
    printf("Enter the maximum matrix (%d x %d):\n", n, m);
    for (int i = 0; i < n; i++) {
        max[i] = (int *)malloc(m * sizeof(int));
        for (int j = 0; j < m; j++) {
            scanf("%d", &max[i][j]);
        }
    }

    // Input available resources
    printf("Enter the available resources (%d):\n", m);
    for (int i = 0; i < m; i++) {
        scanf("%d", &avail[i]);
    }

    // Initialize finish array to 0
    for (int i = 0; i < n; i++) {
        f[i] = 0;
        avail_log[i] = (int *)malloc(m * sizeof(int));  // Allocate memory for each log
    }

    // Calculate the need matrix
    for (int i = 0; i < n; i++) {
        need[i] = (int *)malloc(m * sizeof(int));
        for (int j = 0; j < m; j++) {
            need[i][j] = max[i][j] - alloc[i][j];  // Need = Max - Alloc
        }
    }

    // Banker's algorithm to find the safe sequence
    int ind = 0;  // Index to keep track of the sequence
    int count = 0;  // Number of processes finished
    int canAllocate;

    while (count < n) {
        canAllocate = 0;  // Flag to check if any process can be allocated resources

        for (int i = 0; i < n; i++) {
            if (f[i] == 0) {  // Process not yet finished
                int flag = 0;
                for (int j = 0; j < m; j++) {
                    if (need[i][j] > avail[j]) {
                        flag = 1;  // Resources not available
                        break;
                    }
                }

                if (flag == 0) {  // If all resources are available for the process
                    // Process can be safely allocated resources
                    ans[ind++] = i;

                    // Record available resources before allocation for logging
                    for (int y = 0; y < m; y++) {
                        avail_log[count][y] = avail[y];
                    }

                    for (int y = 0; y < m; y++) {
                        avail[y] += alloc[i][y];  // Free the allocated resources
                    }
                    f[i] = 1;  // Mark process as finished
                    count++;
                    canAllocate = 1;  // Indicate that allocation was possible in this pass
                }
            }
        }

        if (!canAllocate) {
            // No process can be allocated resources, so break the loop
            printf("The given sequence is not safe.\n");
            return 0;
        }
    }

    // Display all matrices in a table format
    printf("\nProcess\t\tAllocation\tMax\t\tNeed\t\tAvailable\n");
    printf("-----------------------------------------------------------------\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t\t", i);  // Process label

        // Print Allocation matrix
        for (int j = 0; j < m; j++) {
            printf("%d ", alloc[i][j]);
        }
        printf("\t\t");

        // Print Max matrix
        for (int j = 0; j < m; j++) {
            printf("%d ", max[i][j]);
        }
        printf("\t\t");

        // Print Need matrix
        for (int j = 0; j < m; j++) {
            printf("%d ", need[i][j]);
        }

        // Print Available matrix
        printf("\t\t");
        for (int j = 0; j < m; j++) {
            printf("%d ", avail_log[i][j]);  // Log current available
        }

        printf("\n");
    }

    // Display all available resources after each step
    printf("\nAvailable Resources at Each Step:\n");
    for (int i = 0; i < count; i++) {
        printf("After executing P%d: ", ans[i]);
        for (int j = 0; j < m; j++) {
            printf("%d ", avail_log[i][j]);
        }
        printf("\n");
    }

    // If the system is in a safe state, print the safe sequence
    printf("\nFollowing is the SAFE Sequence:\n");
    for (int i = 0; i < n - 1; i++) {
        printf("P%d -> ", ans[i]);
    }
    printf("P%d\n", ans[n - 1]);

    // Free dynamically allocated memory
    for (int i = 0; i < n; i++) {
        free(alloc[i]);
        free(max[i]);
        free(need[i]);
        free(avail_log[i]);  // Freeing the log memory
    }
    free(alloc);
    free(max);
    free(need);
    free(avail);
    free(f);
    free(ans);
    free(avail_log);

    return 0;
}

// Process         Allocation      Max             Need            Available
// -----------------------------------------------------------------
// P0              0 1 0           7 5 3           7 4 3           3 3 2
// P1              2 0 0           3 2 2           1 2 2           5 3 2
// P2              3 0 2           9 0 2           6 0 0           7 4 3
// P3              2 1 1           2 2 2           0 1 1           7 4 5
// P4              0 0 2           4 3 3           4 3 1           7 5 5

/////////////////////////////

// LRU

#include <stdio.h>

// Function to implement LRU Page Replacement Algorithm
void lru(int n, int str[], int nf) {
    int i, k, timer = 1, time[5], pos = 0, frame[nf], hit = 0, fault = 0;

    // Initialize frame and time arrays
    for (k = 0; k < nf; k++) {
        frame[k] = -1;
        time[k] = 0;
    }

    printf("\n\tRef String\tPage Frames\n");
    printf("\t-----------\t-------------------\n");
    
    for (i = 1; i <= n; i++) {
        int flag = 0;
        for (k = 0; k < nf; k++) {
            if (str[i] == frame[k]) { // Page found
                flag = 1;
                time[k] = timer;
                timer++;
                hit++; // Increment page hit count
                break;
            }
        }

        if (flag == 0) { // Page not found
            fault++;
            // Increment page fault count
            int min_time = time[0];
            pos = 0;
            for (k = 1; k < nf; k++) {
                if (time[k] < min_time) {
                    min_time = time[k];
                    pos = k;
                }
            }
            frame[pos] = str[i];
            time[pos] = timer;
            timer++;
        }

        // Display current page frames
        printf("\t%d\t\t", str[i]);
        for (k = 0; k < nf; k++) {
            if (frame[k] != -1) {
                printf("%d ", frame[k]);
            } else {
                printf("- ");
            }
        }
        printf("\n");
    }

    printf("\nNumber of Page Faults: %d\n", fault);
    printf("Number of Page Hits: %d\n", hit);
   // float ratio=hit/7;
       float ratio = (float)hit / n;
    printf("atio is s: %f\n", ratio);

}

int main() {
    int i, n, str[50], nf;

    // Input the length of reference string
    printf("\nEnter the length of reference string:\n");
    scanf("%d", &n);

    // Input the page numbers string
    printf("\nEnter the page numbers string:\n");
    for (i = 1; i <= n; i++)
        scanf("%d", &str[i]);

    // Input the number of frames
    printf("\nEnter the number of frames: ");
    scanf("%d", &nf);

    // Call LRU function
    lru(n, str, nf);

    return 0;
}


///////////////////////////////////////////////


// FIFO

#include <stdio.h>

// Function to implement FIFO Page Replacement Algorithm
void fifo(int n, int str[], int nf) {
    int i, j = 0, k, frame[nf], flag, hit = 0, fault = 0;

    // Initialize frame array
    for (i = 0; i < nf; i++)
        frame[i] = -1;

    printf("\n\tRef String\tPage Frames\n");
    printf("\t-----------\t-------------------\n");

    for (i = 1; i <= n; i++) {
        printf("\t%d\t\t", str[i]);
        flag = 0;
        for (k = 0; k < nf; k++) {
            if (frame[k] == str[i]) {
                flag = 1;
                hit++;
                break;
            }
        }
        if (flag == 0) {
            frame[j] = str[i];
            j = (j + 1) % nf;
            fault++;
        }

        // Display current page frames
        for (k = 0; k < nf; k++) {
            if (frame[k] != -1) {
                printf("%d ", frame[k]);
            } else {
                printf("- ");
            }
        }
        printf("\n");
    }

    printf("\nPage Faults: %d\n", fault);
    printf("Page Hits: %d\n", hit);
}

int main() {
    int i, n, str[50], nf;

    // Input the length of reference string
    printf("\nEnter the length of reference string:\n");
    scanf("%d", &n);

    // Input the page numbers string
    printf("\nEnter the page numbers string:\n");
    for (i = 1; i <= n; i++)
        scanf("%d", &str[i]);

    // Input the number of frames
    printf("\nEnter the number of frames: ");
    scanf("%d", &nf);

    // Call FIFO function
    fifo(n, str, nf);

    return 0;
}


///////////////////////////////////////////////////////


// FCFS Disk Scheduling Assignment 8

#include <stdio.h>
#include <stdlib.h>

int main() {
    int r, hp, s = 0;

    printf("Enter the size of the request queue: ");
    scanf("%d", &r);

    int req[r];
    printf("Enter the request queue: ");
    for (int i = 0; i < r; i++) {
        scanf("%d", &req[i]);
    }

    printf("Enter the initial head position: ");
    scanf("%d", &hp);

    // FCFS scheduling
    s = abs(hp - req[0]);
    for (int i = 0; i < r - 1; i++) {
        s += abs(req[i] - req[i + 1]);
        printf("Req %d is completed\n", req[i]);
    }
    printf("Req %d is completed\n", req[r - 1]);

    printf("Total seek time is: %d\n", s);
    return 0;
}

////////////////////////////////////////////

/// SCAN Disk Scheduling

#include <stdio.h>
#include <stdlib.h>

int main() {
    int r, hp, s = 0;

    printf("Enter the size of the request queue: ");
    scanf("%d", &r);

    int req[r], left[50], right[50];
    int l = 0, r1 = 0;

    printf("Enter the request queue: ");
    for (int i = 0; i < r; i++) {
        scanf("%d", &req[i]);
    }

    printf("Enter the initial head position: ");
    scanf("%d", &hp);

    // Separate requests into left and right of the head position
    for (int i = 0; i < r; i++) {
        if (req[i] <= hp) {
            left[l++] = req[i];
        } else {
            right[r1++] = req[i];
        }
    }

    // Sort left and right arrays
    for (int i = 0; i < l - 1; i++) {
        for (int j = 0; j < l - i - 1; j++) {
            if (left[j] > left[j + 1]) {
                int temp = left[j];
                left[j] = left[j + 1];
                left[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < r1 - 1; i++) {
        for (int j = 0; j < r1 - i - 1; j++) {
            if (right[j] > right[j + 1]) {
                int temp = right[j];
                right[j] = right[j + 1];
                right[j + 1] = temp;
            }
        }
    }

    // SCAN scheduling
    if (l > 0) s += abs(hp - left[l - 1]);
    for (int i = l - 1; i >= 0; i--) {
        printf("Req %d is completed\n", left[i]);
        if (i > 0) s += abs(left[i] - left[i - 1]);
    }
    if (r1 > 0) s += abs(left[0] - right[0]);
    for (int i = 0; i < r1; i++) {
        printf("Req %d is completed\n", right[i]);
        if (i < r1 - 1) s += abs(right[i] - right[i + 1]);
    }

    printf("Total seek time is: %d\n", s);
    return 0;
}


///////////////////////////////////////

// SSTF Disk Scheduling 

#include <stdio.h>
#include <stdlib.h>

int main() {
    int r, hp, s = 0;

    printf("Enter the size of the request queue: ");
    scanf("%d", &r);

    int req[r], temp[r];
    printf("Enter the request queue: ");
    for (int i = 0; i < r; i++) {
        scanf("%d", &req[i]);
    }

    printf("Enter the initial head position: ");
    scanf("%d", &hp);

    // SSTF scheduling
    for (int i = 0; i < r; i++) {
        for (int j = 0; j < r; j++) {
            temp[j] = abs(hp - req[j]);
        }
        int min = temp[0], pos = 0;
        for (int k = 1; k < r; k++) {
            if (min > temp[k] && req[k] != 9999) {
                min = temp[k];
                pos = k;
            }
        }

        s += abs(hp - req[pos]);
        printf("Req %d is completed\n", req[pos]);
        hp = req[pos];
        req[pos] = 9999; // Mark completed request
    }

    printf("Total seek time is: %d\n", s);
    return 0;
}

//////////////////////////////////
