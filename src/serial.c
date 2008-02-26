// 16bit code to handle serial and printer services.
//
// Copyright (C) 2008  Kevin O'Connor <kevin@koconnor.net>
// Copyright (C) 2002  MandrakeSoft S.A.
//
// This file may be distributed under the terms of the GNU GPLv3 license.

#include "biosvar.h" // struct bregs
#include "util.h" // debug_enter


/****************************************************************
 * COM ports
 ****************************************************************/

static u16
getComAddr(struct bregs *regs)
{
    if (regs->dx >= 4) {
        set_cf(regs, 1);
        return 0;
    }
    u16 addr = GET_BDA(port_com[regs->dx]);
    if (! addr)
        set_cf(regs, 1);
    return addr;
}

static void
handle_1400(struct bregs *regs)
{
    u16 addr = getComAddr(regs);
    if (!addr)
        return;
    outb(inb(addr+3) | 0x80, addr+3);
    if ((regs->al & 0xE0) == 0) {
        outb(0x17, addr);
        outb(0x04, addr+1);
    } else {
        u16 val16 = 0x600 >> ((regs->al & 0xE0) >> 5);
        outb(val16 & 0xFF, addr);
        outb(val16 >> 8, addr+1);
    }
    outb(regs->al & 0x1F, addr+3);
    regs->ah = inb(addr+5);
    regs->al = inb(addr+6);
    set_cf(regs, 0);
}

static void
handle_1401(struct bregs *regs)
{
    u16 addr = getComAddr(regs);
    if (!addr)
        return;
    u16 timer = GET_BDA(timer_counter);
    u16 timeout = GET_BDA(com_timeout[regs->dx]);
    while (((inb(addr+5) & 0x60) != 0x60) && (timeout)) {
        u16 val16 = GET_BDA(timer_counter);
        if (val16 != timer) {
            timer = val16;
            timeout--;
        }
    }
    if (timeout)
        outb(regs->al, addr);
    regs->ah = inb(addr+5);
    if (!timeout)
        regs->ah |= 0x80;
    set_cf(regs, 0);
}

static void
handle_1402(struct bregs *regs)
{
    u16 addr = getComAddr(regs);
    if (!addr)
        return;
    u16 timer = GET_BDA(timer_counter);
    u16 timeout = GET_BDA(com_timeout[regs->dx]);
    while (((inb(addr+5) & 0x01) == 0) && (timeout)) {
        u16 val16 = GET_BDA(timer_counter);
        if (val16 != timer) {
            timer = val16;
            timeout--;
        }
    }
    if (timeout) {
        regs->ah = 0;
        regs->al = inb(addr);
    } else {
        regs->ah = inb(addr+5);
    }
    set_cf(regs, 0);
}

static void
handle_1403(struct bregs *regs)
{
    u16 addr = getComAddr(regs);
    if (!addr)
        return;
    regs->ah = inb(addr+5);
    regs->al = inb(addr+6);
    set_cf(regs, 0);
}

static void
handle_14XX(struct bregs *regs)
{
    // Unsupported
    set_cf(regs, 1);
}

// INT 14h Serial Communications Service Entry Point
void VISIBLE
handle_14(struct bregs *regs)
{
    debug_enter(regs);

    irq_enable();

    switch (regs->ah) {
    case 0x00: handle_1400(regs); break;
    case 0x01: handle_1401(regs); break;
    case 0x02: handle_1402(regs); break;
    case 0x03: handle_1403(regs); break;
    default:   handle_14XX(regs); break;
    }
    debug_exit(regs);
}


/****************************************************************
 * LPT ports
 ****************************************************************/

static u16
getLptAddr(struct bregs *regs)
{
    if (regs->dx >= 3) {
        set_cf(regs, 1);
        return 0;
    }
    u16 addr = GET_BDA(port_lpt[regs->dx]);
    if (! addr)
        set_cf(regs, 1);
    return addr;
}

static void
lpt_ret(struct bregs *regs, u16 addr, u16 timeout)
{
    u8 val8 = inb(addr+1);
    regs->ah = (val8 ^ 0x48);
    if (!timeout)
        regs->ah |= 0x01;
    set_cf(regs, 0);
}

// INT 17 - PRINTER - WRITE CHARACTER
static void
handle_1700(struct bregs *regs)
{
    u16 addr = getLptAddr(regs);
    if (!addr)
        return;
    u16 timeout = GET_BDA(lpt_timeout[regs->dx]) << 8;

    outb(regs->al, addr);
    u8 val8 = inb(addr+2);
    outb(val8 | 0x01, addr+2); // send strobe
    nop();
    outb(val8 & ~0x01, addr+2);
    while (((inb(addr+1) & 0x40) == 0x40) && (timeout))
        timeout--;

    lpt_ret(regs, addr, timeout);
}

// INT 17 - PRINTER - INITIALIZE PORT
static void
handle_1701(struct bregs *regs)
{
    u16 addr = getLptAddr(regs);
    if (!addr)
        return;
    u16 timeout = GET_BDA(lpt_timeout[regs->dx]) << 8;

    u8 val8 = inb(addr+2);
    outb(val8 & ~0x04, addr+2); // send init
    nop();
    outb(val8 | 0x04, addr+2);

    lpt_ret(regs, addr, timeout);
}

// INT 17 - PRINTER - GET STATUS
static void
handle_1702(struct bregs *regs)
{
    u16 addr = getLptAddr(regs);
    if (!addr)
        return;
    u16 timeout = GET_BDA(lpt_timeout[regs->dx]) << 8;

    lpt_ret(regs, addr, timeout);
}

static void
handle_17XX(struct bregs *regs)
{
    // Unsupported
    set_cf(regs, 1);
}

// INT17h : Printer Service Entry Point
void VISIBLE
handle_17(struct bregs *regs)
{
    debug_enter(regs);

    irq_enable();

    switch (regs->ah) {
    case 0x00: handle_1700(regs); break;
    case 0x01: handle_1701(regs); break;
    case 0x02: handle_1702(regs); break;
    default:   handle_17XX(regs); break;
    }
    debug_exit(regs);
}
