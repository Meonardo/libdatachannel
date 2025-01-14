/**
 * Copyright (c) 2020 Staz Modrzynski
 * Copyright (c) 2020 Paul-Louis Ageneau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef RTC_RTCP_RECEIVING_SESSION_H
#define RTC_RTCP_RECEIVING_SESSION_H

#if RTC_ENABLE_MEDIA

#include "common.hpp"
#include "mediahandler.hpp"
#include "message.hpp"
#include "rtp.hpp"

#include <atomic>

namespace rtc {

// An RtcpSession can be plugged into a Track to handle the whole RTCP session
class RTC_CPP_EXPORT RtcpReceivingSession : public MediaHandler {
public:
	RtcpReceivingSession() = default;
	virtual ~RtcpReceivingSession() = default;

	void incoming(message_vector &messages, const message_callback &send) override;
	bool requestKeyframe(const message_callback &send) override;
	bool requestBitrate(unsigned int bitrate, const message_callback &send) override;

	// For backward compatibility
	[[deprecated("Use Track::requestKeyframe()")]] inline bool requestKeyframe() { return false; };
	[[deprecated("Use Track::requestBitrate()")]] inline void requestBitrate(unsigned int) {};

	void setReceiverReportCallback(std::function<void(const RtcpRr *)> onReceiverReport) {
		mOnReceiverReport = onReceiverReport;
	}
	void setSenderReportCallback(std::function<void(double, double)> onSenderReport) {
		mOnSenderReport = onSenderReport;
	}

 protected:
	void pushREMB(const message_callback &send, unsigned int bitrate);
	void pushRR(const message_callback& send, unsigned int lastSrDelay);
	void pushRR(const message_callback &send,unsigned int lastSrDelay, 
		uint8_t fractionLost, uint32_t packetsLostCount);
	void pushPLI(const message_callback &send);

	SSRC mSsrc = 0;
	uint32_t mGreatestSeqNo = 0;
	uint64_t mSyncRTPTS, mSyncNTPTS;

	// for calculate lost packets
	uint16_t mLastRtpSeq = 0;
	uint16_t mSeqCycles = 0;
	size_t mLastCyclePackets = 0;
	uint16_t mSeqMax = 0;
	uint16_t mSeqBase = 0;
	size_t mPackets = 0;
	size_t mLastExpected = 0;
	size_t mLastLost = 0;

	size_t getExpectedPackets() const {
		return (mSeqCycles << 16) + mSeqMax - mSeqBase + 1;
	}
	size_t getExpectedPacketsInterval() {
		auto expected = getExpectedPackets() - mPackets;
		auto ret = expected - mLastExpected;
		mLastExpected = expected;
		return ret;
	}
	size_t getLost() {
		return getExpectedPackets() - mPackets;
	}
	size_t getLostInterval() {
		auto lost = getLost();
		auto ret = lost - mLastLost;
		mLastLost = lost;
		return ret;
	}

	std::atomic<unsigned int> mRequestedBitrate = 0;
	rtc::synchronized_callback<const rtc::RtcpRr *> mOnReceiverReport;
	rtc::synchronized_callback<double, double> mOnSenderReport;
};

} // namespace rtc

#endif // RTC_ENABLE_MEDIA

#endif // RTC_RTCP_RECEIVING_SESSION_H
