
#include "fixture_jitter_buffer.h"
// third-party
#include <boost/thread.hpp>

namespace
{

/**
 *  Data fragmentation routine: divide incoming data into chunked
 *  blocks of given length and package them to output list in the straight order
 *
 *  @param inputData - input data string
 *  @param blockSize - block size we need the input data to be divided into
 *  @param fragmentedData - container with fragmented data
 */
void FragmentData(const std::string& inputData, const size_t blockSize, std::vector<std::string>& fragmentedData)
{
   for (size_t i = 0; i < inputData.size(); i += blockSize)
   {
      size_t count = std::min(blockSize, inputData.size() - i + 1);
      std::string tempBlock = inputData.substr(i, count);
      fragmentedData.push_back(tempBlock);
   }
}

/**
 * Data generation routine: generate buffer of the given size and package it
 * to the std::string
 *
 * @param dataSize - data length we need
 * @returns - string with generated data
 */
std::string GenerateData(const size_t dataSize)
{
   std::string tempString(dataSize, '\0');
   for (size_t i = 0; i < dataSize; ++i)
   {
      tempString[i] = rand() % 255;
   }
   return tempString;
}

} // unnamed namespace

namespace video_coding
{
namespace test
{

/// maximum number of frames in JB simutaneously
static const int MaxFrameNumber = 99;  // 1 in reserve

/*
 @about Check JitterBuffer initialization fails with empty input parameters
 */
TEST_F(FixtureJitterBuffer, Initialization_CheckZeroInputArguments)
{
   result_t code = CreateJB(0, 0);
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check JitterBuffer initialization fails with empty pointer to IRenderer
 */
TEST_F(FixtureJitterBuffer, Initialization_CheckZeroRenderer)
{
   result_t code = CreateJB(GetDecoder().get(), 0);
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check JitterBuffer initialization fails with empty pointer to IDecoder
 */
TEST_F(FixtureJitterBuffer, Initialization_CheckZeroDecoder)
{
   result_t code = CreateJB(0, GetRenderer().get());
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check JitterBuffer initialization goes fine with correct input arguments
 */
TEST_F(FixtureJitterBuffer, Initialization_CheckCorrectArguments)
{
   result_t code = CreateJB(GetDecoder().get(), GetRenderer().get());
   ASSERT_EQ(result_code::sOk, code);
}

/*
 @about Check call to ReceivePacket fails if pointer to the buffer is zero
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_InvalidArgs_ZeroBuffer)
{
   result_t code = CheckReceiverFunction(0, 1, 1, 1, 1);
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check call to ReceivePacket fails if buffer length is zero
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_InvalidArgs_ZeroBufferLength)
{
   result_t code = CheckReceiverFunction((char*)1, 0, 1, 1, 1);
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check call to ReceivePacket fails if frame number is negative
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_InvalidArgs_NegativeFrameNumber)
{
   result_t code = CheckReceiverFunction((char*)1, 1, -1, 1, 1);
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check call to ReceivePacket fails if fragment number is negative
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_InvalidArgs_NegativeFragmentNumber)
{
   result_t code = CheckReceiverFunction((char*)1, 1, 1, -1, 1);
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check call to ReceivePacket fails if number of fragments is non-positive
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_InvalidArgs_ZeroFragmentCount)
{
   result_t code = CheckReceiverFunction((char*)1, 1, 1, 1, 0);
   ASSERT_EQ(result_code::eInvalidArgument, code);
}

/*
 @about Check that single frame chunked into pieces and delivered to the
 component in the normal (forward) order will be assembled properly
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_SingleChunkedFrame_ForwardOrder)
{
   JitterBufferPtr jitterBuffer = GetJB();

   const int chunkSize = 5;
   std::string tempString = GenerateData(1024); // 1Kb
   std::vector<std::string> chunkedData;
   FragmentData(tempString, chunkSize, chunkedData);

   for (int i = 0; i < (int)chunkedData.size(); ++i)
   {
      jitterBuffer->ReceivePacket(chunkedData[i].c_str(), chunkedData[i].length(), 0, i, chunkedData.size());
   }

   boost::this_thread::sleep(boost::posix_time::seconds(5));
   ASSERT_EQ(tempString, GetRenderer()->GetRenderedData());
}

/*
 @about Check that single frame chunked into pieces and delivered to the
 component in the reverse order will be assembled properly
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_SingleChunkedFrame_ReverseOrder)
{
   JitterBufferPtr jitterBuffer = GetJB();

   const int chunkSize = 5;
   const int frameSize = 1024; // 1Kb
   std::string tempString = GenerateData(frameSize);
   std::vector<std::string> chunkedData;
   FragmentData(tempString, chunkSize, chunkedData);

   for (int i = chunkedData.size()-1; i >= 0; --i)
   {
      jitterBuffer->ReceivePacket(chunkedData[i].c_str(), chunkedData[i].length(),
            0, i, chunkedData.size());
   }

   boost::this_thread::sleep(boost::posix_time::seconds(5));
   ASSERT_EQ(tempString, GetRenderer()->GetRenderedData());
}

/*
 @about Check that multiple frames, each of 1 fragment only, delivered to the
 component in the normal (forward) order, will be assembled properly
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_MultipleFrames_NoChunks_ForwardOrder)
{
   JitterBufferPtr jitterBuffer = GetJB();

   const int frameSize = MaxFrameNumber - 1;
   std::string tempString = GenerateData(frameSize);
   std::string resultingString;

   for (int i = 0; i < frameSize; ++i)
   {
      jitterBuffer->ReceivePacket(tempString.c_str(), frameSize, i, 0, 1);
      resultingString += tempString;
   }

   boost::this_thread::sleep(boost::posix_time::seconds(5));
   ASSERT_EQ(resultingString, GetRenderer()->GetRenderedData());
}

/*
 @about Check that multiple frames, each of 1 fragment only, delivered to the
 component in the reverse order, will be assembled properly
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_MultipleFrames_NoChunks_ReverseOrder)
{
   JitterBufferPtr jitterBuffer = GetJB();

   const int frameSize = MaxFrameNumber - 1;
   std::string tempString = GenerateData(frameSize);
   std::string resultingString;

   for (int i = frameSize; i >= 0; --i)
   {
      jitterBuffer->ReceivePacket(tempString.c_str(), frameSize, i, 0, 1);
      resultingString += tempString;
   }

   boost::this_thread::sleep(boost::posix_time::seconds(5));
   ASSERT_EQ(resultingString, GetRenderer()->GetRenderedData());
}

/*
 @about Check that multiple frames, each of several multiple chunks,
 delivered to the component in the forward order, will be assembled properly
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_MultipleFrames_Chunked_ForwardOrder)
{
   JitterBufferPtr jitterBuffer = GetJB();

   const int chunkSize = 5;
   const int frameSize = MaxFrameNumber - 1;
   std::string tempString = GenerateData(frameSize);
   std::vector<std::string> chunkedData;
   FragmentData(tempString, chunkSize, chunkedData);

   std::string resultingString;
   for (int i = 0; i < frameSize; ++i)
   {
      for (int k = 0; k < (int)chunkedData.size(); ++k)
      {
         jitterBuffer->ReceivePacket(chunkedData[k].c_str(), chunkedData[k].length(),
               i, k, chunkedData.size());
         resultingString += chunkedData[k];
      }
   }

   boost::this_thread::sleep(boost::posix_time::seconds(5));
   ASSERT_EQ(resultingString, GetRenderer()->GetRenderedData());
}

/*
 @about Check that multiple frames, each of several multiple chunks,
 delivered to the component in the forward order, will be assembled properly
 */
TEST_F(FixtureJitterBuffer, ReceivePacket_MultipleFrames_Chunked_ReverseOrder)
{
   JitterBufferPtr jitterBuffer = GetJB();

   const int chunkSize = 5;
   const int frameSize = MaxFrameNumber - 1;
   std::string tempString = GenerateData(frameSize);
   std::vector<std::string> chunkedData;
   FragmentData(tempString, chunkSize, chunkedData);

   std::string resultingString;
   for (int i = frameSize; i >= 0; --i)
   {
      for (int k = chunkedData.size()-1; k >= 0; --k)
      {
         jitterBuffer->ReceivePacket(chunkedData[k].c_str(), chunkedData[k].length(),
               i, k, chunkedData.size());
         resultingString = chunkedData[k] + resultingString;
      }
   }

   boost::this_thread::sleep(boost::posix_time::seconds(5));
   ASSERT_EQ(resultingString, GetRenderer()->GetRenderedData());
}



}
}
