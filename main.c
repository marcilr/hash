/*
 * main.c
 * Created Wed Aug  7 13:15:06 AKDT 2002
 * This is a quick test driver for the Hash ADT.
 */

#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "str.h"

/* USGS record structure */
struct quadData {
  char      quadname[41];
  char      state[3];
  char      drgname[9];
  float     x1,y1,x2,y2;
} quadData;


/* Function Prototypes */
void testSimple(void);
void testDatafile(void);
void testDatafile2(Hash **hash, const char *datafile);
void destructor(void *data);
void printer(void *data);
void printer2(void *data);

int main(void){
  Hash *hash;
  const char   datafile[]="data/63360.lst";

  /* Perform simple tests */
  testSimple();

  /* Test against data file */
  /*
  testDatafile();
  */

  /* Test using testDatafile2() */
  testDatafile2(&hash,datafile);
  printf("testDatafile(): hashDestroy\n");
  hashDestroy(hash,&destructor);

  return 0;
}

/* ===================== testSimple() ==================== */
/* ===================== testSimple() ==================== */

/*
 * testSimple
 * This function performs some simple tests of the
 * hash ADT.
 *
 * INPUT:     NONE
 * USES:      printer(), destructor()
 *
 */
void testSimple(void){

  Hash         *hash;
  unsigned int  count = 0;
  char         *data;
  char         *data2;
  char         *data3;

  printf("testSimple()\n");

  data = (char *) malloc (strlen("dogmeat") * (sizeof(char)));
  strcpy(data,"dogmeat");

  data3 = (char *) malloc (strlen("catmeat") * (sizeof(char)));
  strcpy(data3,"catmeat");

  printf("Hash ADT - Test driver by Raymond E. Marcil\n");

  /* Create a new hash */
  hash = hashCreate(10);

  printf("Test hashSize(): num_buckets: %u\n",hashSize(hash));
  printf("Test hashCount(): %u\n",hashCount(hash));

  printf("Test hashAdd(): %s\n",data);
  hashAdd(hash, "key1",data);
  printf("Test hashCount(): %u\n",hashCount(hash));

  printf("Test hashAdd(): %s\n",data3);
  hashAdd(hash, "key3",data3);
  printf("Test hashCount(): %u\n",hashCount(hash));

  printf("Test hashPrint(): \n");
  hashPrint(hash, &printer);

  printf("Test hashGet(): ");
  data2 = hashGet(hash,"key1");
  printf("data2: %s\n",data2);

  /* Destroy hash */
  printf("Test hashDestroy()\n");
  hashDestroy(hash, &destructor);

}

/* ==================== testDataFile() =================== */
/* ==================== testDataFile() =================== */

/*
 * testDatafile()
 * This function tests the hash ADT using
 * a large USGS datafile of quadrangle
 * meta information.
 */
void testDatafile(void){
  Hash   *hash;
  char   line[256];

  struct quadData  *data;

  const char   datafile[]="data/63360.lst";
  FILE         *fp;

  if ((fp = fopen(datafile,"r")) == NULL){
    printf("Cannot open file: %s\n",datafile);
    exit(1);
  }

  /* Create a new hash */
  printf("testDatafile(): hashCreate()\n");
  hash = hashCreate(10);

  printf("testDatafile(): count: %d\n",hashCount(hash));
  printf("testDatafile(): num_buckets: %d\n",hashSize(hash));

  while (lineRead(fp,line,256) != EOF){

    data = (struct quadData *) malloc (sizeof(struct quadData));
    if (data != NULL){

      memset(data->quadname,0,sizeof(data->quadname));
      memset(data->state,0,sizeof(data->state));
      memset(data->drgname,0,sizeof(data->drgname));

      sscanf(line,"%40c%s%s%f%f%f%f",
             &data->quadname,
             &data->state,
             &data->drgname,
             &data->x1,
             &data->y1,
             &data->x2,
             &data->y2);

      /* Clean up data */
      strTrimTail(data->quadname);
      strStrip(data->drgname);

      /* Add container to hash */
      hashAdd(hash,data->drgname,data);
    }
    else {

        printf("Error allocating memory, aborting...\n");
        exit(1);

    } /* end if (data != NULL) */

  } /* end while (lineRead(fp,line,256) != EOF) */

  hashPrint(hash,&printer2);

  printf("testDatafile(): count: %d\n",hashCount(hash));
  printf("testDatafile(): num_buckets: %d\n",hashSize(hash));

  /* Destroy hash */
  printf("testDatafile(): hashDestroy\n");
  hashDestroy(hash,&destructor);

  fclose(fp);
}
/* ==================== testDataFile2() =================== */
/* ==================== testDataFile2() =================== */

/*
 * testDatafile2()
 * This function tests the hash ADT using
 * a large USGS datafile of quadrangle
 * meta information.
 */
void testDatafile2(Hash **hash, const char *datafile){

  char              line[256];
  struct quadData  *data;
  FILE             *fp;

  if ((fp = fopen(datafile,"r")) == NULL){
    printf("Cannot open file: %s\n",datafile);
    exit(1);
  }

  /* Create a new hash */
  printf("testDatafile(): hashCreate()\n");
  *hash = hashCreate(10);

  printf("testDatafile(): count: %d\n",hashCount(*hash));
  printf("testDatafile(): num_buckets: %d\n",hashSize(*hash));

  while (lineRead(fp,line,256) != EOF){

    data = (struct quadData *) malloc (sizeof(struct quadData));
    if (data != NULL){

      memset(data->quadname,0,sizeof(data->quadname));
      memset(data->state,0,sizeof(data->state));
      memset(data->drgname,0,sizeof(data->drgname));

      sscanf(line,"%40c%s%s%f%f%f%f",
             &data->quadname,
             &data->state,
             &data->drgname,
             &data->x1,
             &data->y1,
             &data->x2,
             &data->y2);

      /* Clean up data */
      strTrimTail(data->quadname);
      strStrip(data->drgname);

      /* Add container to hash */
      hashAdd(*hash,data->drgname,data);
    }
    else {

        printf("Error allocating memory, aborting...\n");
        exit(1);

    } /* end if (data != NULL) */

  } /* end while (lineRead(fp,line,256) != EOF) */

  hashPrint(*hash,&printer2);

  printf("testDatafile(): count: %d\n",hashCount(*hash));
  printf("testDatafile(): num_buckets: %d\n",hashSize(*hash));

  fclose(fp);
}


/* ===================== destructor() ==================== */
/* ===================== destructor() ==================== */

/*
 * destructor()
 * This is a simple destructor for strings
 * required for use of the stack ADT.
 *
 * INPUT:       data    Pointer to data to deallocate
 * RETURNS:     NONE
 */

void destructor(void *data){

  if (data != NULL)
        free(data);

} /* end destructor */



/* ======================= printer() ===================== */
/* ======================= printer() ===================== */

/*
 * printer()
 * This function required by stackPrint() to
 * print the data container.
 *
 * INPUT:    *data     Pointer to data section.
 */

void printer(void *data){

  printf("  vkey: %s\n",(char *)data);

}

/*
 * printer2()
 * This function knows how to print the USGS records.
 */
void printer2(void *data){

  printf("%s, %s, %s, (%3.3f,%3.3f), (%3.3f,%3.3f)\n",
    ((struct quadData *)data)->quadname,
    ((struct quadData *)data)->state,
    ((struct quadData *)data)->drgname,
    ((struct quadData *)data)->x1,
    ((struct quadData *)data)->y1,
    ((struct quadData *)data)->x2,
    ((struct quadData *)data)->y2);

}
