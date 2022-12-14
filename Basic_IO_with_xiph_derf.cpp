#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <fstream>
#include <tuple>

using namespace std;

void getblock(int blocksize, unsigned int *blockData, unsigned char *frameBuffer, int width, int x, int y)
{
    int nOffset = y * width + x;
    for (int j = 0; j < blocksize; j++)
    {
        for (int i = 0; i < blocksize; i++)
        {
            blockData[i * blocksize + j] = (frameBuffer + nOffset)[i * width + j];
        }
    }
}
void printBlock(int blocksize, unsigned int *blockData, ofstream& myfile)
{
    for (int i = 0; i < blocksize; i++)
    {
        for (int j = 0; j < blocksize; j++)
        {
            //myfile << blockData[i * blocksize + j] << ",";
            cout << blockData[i * blocksize + j] << " ";
            // printf("%d ", blockData[i*blocksize+j]);
        }
        cout << "\n";
    }
}
tuple <int, int, double> avgBlock(int blocksize, unsigned int *blockData)
{
    double sum = 0;
    auto min = blockData[0];
    auto max = blockData[0];

    //cout << "\\" << blockData[0] << " " << blockData[(blocksize-1)*blocksize + (blocksize-1)] << "\\ ";
    for (int i = 0; i < blocksize; i++)
    {
        for (int j = 0; j < blocksize; j++)
        {
            if (blockData[i* blocksize + j] < min){
                min = blockData[i*blocksize + j];
            }
            if (blockData[i* blocksize + j] > max){
                max = blockData[i* blocksize +j];
            }
            sum += blockData[i * blocksize + j];
        }
        // cout << sum << endl;
    }
    return make_tuple(min, max, (sum = sum / (blocksize * blocksize)));
    // cout << "Average for block: " << sum << endl << endl;
}
//blocksize, array with macroblock, size of corner quadrant, x,y position of macroblock
double avgQuadrantBlock(int blocksize, unsigned int *blockData, int q_size, int x, int y)
{
    double sum = 0;
    int xlimit = x + q_size;    // limiters of the loops so that we only get a quadrant of size q_size x q_size
    int ylimit = y + q_size;
    for (int i = x; i < xlimit; i++)
    {
        for (int j = y; j < ylimit; j++)
        {
            //cout << blockData[i*blocksize+j] << " ";
            sum += blockData[i * blocksize + j];    // add each luma value to sum
        }
    }
    return sum / (q_size * q_size);
}
// block size, array of macroblock, frame buffer, width, height of frame
bool basicCharOutput(int blocksize, unsigned int *blockData, int width, int height)
{
    
    if (blocksize < 4 || ((log(blocksize) / log(2)) != int(log(blocksize) / log(2))))  // make sure number of blocks is power of 2
    {
        cout << "Error, given block size is not compatible with function" << endl;
        exit(1);
    }
    else
    {
        // Variable that will hold averages
        float q1Avg, q2Avg, q3Avg, q4Avg, qcenter = 0;
        // Makes sure that the size of the quadrant is duable
        //int q_size = log(blocksize) / log(2);
        int q_size = 6;
        // cout << q_size << endl;
        q1Avg = avgQuadrantBlock(blocksize, blockData, q_size, 0, 0);   // quadrant 1 is left top corner so start at 0,0 of macroblock
        q2Avg = avgQuadrantBlock(blocksize, blockData, q_size, 0, blocksize - q_size);  // quadrant 2 is right top corner so if a 8x8 macroblock then from (5-7, 0-2)
        q3Avg = avgQuadrantBlock(blocksize, blockData, q_size, blocksize - q_size, 0);  // quadrant 3 is left bottom corner so if a 8x8 macroblock then from (0-2, 5-7)
        q4Avg = avgQuadrantBlock(blocksize, blockData, q_size, blocksize - q_size, blocksize - q_size); // quadrant 4 is the right bottom corner so if a 8x8 macroblock then from (5-7, 5-7)
        qcenter = avgQuadrantBlock(blocksize, blockData, 2, blocksize / 2 - 1, blocksize / 2 - 1); // This retrieves the average of the center in a 2x2 quadrant 
        //cout << q1Avg << " " << q2Avg << " " << q3Avg << " " << q4Avg << " " << qcenter << endl;
        // possible best solution is 30 difference in luma
        if (abs(q1Avg - q2Avg) < 30 && abs(q1Avg - q3Avg) < 30 && abs(q1Avg - 4) && abs(q2Avg - q3Avg) < 30 && abs(q2Avg - q4Avg) < 30 && abs(q3Avg - q4Avg) < 30)
        {
            double qAvg = (q1Avg + q2Avg + q3Avg + q4Avg) / 4;  // average of all quadrants except center
            //cout << qAvg << endl;
            if (abs(qAvg - qcenter) > 8)    // compare to center quadrant 
            {
                //cout << "There is text here!" << endl; // return 1 
                return true;
            }
            else
            {
                //cout << "The whole block is of similar luma value" << endl; // return 0
                return false;
            }
        }
        else
        {
            //cout << "Not all quadrant are similar in luma value" << endl;   // return 0
            return false;
        }
    }
}
void avgofCurrentFrame(int blocksize, unsigned int *blockData, unsigned char *frameBuffer, int width, int height, ofstream&  myfile)
{
    cout << width / blocksize << " " << height / blocksize << endl;
    /*double avgArray[width / blocksize][height / blocksize];
    int i_counter = 0;
    int j_counter = 0;
    */
    for (int j = 0; j < (height - blocksize); j += blocksize)
    {
        // cout << "Printing from: (0," << j << ")" << endl;
        for (int i = 0; i < width; i += blocksize)
        {
            //cout << "(" << i << "," << j << ") ";
            myfile << i << "," << j << ",";

            getblock(blocksize, blockData, frameBuffer, width, i, j);

            bool AVGQIndicator = basicCharOutput(blocksize, blockData, width, height);

            int min, max;
            double average;
            tie(min, max, average) = avgBlock(blocksize, blockData);
            int range = max - min;

            myfile << AVGQIndicator << "," << average << "," << range << "," << max << "," << min << "\n";

            //cout << "(" << i_counter << "," << j_counter << "):" << avgArray[i_counter][j_counter] << " ";
           // j_counter++;
        }
        //cout << endl;
        //j_counter = 0;
        //i_counter++;
        //cout << endl;
    }
    //cout << endl << endl;
         
}
void calc_setFrame(FILE *fp, uint32_t frame_size, int frame_num, uint32_t Y_val_size, unsigned char *frameBuffer, int width, int height, ofstream& myfile)
{
    if(myfile.is_open()){
        cout << "CSV File Opened" << endl;
    }else{
        cout << "Failed to open" << endl;
        return;
    }
    
    if (frame_num >= 3602)
    {
        cout << "Frame number doesn't exist" << endl;
        return;
    }
    else
    {
        fseek(fp, frame_num * frame_size, SEEK_SET);
        int check_size = fread(frameBuffer, 1, Y_val_size, fp);
        if (check_size != Y_val_size)
        {
            cout << "Error reading frame" << endl;
            fclose(fp);
            exit(0);
        }
        else
        {
            cout << "Successful buffer fill" << endl;
        }

        int blocksize = 16;
        // int nOffset = y* width + x;

        unsigned int *blockData;
        blockData = new unsigned int[blocksize * blocksize];
        // example where the whole block is similar in luma

        avgofCurrentFrame(blocksize, blockData, frameBuffer, width, height, myfile);


/*
        int x = 1022, y = 326;
        myfile << x << "," << y << ",";
        getblock(blocksize, blockData, frameBuffer, width, x, y);

        printBlock(blocksize, blockData, myfile);
        //myfile << leftmostval << "," << rightmostval << ",";
        bool AvgQuadrantResult = basicCharOutput(blocksize, blockData, frameBuffer, width, height);
        myfile << AvgQuadrantResult << ",";

        int min, max, range;
        double average;
        tie(min, max, average) = avgBlock(blocksize, blockData);
        range = max - min;

        myfile << average << "," << range << "," << max << "," << min << "\n";
        */
        /*
        // perfect example of a text
        x = 247, y = 27;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        printBlock(blocksize, blockData);
        basicCharOutput(blocksize, blockData, frameBuffer, width, height);
        // Another good example of text
        x = 247, y = 21;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        printBlock(blocksize, blockData);
        basicCharOutput(blocksize, blockData, frameBuffer, width, height);
        // example where not all quadrants are similar in luma
        x = 284, y = 340;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        printBlock(blocksize, blockData);
        basicCharOutput(blocksize, blockData, frameBuffer, width, height);*/

        //unsigned int* blockData;
        //blockData = new unsigned int[blocksize*blocksize];
        /*

        int x = 688, y = 568;
        cout  << "Printing 8x8 block with (x,y) = (0,0): " << endl;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        //printBlock(blocksize, blockData);
        cout << avgBlock(blocksize, blockData) << endl;

        x = 696, y = 568;
        cout  << "Printing 8x8 block with (x,y) = (8,0): " << endl;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        printBlock(blocksize, blockData);
        cout << avgBlock(blocksize, blockData) << endl;
        */
    }
}

int main()
{
    std::ofstream myfile;
    myfile.open("c:\\Users\\nmendez2019\\Documents\\YUV_Manipulation\\Test.csv");
    myfile << "CSGO, Dataset, Frame 0\n";
    myfile << "X, Y, AVGQuadrant, AVG, Range, Max, Min\n";

    FILE *fp;
    fp = fopen("c:\\Users\\nmendez2019\\Documents\\YUV_Manipulation\\CSGO.yuv", "rb");

    int width = 1920;
    int height = 1080;
    int frames = 3602;
    if (fp == NULL)
    {
        cout << "Error, Returned NULL fp.\n";
        return 2;
    }
    else
    {
        cout << "File was opened correctly\n";
    }
    fseek(fp, 0, SEEK_END);
    uint32_t size = ftell(fp);
    uint32_t calculatedSize = (width * height * 1.5 * frames);
    uint32_t frame_size = width * height * 1.5;
    uint32_t Y_val_size = width * height;
    unsigned char *frameBuffer;
    frameBuffer = new unsigned char[Y_val_size];
    int frame_num = 0; // start in first frame

    calc_setFrame(fp, frame_size, frame_num, Y_val_size, frameBuffer, width, height, myfile);
    
    // calc_setFrame(fp, frame_size, frame_num, Y_val_size, frameBuffer, width, height);

    /* if (size != calculatedSize)
     {
         fprintf(stderr, "Wrong size of yuv image : %d bytes, expected %d bytes\n", size, calculatedSize);
         fclose(fp);
         return 2;
     }
     else
     {
         fclose(fp);
         cout << "File read correctly\n";
     }
     */
    myfile.close();
    return 0;
}