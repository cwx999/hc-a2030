#ifndef __PROTOCOL_H
#define __PROTOCOL_H


#ifdef __cplusplus
extern "C" {
#endif


// 错误码定义
typedef enum {
    ENONE = 0, 
    EPACKET, 
    ECHKSUM, 
    EDATASIZE, 
    ENOCTRLWD, 
    ETRANSFER, 
    ERDONLY, 
    EWRONLY, 
    ENOADDR, 
    EPARAM, 
    ENOSLOT, 
    EOPERAT
} prot_ecode_t;

// 错误信息
typedef struct {
    prot_ecode_t code;
    const char *string;
} prot_estr_t;


#ifdef __cplusplus
}
#endif


#endif /* __PROTOCOL_H */
