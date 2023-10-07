#include <stdio.h>
#include <string.h>

#include "protcan.h"
#include "../utils/utils.h"


CProtCan::CProtCan()
{}

CProtCan::~CProtCan()
{}

void CProtCan::unpack(can_stream_t *strm, can_pack_t *pack, int dlc)
{
    for (int i = 0; i < pack->frames; i++) {
        memcpy(strm->buf + i * dlc, pack->frame[i].data, 
                pack->frame[i].can_dlc);
        strm->bytes += pack->frame[i].can_dlc;
    }
    strm->id.value = pack->frame[0].can_id;
}

void CProtCan::packet(can_pack_t *pack, can_stream_t *strm, int dlc)
{
    int i = 0, frames = 0;
    int bytes = 0, candlc = 0, remain = 0;
    can_id_t id = {
        .value = 0
    };
    frames = getCanFrames(strm->bytes);
    for (i = 0; i < frames; i++) {
        remain = strm->bytes - bytes;
        candlc = (remain < dlc) ? (remain) : (dlc);
        id = strm->id;
        id.ext.addr = strm->id.ext.addr + i * dlc;
        id.ext.multi = true;

        pack->frame[i].can_id = id.value;
        pack->frame[i].can_dlc = candlc;
        memcpy(pack->frame[i].data, strm->buf + i * dlc, candlc);
        bytes += candlc;
    }
    id.ext.multi = false;
    pack->frame[i - 1].can_id = id.value;
    pack->frames = frames;
}

unsigned int CProtCan::genExtId(unsigned short addr, unsigned char slot, 
            bool multi, bool master, unsigned char ctrl, 
            bool err, bool rtr, bool eff)
{
    can_id_t id;
    id.value = 0;
    id.ext.addr = addr;
    id.ext.slot = slot;
    id.ext.multi = multi;
    id.ext.master = master;
    id.ext.ctrl = ctrl;
    id.ext.err = err;
    id.ext.rtr = rtr;
    id.ext.eff = eff;
    return id.value;
}

int CProtCan::isError(unsigned int id)
{
    can_id_t canid = {
        .value = id
    };
    if (canid.ext.ctrl > 0) {
        return -canid.ext.ctrl;
    }
    return 0;
}

int CProtCan::getCanFrames(int bytes, int dlc)
{
    int frames = (bytes <= dlc) ? (1) : (bytes / dlc);
    return ((bytes > dlc) && (bytes % dlc)) ? (++frames) : (frames);
}

