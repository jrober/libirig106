
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "i106_decode_pcmf1.h"
#include "unity.h"
#include "unity_fixture.h"


TEST_GROUP_RUNNER(test_decode_pcm){
    RUN_TEST_CASE(test_decode_pcm, TestDecode_FirstPCM);
    RUN_TEST_CASE(test_decode_pcm, TestDecode_NextPCM);
    RUN_TEST_CASE(test_decode_pcm, TestDecodeMinorFrame_PCMF1);
    RUN_TEST_CASE(test_decode_pcm, TestSet_Attributes_PCMF1);
    RUN_TEST_CASE(test_decode_pcm, TestCreateOutputBuffers_PCMF1);
    RUN_TEST_CASE(test_decode_pcm, TestFreeOutputBuffers_PCMF1);
}


TEST_GROUP(test_decode_pcm);
TEST_SETUP(test_decode_pcm){}
TEST_TEAR_DOWN(test_decode_pcm){}


TEST(test_decode_pcm, TestDecode_FirstPCM){
    I106C10Header header;
    char * buffer = malloc(32);
    PCMF1_Message msg;
    PCMF1_Attributes attributes;
    PCMF1_CSDW csdw;

    csdw.Throughput = 1;
    memcpy(buffer, &csdw, sizeof(PCMF1_CSDW));

    attributes.BitPosition = 0;
    attributes.BitsInMinorFrame = 16;
    attributes.CommonWordLength = 2;
    attributes.DataWordBitCount = 1;
    attributes.MinorFrameWordCount = 1;
    attributes.SaveData = 0;
    attributes.WordsInMinorFrame = 3;

    CreateOutputBuffers_PCMF1(&attributes);

    header.DataLength = 32;
    msg.SubPacketBits = 3;
    msg.Attributes = &attributes;

    TEST_ASSERT_EQUAL(I106_NO_MORE_DATA, I106_Decode_FirstPCMF1(&header,
                buffer, &msg));

    free(buffer);
    FreeOutputBuffers_PCMF1(&attributes);
}


TEST(test_decode_pcm, TestDecode_NextPCM){
    PCMF1_Message msg;
    PCMF1_CSDW csdw;
    I106C10Header header;

    msg.Header = &header;
    msg.SubPacketLength = 12;
    msg.CSDW = &csdw;
    msg.BytesRead = 0;
    msg.Length = 32;

    TEST_ASSERT_EQUAL(I106_OK, I106_Decode_NextPCMF1(&msg));
}


TEST(test_decode_pcm, TestDecodeMinorFrame_PCMF1){
    PCMF1_Message msg;
    PCMF1_Attributes attributes;

    msg.Attributes = &attributes;
    msg.Data = malloc(16);

    attributes.BitPosition = 0;
    attributes.TestWord = 0x0000;
    attributes.BitsLoaded = 0;
    attributes.MinorFrameBitCount = 0;
    attributes.BitPosition = 0;
    msg.SubPacketBits = 3;

    TEST_ASSERT_EQUAL(I106_NO_MORE_DATA, DecodeMinorFrame_PCMF1(&msg));

    free(msg.Data);
}


TEST(test_decode_pcm, TestSet_Attributes_PCMF1){
    R_DataSource r_datasource;
    PCMF1_Attributes attributes;
    P_Record p_record;

    TEST_ASSERT_EQUAL(I106_INVALID_PARAMETER, Set_Attributes_PCMF1(&r_datasource, NULL));

    r_datasource.P_Record = NULL;
    TEST_ASSERT_EQUAL(I106_INVALID_PARAMETER, Set_Attributes_PCMF1(&r_datasource, NULL));

    r_datasource.P_Record = &p_record;
    p_record.BitsInMinorFrame = NULL;
    p_record.BitsPerSecond = NULL;
    p_record.CommonWordLength = "0";
    p_record.InSyncCritical = NULL;
    p_record.MinorFrameSyncPattern = "1010";
    p_record.MinorFrameSyncPatternLength = "4"; 
    p_record.MinorFrameSyncPatternLength = NULL;
    p_record.MinorFrameSyncType = NULL;
    p_record.NumberMinorFrames = NULL;
    p_record.ParityTransferOrder = NULL;
    p_record.ParityType = NULL;
    p_record.WordTransferOrder = NULL;
    p_record.WordsInMinorFrame = NULL;
    TEST_ASSERT_EQUAL(I106_OK, Set_Attributes_PCMF1(&r_datasource, &attributes));
}


TEST(test_decode_pcm, TestCreateOutputBuffers_PCMF1){
    PCMF1_Attributes attributes;
    attributes.WordsInMinorFrame = 10;

    TEST_ASSERT_EQUAL(I106_OK, CreateOutputBuffers_PCMF1(&attributes));
}


// TODO: find why malloc/free tests can't free correctly
TEST(test_decode_pcm, TestFreeOutputBuffers_PCMF1){
    PCMF1_Attributes attributes;

    attributes.Buffer = NULL;
    attributes.BufferError = NULL;

    TEST_ASSERT_EQUAL(I106_OK, FreeOutputBuffers_PCMF1(&attributes));
}
