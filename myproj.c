#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

# define NO_OF_CHARS 256
 
// A utility function to get maximum of two integers
int max (int a, int b) { return (a > b)? a: b; }
 
// The preprocessing function for Boyer Moore's
// bad character heuristic
void badCharHeuristic( char *str, int size, 
                        int badchar[NO_OF_CHARS])
{
    int i;
 
    // Initialize all occurrences as -1
    for (i = 0; i < NO_OF_CHARS; i++)
         badchar[i] = -1;
 
    // Fill the actual value of last occurrence 
    // of a character
    for (i = 0; i < size; i++)
         badchar[(int) str[i]] = i;
}
 
/* A pattern searching function that uses Bad
   Character Heuristic of Boyer Moore Algorithm */
void search( char *txt,  char *pat)
{
    int counter = 1;
    int m = strlen(pat);
    int n = strlen(txt);
 
    int badchar[NO_OF_CHARS];
 
    /* Fill the bad character array by calling 
       the preprocessing function badCharHeuristic() 
       for given pattern */
    badCharHeuristic(pat, m, badchar);
 
    int s = 0;  // s is shift of the pattern with 
                // respect to text
    while(s <= (n - m))
    {
        int j = m-1;
 
        /* Keep reducing index j of pattern while 
           characters of pattern and text are 
           matching at this shift s */
        while(j >= 0 && pat[j] == txt[s+j])
            j--;
 
        /* If the pattern is present at current
           shift, then index j will become -1 after
           the above loop */
        if (j < 0)
        {
            printf("\n Boyer Moore-->pattern occurs at shift = %d", s);
 
            /* Shift the pattern so that the next 
               character in text aligns with the last 
               occurrence of it in pattern.
               The condition s+m < n is necessary for 
               the case when pattern occurs at the end 
               of text */
            s += (s+m < n)? m-badchar[txt[s+m]] : 1;
 
        }
 
        else
            /* Shift the pattern so that the bad character
               in text aligns with the last occurrence of
               it in pattern. The max function is used to
               make sure that we get a positive shift. 
               We may get a negative shift if the last 
               occurrence  of bad character in pattern
               is on the right side of the current 
               character. */
            s += max(1, j - badchar[txt[s+j]]);
            counter++;
    }
    if (counter == m)
    {
        printf("The pattern is not found!\n");
    }
}

// Fills lps[] for given patttern pat[0..M-1]
void computeLPSArray(char *pat, int M, int *lps)
{
    // length of the previous longest prefix suffix
    int len = 0;
 
    lps[0] = 0; // lps[0] is always 0
 
    // the loop calculates lps[i] for i = 1 to M-1
    int i = 1;
    while (i < M)
    {
        if (pat[i] == pat[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        else // (pat[i] != pat[len])
        {
            // This is tricky. Consider the example.
            // AAACAAAA and i = 7. The idea is similar 
            // to search step.
            if (len != 0)
            {
                len = lps[len-1];
 
                // Also, note that we do not increment
                // i here
            }
            else // if (len == 0)
            {
                lps[i] = 0;
                i++;
            }
        }
    }
}

void KMPSearch(char *pat, char *txt)
{
    int M = strlen(pat);
    int N = strlen(txt);
    
    // create lps[] that will hold the longest prefix suffix
    // values for pattern
    int lps[M];
 
    // Preprocess the pattern (calculate lps[] array)
    computeLPSArray(pat, M, lps);
    
    int i = 0;  // index for txt[]
    int j  = 0;  // index for pat[]
    while (i < N)
    {

        if (pat[j] == txt[i])
        {

            j++;
            i++;
        }
 
        if (j == M)
        {
            printf("\n KMP-->Found pattern at index %d ", i-j);
            j = lps[j-1];
        }
 
        // mismatch after j matches
        else if (i < N && pat[j] != txt[i])
        {
            // Do not match lps[0..lps[j-1]] characters,
            // they will match anyway
            if (j != 0)
                j = lps[j-1];
            else
                i = i+1;
        }
    }
}
 


int main(int argc, char** argv) 
{
    
    int npes, rank, length;
    char *data;
    //arrays for splitting the data 
    char *split1;
    char *split2;
    char *split3;
    //arrays for sending the split data to processors
    char *sending1;
    char *sending2;
    char *sending3;
    //arrays to receive the split data at all the processors
    char *recvsplit1;
    char *recvsplit2;
    char *recvsplit3;
    //
    int patlen;
    //
    int len1;
    int len2;
    int len3;
    //arrays to recieve the pattern at processors
    char *recvpat1;
    char *recvpat2;
    char *recvpat3;
    char *sendpat;
    //for length of the pattern recv
    int patlenrecv1;
    int patlenrecv2;
    int patlenrecv;
    //for further split
    int size1;
    int rem1;
    //arrays for further decomposition at rank 1
    char *rank1send1;
    char *rank1send2;
    char *rank1send3;
    //arrays for receiving further decomposition at rank 1
    char *rank1recv1;
    char *rank1recv2;
    char *rank1recv3;
    //arrays to receive further decomposition for rank 0
    char *rank0recv1;
    char *rank0recv2;
    char *rank0recv3;
    //arrays for further decomposition at rank 2
    char *rank2send1;
    char *rank2send2;
    char *rank2send3;
    //
    char *rank2recv1;
    char *rank2recv2;
    char *rank2recv3;
    //arrays for further decomposition at rank 3
    char *rank3send1;
    char *rank3send2;
    char *rank3send3;
    //
    char *rank3recv1;
    char *rank3recv2;
    char *rank3recv3;
   


    //for pattern length receiving
    int rpatlen1;
    int rpatlen2;
    int rpatlen3;
    int c;
    size_t b = 0;
    char *pattern;
    pattern = (char*) malloc(sizeof(char));
    MPI_Init(&argc, &argv);
    MPI_Status stat;
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int d = 1;

      FILE* file;
       //getting the file name
        char filename[13] = "sequence.txt"; 
        filename[13] = '\0';
        printf("%s\n", filename);
        //opening the file
        file = fopen(filename, "r");
        if (!file) 
        {
            printf("Could not open %s for reading.\n", filename);
        }
        else 
        {
            printf("Opened!\n");
        }

        //finding the length of the file
        /* find the length of the file */
        fseek(file, 0L, SEEK_END);
        length = ftell(file);
        fseek(file, 0L, SEEK_SET);
        //printing lenght of the file
        printf("Lenght of the file is: %d\n", length );
        //for first stage of decomposition
        int size = length/3;
        int rem = length - (size + size);
        //for further decomposition
        size1 = size/3;
        rem1 = size - (size1 + size1);

    //arrays for further decomposition at rank 1
    rank1send1 = (char*) malloc(size1 * sizeof(char) + 1); 
    rank1send2 = (char*) malloc(size1 * sizeof(char) + 1);
    rank1send3 = (char*) malloc(rem1 * sizeof(char) + 1);
    
    rank0recv1 = (char*) malloc(size1 * sizeof(char) + 1); 
    rank0recv2 = (char*) malloc(size1 * sizeof(char) + 1);
    rank0recv3 = (char*) malloc(rem1 * sizeof(char) + 1);

    //arrays for further decomposition at rank 2
    rank2send1 = (char*) malloc(size1 * sizeof(char) + 1); 
    rank2send2 = (char*) malloc(size1 * sizeof(char) + 1);
    rank2send3 = (char*) malloc(rem1 * sizeof(char) + 1);
    
    //arrays for further decomposition at rank 3
    rank3send1 = (char*) malloc(size1 * sizeof(char) + 1); 
    rank3send2 = (char*) malloc(size1 * sizeof(char) + 1);
    rank3send3 = (char*) malloc(rem1 * sizeof(char) + 1);
    
    //arrays for receiving further decomposition at rank 1
    rank1recv1 = (char*) malloc(size1 * sizeof(char) + 1);
    rank1recv2 = (char*) malloc(size1 * sizeof(char) + 1);
    rank1recv3 = (char*) malloc(rem1 * sizeof(char) + 1);
    
    //arrays for receiving further decomposition at rank 2
    rank2recv1 = (char*) malloc(size1 * sizeof(char) + 1);
    rank2recv2 = (char*) malloc(size1 * sizeof(char) + 1);
    rank2recv3 = (char*) malloc(rem1 * sizeof(char) + 1);
 
    //arrays for receiving further decomposition at rank 3
    rank3recv1 = (char*) malloc(size1 * sizeof(char) + 1);
    rank3recv2 = (char*) malloc(size1 * sizeof(char) + 1);
    rank3recv3 = (char*) malloc(rem1 * sizeof(char) + 1);
   

    sending1 = (char*) malloc(size * sizeof(char) + 1);   
    sending2 = (char*) malloc(size * sizeof(char) + 1);
    sending3 = (char*) malloc(rem * sizeof(char) + 1); 
    recvsplit1 = (char*) malloc(size * sizeof(char) + 1);   
    recvsplit2 = (char*) malloc(size * sizeof(char) + 1);
    recvsplit3 = (char*) malloc(size * sizeof(char) + 1); 

    if (rank == 0)
    {
    
         /* first split of data from the file*/
        split1 = malloc(size * sizeof(char) + 1);
        memset(split1, 0, size);
        fread(split1, sizeof(char), size, file);
        strcpy(sending1, split1);
        
     
     
        //all the data in the file is in data now so process with that!!!
        /* second split of data from the file*/
        split2 = malloc(size * sizeof(char) + 1);
        memset(split1 + size, 0, size);
        fread(split2, sizeof(char), size, file);
        strcpy(sending2, split2);

    

        /* third split of data from the file*/
        split3 = malloc(rem * sizeof(char) + 1);
        memset(split2 + rem, 0, rem);
        fread(split3, sizeof(char), rem, file);
        strcpy(sending3, split3);

        
        //sending the splits to the processors
        printf("Entered rank 0\n");
        MPI_Send(sending1, size  , MPI_CHAR, 1, 0, MPI_COMM_WORLD);//sendin the split to the processors
        MPI_Send(sending2, size  , MPI_CHAR, 2, 0, MPI_COMM_WORLD);
        MPI_Send(sending3, rem  , MPI_CHAR, 3, 0, MPI_COMM_WORLD);
    


        printf("Enter the pattern to be searched: \n" );
        scanf("%s",pattern);
        /* below code block is to send the pattern length to the prcesses to create array*/
        patlen = strlen(pattern);
        printf("Size of patlen: %d\n", patlen );
        int patsend = patlen;
        // printf("Size of patsend %d\n", patsend);
        // sendpat = (char*) malloc(patsend * sizeof(char));
        // strcpy(sendpat, pattern);

        //sending pattern to the processors
        MPI_Send(&patlen, 1 , MPI_INT, 1, 1, MPI_COMM_WORLD);
        MPI_Send(&patlen, 1 , MPI_INT, 2, 1, MPI_COMM_WORLD);
        MPI_Send(&patlen, 1 , MPI_INT, 3, 1, MPI_COMM_WORLD);
       	        
        MPI_Send(pattern, patsend , MPI_CHAR, 1, 1, MPI_COMM_WORLD);//only patter sending to the processors
        MPI_Send(pattern, patsend , MPI_CHAR, 2, 1, MPI_COMM_WORLD);
        MPI_Send(pattern, patsend , MPI_CHAR, 3, 1, MPI_COMM_WORLD);

        //receving further decomposed data
        MPI_Recv(rank0recv1, size1, MPI_CHAR, 1, 1, MPI_COMM_WORLD, &stat);
       // printf("Received further decomposition at rank 0: %s\n", rank0recv1 );
        //printf("Received further decomposition at rank 0: %s\n", rank0recv1 );
      	 MPI_Recv(rank0recv2, size1, MPI_CHAR, 2, 0, MPI_COMM_WORLD, &stat);
     	//printf("Received further decomposition at rank 0: %s\n", rank0recv2 );
         MPI_Recv(rank0recv3, rem1, MPI_CHAR, 3, 7, MPI_COMM_WORLD, &stat);
     	 search(rank0recv1, pattern);
     	 search(rank0recv2, pattern);
     	 search(rank0recv3, pattern);
        
     
   
    }
    
    if (rank == 1)
    {
        /*
        Boyer moore algorithm for pattern matching  
        */
        int rc1;
        printf("enter rank 1\n\n" );
        MPI_Recv(recvsplit1, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
        //printf("%s\n", recvsplit1 );
  
     
        MPI_Recv(&rc1,1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
        printf("\nThe received size of the string is: %d", rc1);
        recvpat1 = (char*) malloc(5*sizeof(char));
        MPI_Recv(recvpat1, rc1, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &stat);
        
        printf("The recieved pattern at rank 1: %s\n", recvpat1);
        //search(recvsplit1, recvpat1);

        //further decomposition of data
        for(int i=0;i<size1;i++)
        {
             rank1send1[i] = recvsplit1[i];
             
        }
         int k=0;
        printf("\n furthersplit2 array\n\n");
        for(int i=size1;i<(2*size1);i++)
        {
            rank1send2[k] = recvsplit1[i];
           
            k++;
        }

        k=0;
        printf("\n furthersplit3 array\n\n");
        for(int i=(2*size1);i<size;i++)
        {
            rank1send3[k] = recvsplit1[i];
            k++;
        } 
        //sending further decomposed array 
         MPI_Send(rank1send1, size1, MPI_CHAR, 0, 1, MPI_COMM_WORLD); 
         MPI_Send(rank1send2, size1, MPI_CHAR, 2, 6, MPI_COMM_WORLD);
         MPI_Send(rank1send3, rem1, MPI_CHAR, 3, 3, MPI_COMM_WORLD);   

         MPI_Recv(rank1recv1, size1, MPI_CHAR, 2, 2, MPI_COMM_WORLD, &stat);
         
   		 MPI_Recv(rank1recv2, size1, MPI_CHAR, 3, 2, MPI_COMM_WORLD, &stat);


   		//boyer moore function call for rank 1
   		 search(rank1recv1, recvpat1);
   		 search(rank1recv2, recvpat1);
    }

    if (rank == 2)
    {
        /*
        Boyer moore algorithm for pattern matching  
        */
    	int rc2;
        printf("enter rank 2\n\n" );
        MPI_Recv(recvsplit2, size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
        //printf("%s\n", recvsplit2 );
   		MPI_Recv(&rc2,1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
        printf("\nThe received size of the string is: %d", rc2);
     
        recvpat2 = (char*) malloc(5*sizeof(char));
        MPI_Recv(recvpat2, rc2, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &stat);
        
        printf("The recieved pattern at rank 2: %s\n", recvpat2);
      	// search(recvsplit2, recvpat2);
      	
        //further decomposition of data at rank 2
        for(int i=0;i<size1;i++)
        {
             rank2send1[i] = recvsplit2[i];
             
        }
        int k=0;
      
        for(int i=size1;i<(2*size1);i++)
        {
            rank2send2[k] = recvsplit2[i];
           
            k++;
        }
     	
        k=0;
        for(int i=(2*size1);i<size;i++)
        {
            rank2send3[k] = recvsplit2[i];
            k++;
        } 
        
        //receving further decomposed data
         MPI_Recv(rank2recv1, size1, MPI_CHAR, 1, 6, MPI_COMM_WORLD, &stat);	
        // //sending further decomposed data
        MPI_Send(rank2send1, size1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);		
        MPI_Send(rank2send2, size1, MPI_CHAR, 1, 2, MPI_COMM_WORLD);
        MPI_Send(rank2send3, size1, MPI_CHAR, 3, 3, MPI_COMM_WORLD);

        MPI_Recv(rank2recv2, rem1, MPI_CHAR, 3, 3, MPI_COMM_WORLD, &stat);

        //boyer moore
        search(rank2recv1, recvpat2);
        search(rank2recv2, recvpat2);

       

    }

       
    if (rank == 3)
    {
    	int rc3;
        printf("Entered rank 3 \n");
        MPI_Recv(recvsplit3, rem, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
        //printf("%s\n", recvsplit3 );
        MPI_Recv(&rc3,1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat);
        printf("\nThe received size of the string is: %d", rc3);
     	//for pattern
        recvpat3 = (char*) malloc(5*sizeof(char));
        MPI_Recv(recvpat3, rc3, MPI_CHAR, 0, 1, MPI_COMM_WORLD, &stat);
       	printf("The recieved pattern at rank 3: %s\n", recvpat3);
       	//search(recvsplit3, recvpat3);
    //    	search(recvsplit3, recvpat3 );

       	for(int i=0;i<size1;i++)
        {
             rank3send1[i] = recvsplit3[i];
             
        }
        
        int k=0;
     
        for(int i=size1;i<(2*size1);i++)
        {
            rank3send2[k] = recvsplit3[i];
           
            k++;
        }
     
        k=0;
      
        for(int i=(2*size1);i<size;i++)
        {
            rank3send3[k] = recvsplit3[i];
            k++;
        } 

        //receving further decomposed data
         MPI_Recv(rank3recv1, rem1, MPI_CHAR, 1, 3, MPI_COMM_WORLD, &stat);

         MPI_Recv(rank3recv2, rem1, MPI_CHAR, 2, 3, MPI_COMM_WORLD, &stat);

         MPI_Send(rank3send1, size1, MPI_CHAR, 0, 7, MPI_COMM_WORLD);
         MPI_Send(rank3send2, size1, MPI_CHAR, 1, 2, MPI_COMM_WORLD);
         MPI_Send(rank3send3, rem1, MPI_CHAR, 2, 3, MPI_COMM_WORLD);

        //boyer moore function call for rank 3
         search(rank3recv1, recvpat3);
         search(rank3recv2, recvpat3);
	}
    
    //Finalize the MPI environment.
    MPI_Finalize();
}
