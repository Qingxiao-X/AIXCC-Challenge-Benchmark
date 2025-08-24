#include <winpr/sysinfo.h>
#include <winpr/path.h>
#include <winpr/crypto.h>
#include <winpr/pipe.h>
#include <freerdp/freerdp.h>
#include <freerdp/gdi/gdi.h>
#include <freerdp/client/cmdline.h>
#include <freerdp/client.h>
#include "../gcc.c"
#include "../tpdu.c"
#include "../tpkt.c"
#include "../mcs.c"
#include "../../crypto/cert_common.c"
#include "../../crypto/certificate.c"
#include "../../crypto/crypto.c"
#include <freerdp/crypto/certificate.h>
#include "../../crypto/x509_utils.c"
#include "../../crypto/privatekey.c"
#include "../../crypto/opensslcompat.c"

void write_big_endian_16(void* address, uint16_t val) {
    uint8_t* addr = (uint8_t*)address;
    addr[0] = (val >> 8) & 0xff;
    addr[1] = val & 0xff;
}
void write_big_endian_32(void* address, uint32_t val) {
    uint8_t* addr = (uint8_t*)address;
    addr[0] = (val >> 0x18) & 0xff;
    addr[1] = (val >> 0x10) & 0xff;
    addr[2] = (val >> 0x8) & 0xff;
    addr[3] = val & 0xff;
}

int TestProtocol(int argc, char* argv[])
{
    size_t total_length = 0;
    uint16_t ber_app_length = 0;
    void *ber_app_length_ptr;
	WINPR_UNUSED(argc);
	WINPR_UNUSED(argv);
    rdpMcs mcs; // this is just written to, no need to fill in
    wStream s;
    s.capacity = 1024;
    s.buffer = calloc(1, s.capacity);
    s.pointer = s.buffer;
    s.length = total_length;
    s.count = 0;
    s.pool = NULL;
    s.isAllocatedStream = FALSE;
    s.isOwner = FALSE;

    /* tpkt */
    *((uint16_t*)s.pointer++) = 0x03; // header
    s.pointer += 3; // skipping padding * size
    
    /* tpdu */
    *((uint8_t*)(s.pointer++)) = 0; // LI
    *((uint8_t*)(s.pointer++)) = X224_TPDU_DATA; // code
    *((uint8_t*)(s.pointer++)) = 0; // padding
    
    /* ber application tag */
    *((uint8_t*)(s.pointer++)) = (BER_CLASS_APPL | BER_CONSTRUCT) | BER_TAG_MASK; // ber type
    *((uint8_t*)(s.pointer++)) = MCS_TYPE_CONNECT_RESPONSE; // app designation
    *((uint8_t*)(s.pointer++)) = 0x80 | 2; // length of length
    ber_app_length_ptr = s.pointer; s.pointer += 2; // actual length we will write later
    *((uint8_t*)(s.pointer++)) = (BER_CLASS_UNIV | BER_PC(FALSE) | (BER_TAG_MASK & BER_TAG_ENUMERATED));
    *((uint8_t*)(s.pointer++)) = 1; // ber length of length
    *((uint8_t*)(s.pointer++)) = 1; // ber length
    *((uint8_t*)(s.pointer++)) = (BER_CLASS_UNIV | BER_PC(FALSE) | (BER_TAG_MASK & BER_TAG_INTEGER));
    *((uint8_t*)(s.pointer++)) = 4; // size of int;
    write_big_endian_32(s.pointer, 0x41414141); s.pointer += 4; // int
    *((uint8_t*)(s.pointer++)) = ((BER_CLASS_UNIV | BER_CONSTRUCT) | (BER_TAG_SEQUENCE_OF));
    *((uint8_t*)(s.pointer++)) = 1; // length (not used, just needs to be > 0)
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // maxChannelIds
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // maxUserIds
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // maxTokenIds
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // numPriorities
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // minThroughput
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // maxHeight
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // maxMCSPDUsize
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    
    *((uint8_t*)(s.pointer++)) = BER_TAG_INTEGER; // protocolVersion
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length

    *((uint8_t*)(s.pointer++)) = BER_TAG_OCTET_STRING; // length
    *((uint8_t*)(s.pointer++)) = 1; // length of length
    *((uint8_t*)(s.pointer++)) = 1; // length
    *((uint8_t*)(s.pointer++)) = 0x5; // data

    *((uint8_t*)(s.pointer++)) = 0x00; // t124_02_98_oid
    *((uint8_t*)(s.pointer++)) = 0x14; // t124_02_98_oid
    *((uint8_t*)(s.pointer++)) = 0x7c; // t124_02_98_oid
    *((uint8_t*)(s.pointer++)) = 0x0; // t124_02_98_oid
    *((uint8_t*)(s.pointer++)) = 0x01; // t124_02_98_oid

    // gcc_read_conference_create_response
    *((uint8_t*)(s.pointer++)) = 1; // ConnectData
    *((uint8_t*)(s.pointer++)) = 1; // ConnectData::connectPDU (OCTET_STRING)
    *((uint8_t*)(s.pointer++)) = 1; // ConferenceCreateResponse::nodeID (UserID)
    write_big_endian_16(s.pointer, 0x1); s.pointer += 2; // ConferenceCreateResponse
    *((uint8_t*)(s.pointer++)) = 1; // ConferenceCreateResponse::tag len
    *((uint8_t*)(s.pointer++)) = 1; // ConferenceCreateResponse::tag val
    *((uint8_t*)(s.pointer++)) = 1; // ConferenceCreateResponse

    *((uint8_t*)(s.pointer++)) = 1; // length of length of h221_sc_key
    *((uint8_t*)(s.pointer++)) = 0; // +length of h221_sc_key
    *((uint8_t*)(s.pointer++)) = 'M'; // h221_sc_key='Duca'
    *((uint8_t*)(s.pointer++)) = 'c'; // h221_sc_key='Duca'
    *((uint8_t*)(s.pointer++)) = 'D'; // h221_sc_key='Duca'
    *((uint8_t*)(s.pointer++)) = 'n'; // h221_sc_key='Duca'
    *((uint8_t*)(s.pointer++)) = 0; // padding

    // write ther ber app length into the appropriate place
    write_big_endian_16(ber_app_length_ptr, (uint16_t)((void*)s.pointer - (void*)ber_app_length_ptr)); 

    // calculate and write the tpkt packet size & write into the appropriate place
    write_big_endian_16(s.buffer + 2, (uint16_t)(s.pointer - s.buffer));
    total_length = (size_t)s.pointer - (size_t)s.buffer;
    s.pointer = s.buffer;
    s.length = total_length;

    // int i;
    // for(i=0; i<total_length; i++) {
    //     fprintf(stderr, "%02x ", *((char*)s.buffer+i) & 0xff);
    //     if (((i % 0x10) == 0) && (i > 0xf))
    //         fprintf(stderr, "\n");
    // }
    // fprintf(stderr, "\n");
        
    if (!mcs_recv_connect_response(&mcs, &s))
        return -1;

	return 0;
}
