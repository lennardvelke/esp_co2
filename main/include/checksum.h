#pragma once


/** 
* This Algorithim is using Fletcher’s Checksum Algorithim
* 
*
**/
uint16_t calculate_checksum(uint8_t *data, uint16_t data_length){
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;

    for(int i = 0; i < data_length; i++){
        sum1 = (sum1 + data[i]) % 255;
        sum2 = (sum2 + sum1) % 255;
    }
    return (sum1 << 8) + sum2;
}