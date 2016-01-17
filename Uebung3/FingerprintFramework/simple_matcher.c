/**
 * Simple C-Tool that performes the simple matching algorithm. 
 * The -h flag can be used to toggle the use of the Hough-Transformation
 * The -p flag is used to determine a probe image in the xyt format
 * The -g flag is used to determine the gallery, either a single image
 * 	or a complete directory with .xyt files
 * The -s flag can be used to test either a single file (when set) in this
 * 	case the -g flag is the path of a single .xyt file, or (when not set)
 *      a whole directory. In this case the -g flag is the path to the dir. 
 * 
 * ************ ASSIGNMENT ***************
 * Implement the functions loadMinutiae, getScore and alignment.
 * The function documentation of the functions, and the exercise 
 * descriptions, contain more information !
 *
 * DO NOT CHANGE ANYTHING IN THE OTHER FUNCTIONS! 
 * BUT YOU ARE FREE TO ADD ADDITIONAL FUNCTIONS.
 * 
 * THE PROGRAMM HAS TO BE COMPILED WITH GCC ON A LINUX SYSTEM VIA:
 * 		gcc -O2 simple_matcher.c -lm -o simple_matcher
 * IT CAN BE EXECUTED VIA:
 * 		./simple_matcher -p images/xxx_x.xyt -g images [-h]
 * ************ ASSIGNMENT ***************
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define MAX_MINUTIAE    130			/* should be ajusted if a file has more minutiae */
#define A_X		400			/* used for Array in alignment, should be */
#define A_Y		500			/* adjusted if out of boundaries error occurs*/
#define threshold_d 	14			/* for getScore */
#define threshold_r 	18			/* for getScore */
#define thres_t		18			/* for alignment */
#define PI 		3.14159 
// Beim Einlesen der xyt-Dateine: 
// Max 16 Zeichen pro Zeile: 3x dreistellig, 1x zweistellig, 3x Leerzeichen, \n und \0
#define ZEILENLAENGE    16 
#define MIN(a,b) (((a)<(b))?(a):(b))

int n = 1;
//Array zu groß fuer den Stack. Hier anlegen, um es auf den Heapspeicher zu legen
static int a[A_X][A_Y][thres_t];

struct xyt_struct {
	int nrows;
	int xcol[MAX_MINUTIAE];
	int ycol[MAX_MINUTIAE];
	int thetacol[MAX_MINUTIAE];
};

struct node {
	struct xyt_struct data;
	char* filepath;
	int score;
	struct node *next;
};

void print_usage(char*);
void test_single(char*, char*, int);
void test_multiple(char*, char*, int);
struct  xyt_struct alignment(struct  xyt_struct probe,struct xyt_struct gallery);
struct xyt_struct loadMinutiae(const char *xyt_file);
int getScore(struct  xyt_struct probe,struct xyt_struct gallery);
double getRad(int degree);

int main( int argc, char ** argv )
{
	/* Shall Hough Transformation be used? */
	int hflag = 0;
	/* If set only one file is tested and galleryname is used a file name
	   If not set, galleryname is used as a directory name and all files in
	   this directory are compared to the probe image */
	int sflag = 0;
	/* String to the probe-image */
	char* probename = NULL;
	/* String to the gallery-image */
	char* galleryname = NULL;
	/* Name of this programm - needed for error msg */
	char* thisfile = argv[0];

	/* ARGUMENT PARSING - START */
	if(argc < 4) {
		print_usage(thisfile);
		exit(1);
	}

	int c = 0;
	opterr = 0;
	while ((c = getopt(argc, argv, "p:g:hs")) != -1) {
		switch (c) {
			case 'p':
				probename = optarg;
				break;
			case 'g':
				galleryname = optarg;
				break;
			case 'h':
				hflag = 1;
				break;
			case 's':
				sflag = 1;
				break;
			case '?':
				if(optopt == 'p' || optopt == 'g') {
					printf("Opion -%c requires an argument!\n", optopt);
					print_usage(thisfile);
					return 1;
				}
			default:
				print_usage(thisfile);
				return 1;
		}
	}

	if(probename[0] == '-') {
		printf("Opion -p requires an argument!\n");
		print_usage(thisfile);
		return 1;
	}

	if(galleryname[0] == '-') {
		printf("Opion -g requires an argument!\n");
		print_usage(thisfile);
		return 1;
	}
	/* ARGUMENT PARSING - DONE */


	/* Test Minutiae, either single file or whole directory */
	if (sflag) {
		test_single(probename, galleryname, hflag);
	} else {
		test_multiple(probename, galleryname, hflag);
	}

	return 0;
}

void print_usage(char* thisfile) {
	printf("USAGE: %s -p probe-image -g gallery-image [-h] [-s]\n", thisfile);
	printf("\t -p \t The probe image that has to be tested!\n");
	printf("\t -g \t Path to galery images!\n");
	printf("\t[-h]\t If set the Hough Transformation is used!\n");
	printf("\t[-s]\t If set only a single file is tested and -g takes filename!\n");
	printf("\n");
}

void test_single(char* probename, char* galleryname, int hflag) {
	/* Load .xyt images to struct */
	struct xyt_struct probe = loadMinutiae(probename);
	struct xyt_struct gallery = loadMinutiae(galleryname);
	
	/* If Hough Should be used */
	if(hflag) {
		gallery = alignment(probe, gallery);
	}

	/* Calculate the score between the two .xyt images */
	int score = getScore(probe,gallery);	

	printf("The score is: %d\n",score);
}

void test_multiple(char* probename, char* dirname, int hflag) {
	/* Needed for dir scanning */
	DIR *d;
	struct dirent *dir;
	/* Needed for linked list of files */
	struct node *root;	//Root node
	struct node *curr;	//Current node
	/* To be loaded path of file is composed in it */
	char* toload = malloc(256 * sizeof(char));

	/* Create a new root node */
	root = (struct node *) malloc(sizeof(struct node));
	root->next = NULL;
	curr = root;

	printf("Looking for images in dir: %s\n", dirname);

	d = opendir(dirname);
	if(d) {
		while((dir = readdir(d)) != NULL) {
			//Leave out '.' and '..'
			if(dir->d_name[0] == '.') {
				continue;
			}
			int len = strlen(dir->d_name);

			//If the filename ends with xyt load the image to our list
			if(dir->d_name[len - 3] == 'x' && dir->d_name[len - 2] == 'y' && dir->d_name[len - 1] == 't') {
				//Compose the path of the to be loaded image in toload
				snprintf(toload, 256 * sizeof(char), "%s/%s", dirname, dir->d_name);

				//Store pathname in the struct
				int path_len = strlen(toload);
				curr->filepath = malloc(path_len * sizeof(char));
				strcpy(curr->filepath, toload);

				//Store loaded Minutiae in single linked list and add new next node
				curr->data = loadMinutiae(toload);
				curr->next = (struct node *) malloc(sizeof(struct node));
				curr = curr->next;
				//If the pointer returned by malloc was null we ran out of memory
				if(curr == NULL) {
					printf("Out of memory!\n");
					exit(1);
				}
				//Set next node to NULL so we have a sentinel at the end. 
				curr->next = NULL;
			}
		}
		//Close directory and free filepath composer when all files are loaded
 		closedir(d);
		free(toload);
	}

	//Load the probe image
	struct xyt_struct probe = loadMinutiae(probename);

	//Set the current node back to root, so we can traverse the list again.
	curr = root;

	if(curr != NULL) {
		while(curr != NULL && curr->filepath != NULL) {

			/* If Hough Transformation shall be used */
			if(hflag) {
				curr->data = alignment(probe, curr->data);
			}

			curr->score = getScore(probe, curr->data);

 			printf("Node: %p, Path: %s, Score: %d\n", curr, curr->filepath, curr->score);

			curr = curr->next;
		}
	} else {
		printf("The list is empty!\n");
		exit(1);
	}
}
 
 
 
/** 
 * TODO: Implement the Generalised Hough Transform
 * 
 * The probe image, is the image you want to align, the gallery image to. 
 * Return the aligned gallery xyt_struct as the result of this function.
 */
struct  xyt_struct alignment(struct  xyt_struct probe, struct xyt_struct galleryimage){
        int deltaT, deltaX, deltaY, arrayX, arrayY, arrayT, maximum, maxX, maxY, maxT;
        int i, j ,k;
        
        maximum = maxX = maxY = maxT = 0;
        for(i = 0; i < A_X; i++) {
            for(j = 0; j < A_Y; j++) {
                for(k = 0; k < thres_t; k++) {
                    a[i][j][k] = 0;
                }
            }
        }
        
        for(i = 0; i < galleryimage.nrows; i++) {
            for(j = 0; j < probe.nrows; j++) {
                deltaT = (int)floor(probe.thetacol[j] - galleryimage.thetacol[i]);
                deltaX = (int)floor(probe.xcol[j] - (galleryimage.xcol[i] * cos(getRad(deltaT))) - (galleryimage.ycol[i] * sin(getRad(deltaT))));
                deltaY = (int)floor(probe.ycol[j] - (galleryimage.ycol[i] * cos(getRad(deltaT))) + (galleryimage.xcol[i] * sin(getRad(deltaT))));
                // a maximal 400 Eintraege fuer x. deltaX bei Auswertung [-457, 825]
                // -> 1282 Eintraege theoretisch notwendig -> teilen durch 4 (binning) 
                // Offset +150, um keine negativen Indizes zu erhalten
                // Analog für deltaY [-492, 988] und getRad(deltaT) [-7, 6]
                arrayX = (int)floor(deltaX/4) + 150;
                arrayY = (int)floor(deltaY/3) + 170;
                arrayT = (int)floor(getRad(deltaT)) + 9;
                a[arrayX][arrayY][arrayT]++;
                if(a[arrayX][arrayY][arrayT] > maximum) {
                    maximum = a[arrayX][arrayY][arrayT];
                    maxX = deltaX;
                    maxY = deltaY;
                    maxT = deltaT;
                }
            }
        }
        for(i = 0; i < galleryimage.nrows; i++) {
            galleryimage.xcol[i] += maxX;
            galleryimage.ycol[i] += maxY;
            galleryimage.thetacol[i] += maxT;
        }
        
	return galleryimage;
}

double getRad(int degree) {
    return degree*PI/180;
}

/**
 * TODO: Implement the simple Minutiae Pairing Algorithm
 * Compare the gallery image to the probe image and return
 * the comparison score as an integer. 
 */
int getScore(struct  xyt_struct probe, struct xyt_struct galleryimage){
	int score, usedProbe[probe.nrows], usedGalleryImage[galleryimage.nrows];
        double spatialDistanceX, spatialDistanceY, spatialDistance, directionDifference;
        int i, j;
        
        score = 0;
        for(i = 0; i < probe.nrows; i++) {
            usedProbe[i] = 0;
        }
        for(i = 0; i < probe.nrows; i++) {
            usedGalleryImage[i] = 0;
        }
        
        for(i = 0; i < probe.nrows; i++) {
            for(j = 0; j < galleryimage.nrows; j++) {
                spatialDistanceX = pow((galleryimage.xcol[j] - probe.xcol[i]), 2);
                spatialDistanceY = pow((galleryimage.ycol[j] - probe.ycol[i]), 2);
                spatialDistance = sqrt(spatialDistanceX + spatialDistanceY);
                
                directionDifference = MIN(fabs(galleryimage.thetacol[j] - probe.thetacol[i]), 
                        360 - fabs(galleryimage.thetacol[j] - probe.thetacol[i]));
                
                if(spatialDistance <= threshold_d && directionDifference <= threshold_r
                        && usedGalleryImage[j] == 0 && usedProbe[i] == 0  ) {
                    usedGalleryImage[j] = 1;
                    usedProbe[i] = 1;
                    score++;
                }
            }
        }
	return score;
}



/** 
 * TODO: Load minutiae from file (filepath is given as char *xyt_file) 
 * into the xyt_struct 'res' and return it as the result.
 *
 * Check for corrupted files, e.g. a line has less than 3, or more than 4
 * values, and fill the 'xcol', 'ycol' and 'thetacol' in the xyt_struct. 
 * At last set the 'numofrows' variable in the xyt_struct to the amount of 
 * loaded rows.
 */
struct xyt_struct loadMinutiae(const char *xyt_file){
	struct xyt_struct res;
        FILE *datei;
        char puffer[ZEILENLAENGE];
        char *xTemp, *yTemp, *angleTemp, *qualityTemp, *fifthTemp;
        res.nrows = 0;
        
        if( (datei=fopen(xyt_file, "r")) == NULL) {
            fprintf(stderr, "Kann %s nicht oeffnen\n", xyt_file);
            exit(EXIT_FAILURE);
        }
        while(fgets(puffer, ZEILENLAENGE, datei)) {
            xTemp = strtok(puffer, " ");
            yTemp = strtok(NULL, " ");
            angleTemp = strtok(NULL, " ");
            qualityTemp = strtok(NULL, " ");
            fifthTemp = strtok(NULL, " ");
            //Checke, ob Zeile heile:
            //Wenn es weniger als 3 Werte gibt, entspricht qualityTemp "\n". 
            //Wenn es mehr als 4 Werte gibt, ist fifthTemp != NULL
            if(strcmp(angleTemp, "\n") == 0 || fifthTemp != NULL) {
                printf("Spalte %d in Datei %s ist fehlerhaft! Programm wird beendet.\n", res.nrows, xyt_file);       
                exit(EXIT_FAILURE);
            } 
            // Char-Werte in int konvertieren und in die Struktur schreiben
            res.xcol[res.nrows] = atoi(xTemp);
            res.ycol[res.nrows] = atoi(yTemp);
            res.thetacol[res.nrows] = atoi(angleTemp);
            res.nrows++;
            if(res.nrows == MAX_MINUTIAE) {
                return res;
            }
        }
	return res;
}
