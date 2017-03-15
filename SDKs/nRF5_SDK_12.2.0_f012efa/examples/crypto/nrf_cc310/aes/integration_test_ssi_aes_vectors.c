/*
 * Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 *
 */
#include "ssi_pal_types.h"
#include "integration_test_ssi_data.h"
#include "ssi_aes.h"

aesDataStuct aesVectors[] = {
	{
		/*symTest_name*/
		{"CRYS_AES_CBC_mode/NonIntegrated/Inplace/CRYS_AES_Encrypt/CRYS_AES_Key128BitSize/16bytes"},
		/*Key*/
		{ 0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c},
        16 ,
		/*IV*/
		{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f},
		/*Input data*/
		{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a},
		/*Output data*/
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		/*Expected output*/
		{0x76,0x49,0xab,0xac,0x81,0x19,0xb2,0x46,0xce,0xe9,0x8e,0x9b,0x12,0xe9,0x19,0x7d},
		/*Data size*/
		16,
		/*AES mode */
		SASI_AES_MODE_CBC,
		/*Operation mode */
		SASI_AES_ENCRYPT,
		/*Output data size*/
		16
	},
	{	/*symTest_name*/
		{"CRYS_AES_ECB_mode/NonIntegrated/Inplace/CRYS_AES_Decrypt/CRYS_AES_Key128BitSize/32bytes"},
		/*Key*/
		{0x43,0xb1,0xcd,0x7f,0x59,0x8e,0xce,0x23,0x88,0x1b,0x00,0xe3,0xed,0x03,0x06,0x88},
		16,
		/*IV*/
		{0},
		/*Input data*/
		{0xb9,0xaf,0xfe,0xe2,0x98,0xcd,0x0f,0x4a,0x67,0x08,0x44,0xa6,0xd6,0xce,0xef,0x87,
		 0xcf,0x41,0x2b,0x67,0xb3,0xad,0x95,0x18,0x94,0x4c,0x52,0x1e,0x91,0xb8,0x24,0xfc},
		/*Output data*/
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		/*Expected output*/
		{0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11,0xe5,0xfb,0xc1,0x19,0x1a,0x0a,0x52,0xef,
		 0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a},
		/*Data size*/
		32,
		/*AES mode */
		SASI_AES_MODE_ECB,
		/*Operation mode */
		SASI_AES_DECRYPT,
		/*Output data size*/
		32
	},
	{
		{"CRYS_AES_CTR_mode/Integrated/CRYS_AES_Decrypt/CRYS_AES_Key128BitSize/212bytes"},
		/*Key*/
		{0x08,0x82,0x94,0x8f,0xf5,0xf0,0xea,0xf1,0xe9,0x08,0x0a,0x98,0x1e,0x27,0xac,0xee},
		16,
		/*IvCounter*/
		{0x25,0xc3,0x38,0xde,0x08,0xa8,0x20,0x9e,0xa0,0x97,0x71,0x95,0x42,0x60,0x47,0xbc},
		/*input_data*/
		{0x31,0xb5,0x74,0x50,0x2e,0x66,0x56,0xf7,0xb0,0x23,0x52,0xc2,0x9d,0xa2,0x7d,0xca,
		 0xa6,0xcc,0xc8,0x7b,0xae,0xd1,0xa2,0x32,0x5e,0x92,0xa7,0xab,0xa4,0x11,0xce,0xbe,
		 0x38,0x42,0x4e,0x53,0x7f,0xe0,0xda,0x17,0xba,0x19,0x4f,0xf1,0xe8,0x81,0xd4,0x05,
		 0x1f,0x59,0xf1,0xe2,0x57,0x5d,0xf1,0x0f,0xab,0xb9,0xfd,0x7c,0x5c,0xff,0xf9,0xc7,
		 0xf4,0xa4,0xff,0x40,0x6e,0x98,0xa6,0x5f,0x55,0x57,0x1f,0x92,0x8e,0xa4,0xf7,0xdf,
		 0xf8,0x94,0xf2,0xc3,0x08,0xfb,0x7b,0xea,0x43,0x71,0x1e,0x64,0xd1,0xec,0xdc,0xa8,
		 0x85,0x7c,0xd3,0x6c,0x8b,0x84,0x8d,0xe4,0x38,0x1e,0x12,0x20,0x8a,0x38,0x1f,0x19,
		 0x41,0x14,0xfc,0x4d,0x24,0xe3,0x55,0x43,0xf5,0x15,0x35,0x54,0x9e,0xb4,0x69,0x72,
		 0x94,0x76,0x73,0x52,0x13,0xfd,0xb2,0x64,0x2b,0x1d,0xd0,0x30,0xde,0xfa,0xe2,0x18,
		 0xf8,0xfb,0x22,0xf9,0xe2,0xf9,0x4d,0x8b,0xd7,0x55,0xf4,0xd8,0xbd,0x4e,0x03,0xf3,
		 0x34,0xc0,0x62,0x8d,0xe0,0x49,0xc0,0x07,0x34,0x47,0x73,0x6b,0xdf,0x8f,0x27,0x0f,
		 0xa4,0xe2,0xa8,0xa3,0x90,0x2e,0xb4,0x20,0xc0,0x5f,0x47,0xd8,0x65,0x18,0x3c,0x6a,
		 0x71,0x72,0xd7,0x41,0x34,0x65,0x5d,0x02,0x27,0x6b,0x73,0x68,0xd8,0x9e,0xfa,0x9c,
		 0xe3,0xaf,0x97,0xc5},
		/*Output data*/
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		 0x00,0x00,0x00,0x00},
		 /*Expected output*/
		{0x5a,0xb6,0x19,0xc3,0xbb,0x9e,0x82,0x62,0x8d,0x56,0x0d,0xe8,0xc3,0x98,0xd8,0x0a,
		 0x11,0xd7,0xe1,0xe2,0xf4,0x2e,0x23,0x89,0xf2,0x96,0x02,0xe8,0x23,0x5b,0xe9,0xcb,
		 0xf0,0xd1,0x32,0x37,0x1b,0x43,0x46,0xfe,0x00,0x2c,0x05,0x90,0x1a,0xa9,0x68,0x14,
		 0xc8,0x2f,0x64,0xb8,0x51,0x79,0x12,0x45,0x26,0xc5,0x0e,0xf6,0x68,0x40,0x1d,0x8a,
		 0xaa,0xbf,0x12,0x98,0xf8,0xaf,0xf2,0x23,0x18,0x4f,0x56,0x6e,0x0f,0x1d,0x11,0x11,
		 0xe8,0x8d,0x13,0x4d,0xb0,0x01,0x8d,0x9d,0xc5,0xf6,0x56,0x8d,0x51,0x7b,0x8e,0xcf,
		 0x11,0xe8,0x81,0x8c,0x5b,0xcc,0xcd,0xf8,0x5f,0x27,0xc8,0x29,0xad,0xda,0x1c,0x29,
		 0xf7,0x5b,0xb4,0x4a,0x18,0xad,0xdc,0xb9,0x56,0x90,0xa3,0x56,0xe4,0xf5,0x84,0xc3,
		 0xab,0xb4,0x46,0xbb,0x4b,0x81,0x21,0xa6,0x5c,0x1d,0x22,0x6a,0xf9,0xc9,0xd1,0x83,
		 0x7f,0x00,0x10,0x56,0x92,0x65,0x46,0xc2,0x61,0xfb,0xbd,0xf9,0x2b,0x94,0x49,0x8e,
		 0x03,0x8c,0x2b,0xcf,0xd0,0xb6,0x34,0x54,0x97,0x97,0x2e,0xd8,0xfd,0xd2,0x77,0x48,
		 0x07,0xe4,0xef,0x1a,0x25,0x11,0x14,0xe0,0x6f,0x9e,0x6c,0x1d,0x2e,0x41,0x24,0x57,
		 0x9e,0xd6,0xf6,0x6d,0xf2,0x53,0x50,0x2b,0x99,0xfe,0xb2,0x1c,0xc0,0xde,0x57,0x9f,
		 0x18,0x6f,0x19,0x3d},
		/*Data size*/
		212,
		/*AES mode*/
		SASI_AES_MODE_CTR,
		/*Operation mode */
		SASI_AES_DECRYPT,
		/*Output data size*/
		212
	},
	{
		{"CRYS_AES_CMAC_mode/Integrated/CRYS_AES_Encrypt/CRYS_AES_Key128BitSize/40bytes"},
		/*Key*/
		{0x2b,0x7e,0x15,0x16,0x28,0xae,0xd2,0xa6,0xab,0xf7,0x15,0x88,0x09,0xcf,0x4f,0x3c},
		16,
		/*IvCounter*/
		{0x00},
		/*input_data*/
		{0x6b,0xc1,0xbe,0xe2,0x2e,0x40,0x9f,0x96,0xe9,0x3d,0x7e,0x11,0x73,0x93,0x17,0x2a,
		 0xae,0x2d,0x8a,0x57,0x1e,0x03,0xac,0x9c,0x9e,0xb7,0x6f,0xac,0x45,0xaf,0x8e,0x51,
		 0x30,0xc8,0x1c,0x46,0xa3,0x5c,0xe4,0x11 },
		/*output_data*/
		{0},
		/*Expected output*/
		{ 0xdf,0xa6,0x67,0x47,0xde,0x9a,0xe6,0x30,0x30,0xca,0x32,0x61,0x14,0x97,0xc8,0x27},
		/*Data size*/
		40,
		/*AES mode*/
		SASI_AES_MODE_CMAC,
		/*Operation mode */
		SASI_AES_ENCRYPT,
		/*Output data size*/
		16
	}
};


aesCCMDataStuct aesCCMVectors[] = {
	{
		{"CCM/Integrated/4ByteMac/7ByteNonce/16ByteAdata/16ByteTextData/128Key"},
		/*Key*/
		{0xC6,0xBB,0x2D,0x94,0x90,0xB9,0x65,0x23,0x98,0xED,0x83,0x3F,0x9B,0x9D,0x02,0xCC},
		/*Key size*/
		CRYS_AES_Key128BitSize,
		/*Nonce data*/
		{0x14,0xD7,0xF6,0xC9,0x3D,0x1F,0x53},
		/*size of Nonce*/
		7,
		/*additional data*/
		{0x06,0x55,0xAD,0xA9,0x1B,0xFC,0x6D,0xD2,0x88,0x27,0xB7,0x4A,0x76,0x9B,0xDB,0xD6},
		/*size of additional data*/
		16,
		/*text data*/
		{0xF7,0x44,0x6E,0xB2,0x73,0xED,0x03,0xE8,0x1D,0x24,0x14,0xFA,0xB8,0x7D,0xC6,0xCB},
		/*data input size*/
		16,
		/*expected data */
		{0x29,0xFC,0x76,0xC8,0x92,0x89,0x65,0x58,0xE5,0x99,0x8B,0xBA,0x89,0x02,0xC3,0xDF},
		/*Output*/
		{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
		 16,
		/* AESCCM_Mac_ExpectedRes */
		{0xFD,0xAF,0x7E,0xF2},
		/*Output Mac*/
		{0x00,0x00,0x00,0x00},
		/*size of CCM-MAC (T) */
		4
	}

};






