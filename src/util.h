// Misc function and variable declarations.
#ifndef __UTIL_H
#define __UTIL_H

#include "types.h" // u32

// kbd.c
void kbd_init(void);
struct bregs;
void handle_15c2(struct bregs *regs);
void process_key(u8 key);

// mouse.c
void mouse_init(void);
void process_mouse(u8 data);

// serial.c
void serial_setup(void);
void lpt_setup(void);

// clock.c
void clock_setup(void);
void handle_1583(struct bregs *regs);
void handle_1586(struct bregs *regs);
void useRTC(void);
void releaseRTC(void);

// hw/timer.c
void timer_setup(void);
void pmtimer_setup(u16 ioport);
u32 timer_calc(u32 msecs);
u32 timer_calc_usec(u32 usecs);
int timer_check(u32 end);
void ndelay(u32 count);
void udelay(u32 count);
void mdelay(u32 count);
void nsleep(u32 count);
void usleep(u32 count);
void msleep(u32 count);
u32 ticks_to_ms(u32 ticks);
u32 ticks_from_ms(u32 ms);
u32 irqtimer_calc_ticks(u32 count);
u32 irqtimer_calc(u32 msecs);
int irqtimer_check(u32 end);

// apm.c
void apm_shutdown(void);
void handle_1553(struct bregs *regs);

// optionroms.c
struct rom_header;
void callrom(struct rom_header *rom, u16 bdf);

// pcibios.c
void handle_1ab1(struct bregs *regs);
void bios32_init(void);

// fw/acpi.c
extern struct rsdp_descriptor *RsdpAddr;
extern u32 acpi_pm1a_cnt;
void acpi_setup(void);
u32 find_resume_vector(void);
void find_acpi_features(void);
struct acpi_20_generic_address;
void acpi_set_reset_reg(struct acpi_20_generic_address *reg, u8 val);
void acpi_reboot(void);

// fw/csm.c
struct pci_device;
int csm_bootprio_fdc(struct pci_device *pci, int port, int fdid);
int csm_bootprio_ata(struct pci_device *pci, int chanid, int slave);
int csm_bootprio_pci(struct pci_device *pci);

// fw/mptable.c
void mptable_setup(void);

// fw/shadow.c
void make_bios_writable(void);
void make_bios_readonly(void);
void qemu_prep_reset(void);

// fw/smbios.c
extern struct smbios_entry_point *SMBiosAddr;
void smbios_setup(void);
void display_uuid(void);

// fw/pciinit.c
extern const u8 pci_irqs[4];
void pci_setup(void);

// fw/pirtable.c
extern struct pir_header *PirAddr;
void pirtable_setup(void);

// fw/smm.c
void smm_device_setup(void);
void smm_setup(void);

// fw/smp.c
extern u32 CountCPUs;
extern u32 MaxCountCPUs;
void wrmsr_smp(u32 index, u64 val);
void smp_setup(void);
int apic_id_is_present(u8 apic_id);

// fw/coreboot.c
extern const char *CBvendor, *CBpart;
struct cbfs_file;
void debug_cbmem(char c);
void cbfs_run_payload(struct cbfs_file *file);
void coreboot_platform_setup(void);
void cbfs_payload_setup(void);
void coreboot_preinit(void);
void coreboot_cbfs_init(void);

// fw/biostable.c
void copy_smbios(void *pos);
void copy_table(void *pos);

// vgahooks.c
void handle_155f(struct bregs *regs);
void vgahook_setup(struct pci_device *pci);

// optionroms.c
void call_bcv(u16 seg, u16 ip);
int is_pci_vga(struct pci_device *pci);
void optionrom_setup(void);
void vgarom_setup(void);
void s3_resume_vga(void);
extern int ScreenAndDebug;

// bootsplash.c
void enable_vga_console(void);
void enable_bootsplash(void);
void disable_bootsplash(void);

// resume.c
extern int HaveRunPost;
void dma_setup(void);

// pnpbios.c
u16 get_pnp_offset(void);
void pnp_init(void);

// pmm.c
void pmm_init(void);
void pmm_prepboot(void);

// fw/mtrr.c
void mtrr_setup(void);

// romlayout.S
void reset_vector(void) __noreturn;

// misc.c
void mathcp_setup(void);
extern u8 BiosChecksum;

// version (auto generated file out/version.c)
extern const char VERSION[];

#endif // util.h
