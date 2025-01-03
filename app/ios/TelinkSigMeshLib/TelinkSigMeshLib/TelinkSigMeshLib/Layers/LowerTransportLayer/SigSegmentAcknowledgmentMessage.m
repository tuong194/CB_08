/********************************************************************************************************
 * @file     SigSegmentAcknowledgmentMessage.m
 *
 * @brief    for TLSR chips
 *
 * @author   Telink, 梁家誌
 * @date     2019/9/16
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *******************************************************************************************************/

#import "SigSegmentAcknowledgmentMessage.h"
#import "SigSegmentedMessage.h"

@implementation SigSegmentAcknowledgmentMessage

/// Initialize
- (instancetype)init {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
    }
    return self;
}

- (instancetype)initBusySegmentAcknowledgmentMessageWithNetworkPdu:(SigNetworkPdu *)networkPdu {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
        _opCode = 0x00;
        _isOnBehalfOfLowPowerNode = false;// Friendship is not supported.
        NSData *data = networkPdu.transportPdu;
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem1 = 0,tem2=0;
        memcpy(&tem1, dataByte+1, 1);
        memcpy(&tem2, dataByte+2, 1);
        _sequenceZero = (UInt16)(((tem1 & 0x7F) << 6) | (UInt16)tem2 >> 2);
        _blockAck = 0;
        UInt32 bigAck = 0;
        self.upperTransportPdu = [NSData dataWithBytes:&bigAck length:4];
        self.source = networkPdu.destination;
        self.destination = networkPdu.source;
        self.networkKey = networkPdu.networkKey;
    }
    return self;
}

/// Creates the Segmented Acknowledgement Message from the given Network PDU.
/// If the PDU is not valid, it will return `nil`.
///
/// - parameter networkPdu: The Network PDU received.
- (instancetype)initFromNetworkPdu:(SigNetworkPdu *)networkPdu {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
        NSData *data = networkPdu.transportPdu;
        Byte *dataByte = (Byte *)data.bytes;
        UInt8 tem = 0;
        memcpy(&tem, dataByte, 1);
        if (data.length != 7 || (tem & 0x80) != 0) {
            TelinkLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        _opCode = tem & 0x7F;
        if (_opCode != 0) {
            TelinkLogError(@"initFromUnsegmentedPdu fail.");
            return nil;
        }
        UInt8 tem1 = 0,tem2=0;
        memcpy(&tem1, dataByte+1, 1);
        memcpy(&tem2, dataByte+2, 1);
        UInt32 tem3 = 0;
        memcpy(&tem3, dataByte+3, 4);
        _isOnBehalfOfLowPowerNode = (tem1 & 0x80) != 0;
        _sequenceZero = (UInt16)(((tem1 & 0x7F) << 6) | (UInt16)tem2 >> 2);
        _blockAck = CFSwapInt32HostToBig(tem3);
        UInt32 byte = CFSwapInt32HostToBig(tem3);
        self.upperTransportPdu = [NSData dataWithBytes:&byte length:4];
        self.source = networkPdu.source;
        self.destination = networkPdu.destination;
        self.networkKey = networkPdu.networkKey;
    }
    return self;
}

/// Creates the ACK for given array of segments. At least one of
/// segments must not be `nil`.
///
/// - parameter segments: The list of segments to be acknowledged.
- (instancetype)initForSegments:(NSArray <SigSegmentedMessage *>*)segments {
    /// Use the init method of the parent class to initialize some properties of the parent class of the subclass instance.
    if (self = [super init]) {
        /// Initialize self.
        self.type = SigLowerTransportPduType_transportControlMessage;
        _opCode = 0x00;
        _isOnBehalfOfLowPowerNode = false;// Friendship is not supported.
        SigSegmentedMessage *segment = segments.firstObject;
        if (segment == nil || ![segment isKindOfClass:SigSegmentedMessage.class]) {
            for (SigSegmentedMessage *tem in segments) {
                if (tem != nil && ![tem isEqual:[NSNull null]] && [tem isKindOfClass:SigSegmentedMessage.class]) {
                    segment = tem;
                    break;
                }
            }
        }
        _sequenceZero = segment.sequenceZero;
        UInt32 ack = 0;
        NSArray *temArray = [NSArray arrayWithArray:segments];
        for (SigSegmentedMessage *seg in temArray) {
            if (seg != nil && ![seg isEqual:[NSNull null]]) {
                ack |= 1 << seg.segmentOffset;
            }
        }
        _blockAck = ack;
        UInt32 bigAck = CFSwapInt32HostToBig(ack);
        self.upperTransportPdu = [NSData dataWithBytes:&bigAck length:4];
//        TelinkLogInfo(@"node response last segment,send response is acknowledged.ack.blockAck=0x%x,sequenceZero=0x%X,upperTransportPdu=%@",ack,_sequenceZero,self.upperTransportPdu);
        // Assuming all segments have the same source and destination addresses and network key.
        // Swapping source with destination. Destination here is guaranteed to be a Unicast Address.
        self.source = segment.destination;
        self.destination = segment.source;
        self.networkKey = segment.networkKey;
    }
    return self;
}

/// Returns whether the segment with given index has been received.
///
/// - parameter m: The segment number.
/// - returns: `True`, if the segment of the given number has been
///            acknowledged, `false` otherwise.
- (BOOL)isSegmentReceived:(int)m {
    return (_blockAck & (1<<m)) != 0;
}

/// Returns whether all segments have been received.
///
/// - parameter segments: The array of segments received and expected.
/// - returns: `True` if all segments were received, `false` otherwise.
- (BOOL)areAllSegmentsReceivedOfSegments:(NSArray <SigSegmentedMessage *>*)segments {
    return [self areAllSegmentsReceivedLastSegmentNumber:segments.count - 1];
}

/// Returns whether all segments have been received.
///
/// - parameter lastSegmentNumber: The number of the last expected
///             segments (segN).
/// - returns: `True` if all segments were received, `false` otherwise.
- (BOOL)areAllSegmentsReceivedLastSegmentNumber:(UInt8)lastSegmentNumber {
    return _blockAck == ((1 << (_lastSegmentNumber + 1)) - 1);
}

/// Whether the source Node is busy and the message should be cancelled, or not.
- (BOOL)isBusy {
    return _blockAck == 0;
}

- (NSData *)transportPdu {
    UInt8 octet0 = (UInt8)(_opCode & 0x7F);
    UInt8 octet1 = (UInt8)(_isOnBehalfOfLowPowerNode ? 0x80 : 0x00) | (UInt8)(_sequenceZero >> 6);
    UInt8 octet2 = (UInt8)((_sequenceZero & 0x3F) << 2);
    NSMutableData *mData = [NSMutableData data];
    [mData appendData:[NSData dataWithBytes:&octet0 length:1]];
    [mData appendData:[NSData dataWithBytes:&octet1 length:1]];
    [mData appendData:[NSData dataWithBytes:&octet2 length:1]];
    [mData appendData:self.upperTransportPdu];
    return mData;
}

- (NSString *)description {
    return [NSString stringWithFormat:@"<%p> - SigSegmentAcknowledgmentMessage, source:(0x%X), destination:(0x%X) type:(%@), sequenceZero:(0x%X), opCode:(0x%X), isOnBehalfOfLowPowerNode:%d,blockAck:(0x%X),segmentOffset:(0x%X),lastSegmentNumber:(0x%X)", self, self.source,self.destination,self.type==SigLowerTransportPduType_accessMessage?@"accessMessage":@"controlMessage",_sequenceZero,_opCode,_isOnBehalfOfLowPowerNode,(unsigned int)_blockAck,_segmentOffset,_lastSegmentNumber];
}

@end
