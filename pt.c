#include "os.h"
#include <stdbool.h>
#include <stdio.h>
#define MaxWalk 5

typedef struct walkInfo{
    uint64_t ppn;
    bool isValid;
    uint8_t indWalk;
    uint64_t* before_the_last_pt;
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

typedef union physical_address
{
    uint64_t raw;
    struct 
    {
        uint64_t valid: 1;
        uint64_t unused: 11;
        uint64_t frameNumber: 52;
    }order;
}physical_address;

walkInfo perform_page_walk(uint64_t pt, uint64_t vpn)
{
    virtual_address va = {0};
    va.order.vpn = vpn;

    uint64_t* pt_as_va_ptr = (uint64_t*)phys_to_virt((pt) << 12);
    walkInfo result={NO_MAPPING,false,0,pt_as_va_ptr};
    uint64_t currPt;
    uint8_t indWalk;
    for(indWalk = 0;indWalk < MaxWalk;indWalk++)
    {
        pt_as_va_ptr += (va.order.vpn >> 9 *(MaxWalk - indWalk - 1)) & 0x1FF;
        currPt = *pt_as_va_ptr;
        if( !(currPt & 0x1) )
        {
            result.indWalk = indWalk;
            result.before_the_last_pt = pt_as_va_ptr;
            return result;
        }
        result.before_the_last_pt = pt_as_va_ptr;
        pt_as_va_ptr = (uint64_t *)phys_to_virt((currPt - 0x1));
    }
    result.ppn = currPt  >> 12;
    result.isValid = true;
    result.indWalk = MaxWalk - 1;
    return result;
}

void page_table_update(uint64_t pt, uint64_t vpn, uint64_t ppn)
{
    walkInfo walkInfo;
    walkInfo = perform_page_walk(pt,vpn);
    if(ppn == NO_MAPPING)
    {
        
        if(walkInfo.isValid) // For destruction
        {
            *(walkInfo.before_the_last_pt) = 0x0;
        }
    }
    else
    {
        uint64_t* pt_as_va_ptr = walkInfo.before_the_last_pt;
        for(uint8_t indwalk = walkInfo.indWalk; indwalk < MaxWalk;indwalk++)
        {
            if( indwalk > walkInfo.indWalk)
                pt_as_va_ptr += (vpn >> 9 *(MaxWalk - indwalk - 1)) & 0x1FF;

            *pt_as_va_ptr =indwalk<MaxWalk -1 ? ((alloc_page_frame()) << 12) + 0x1 : (ppn <<12) + 0x1;
            pt_as_va_ptr = (uint64_t*)phys_to_virt(*pt_as_va_ptr - 0x1);
        }
    }
}

uint64_t page_table_query(uint64_t pt, uint64_t vpn)
{
    walkInfo walkInfo = perform_page_walk(pt,vpn);
    uint64_t ppn = walkInfo.isValid ? walkInfo.ppn : NO_MAPPING;
    return ppn;
}
