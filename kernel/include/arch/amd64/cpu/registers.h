#ifndef CPU_REGISTERS_H
#define CPU_REGISTERS_H 1

#define NULL_SELECTOR 0x00
#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10
#define USER_DATA_SELECTOR 0x18
#define USER_CODE_SELECTOR 0x20
#define TSS_SELECTOR 0x28

#define INTERRUPT_STACK_SIZE (4096)
#define PERCPU_INTERRUPT_STACKS_NMI_OFFSET 0x20e0

#define IFRAME_OFFSET_RDI (0 * 8)
#define IFRAME_OFFSET_RSI (1 * 8)
#define IFRAME_OFFSET_RBP (2 * 8)
#define IFRAME_OFFSET_RBX (3 * 8)
#define IFRAME_OFFSET_RDX (4 * 8)
#define IFRAME_OFFSET_RCX (5 * 8)
#define IFRAME_OFFSET_RAX (6 * 8)
#define IFRAME_OFFSET_R8 (7 * 8)
#define IFRAME_OFFSET_R9 (8 * 8)
#define IFRAME_OFFSET_R10 (9 * 8)
#define IFRAME_OFFSET_R11 (10 * 8)
#define IFRAME_OFFSET_R12 (11 * 8)
#define IFRAME_OFFSET_R13 (12 * 8)
#define IFRAME_OFFSET_R14 (13 * 8)
#define IFRAME_OFFSET_R15 (14 * 8)

#define IFRAME_OFFSET_VECTOR (15 * 8)
#define IFRAME_OFFSET_ERR_CODE (16 * 8)

#define IFRAME_OFFSET_IP (17 * 8)
#define IFRAME_OFFSET_CS (18 * 8)
#define IFRAME_OFFSET_FLAGS (19 * 8)
#define IFRAME_OFFSET_USER_SP (20 * 8)
#define IFRAME_OFFSET_USER_SS (21 * 8)

#define IFRAME_SIZE (22 * 8)

// This header is intended to be included in both C and ASM
#define CR0_PE 0x00000001 /* protected mode enable */
#define CR0_MP 0x00000002 /* monitor coprocessor */
#define CR0_EM 0x00000004 /* emulation */
#define CR0_TS 0x00000008 /* task switched */
#define CR0_ET 0x00000010 /* extension type */
#define CR0_NE 0x00000020 /* enable x87 exception */
#define CR0_WP 0x00010000 /* supervisor write protect */
#define CR0_NW 0x20000000 /* not write-through */
#define CR0_CD 0x40000000 /* cache disable */
#define CR0_PG 0x80000000 /* enable paging */
#define CR4_PAE 0x00000020 /* PAE paging */
#define CR4_PGE 0x00000080 /* page global enable */
#define CR4_OSFXSR 0x00000200 /* os supports fxsave */
#define CR4_OSXMMEXPT 0x00000400 /* os supports xmm exception */
#define CR4_UMIP 0x00000800 /* User-mode instruction prevention */
#define CR4_VMXE 0x00002000 /* enable vmx */
#define CR4_FSGSBASE 0x00010000 /* enable {rd,wr}{fs,gs}base */
#define CR4_PCIDE 0x00020000 /* Process-context ID enable  */
#define CR4_OSXSAVE 0x00040000 /* os supports xsave */
#define CR4_SMEP 0x00100000 /* SMEP protection enabling */
#define CR4_SMAP 0x00200000 /* SMAP protection enabling */
#define CR4_PKE 0x00400000 /* Enable protection keys */
#define EFER_SCE 0x00000001 /* enable SYSCALL */
#define EFER_LME 0x00000100 /* long mode enable */
#define EFER_LMA 0x00000400 /* long mode active */
#define EFER_NXE 0x00000800 /* to enable execute disable bit */
#define MSR_PLATFORM_ID 0x00000017 /* platform id */
#define MSR_APIC_BASE 0x0000001b /* APIC base physical address */
#define MSR_TSC_ADJUST 0x0000003b /* TSC adjust */
#define MSR_SPEC_CTRL 0x00000048 /* Speculative Execution Controls */
#define SPEC_CTRL_IBRS (1ull << 0)
// Partitions indirect branch predictors across hyperthreads
#define SPEC_CTRL_STIBP (1ull << 1) /* Single Thread Indirect Branch Predictors */
#define SPEC_CTRL_SSBD (1ull << 2)
#define MSR_SMI_COUNT 0x00000034 /* Number of SMI interrupts since boot */
#define MSR_PRED_CMD 0x00000049 /* Indirect Branch Prediction Command */
#define MSR_BIOS_UPDT_TRIG 0x00000079u /* Microcode Patch Loader */
#define MSR_BIOS_SIGN_ID 0x0000008b /* BIOS update signature */
#define MSR_MTRRCAP 0x000000fe /* MTRR capability */
#define MSR_ARCH_CAPABILITIES 0x0000010a
#define ARCH_CAPABILITIES_RDCL_NO (1ull << 0)
#define ARCH_CAPABILITIES_IBRS_ALL (1ull << 1)
#define ARCH_CAPABILITIES_RSBA (1ull << 2)
#define ARCH_CAPABILITIES_SSB_NO (1ull << 4)
#define ARCH_CAPABILITIES_MDS_NO (1ull << 5)
#define ARCH_CAPABILITIES_TSX_CTRL (1ull << 7)
#define ARCH_CAPABILITIES_TAA_NO (1ull << 8)
#define MSR_FLUSH_CMD 0x0000010b /* L1D$ Flush control */
#define MSR_TSX_CTRL 0x00000122 /* Control to enable/disable TSX instructions */
#define TSX_CTRL_RTM_DISABLE (1ull << 0) /* Force all RTM instructions to abort */
#define TSX_CTRL_CPUID_DISABLE (1ull << 1) /* Mask RTM and HLE in CPUID */
#define MSR_SYSENTER_CS 0x00000174 /* SYSENTER CS */
#define MSR_SYSENTER_ESP 0x00000175 /* SYSENTER ESP */
#define MSR_SYSENTER_EIP 0x00000176 /* SYSENTER EIP */
#define MSR_MCG_CAP 0x00000179 /* global machine check capability */
#define MSR_MCG_STATUS 0x0000017a /* global machine check status */
#define MSR_MISC_ENABLE 0x000001a0 /* enable/disable misc processor features */
#define MSR_MISC_ENABLE_TURBO_DISABLE (1ull << 38)
#define MSR_TEMPERATURE_TARGET 0x000001a2 /* Temperature target */
#define MSR_ENERGY_PERF_BIAS 0x000001b0 /* Energy / Performance Bias */
#define MSR_MTRR_PHYSBASE0 0x00000200 /* MTRR PhysBase0 */
#define MSR_MTRR_PHYSMASK0 0x00000201 /* MTRR PhysMask0 */
#define MSR_MTRR_PHYSMASK9 0x00000213 /* MTRR PhysMask9 */
#define MSR_MTRR_DEF_TYPE 0x000002ff /* MTRR default type */
#define MSR_MTRR_FIX64K_00000 0x00000250 /* MTRR FIX64K_00000 */
#define MSR_MTRR_FIX16K_80000 0x00000258 /* MTRR FIX16K_80000 */
#define MSR_MTRR_FIX16K_A0000 0x00000259 /* MTRR FIX16K_A0000 */
#define MSR_MTRR_FIX4K_C0000 0x00000268 /* MTRR FIX4K_C0000 */
#define MSR_MTRR_FIX4K_F8000 0x0000026f /* MTRR FIX4K_F8000 */
#define MSR_PAT 0x00000277 /* PAT */
#define MSR_TSC_DEADLINE 0x000006e0 /* TSC deadline */

#define MSR_X2APIC_APICID 0x00000802 /* x2APIC ID Register (R/O) */
#define MSR_X2APIC_VERSION 0x00000803 /* x2APIC Version Register (R/O) */
#define MSR_X2APIC_TPR 0x00000808 /* x2APIC Task Priority Register (R/W) */
#define MSR_X2APIC_PPR 0x0000080A /* x2APIC Processor Priority Register (R/O) */
#define MSR_X2APIC_EOI 0x0000080B /* x2APIC EOI Register (W/O) */
#define MSR_X2APIC_LDR 0x0000080D /* x2APIC Logical Destination Register (R/O) */
#define MSR_X2APIC_SIVR 0x0000080F /* x2APIC Spurious Interrupt Vector Register (R/W) */
#define MSR_X2APIC_ISR0 0x00000810 /* x2APIC In-Service Register Bits 31:0 (R/O) */
#define MSR_X2APIC_ISR1 0x00000811 /* x2APIC In-Service Register Bits 63:32 (R/O) */
#define MSR_X2APIC_ISR2 0x00000812 /* x2APIC In-Service Register Bits 95:64 (R/O) */
#define MSR_X2APIC_ISR3 0x00000813 /* x2APIC In-Service Register Bits 127:96 (R/O) */
#define MSR_X2APIC_ISR4 0x00000814 /* x2APIC In-Service Register Bits 159:128 (R/O) */
#define MSR_X2APIC_ISR5 0x00000815 /* x2APIC In-Service Register Bits 191:160 (R/O) */
#define MSR_X2APIC_ISR6 0x00000816 /* x2APIC In-Service Register Bits 223:192 (R/O) */
#define MSR_X2APIC_ISR7 0x00000817 /* x2APIC In-Service Register Bits 255:224 (R/O) */
#define MSR_X2APIC_TMR0 0x00000818 /* x2APIC Trigger Mode Register Bits 31:0 (R/O) */
#define MSR_X2APIC_TMR1 0x00000819 /* x2APIC Trigger Mode Register Bits 63:32 (R/O) */
#define MSR_X2APIC_TMR2 0x0000081A /* x2APIC Trigger Mode Register Bits 95:64 (R/O) */
#define MSR_X2APIC_TMR3 0x0000081B /* x2APIC Trigger Mode Register Bits 127:96 (R/O) */
#define MSR_X2APIC_TMR4 0x0000081C /* x2APIC Trigger Mode Register Bits 159:128 (R/O) */
#define MSR_X2APIC_TMR5 0x0000081D /* x2APIC Trigger Mode Register Bits 191:160 (R/O) */
#define MSR_X2APIC_TMR6 0x0000081E /* x2APIC Trigger Mode Register Bits 223:192 (R/O) */
#define MSR_X2APIC_TMR7 0x0000081F /* x2APIC Trigger Mode Register Bits 255:224 (R/O) */
#define MSR_X2APIC_IRR0 0x00000820 /* x2APIC Interrupt Request Register Bits 31:0 (R/O) */
#define MSR_X2APIC_IRR1 0x00000821 /* x2APIC Interrupt Request Register Bits 63:32 (R/O) */
#define MSR_X2APIC_IRR2 0x00000822 /* x2APIC Interrupt Request Register Bits 95:64 (R/O) */
#define MSR_X2APIC_IRR3 0x00000823 /* x2APIC Interrupt Request Register Bits 127:96 (R/O) */
#define MSR_X2APIC_IRR4 0x00000824 /* x2APIC Interrupt Request Register Bits 159:128 (R/O) */
#define MSR_X2APIC_IRR5 0x00000825 /* x2APIC Interrupt Request Register Bits 191:160 (R/O) */
#define MSR_X2APIC_IRR6 0x00000826 /* x2APIC Interrupt Request Register Bits 223:192 (R/O) */
#define MSR_X2APIC_IRR7 0x00000827 /* x2APIC Interrupt Request Register Bits 255:224 (R/O) */
#define MSR_X2APIC_ESR 0x00000828 /* x2APIC Error Status Register (R/W) */
#define MSR_X2APIC_LVT_CMCI \
	0x0000082F /* x2APIC LVT Corrected Machine Check Interrupt Register (R/W) */
#define MSR_X2APIC_ICR 0x00000830 /* x2APIC Interrupt Command Register (R/W) */
#define MSR_X2APIC_LVT_TIMER 0x00000832 /* x2APIC LVT Timer Interrupt Register (R/W) */
#define MSR_X2APIC_LVT_THERMAL 0x00000833 /* x2APIC LVT Thermal Sensor Interrupt Register (R/W) */
#define MSR_X2APIC_LVT_PMI                                                \
	0x00000834 /* x2APIC LVT Performance Monitor Interrupt Register (R/W) \
				*/
#define MSR_X2APIC_LVT_LINT0 0x00000835 /* x2APIC LVT LINT0 Register (R/W) */
#define MSR_X2APIC_LVT_LINT1 0x00000836 /* x2APIC LVT LINT1 Register (R/W) */
#define MSR_X2APIC_LVT_ERROR 0x00000837 /* x2APIC LVT Error Register (R/W) */
#define MSR_X2APIC_INIT_COUNT 0x00000838 /* x2APIC Initial Count Register (R/W) */
#define MSR_X2APIC_CUR_COUNT 0x00000839 /* x2APIC Current Count Register (R/O) */
#define MSR_X2APIC_DIV_CONF 0x0000083E /* x2APIC Divide Configuration Register (R/W) */
#define MSR_X2APIC_SELF_IPI 0x0000083F /* x2APIC Self IPI Register (W/O) */

#define MSR_EFER 0xc0000080 /* EFER */
#define MSR_STAR 0xc0000081 /* system call address */
#define MSR_LSTAR 0xc0000082 /* long mode call address */
#define MSR_CSTAR 0xc0000083 /* ia32-e compat call address */
#define MSR_FMASK 0xc0000084 /* system call flag mask */
#define MSR_FS_BASE 0xc0000100 /* fs base address */
#define MSR_GS_BASE 0xc0000101 /* gs base address */
#define MSR_KERNEL_GS_BASE 0xc0000102 /* kernel gs base */
#define MSR_TSC_AUX 0xc0000103 /* TSC aux */
#define MSR_PM_ENABLE 0x00000770 /* enable/disable HWP */
#define MSR_HWP_CAPABILITIES 0x00000771 /* HWP performance range enumeration */
#define MSR_HWP_REQUEST 0x00000774 /* power manage control hints */
#define MSR_AMD_VIRT_SPEC_CTRL 0xc001011f /* AMD speculative execution controls */
/* See IA32_SPEC_CTRL */
#define CR4_PSE 0xffffffef /* Disabling PSE bit in the CR4 */

// Non-architectural MSRs
#define MSR_POWER_CTL 0x000001fc /* Power Control Register */
#define MSR_RAPL_POWER_UNIT 0x00000606 /* RAPL unit multipliers */
#define MSR_PKG_POWER_LIMIT 0x00000610 /* Package power limits */
#define MSR_PKG_ENERGY_STATUS 0x00000611 /* Package energy status */
#define MSR_PKG_POWER_INFO 0x00000614 /* Package power range info */
#define MSR_DRAM_POWER_LIMIT 0x00000618 /* DRAM RAPL power limit control */
#define MSR_DRAM_ENERGY_STATUS 0x00000619 /* DRAM energy status */
#define MSR_PP0_POWER_LIMIT 0x00000638 /* PP0 RAPL power limit control */
#define MSR_PP0_ENERGY_STATUS 0x00000639 /* PP0 energy status */
#define MSR_PP1_POWER_LIMIT 0x00000640 /* PP1 RAPL power limit control */
#define MSR_PP1_ENERGY_STATUS 0x00000641 /* PP1 energy status */
#define MSR_PLATFORM_ENERGY_COUNTER 0x0000064d /* Platform energy counter */
#define MSR_PPERF 0x0000064e /* Productive performance count */
#define MSR_PERF_LIMIT_REASONS 0x0000064f /* Clipping cause register */
#define MSR_GFX_PERF_LIMIT_REASONS 0x000006b0 /* Clipping cause register for graphics */
#define MSR_PLATFORM_POWER_LIMIT 0x0000065c /* Platform power limit control */
#define MSR_AMD_F10_DE_CFG 0xc0011029 /* AMD Family 10h+ decode config */
#define MSR_AMD_F10_DE_CFG_LFENCE_SERIALIZE (1 << 1)

#define MSR_AMD_LS_CFG 0xc0011020 /* Load/store unit configuration */
#define AMD_LS_CFG_F15H_SSBD (1ull << 54)
#define AMD_LS_CFG_F16H_SSBD (1ull << 33)
#define AMD_LS_CFG_F17H_SSBD (1ull << 10)
#define MSR_K7_HWCR 0xc0010015 /* AMD Hardware Configuration */
#define MSR_K7_HWCR_CPB_DISABLE (1ull << 25) /* Set to disable turbo ('boost') */

// KVM MSRs
#define MSR_KVM_PV_EOI_EN 0x4b564d04 /* Enable paravirtual fast APIC EOI */
#define MSR_KVM_PV_EOI_EN_ENABLE (1ull << 0)

/* EFLAGS/RFLAGS */
#define FLAGS_CF (1 << 0)
#define FLAGS_PF (1 << 2)
#define FLAGS_AF (1 << 4)
#define FLAGS_ZF (1 << 6)
#define FLAGS_SF (1 << 7)
#define FLAGS_TF (1 << 8)
#define FLAGS_IF (1 << 9)
#define FLAGS_DF (1 << 10)
#define FLAGS_OF (1 << 11)
#define FLAGS_STATUS_MASK (0xfff)
#define FLAGS_IOPL_MASK (3 << 12)
#define FLAGS_IOPL_SHIFT (12)
#define FLAGS_NT (1 << 14)
#define FLAGS_RF (1 << 16)
#define FLAGS_VM (1 << 17)
#define FLAGS_AC (1 << 18)
#define FLAGS_VIF (1 << 19)
#define FLAGS_VIP (1 << 20)
#define FLAGS_ID (1 << 21)
#define FLAGS_RESERVED_ONES 0x2
#define FLAGS_RESERVED 0xffc0802a
#define FLAGS_USER                                                                           \
	(FLAGS_CF | FLAGS_PF | FLAGS_AF | FLAGS_ZF | FLAGS_SF | FLAGS_TF | FLAGS_DF | FLAGS_OF | \
	 FLAGS_NT | FLAGS_AC | FLAGS_ID)

/* DR6 */
#define DR6_B0 (1ul << 0)
#define DR6_B1 (1ul << 1)
#define DR6_B2 (1ul << 2)
#define DR6_B3 (1ul << 3)
#define DR6_BD (1ul << 13)
#define DR6_BS (1ul << 14)
#define DR6_BT (1ul << 15)

// NOTE: DR6 is used as a read-only status registers, and it is not writeable through userspace.
//       Any bits attempted to be written will be ignored.
#define DR6_USER_MASK (DR6_B0 | DR6_B1 | DR6_B2 | DR6_B3 | DR6_BD | DR6_BS | DR6_BT)
/* Only bits in DR6_USER_MASK are writeable.
 * Bits 12 and 32:63 must be written with 0, the rest as 1s */
#define DR6_MASK (0xffff0ff0ul)

/* DR7 */
#define DR7_L0 (1ul << 0)
#define DR7_G0 (1ul << 1)
#define DR7_L1 (1ul << 2)
#define DR7_G1 (1ul << 3)
#define DR7_L2 (1ul << 4)
#define DR7_G2 (1ul << 5)
#define DR7_L3 (1ul << 6)
#define DR7_G3 (1ul << 7)
#define DR7_LE (1ul << 8)
#define DR7_GE (1ul << 9)
#define DR7_GD (1ul << 13)
#define DR7_RW0 (3ul << 16)
#define DR7_LEN0 (3ul << 18)
#define DR7_RW1 (3ul << 20)
#define DR7_LEN1 (3ul << 22)
#define DR7_RW2 (3ul << 24)
#define DR7_LEN2 (3ul << 26)
#define DR7_RW3 (3ul << 28)
#define DR7_LEN3 (3ul << 30)

// NOTE1: Even though the GD bit is writable, we disable it for the write_state syscall because it
//        complicates a lot the reasoning about how to access the registers. This is because
//        enabling this bit would make any other access to debug registers to issue an exception.
//        New syscalls should be define to lock/unlock debug registers.
// NOTE2: LE/GE bits are normally ignored, but the manual recommends always setting it to 1 in
//        order to be backwards compatible. Hence they are not writable from userspace.
#define DR7_USER_MASK                                                                             \
	(DR7_L0 | DR7_G0 | DR7_L1 | DR7_G1 | DR7_L2 | DR7_G2 | DR7_L3 | DR7_G3 | DR7_RW0 | DR7_LEN0 | \
	 DR7_RW1 | DR7_LEN1 | DR7_RW2 | DR7_LEN2 | DR7_RW3 | DR7_LEN3)

/* Bits 11:12, 14:15 and 32:63 must be cleared to 0. Bit 10 must be set to 1. */
#define DR7_MASK ((1ul << 10) | DR7_LE | DR7_GE)

#define HW_DEBUG_REGISTERS_COUNT 4

#ifndef __ASSEMBLER__

struct Iframe
{
	unsigned long rdi, rsi, rbp, rbx, rdx, rcx, rax;
	unsigned long r8, r9, r10, r11, r12, r13, r14, r15;
	unsigned long vector;
	unsigned long err_code;
	unsigned long ip, cs, flags;
	unsigned long user_sp, user_ss;
};
#endif

#endif // CPU_REGISTERS_H