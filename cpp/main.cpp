/*
Sensor-polling script for Strange Attractor
Chris Chronopoulos, 20150619
*/

#include "mcp3008Spi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


using namespace std;

// ADC objects
static mcp3008Spi adc0("/dev/spidev0.0", SPI_MODE_0, 1000000, 8);

// board sample structures
static unsigned int iCurrent=1;
static unsigned int iMinus1=0;
static int positionData[2][8];
static int velocityData[8];

//PCA
static float pcBasis[8][4];
static float principalComponents[4];

// hits
static int hitThreshold1 = 10;
static int hitThreshold2 = -3;
static int potentialHitChanL = -1;
static int potentialHitVelL = 0;
static int potentialHitChanR = -1;
static int potentialHitVelR = 0;

// scale
static int scale[] =   {0, 2, 4, 7, 9,
                        12, 14, 16, 19, 21,
                        24, 26, 28, 31, 33, 36};

static float aveL, aveR;

void takeBoardSample(void)
{

    unsigned char data[3];
    int tmpVal = 0;
    unsigned int i;

    for (i=0; i<8; i++) {
        data[0] = 1;            // start bit
        data[1] = 0b10000000 | (i << 4);   // single ended, channel i
        data[2] = 0;            // don't care
        adc0.spiWriteRead(data, sizeof(data) );
        tmpVal = 0;
        tmpVal = data[1];
        tmpVal &= 0b00000011;
        tmpVal <<= 8;
        tmpVal |= data[2];
        positionData[iCurrent][i] = tmpVal;
        velocityData[i] = tmpVal - positionData[iMinus1][i];
    }


}

void cycleIndices(void)
{
    // for 2nd order, could just do logical negation,
    //   but this is more general to higher-order buffering
    iCurrent = (iCurrent+1) % 2;
    iMinus1 = (iMinus1+1) % 2;
}


void followUpPotentialHits(void)
{
    int diff;

    if (potentialHitChanL > 0) {
        diff = positionData[iMinus1][potentialHitChanL] - positionData[iCurrent][potentialHitChanL];
        if (diff < hitThreshold2){
            cout << "hitL " << scale[potentialHitChanL] << " " << potentialHitVelL << ";" << endl;
            if (aveR < 20.){
                cout << "modeselektor bang;" << endl;
            }
        }
    }

    if (potentialHitChanR > 0) {
        diff = positionData[iMinus1][potentialHitChanR] - positionData[iCurrent][potentialHitChanR];
        if (diff < hitThreshold2){
            cout << "hitR " << scale[potentialHitChanR] << " " << potentialHitVelR << ";" << endl;
            if (aveL < 20.){
                cout << "modeselektor bang;" << endl;
            }
        }
    }

}

void findPotentialHits(void)
{
    int i, diff, currentMax;

    potentialHitChanL = -1;
    currentMax = 0;
    for (i=0; i<8; i++) {
        diff = positionData[iMinus1][i] - positionData[iCurrent][i];
        if ( (diff > hitThreshold1) && (diff > currentMax) ) {
            potentialHitChanL = i;
            currentMax = diff;
        }
    }
    potentialHitVelL = currentMax;

    potentialHitChanR = -1;
    currentMax = 0;
    for (i=8; i<8; i++) {
        diff = positionData[iMinus1][i] - positionData[iCurrent][i];
        if ( (diff > hitThreshold1) && (diff > currentMax) ) {
            potentialHitChanR = i;
            currentMax = diff;
        }
    }
    potentialHitVelR = currentMax;
}

void printRawData(void)
// use this to save raw data files for PCA calculation
{
    int i;
    for (i=0; i<8; i++){
        cout << positionData[iCurrent][i] << " ";
    }
    for (i=0; i<8; i++){
        cout << velocityData[i] << " ";
    }
    cout << ";" << endl;
}

void sendPrincipalComponents(void)
{
    int i;
    cout << "pc ";
    for (i=0; i<4; i++){
        cout << principalComponents[i] << " ";
    }
    cout << ";" << endl;
}

void readPcFile(char* filename)
{
    FILE* pcFile;
    pcFile = fopen(filename, "r");

    float tmpFloat;
    int i,j;
    for (j=0; j<4; j++) {
        for (i=0; i<8; i++) {
            fscanf(pcFile, "%f", &tmpFloat);
            pcBasis[i][j] = tmpFloat;
        }
    }

    fclose(pcFile);
}

void printPcBasis(void)
// for debugging
{
    int i,j;
    for (j=0; j<4; j++) {
        for (i=0; i<8; i++) {
            printf("%.5f ", pcBasis[i][j]);
        }
        printf("\n");
    }
}

void projectPCA(void)
{
    int i,j;
    float tmpFloat;
    for (j=0; j<4; j++) {
        tmpFloat = 0.;
        for (i=0; i<8; i++) {
            tmpFloat += positionData[iCurrent][i] * pcBasis[i][j];
        }
        principalComponents[j] = tmpFloat;
    }

}


int main(int argc, char *argv[])
{

    int calibration;
    if (argc > 1) {
        readPcFile(argv[1]);
        //printPcBasis();
        calibration = 0;
    } else {
        calibration = 1;
    }


    // warm up
    for (int i=0; i<4; i++){
        takeBoardSample();
        cycleIndices();
    }

    while(1){

        takeBoardSample(); // update positionData and velocityData

        if (calibration) {
            printRawData();
        } else {
            projectPCA();
            sendPrincipalComponents();
        }

        cycleIndices();
        usleep(30000); // 30 ms
    }

    return 0;

}
