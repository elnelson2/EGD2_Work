#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <cmath>
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
void printBlock(int blocksize, unsigned int *blockData)
{
    for (int i = 0; i < blocksize; i++)
    {
        for (int j = 0; j < blocksize; j++)
        {
            cout << blockData[i * blocksize + j] << " ";
            // printf("%d ", blockData[i*blocksize+j]);
        }
        cout << endl;
    }
}
double avgBlock(int blocksize, unsigned int *blockData)
{
    double sum = 0;
    for (int i = 0; i < blocksize; i++)
    {
        for (int j = 0; j < blocksize; j++)
        {
            sum += blockData[i * blocksize + j];
        }
        // cout << sum << endl;
    }
    return sum = sum / (blocksize * blocksize);
    // cout << "Average for block: " << sum << endl << endl;
}
tuple<double, double> avgQuadrantBlock(int blocksize, unsigned int *blockData, int q_size, int x, int y)
{
    double sum = 0;
    int xlimit = x + q_size;
    int ylimit = y + q_size;
    for (int i = x; i < xlimit; i++)
    {
        for (int j = y; j < ylimit; j++)
        {
            // cout << blockData[i*blocksize+j] << " ";
            sum += blockData[i * blocksize + j];
        }
    }
    //return sum / (q_size * q_size);
    return make_tuple(sum, sum / (q_size* q_size));
}
void avgofCurrentFrame(int blocksize, unsigned int *blockData, unsigned char *frameBuffer, int width, int height)
{
    cout << width / blocksize << " " << height / blocksize << endl;
    double avgArray[width / blocksize][height / blocksize];
    int i_counter = 0;
    int j_counter = 0;
    for (int j = 0; j < height; j += blocksize)
    {
        // cout << "Printing from: (0," << j << ")" << endl;
        for (int i = 0; i < width; i += blocksize)
        {
            getblock(blocksize, blockData, frameBuffer, width, i, j);
            avgArray[i_counter][j_counter] = avgBlock(blocksize, blockData);
            cout << "(" << i_counter << "," << j_counter << "):" << avgArray[i_counter][j_counter] << " ";
            j_counter++;
        }
        j_counter = 0;
        i_counter++;
        cout << endl;
    }
    cout << endl
         << endl;
}
void basicCharOutput(int blocksize, unsigned int *blockData, unsigned char *frameBuffer, int width, int height)
{
    if (blocksize < 4 || ((log(blocksize) / log(2)) != int(log(blocksize) / log(2))))
    {
        cout << "Error, given block size is not compatible with function" << endl;
        return;
    }
    else
    {
        float q1Avg, q2Avg, q3Avg, q4Avg, qcenterAvg = 0;
        float q1sum, q2sum, q3sum, q4sum, qcentersum = 0;
        //int q_size = log(blocksize) / log(2);
        int q_size = 6;
        // cout << q_size << endl;
        tie(q1sum, q1Avg) = avgQuadrantBlock(blocksize, blockData, q_size, 0, 0);
        tie(q2sum, q2Avg) = avgQuadrantBlock(blocksize, blockData, q_size, 0, blocksize - q_size);
        tie(q3sum, q3Avg) = avgQuadrantBlock(blocksize, blockData, q_size, blocksize - q_size, 0);
        tie(q4sum, q4Avg) = avgQuadrantBlock(blocksize, blockData, q_size, blocksize - q_size, blocksize - q_size);
        tie(qcentersum, qcenterAvg) = avgQuadrantBlock(blocksize, blockData, 3, blocksize / 2 - 1, blocksize / 2 - 1);
        cout << q1Avg << " " << q2Avg << " " << q3Avg << " " << q4Avg << " " << qcenterAvg << endl;
        cout << q1sum << " " << q2sum << " " << q3sum << " " << q4sum << " " << qcentersum << endl;

        float q12sum_difference, q13sum_difference, q14sum_difference, q1center_AvgDifference = 0;
        q12sum_difference = abs(q1sum - q2sum);
        q13sum_difference = abs(q1sum - q3sum);
        q14sum_difference = abs(q1sum - q4sum);
        cout << q12sum_difference << " " << q13sum_difference << " " << q14sum_difference << " " << q1center_AvgDifference << endl;

        float q23sum_difference, q24sum_difference, q2center_AvgDifference = 0;
        q23sum_difference = abs(q2sum - q3sum);
        q24sum_difference = abs(q2sum - q4sum);
        cout << q23sum_difference << " " << q24sum_difference << endl;

        float q34sum_difference, q3center_AvgDifference = 0;
        q34sum_difference = abs(q3sum - q4sum);
        cout << q34sum_difference << endl;

        float q4center_AvgDifference = 0;
        q1center_AvgDifference = abs(q1Avg - qcenterAvg);
        q2center_AvgDifference = abs(q2Avg - qcenterAvg);
        q3center_AvgDifference = abs(q3Avg - qcenterAvg);
        q4center_AvgDifference = abs(q4Avg - qcenterAvg);

        cout << q1center_AvgDifference << " " << q2center_AvgDifference << " " << q3center_AvgDifference << " " << q4center_AvgDifference << endl;

        // Return first the absolute difference of sums between the 4 corner quadrants
        // Then return the difference between average of each corner with center quadrant of 3x3

        // Since in this implementation we have a void the return is commented 

        // Can return the sum quantities of each quadrant
        // return make_tuple(q1Avg, q2Avg, q3Avg, q4Avg, qcenterAvg) 
        // or sum which I think is ideal but for this only the corners because the center quadrant is much smaller so disproportional 
        // return make_tuple(q1Avg, q2Avg, q3Avg, q4Avg)
        // or we can do both as to prevent from having to go back and simply in creating the models we can opt either concept 

        // Now return the absolute difference of sums between quadrant first and then between the center
        // return make_tuple(q12sum_difference, q13sum_difference, q14sum_difference, q23sum_difference, q24sum_difference, q34sum_difference, q1center_AvgDifference, q2center_AvgDifference, q3center_AvgDifference, q4center_AvgDifference)

        // possible best solution is 30 difference in luma
        /*if (abs(q1Avg - q2Avg) < 30 && abs(q1Avg - q3Avg) < 30 && abs(q1Avg - 4) && abs(q2Avg - q3Avg) < 30 && abs(q2Avg - q4Avg) < 30 && abs(q3Avg - q4Avg) < 30)
        {
            double qAvg = (q1Avg + q2Avg + q3Avg + q4Avg) / 4;
            cout << qAvg << endl;
            if (abs(qAvg - qcenterAvg) > 8)
            {
                cout << "There is text here!" << endl;
            }
            else
            {
                cout << "The whole block is of similar luma value" << endl;
            }
        }
        else
        {
            cout << "Not all quadrant are similar in luma value" << endl;
        }
        */
    }
}
void calc_setFrame(FILE *fp, uint32_t frame_size, int frame_num, uint32_t Y_val_size, unsigned char *frameBuffer, int width, int height)
{
    if (frame_num >= 300)
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
        int x = 0, y = 0;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        printBlock(blocksize, blockData);
        basicCharOutput(blocksize, blockData, frameBuffer, width, height);
        // perfect example of a text
        x = 247, y = 27;
 //       getblock(blocksize, blockData, frameBuffer, width, x, y);
   //     printBlock(blocksize, blockData);
     //   basicCharOutput(blocksize, blockData, frameBuffer, width, height);
        // Another good example of text
        x = 247, y = 21;
    //    getblock(blocksize, blockData, frameBuffer, width, x, y);
      //  printBlock(blocksize, blockData);
        //basicCharOutput(blocksize, blockData, frameBuffer, width, height);
        // example where not all quadrants are similar in luma
    //    x = 284, y = 340;
  //      getblock(blocksize, blockData, frameBuffer, width, x, y);
        //printBlock(blocksize, blockData);
       // basicCharOutput(blocksize, blockData, frameBuffer, width, height);

        /*avgofCurrentFrame(blocksize, blockData, frameBuffer, width, height);
        //unsigned int* blockData;
        //blockData = new unsigned int[blocksize*blocksize];

        int x = 688, y = 568;
        cout  << "Printing 8x8 block with (x,y) = (0,0): " << endl;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        //printBlock(blocksize, blockData);
        cout << avgBlock(blocksize, blockData) << endl;

        x = 696, y = 568;
        cout  << "Printing 8x8 block with (x,y) = (8,0): " << endl;
        getblock(blocksize, blockData, frameBuffer, width, x, y);
        printBlock(blocksize, blockData);
        cout << avgBlock(blocksize, blockData) << endl;*/
    }
}

int main()
{
    // auto* fp;
    FILE *fp;
    //fp = fopen("c:\\Users\\nmendez2019\\Documents\\YUV_Manipulation\\CSGO.yuv", "rb");
    fp = fopen("c:\\Users\\nmendez2019\\Documents\\YUV_Manipulation\\CREW_704x576_30_orig_01.yuv", "rb");
    int width = 704;
    int height = 576;
    int frames = 300;
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

    calc_setFrame(fp, frame_size, frame_num, Y_val_size, frameBuffer, width, height);
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
}