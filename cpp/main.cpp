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
static int calibration[8];
static int droneThreshold[8];
static int doubleBuffer[2][8];
static unsigned int iCurrent=2;
static unsigned int iMinus1=1;

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

void calibrate(void)
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

        calibration[i] = tmpVal;
        droneThreshold[i] = tmpVal/26;   // ~30 for calibration=800
    }

}

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
        doubleBuffer[iCurrent][i] = tmpVal;
    }


}

int minOfBoardSample(void)
{
    int tmp, i;
    int min=1024;
    for (i=0; i<8; i++){
        tmp = doubleBuffer[iCurrent][i];
        if (tmp<min){
            min = tmp;
        }
    }

    return min;
}

void followUpPotentialHits(void)
{
    int diff;

    if (potentialHitChanL > 0) {
        diff = doubleBuffer[iMinus1][potentialHitChanL] - doubleBuffer[iCurrent][potentialHitChanL];
        if (diff < hitThreshold2){
            cout << "hitL " << scale[potentialHitChanL] << " " << potentialHitVelL << ";" << endl;
            if (aveR < 20.){
                cout << "modeselektor bang;" << endl;
            }
        }
    }

    if (potentialHitChanR > 0) {
        diff = doubleBuffer[iMinus1][potentialHitChanR] - doubleBuffer[iCurrent][potentialHitChanR];
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
        diff = doubleBuffer[iMinus1][i] - doubleBuffer[iCurrent][i];
        if ( (diff > hitThreshold1) && (diff > currentMax) ) {
            potentialHitChanL = i;
            currentMax = diff;
        }
    }
    potentialHitVelL = currentMax;

    potentialHitChanR = -1;
    currentMax = 0;
    for (i=8; i<8; i++) {
        diff = doubleBuffer[iMinus1][i] - doubleBuffer[iCurrent][i];
        if ( (diff > hitThreshold1) && (diff > currentMax) ) {
            potentialHitChanR = i;
            currentMax = diff;
        }
    }
    potentialHitVelR = currentMax;
}

void sendBoardSample(void)
{
    int i;
    for (i=0; i<8; i++){
        cout << doubleBuffer[iCurrent][i] << " ";
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

void cycleIndices(void)
{
    // for 2nd order, could just do logical negation,
    //   but this is more general to higher-order buffering
    iCurrent = (iCurrent+1) % 2;
    iMinus1 = (iMinus1+1) % 2;
}

void readPcFile(void)
{
    FILE* pcFile;
    pcFile = fopen("../pca/centralSquare.pca", "r");

    float tmpFloat;
    int i,j;
    for (j=0; j<4; j++) {
        for (i=0; i<8; i++) {
            fscanf(pcFile, "%f", &tmpFloat);
            pcBasis[i][j] = tmpFloat;
        }
    }
}

void printPcBasis(void)
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
            tmpFloat += doubleBuffer[iCurrent][i] * pcBasis[i][j];
        }
        principalComponents[j] = tmpFloat;
    }

}


int main(void)
{

    readPcFile();

    calibrate();

    // warm up
    for (int i=0; i<4; i++){
        takeBoardSample();
        cycleIndices();
    }

    while(1){

        takeBoardSample();

        projectPCA();

        //sendBoardSample();
        sendPrincipalComponents();

        //followUpPotentialHits();
        //findPotentialHits();

        cycleIndices();
        usleep(30000); // 30 ms
    }

    return 0;

}
