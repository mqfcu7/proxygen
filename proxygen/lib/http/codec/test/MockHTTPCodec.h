/*
 *  Copyright (c) 2015, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include <gmock/gmock.h>
#include <proxygen/lib/http/codec/HTTPCodec.h>

namespace proxygen {

class MockHTTPCodec: public HTTPCodec {
 public:
  MOCK_CONST_METHOD0(getProtocol, CodecProtocol());
  MOCK_CONST_METHOD0(getTransportDirection,  TransportDirection());
  MOCK_CONST_METHOD0(supportsStreamFlowControl, bool());
  MOCK_CONST_METHOD0(supportsSessionFlowControl, bool());
  MOCK_METHOD0(createStream, HTTPCodec::StreamID());
  MOCK_METHOD1(setCallback, void(Callback*));
  MOCK_CONST_METHOD0(isBusy, bool());
  MOCK_CONST_METHOD0(hasPartialTransaction, bool());
  MOCK_METHOD1(setParserPaused, void(bool));
  MOCK_METHOD1(onIngress, size_t(const folly::IOBuf&));
  MOCK_METHOD0(onIngressEOF, void());
  MOCK_CONST_METHOD0(isReusable, bool());
  MOCK_CONST_METHOD0(isWaitingToDrain, bool());
  MOCK_CONST_METHOD0(closeOnEgressComplete, bool());
  MOCK_CONST_METHOD0(supportsParallelRequests, bool());
  MOCK_CONST_METHOD0(supportsPushTransactions, bool());
  MOCK_METHOD6(generateHeader, void(folly::IOBufQueue&,
                                    HTTPCodec::StreamID,
                                    const HTTPMessage&,
                                    HTTPCodec::StreamID,
                                    bool eom,
                                    HTTPHeaderSize*));
  MOCK_METHOD4(generateBody, size_t(folly::IOBufQueue&,
                                    HTTPCodec::StreamID,
                                    std::shared_ptr<folly::IOBuf>,
                                    bool));
  size_t generateBody(folly::IOBufQueue& writeBuf,
                      HTTPCodec::StreamID stream,
                      std::unique_ptr<folly::IOBuf> chain,
                      bool eom) override {
    return generateBody(writeBuf,
                        stream,
                        std::shared_ptr<folly::IOBuf>(chain.release()),
                        eom);
  }
  MOCK_METHOD3(generateChunkHeader, size_t(folly::IOBufQueue&,
                                           HTTPCodec::StreamID,
                                           size_t));
  MOCK_METHOD2(generateChunkTerminator, size_t(folly::IOBufQueue&,
                                               HTTPCodec::StreamID));
  MOCK_METHOD3(generateTrailers, size_t(folly::IOBufQueue&,
                                        HTTPCodec::StreamID,
                                        const HTTPHeaders&));
  MOCK_METHOD2(generateEOM, size_t(folly::IOBufQueue&,
                                   HTTPCodec::StreamID));
  MOCK_METHOD3(generateRstStream, size_t(folly::IOBufQueue&,
                                         HTTPCodec::StreamID,
                                         ErrorCode));
  MOCK_METHOD3(generateGoaway, size_t(folly::IOBufQueue&,
                                      StreamID,
                                      ErrorCode));
  MOCK_METHOD1(generatePingRequest, size_t(folly::IOBufQueue&));
  MOCK_METHOD2(generatePingReply, size_t(folly::IOBufQueue&,
                                         uint64_t));
  MOCK_METHOD1(generateSettings, size_t(folly::IOBufQueue&));
  MOCK_METHOD3(generateWindowUpdate, size_t(folly::IOBufQueue&,
                                            StreamID,
                                            uint32_t));
  MOCK_METHOD0(getEgressSettings, HTTPSettings*());
  MOCK_CONST_METHOD0(getIngressSettings, const HTTPSettings*());
  MOCK_METHOD0(enableDoubleGoawayDrain, void());
 };

class MockHTTPCodecCallback: public HTTPCodec::Callback {
 public:
  MOCK_METHOD2(onMessageBegin, void(HTTPCodec::StreamID, HTTPMessage*));
  MOCK_METHOD3(onPushMessageBegin, void(HTTPCodec::StreamID,
                                        HTTPCodec::StreamID,
                                        HTTPMessage*));
  MOCK_METHOD2(onHeadersComplete, void(HTTPCodec::StreamID,
                                       std::shared_ptr<HTTPMessage>));
  void onHeadersComplete(HTTPCodec::StreamID stream,
                         std::unique_ptr<HTTPMessage> msg) override {
    onHeadersComplete(stream, std::shared_ptr<HTTPMessage>(msg.release()));
  }
  MOCK_METHOD2(onBody, void(HTTPCodec::StreamID,
                            std::shared_ptr<folly::IOBuf>));
  void onBody(HTTPCodec::StreamID stream,
              std::unique_ptr<folly::IOBuf> chain) override {
     onBody(stream, std::shared_ptr<folly::IOBuf>(chain.release()));
  }
  MOCK_METHOD2(onChunkHeader, void(HTTPCodec::StreamID, size_t));
  MOCK_METHOD1(onChunkComplete, void(HTTPCodec::StreamID));
  MOCK_METHOD2(onTrailersComplete, void(HTTPCodec::StreamID,
                                        std::shared_ptr<HTTPHeaders>));
  void onTrailersComplete(HTTPCodec::StreamID stream,
                          std::unique_ptr<HTTPHeaders> trailers) override {
    onTrailersComplete(stream,
                       std::shared_ptr<HTTPHeaders>(trailers.release()));
  }
  MOCK_METHOD2(onMessageComplete, void(HTTPCodec::StreamID, bool));
  MOCK_METHOD3(onError, void(HTTPCodec::StreamID,
                             std::shared_ptr<HTTPException>, bool));
  void onError(HTTPCodec::StreamID stream,
               const HTTPException& exc,
               bool newStream) override {
    onError(stream,
            std::shared_ptr<HTTPException>(new HTTPException(exc)),
            newStream);
  }
  MOCK_METHOD2(onAbort, void(HTTPCodec::StreamID, ErrorCode));
  MOCK_METHOD2(onGoaway, void(uint64_t, ErrorCode));
  MOCK_METHOD1(onPingRequest, void(uint64_t));
  MOCK_METHOD1(onPingReply, void(uint64_t));
  MOCK_METHOD2(onWindowUpdate, void(HTTPCodec::StreamID, uint32_t));
  MOCK_METHOD1(onSettings, void(const SettingsList&));
  MOCK_METHOD0(onSettingsAck, void());
  MOCK_CONST_METHOD0(numOutgoingStreams, uint32_t());
  MOCK_CONST_METHOD0(numIncomingStreams, uint32_t());
};

}
