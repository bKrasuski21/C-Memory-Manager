#include <iostream>
#include <string>
#include <cmath>
#include <array>
#include <sstream>
#include <fstream>
#include <vector>
#include <functional>
#include "allocator.h"
#pragma once
int bestFit(int a, void* ptr) {
    if(ptr == nullptr){
        //cout << " o in bestfit" << endl;
        return 0;
    }
    uint16_t * temp = static_cast<uint16_t *>(ptr);
    //cout << temp[0] << " num of holes " << endl;
    int num = (temp[0] * 2) + 1;
    int min = 100000000;
    int holeNum = -1;
    //cout << "get list: " << temp[0] << " ";
    for(int i =1; i < num; i+=2){
        //cout << temp[i] << " " << temp[i+1] << " ";
        uint16_t holeSize = temp[i+1];
        if(holeSize >= a){
            int sizeDif = holeSize - a;
            if(sizeDif <= min){
                min = sizeDif;
                holeNum = temp[i];
            }
        }
    }

    return holeNum;
}
int worstFit(int a, void* ptr) {
    if(ptr == nullptr){return 0;}
    uint16_t * temp = static_cast<uint16_t *>(ptr);
    //cout << temp[0] << " num of holes " << endl;
    int num = (temp[0] * 2) + 1;
    int max = 0;
    int holeNum = -1;
    //cout << "get list: " << temp[0] << " ";
    for(int i =1; i < num; i+=2){
        // cout << temp[i] << " " << temp[i+1] << " ";
        uint16_t holeSize = temp[i+1];
        if(holeSize >= a){
            int sizeDif = holeSize - a;
            if(sizeDif >= max){
                max = sizeDif;
                holeNum = temp[i];
            }
        }
    }

    return holeNum;
}
