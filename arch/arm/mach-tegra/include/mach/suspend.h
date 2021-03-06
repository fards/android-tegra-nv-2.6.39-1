/*
 * arch/arm/mach-tegra/include/mach/suspend.h
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * Author:
 *      Colin Cross <ccross@google.com>
 *
 * Copyright (C) 2011 NVIDIA Corporation
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#ifndef _MACH_TEGRA_SUSPEND_H_
#define _MACH_TEGRA_SUSPEND_H_

#include <mach/clk.h>

enum tegra_suspend_mode {
	TEGRA_SUSPEND_NONE = 0,
	TEGRA_SUSPEND_LP2,	/* CPU voltage off */
	TEGRA_SUSPEND_LP1,	/* CPU voltage off, DRAM self-refresh */
	TEGRA_SUSPEND_LP0,	/* CPU + core voltage off, DRAM self-refresh */
	TEGRA_MAX_SUSPEND_MODE,
};

enum suspend_stage {
	TEGRA_SUSPEND_BEFORE_PERIPHERAL,
	TEGRA_SUSPEND_BEFORE_CPU,
};

enum resume_stage {
	TEGRA_RESUME_AFTER_PERIPHERAL,
	TEGRA_RESUME_AFTER_CPU,
};

struct tegra_suspend_platform_data {
	unsigned long cpu_timer;	/* CPU power good time in us, LP2/LP1 */
	unsigned long cpu_off_timer;	/* CPU power off time us, LP2/LP1 */
	unsigned long core_timer;	/* core power good time in ticks, LP0 */
	unsigned long core_off_timer;	/* core power off time ticks, LP0 */
	unsigned long long wake_enb;	/* mask of enabled wake pads */
	unsigned long long wake_high;	/* high-level-triggered wake pads */
	unsigned long long wake_low;	/* low-level-triggered wake pads */
	unsigned long long wake_any;	/* any-edge-triggered wake pads */
	bool corereq_high;		/* Core power request active-high */
	bool sysclkreq_high;		/* System clock request is active-high */
	bool separate_req;		/* Core & CPU power request are separate */
	enum tegra_suspend_mode	suspend_mode;
	unsigned long cpu_lp2_min_residency; /* Min LP2 state residency in us */
	void (*board_suspend)(int lp_state, enum suspend_stage stg);
	/* lp_state = 0 for LP0 state, 1 for LP1 state, 2 for LP2 state */
	void (*board_resume)(int lp_state, enum resume_stage stg);
};

unsigned long tegra_cpu_power_good_time(void);
unsigned long tegra_cpu_power_off_time(void);
unsigned long tegra_cpu_lp2_min_residency(void);
enum tegra_suspend_mode tegra_get_suspend_mode(void);

void __tegra_lp1_reset(void);
void __tegra_iram_end(void);

void lp0_suspend_init(void);

void tegra_pinmux_suspend(void);
void tegra_irq_suspend(void);
void tegra_gpio_suspend(void);
void tegra_clk_suspend(void);
void tegra_dma_suspend(void);
void tegra_timer_suspend(void);

void tegra_pinmux_resume(void);
void tegra_irq_resume(void);
void tegra_gpio_resume(void);
void tegra_clk_resume(void);
void tegra_dma_resume(void);
void tegra_timer_resume(void);

int tegra_irq_to_wake(int irq);
int tegra_wake_to_irq(int wake);

#ifndef CONFIG_ARCH_TEGRA_2x_SOC
void tegra_irq_disable_affinity(void);
void tegra_irq_affinity_to_cpu0(void);
void tegra_irq_restore_affinity(void);
void tegra_irq_pass_through_disable(void);
#endif


int tegra_set_lp0_wake(int irq, int enable);
int tegra_set_lp0_wake_type(int irq, int flow_type);
int tegra_set_lp1_wake(int irq, int enable);
void tegra_set_lp0_wake_pads(u64 wake_enb, u64 wake_level, u64 wake_any);

void __init tegra_init_suspend(struct tegra_suspend_platform_data *plat);

/*
 * Callbacks for platform drivers to implement.
 */
extern void (*tegra_deep_sleep)(int);

/* The debug channel uart base physical address */
extern unsigned long  debug_uart_port_base;

extern struct clk *debug_uart_clk;
void tegra_console_uart_suspend(void);
void tegra_console_uart_resume(void);

#endif /* _MACH_TEGRA_SUSPEND_H_ */
