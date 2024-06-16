#include "os.h"
typedef struct walkInfo{
    uint64_t ppn
} walkInfo;

typedef union virtual_address
{
    uint64_t raw;
    struct 
    {
        uint64_t offset: 12;
        uint64_t vpn: 45;
        uint64_t sign_ext: 7;
    }order;
}virtual_address;




#define MaxWalk 5
walkInfo perform_page_walk(uint64_t pt, uint64_t vpn)
{
    virtual_address va = {0};
    va.order.vpn = vpn;
    walkInfo result={NO_MAPPING};
    uint64_t* pt_as_ptr = (uint64_t*)pt;
    uint64_t currPt;
    for(uint8_t indWalk = 0;indWalk < MaxWalk;indWalk++)
    {
        pt_as_ptr += (va.order.vpn >> 9 *(MaxWalk - indWalk - 1)) & 0x1FF;
        currPt = *pt_as_ptr;
        if( !(currPt & 0x1) )
            return result;

        pt_as_ptr = (uint64_t *)((currPt >> 12) << 12);
    }
    return result;
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn)
{
    if(ppn == NO_MAPPING)
    {

    }
    else
    {

    }
}


