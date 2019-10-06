#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Run program on terminal using gcc compiler

// Compile using Makefile or gcc -g -Wall -Werror -fsanitize=address -o cacheSimulator -lm cacheSimulator.c
//  ./cacheSimulator 32 assoc:2 lru 4 traceFile.txt (simulating a 2-way set associate cache of size 32 bytes with cache blocks of size 4 bytes)
// ./cacheSimulator 32 direct lru 4 traceFile.txt (simulating a direct cache)

// no-prefetch
int readsN = 0;
int writesN = 0;
int hitsN = 0;
int missesN = 0;

// with-prefetch
int readsW = 0;
int writesW = 0;
int hitsW = 0;
int missesW = 0;

int hitOrMiss = -1;
typedef struct Node{
  long int tag;
  struct Node* next;
} Node;

Node* createNode(long int t, Node* n){
  Node* ptr = (Node *)malloc(sizeof(Node));
  ptr->tag = t;
  ptr->next = n;
  return ptr;
}

//free nodes in cache
void freeCache(Node** cache, int numberOfSets){
  int i;
  for(i = 0; i<numberOfSets ; i++){
    Node* tmp;
    Node* ptr = cache[i];
    while(ptr!=NULL){
      tmp = ptr;
      ptr=ptr->next;
      free(tmp);
    }
  }
}


void printCache(Node** cache, int numberOfSets, int blocksPerRow){
  int i;
  int j;

  for(i = 0; i<numberOfSets ; i++){
    Node* curr = cache[i];
    for(j = 0; j<blocksPerRow ; j++){
      if (curr->next != NULL){
        curr=curr->next;
      }
      else{break;}
    }
  }
}

int runCache(Node** cache, long int tag, int setIndex, int numberOfSets, int blocksPerRow, int readORwrite, int policy, int preOrnopre){  //prefetch == 1, no pre == 2

  Node* ptr = cache[setIndex];
  Node* prev = cache[setIndex];
  Node* temp = (Node *) malloc(sizeof(Node));
  temp->tag = tag;

  if(ptr->tag == -1){ //cache is empty, insert at the front
    temp -> next = NULL;
    cache[setIndex]=temp;
    return 0; //miss
  }

  int found = 0;
  int i = 0;
  for(i = 0; i<blocksPerRow ; i++){
    if(ptr->tag == tag){
      found = 1;
      if(found == 1 && preOrnopre == 1){ return 1; } //hit and no replacement
      Node* firstNode = cache[setIndex];
      if(firstNode->tag == tag){
        return 1; //hit and no replacement because its already at the front
      }
      prev->next = ptr->next;
      ptr->next = NULL;
      break;
    }
    else{
      if(ptr->next != NULL){
        prev = ptr;
        ptr = ptr->next;
      }
      else{
        break;
      }
    }
  }


  if(found == 1 && preOrnopre == 1){ // hit and do not add to front
    return 1; // returning because prefetched block is already in the cache
  }
  if(found == 0){ // insert at the front of the cache
    temp -> next = cache[setIndex];
    cache[setIndex] = temp;
    return 0; // miss
  }
  if(found == 1){ // insert at the front of the cache
    temp -> next = cache[setIndex];
    cache[setIndex] = temp;
    return 1; // hit
  }

  return 0;// miss
}


char* HexToBin(char* hex){
  char* bin = malloc(sizeof(char)*49);

  int i = 0;
  for(i = 0; i<strlen(hex) ; i++){
    switch(hex[i]){
      case '0':
        strcat(bin,"0000");
        break;
      case '1':
        strcat(bin,"0001");
        break;
      case '2':
        strcat(bin,"0010");
        break;
      case '3':
        strcat(bin,"0011");
        break;
      case '4':
        strcat(bin,"0100");
        break;
      case '5':
        strcat(bin,"0101");
        break;
      case '6':
        strcat(bin,"0110");
        break;
      case '7':
        strcat(bin,"0111");
        break;
      case '8':
        strcat(bin,"1000");
        break;
      case '9':
        strcat(bin,"1001");
        break;
      case 'a':
      case 'A':
        strcat(bin,"1010");
        break;
      case 'b':
      case 'B':
        strcat(bin,"1011");
        break;
      case 'c':
      case 'C':
        strcat(bin,"1100");
        break;
      case 'd':
      case 'D':
        strcat(bin,"1101");
        break;
      case 'e':
      case 'E':
        strcat(bin,"1110");
        break;
      case 'f':
      case 'F':
        strcat(bin,"1111");
        break;
      default:
        break;
    }
  }

  return bin;
}


long int BinToDec (char* bin){
  int l = (int) strlen(bin);
  int i;
  long int dec = 0;
  int power = 0;
  for(i=l-1; i>=0 ; i--){
    dec += pow(2,power)*(bin[i]-'0'); // convert binary to int then multiply by index power
    power++;
  }
  return dec;
}


char* DecToBin (long int bin){
  int c;
  int d;
  int counter;
  char* pointer;

  counter = 0;
  pointer = malloc(sizeof(char)*49);

  for(c = 47 ; c>=0 ; c--){
    d = bin >> c;
    if(d & 1){
      *(pointer+counter) = 1 + '0';
    }
    else{
      *(pointer+counter) = 0 + '0';
    }
    counter++;
  }

  return pointer;

}


int main(int argc, char** argv){

  if (argc != 6){
    printf("error\n");
    return 0;
  }

  int cache_size = atoi(argv[1]); /*total size of the cache in bytes*/
  char* associativity = argv[2];
  char* cache_policy = argv[3];
  int block_size = atoi(argv[4]); /*size of the cache block in bytes*/

  if(strcmp(cache_policy,"lru") != 0){
    printf("error\n");
    return 0;
  }

  int assoc;
  int sets;

  if(strcmp(associativity,"direct") == 0){
    assoc = 1;
    sets = cache_size/block_size;
  }
  else if(strcmp(associativity,"assoc") == 0){ /*fully associative*/
    assoc = cache_size/block_size;
    sets = 1;
  }
  else{
    if(associativity[0] != 'a' || associativity[1] != 's' || associativity[2] != 's' || associativity[3] != 'o' || associativity[4] != 'c' || associativity[5] != ':') {
      printf("error\n");
      return 0;
    }

    sscanf(argv[2],"assoc:%d",&assoc);
    sets = cache_size/(block_size * assoc);
  }

  int setBits = (int) log2(sets);
  int blockBits = (int) log2(block_size);
  int tagBits = 48 - setBits - blockBits;

  int numberOfSets = sets;
  int blocksPerRow = assoc;

  Node** cache = (Node **) malloc(sizeof(Node *) * numberOfSets);
  Node** preCache = (Node **) malloc(sizeof(Node *) * numberOfSets);


  int i;

  for (i=0; i<numberOfSets; i++) { // filling in the cache
    Node* nod = createNode(-1,NULL);
    cache[i]=nod;
    preCache[i]=nod;
  }

  FILE* f = fopen(argv[5], "r");

  if (f == NULL){
    printf("error\n");
    return 0;
  }

  char progCounter[50];
  char operation[5];
  char memAddress[50];
  while(!feof(f)){

    fscanf(f, "%s %s %s\n" , progCounter, operation , memAddress);
    if (strcmp(progCounter,"#eof") == 0){
      break;
    }

    memAddress[strlen(memAddress)] = '\0';

    char hexadecimal[20]; // store mem address in here to convert

    memcpy(hexadecimal, &memAddress[2], strlen(memAddress)); // store without the 0x
    hexadecimal[strlen(hexadecimal)] = '\0';

    char* binary; // convert hex to binary and store in here
    binary = HexToBin(hexadecimal);
    binary[4*strlen(hexadecimal)] = '\0';

    /*leading 0s*/
    int leadingZeros = 48 - strlen(binary);
    char zeros[49];
    int z;
    int endZeros = 0;
    for (z = 0; z<leadingZeros; z++){
      zeros[z]= '0';
      endZeros++;
    }
    zeros[leadingZeros] = '\0';


    strcat(zeros,binary);
    zeros[48]='\0';

    char* setbitArr = (char*) malloc(sizeof(char) * (setBits+1));
    char* blockbitArr = (char*) malloc(sizeof(char) * (blockBits+1));
    char* tagbitArr = (char*) malloc(sizeof(char) * (tagBits+1));
    setbitArr[setBits] = '\0';
    blockbitArr[blockBits] = '\0';
    tagbitArr[tagBits] = '\0';

    for(i = 0; i<tagBits; i++){
      tagbitArr[i] = zeros[i];
    }

    int counter = 0;
    for(i = tagBits; i<setBits+tagBits;i++){
      setbitArr[counter] = zeros[i];
      counter++;
    }

    counter = 0;
    for(i = setBits+tagBits; i<setBits+tagBits+blockBits;i++){
      blockbitArr[counter] = zeros[i];
      counter++;
    }

    long int tagInt = BinToDec(tagbitArr);
    int indexInt = (int) BinToDec(setbitArr);

    char* decimalOfHex;
    decimalOfHex = HexToBin(hexadecimal);
    long int decPre = BinToDec(decimalOfHex) + block_size;

    char* preBin; // convert hex to binary to store
    preBin = DecToBin(decPre);
    preBin[strlen(preBin)] = '\0';

    char* setbitArrPre = (char*) malloc(sizeof(char) * (setBits+1));
    char* blockbitArrPre = (char*) malloc(sizeof(char) * (blockBits+1));
    char* tagbitArrPre = (char*) malloc(sizeof(char) * (tagBits+1));
    setbitArrPre[setBits] = '\0';
    blockbitArrPre[blockBits] = '\0';
    tagbitArrPre[tagBits] = '\0';

    for(i = 0; i<tagBits; i++){
      tagbitArrPre[i] = preBin[i];
    }

    counter = 0;
    for(i = tagBits; i<setBits+tagBits;i++){
      setbitArrPre[counter] = preBin[i];
      counter++;
    }

    counter = 0;
    for(i = setBits+tagBits; i<setBits+tagBits+blockBits;i++){
      blockbitArrPre[counter] = preBin[i];
      counter++;
    }

    long int tagIntPre = BinToDec(tagbitArrPre);
    int indexIntPre = (int) BinToDec(setbitArrPre);

    if(runCache(cache, tagInt, indexInt, numberOfSets, blocksPerRow, 1, 1, 2)==1){ // hit
      hitsN++;
      if(strcmp(operation,"W") == 0){
        writesN++;
      }
    }
    else{ // miss
      missesN++;
      if(strcmp(operation,"R") == 0){
        readsN++;
      }
      if(strcmp(operation,"W") == 0){
        writesN++;
        readsN++;
      }
    }

    // PREFETCHER
    if(runCache(preCache, tagInt, indexInt, numberOfSets, blocksPerRow, 1, 1, 2)==1){ // hit
      hitsW++;
      if(strcmp(operation,"W") == 0){
        writesW++;
      }
    }
    else{// miss
      missesW++;
      if(strcmp(operation,"R") == 0){
        readsW++;
      }
      if(strcmp(operation,"W") == 0){
        writesW++;
        readsW++;
      }
      if (runCache(preCache, tagIntPre, indexIntPre, numberOfSets, blocksPerRow, 1, 1, 1) != 1){
        readsW++;
      }
    }
  }

  fclose(f);
  freeCache(cache,numberOfSets);
  free(cache);
  free(preCache);

  printf("no-prefetch\n");
  printf("Memory reads: %d\n", readsN);
  printf("Memory writes: %d\n", writesN);
  printf("Cache hits: %d\n",hitsN);
  printf("Cache misses: %d\n",missesN);
  printf("with-prefetch\n");
  printf("Memory reads: %d\n", readsW);
  printf("Memory writes: %d\n", writesW);
  printf("Cache hits: %d\n",hitsW);
  printf("Cache misses: %d\n",missesW);

  return 0;
}
